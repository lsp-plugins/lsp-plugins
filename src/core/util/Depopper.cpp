/*
 * Depopper.cpp
 *
 *  Created on: 4 июл. 2020 г.
 *      Author: sadko
 */

#include <core/util/Depopper.h>

namespace lsp
{
    
    Depopper::Depopper()
    {
        construct();
    }
    
    Depopper::~Depopper()
    {
    }

    void Depopper::construct()
    {
        nSampleRate     = DEFAULT_SAMPLE_RATE;
        fClosedGain     = 0.0f;
        fOpenedGain     = 1.0f;
        fFadeTime       = 50.0f;
        fThreshold      = GAIN_AMP_M_80_DB;
        fAttack         = 10.0f;
        fRelease        = 10.0f;
        fEnvelope       = 0.0f;
        nClipCounter    = 0;
        nState          = ST_CLOSED;
        bReconfigure    = true;
        enMode          = DPM_LINEAR;

        fTauAttack      = 0.0f;
        fTauRelease     = 0.0f;
        nFade           = 0;
        fPoly[0]        = 0.0f;
        fPoly[1]        = 0.0f;
        fPoly[2]        = 0.0f;
        fPoly[3]        = 0.0f;
    }

    void Depopper::reconfigure()
    {
        if (!bReconfigure)
            return;

        float att           = millis_to_samples(nSampleRate, fAttack);
        float rel           = millis_to_samples(nSampleRate, fRelease);
        float fade          = millis_to_samples(nSampleRate, fFadeTime);
        float thresh        = fabs(fThreshold);

        if (thresh > 1.0f)
            thresh              = 1.0f;

        fTauAttack          = (att < 1.0f)  ? 1.0f : 1.0f - expf(logf(1.0f - M_SQRT1_2) / att);
        fTauRelease         = (rel < 1.0f)  ? 1.0f : 1.0f - expf(logf(1.0f - M_SQRT1_2) / rel);
        nFade               = fade;
        float k             = 1.0f / fade;

        switch (enMode)
        {
            case DPM_LINEAR:
                fPoly[0]    = 0.0f;
                fPoly[1]    = k;
                fPoly[2]    = 0.0f;
                fPoly[3]    = 0.0f;
                break;

            case DPM_CUBIC:
                // y(x) = -2*x^3 + 3*x^2
                fPoly[0]    = 0.0f;
                fPoly[1]    = 0.0f;
                fPoly[2]    = +3.0f * k*k;
                fPoly[3]    = -2.0f * k*k*k;
                break;

            case DPM_SINE:
                fPoly[0]    = M_PI * 0.5f * k;
                fPoly[1]    = 0.0f;
                fPoly[2]    = 0.0f;
                fPoly[3]    = 0.0f;
                break;

            case DPM_PARABOLIC:
                fPoly[0]    = 0.0f;
                fPoly[1]    = 0.0f;
                fPoly[2]    = k*k;
                fPoly[3]    = 0.0f;
                break;

            case DPM_GAUSSIAN:
            {
                // mu       = 1
                // sigma    = sqrt(2)/2;
                // g(x)     = 1/sqrtf(PI) * exp( -(x-4)^2 )
                float f0    = expf(-16.0f); // g(0)
                float f4    = 1.0f; // g(4)

                fPoly[0]    = 4.0f * k;
                fPoly[1]    = 1.0f / (f4 - f0); // Norming: 1 / (g(4) - g(0))
                fPoly[2]    = -f0;
                fPoly[3]    = 0.0f;
                break;
            }

            default:
                fPoly[0]    = 0.0f;
                fPoly[1]    = 0.0f;
                fPoly[2]    = 0.0f;
                fPoly[3]    = 0.0f;
                break;
        }

        bReconfigure        = false;
    }

    size_t Depopper::set_sample_rate(size_t sr)
    {
        size_t old      = nSampleRate;
        if (old == sr)
            return old;

        nSampleRate     = sr;
        bReconfigure    = true;
        return old;
    }

    float Depopper::set_closed_gain(float gain)
    {
        float old       = fClosedGain;
        fClosedGain     = gain;
        return old;
    }

    float Depopper::set_opened_gain(float gain)
    {
        float old       = fOpenedGain;
        fOpenedGain     = gain;
        return old;
    }

    float Depopper::set_fade_time(float time)
    {
        float old       = fFadeTime;
        if (old == time)
            return old;

        fFadeTime       = time;
        bReconfigure    = true;

        return old;
    }

    float Depopper::set_threshold(float thresh)
    {
        float old       = fThreshold;
        if (old == thresh)
            return old;

        fThreshold      = thresh;
        bReconfigure    = true;

        return old;
    }

    float Depopper::set_attack(float attack)
    {
        float old       = fAttack;
        if (old == attack)
            return old;

        fAttack         = attack;
        bReconfigure    = true;

        return old;
    }

    float Depopper::set_release(float release)
    {
        float old       = fRelease;
        if (old == release)
            return old;

        fRelease        = release;
        bReconfigure    = true;

        return old;
    }

    depopper_mode_t Depopper::set_mode(depopper_mode_t mode)
    {
        depopper_mode_t old = enMode;
        if (mode == old)
            return old;

        enMode          = mode;
        bReconfigure    = true;

        return old;
    }

    float Depopper::crossfade()
    {
        float x, gain = fOpenedGain;

        if ((nClipCounter++) >= nFade)
        {
            nState      = ST_OPENED;
            return gain;
        }

        switch (enMode)
        {
            case DPM_LINEAR:
            case DPM_CUBIC:
            case DPM_PARABOLIC:
            {
                x           = nClipCounter;
                gain       *= fPoly[0] + x*(fPoly[1] + x*(fPoly[2] + x*fPoly[3]));
                break;
            }

            case DPM_SINE:
            {
                x           = sinf(fPoly[0] * nClipCounter);
                gain       *= x*x;
                break;
            }

            case DPM_GAUSSIAN:
            {
                x           = (fPoly[0] * nClipCounter - 4.0f);
                gain       *= fPoly[1] * expf(-x*x) + fPoly[2];
                break;
            }

            default:
                break;
        }

        return gain;
    }

    void Depopper::process(float *dst, const float *src, size_t count)
    {
        // Reconfigure if needed
        reconfigure();

        // Process samples
        for (size_t i=0; i<count; ++i)
        {
            float s         = fabs(src[i]);
            float x         = s - fEnvelope;
            fEnvelope     += (x > 0.0f) ? fTauAttack * x : fTauRelease  * x;

            // Produce output gain
            switch (nState)
            {
                case ST_CLOSED:
                    if (fEnvelope < fThreshold)
                    {
                        dst[i]      = fClosedGain; // Closed
                        break;
                    }

                    nClipCounter= 0;
                    nState      = ST_FADING;
                    dst[i]      = crossfade();
                    break;

                case ST_FADING:
                    // Compute gain
                    dst[i]      = crossfade();

                    // Fall-off below threshold ?
                    if (fEnvelope < fThreshold)
                        nState      = ST_CLOSED;
                    break;

                case ST_OPENED:
                    dst[i]      = 1.0f; // Normal gain

                    // Fall-off before threshold ?
                    if (fEnvelope < fThreshold)
                        nState      = ST_CLOSED;

                    break;
                default:
                    dst[i]      = fOpenedGain;
                    break;
            }
        }
    }

    void Depopper::dump(IStateDumper *v) const
    {
        v->write("nSampleRate", nSampleRate);
        v->write("fClosedGain", fClosedGain);
        v->write("fOpenedGain", fOpenedGain);
        v->write("fFadeTime", fFadeTime);
        v->write("fThreshold", fThreshold);
        v->write("fAttack", fAttack);
        v->write("fRelease", fRelease);
        v->write("fEnvelope", fEnvelope);
        v->write("nClipCounter", nClipCounter);
        v->write("nState", nState);
        v->write("bReconfigure", bReconfigure);
        v->write("enMode", enMode);

        v->write("fTauAttack", fTauAttack);
        v->write("fTauRelease", fTauRelease);
        v->write("nFade", nFade);
        v->writev("fPoly", fPoly, 4);
    }

} /* namespace lsp */
