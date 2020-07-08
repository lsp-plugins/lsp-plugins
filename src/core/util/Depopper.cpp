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

            case DPM_RMS:
                fPoly[0]    = 0.0f;
                fPoly[1]    = (fade < 1.0f)  ? 1.0f : 1.0f - expf(logf(1.0f - M_SQRT1_2) / fade);
                break;

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
        float gain;

        switch (enMode)
        {
            case DPM_LINEAR:
            case DPM_CUBIC:
            {
                if ((nClipCounter++) >= nFade)
                {
                    nState      = ST_OPENED;
                    gain        = 1.0f;
                    break;
                }

                float x     = nClipCounter;
                gain        = fPoly[0] + x*(fPoly[1] + x*(fPoly[2] + x*fPoly[3]));
                break;
            }

            case DPM_RMS:
            {
                gain        = fPoly[0];
                if (gain >= 0.99999f)
                    nState      = ST_OPENED;
                else if (nClipCounter++ <= 0)
                    fPoly[0]    = 0.0f;

                float dx    = 1.0 - fPoly[0];
                fPoly[0]   += dx * fPoly[1];

                break;
            }

            default:
                gain        = 1.0f;
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
                        dst[i]      = 1.0f; // Normal gain
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
                    dst[i]      = 1.0f;
                    break;
            }
        }
    }

} /* namespace lsp */
