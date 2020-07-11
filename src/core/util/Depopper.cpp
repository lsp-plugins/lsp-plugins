/*
 * Depopper.cpp
 *
 *  Created on: 4 июл. 2020 г.
 *      Author: sadko
 */

#include <core/util/Depopper.h>
#include <core/stdlib/math.h>
#include <dsp/dsp.h>

#define BUF_SIZE            0x1000

namespace lsp
{
    
    Depopper::Depopper()
    {
        construct();
    }
    
    Depopper::~Depopper()
    {
        destroy();
    }

    void Depopper::construct()
    {
        nSampleRate         = -1;
        nState              = ST_CLOSED;
        fMaxLookahead       = 0.0f;
        fAttack             = 10.0f;
        fRelease            = 10.0f;
        fEnvelope           = 0.0f;
        bReconfigure        = true;

        fTauAttack          = 0.0f;
        fTauRelease         = 0.0f;
        nCounter            = 0;

        sFadeIn.enMode      = DPM_LINEAR;
        sFadeIn.fThresh     = GAIN_AMP_M_80_DB;
        sFadeIn.fTime       = 50.0f;
        sFadeIn.nSamples    = 0;
        sFadeIn.fPoly[0]    = 0.0f;
        sFadeIn.fPoly[1]    = 0.0f;
        sFadeIn.fPoly[2]    = 0.0f;
        sFadeIn.fPoly[3]    = 0.0f;

        sFadeOut.enMode     = DPM_LINEAR;
        sFadeOut.fThresh    = GAIN_AMP_M_80_DB;
        sFadeOut.fTime      = 0.0f;
        sFadeOut.nSamples   = 0;
        sFadeOut.fPoly[0]   = 0.0f;
        sFadeOut.fPoly[1]   = 0.0f;
        sFadeOut.fPoly[2]   = 0.0f;
        sFadeOut.fPoly[3]   = 0.0f;

        pGainBuf            = NULL;
        pData               = NULL;
    }

    void Depopper::destroy()
    {
        if (pData != NULL)
        {
            free_aligned(pData);
            pData               = NULL;
        }

        pGainBuf            = NULL;
    }

    bool Depopper::init(size_t srate, float max_lookahead)
    {
        // Properties have changed?
        if ((nSampleRate == srate) && (fMaxLookahead == max_lookahead))
            return true;

        destroy();

        // Update settings
        nSampleRate         = srate;
        fMaxLookahead       = max_lookahead;
        nLookahead          = millis_to_samples(nSampleRate, fMaxLookahead);

        size_t buf_sz       = ALIGN_SIZE(nLookahead, DEFAULT_ALIGN) + BUF_SIZE;

        float *data         = alloc_aligned<float>(pData, buf_sz);
        if (data == NULL)
            return false;

        dsp::fill_zero(data, buf_sz);

        pGainBuf            = data;

        nState              = ST_CLOSED;
        bReconfigure        = true;

        return true;
    }

    void Depopper::calc_fade(fade_t *fade, bool in)
    {
        float time          = millis_to_samples(nSampleRate, fade->fTime);
        fade->nSamples      = time;
        float k             = 1.0f / time;

        switch (fade->enMode)
        {
            case DPM_LINEAR:
                // in:  y(x) = k * x + 0;
                // out: y(x) = 1 - k
                if (in)
                {
                    fade->fPoly[0]  = 0.0f;
                    fade->fPoly[1]  = k;
                }
                else
                {
                    fade->fPoly[0]  = 1.0f;
                    fade->fPoly[1]  = -k;
                }
                fade->fPoly[2]  = 0.0f;
                fade->fPoly[3]  = 0.0f;
                break;

            case DPM_CUBIC:
                // in:  y(x) = -2*x^3 + 3*x^2
                // out: y(x) =  2*x^3 - 3*x^2 + 1
                if (in)
                {
                    fade->fPoly[0]  =  0.0f;
                    fade->fPoly[1]  =  0.0f;
                    fade->fPoly[2]  = +3.0f * k*k;
                    fade->fPoly[3]  = -2.0f * k*k*k;
                }
                else
                {
                    fade->fPoly[0]  = +1.0f;
                    fade->fPoly[1]  =  0.0f;
                    fade->fPoly[2]  = -3.0f * k*k;
                    fade->fPoly[3]  = +2.0f * k*k*k;
                }
                break;

            case DPM_SINE:
                // in:  y(x) = sin^2( 0.5 * pi * k )
                // out: y(x) = sin^2( 0.5 * pi * k + pi/2 )
                fade->fPoly[0]  = M_PI * 0.5f * k;
                if (in)
                    fade->fPoly[1]  = 0.0f;
                else
                    fade->fPoly[1]  = M_PI_2;

                fade->fPoly[2]  = 0.0f;
                fade->fPoly[3]  = 0.0f;
                break;

            case DPM_PARABOLIC:
                // in:  y(x) = x^2;
                // out: y(x) = x^2 - 2*x + 1
                if (in)
                {
                    fade->fPoly[0]  = 0.0f;
                    fade->fPoly[1]  = 0.0f;
                    fade->fPoly[2]  = k*k;
                }
                else
                {
                    fade->fPoly[0]  = +1.0f;
                    fade->fPoly[1]  = -2.0f * k;
                    fade->fPoly[2]  = k*k;
                }

                fade->fPoly[3]  = 0.0f;
                break;

            case DPM_GAUSSIAN:
            {
                // in:  y(x)    = k/sqrtf(PI) * exp( -(x-4)^2 ) + b
                // out: y(x)    = k/sqrtf(PI) * exp( -(x)^2 ) + b
                float f0        = expf(-16.0f); // y(0)
                float f4        = 1.0f; // y(4)

                fade->fPoly[0]  = 4.0f * k;
                fade->fPoly[1]  = (in) ? -4.0f : 0.0f;
                fade->fPoly[2]  = 1.0f / (f4 - f0); // Norming: 1 / (g(4) - g(0))
                fade->fPoly[3]  = -f0;

                break;
            }

            default:
                fade->fPoly[0]  = 0.0f;
                fade->fPoly[1]  = 0.0f;
                fade->fPoly[2]  = 0.0f;
                fade->fPoly[3]  = 0.0f;
                break;
        }
    }

    void Depopper::reconfigure()
    {
        if (!bReconfigure)
            return;

        float att           = millis_to_samples(nSampleRate, fAttack);
        float rel           = millis_to_samples(nSampleRate, fRelease);

        fTauAttack          = (att < 1.0f)  ? 1.0f : 1.0f - expf(logf(1.0f - M_SQRT1_2) / att);
        fTauRelease         = (rel < 1.0f)  ? 1.0f : 1.0f - expf(logf(1.0f - M_SQRT1_2) / rel);

        calc_fade(&sFadeIn, true);
        calc_fade(&sFadeOut, false);

        bReconfigure        = false;
    }

    depopper_mode_t Depopper::set_fade_in_mode(depopper_mode_t mode)
    {
        depopper_mode_t old = sFadeIn.enMode;
        if (mode == old)
            return old;

        sFadeIn.enMode  = mode;
        bReconfigure    = true;

        return old;
    }

    depopper_mode_t Depopper::set_fade_out_mode(depopper_mode_t mode)
    {
        depopper_mode_t old = sFadeOut.enMode;
        if (mode == old)
            return old;

        sFadeOut.enMode = mode;
        bReconfigure    = true;

        return old;
    }

    float Depopper::set_fade_in_time(float time)
    {
        float old       = sFadeIn.fTime;
        if (old == time)
            return old;

        sFadeIn.fTime   = time;
        bReconfigure    = true;

        return old;
    }

    float Depopper::set_fade_out_time(float time)
    {
        float old       = sFadeOut.fTime;
        if (time > fMaxLookahead)
            time            = fMaxLookahead;
        if (old == time)
            return old;

        sFadeOut.fTime  = time;
        bReconfigure    = true;

        return old;
    }

    float Depopper::set_fade_in_threshold(float thresh)
    {
        float old       = sFadeIn.fThresh;
        if (old == thresh)
            return old;

        sFadeIn.fThresh = thresh;
        bReconfigure    = true;

        return old;
    }

    float Depopper::set_fade_out_threshold(float thresh)
    {
        float old       = sFadeOut.fThresh;
        if (old == thresh)
            return old;

        sFadeOut.fThresh= thresh;
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

    float Depopper::crossfade(fade_t *fade, float x)
    {
        if (x < 0.0f)
            return 0.0f;
        else if (x >= fade->nSamples)
            return 1.0f;

        float gain;

        switch (fade->enMode)
        {
            case DPM_LINEAR:
            case DPM_CUBIC:
            case DPM_PARABOLIC:
            {
                gain        = fade->fPoly[0] + x*(fade->fPoly[1] + x*(fade->fPoly[2] + x*fade->fPoly[3]));
                break;
            }

            case DPM_SINE:
            {
                x           = sinf(fade->fPoly[0] * x + fade->fPoly[1]);
                gain        = x*x;
                break;
            }

            case DPM_GAUSSIAN:
            {
                x           = (fade->fPoly[0] * x + fade->fPoly[1]);
                gain        = fade->fPoly[2] * expf(-x*x) + fade->fPoly[3];
                break;
            }

            default:
                break;
        }

        return gain;
    }

    void Depopper::apply_fadeout(float *dst, ssize_t samples)
    {
        if (sFadeOut.nSamples <= 0)
            return;

        // Determine the length
        if (samples > sFadeOut.nSamples)
            samples     = sFadeOut.nSamples;

        lsp_trace("samples=%d, self=%d", int(samples), int (sFadeOut.nSamples));

        // roll-back destination pointer
        *dst    = 0.0f;     // Closed
        dst    -= samples;  // Roll-back pointer by number of samples

        // Apply fade-out patch
        for (ssize_t x = sFadeOut.nSamples - samples; x < sFadeOut.nSamples; ++x)
            *(dst++)   *= crossfade(&sFadeOut, x);
    }

    void Depopper::process(float *env, float *gain, const float *src, size_t count)
    {
        // Reconfigure if needed
        reconfigure();

        float *gbuf     = &pGainBuf[nLookahead];        // Current read/write position in gain   buffer

        while (count > 0)
        {
            size_t to_do    = (count > BUF_SIZE) ? BUF_SIZE : count;

            // Process each sample
            for (size_t i=0; i<to_do; ++i)
            {
                float s         = fabs(src[i]);
                float x         = s - fEnvelope;
                fEnvelope      += (x > 0.0f) ? fTauAttack * x : fTauRelease * x;
                env[i]          = fEnvelope;

                switch (nState)
                {
                    case ST_CLOSED:
                        gbuf[i]     = 0.0f; // Still closed
                        if (fEnvelope < sFadeIn.fThresh) // Can fade in?
                            break;

                        // Open the fade in
                        nCounter    = 0;
                        nState      = (fEnvelope > sFadeOut.fThresh) ? ST_FADE2 : ST_FADE1;
                        gbuf[i]     = crossfade(&sFadeIn, nCounter++);
                        break;

                    case ST_FADE1:
                        // Compute gain
                        gbuf[i]     = crossfade(&sFadeIn, nCounter++);

                        // Fall-off below threshold ?
                        if (nCounter >= sFadeIn.nSamples)
                            nState      = ST_OPENED;
                        else if (fEnvelope > sFadeOut.fThresh)
                            nState      = ST_FADE2;
                        break;

                    case ST_FADE2:
                        // Compute gain
                        gbuf[i]    = crossfade(&sFadeIn, nCounter++);

                        // Fall-off below threshold ?
                        if (fEnvelope < sFadeOut.fThresh)
                        {
                            apply_fadeout(&gbuf[i], nCounter);
                            nState      = ST_WAIT;
                        }
                        else if (nCounter >= sFadeIn.nSamples)
                            nState      = ST_OPENED;
                        break;

                    case ST_OPENED:
                        gbuf[i]     = 1.0f; // Opened
                        if (nCounter < sFadeOut.nSamples) // Increment counter
                            nCounter++;

                        // Fall-off before threshold ?
                        if (fEnvelope < sFadeOut.fThresh)
                        {
                            apply_fadeout(&gbuf[i], nCounter);
                            nState      = ST_WAIT;
                        }
                        break;
                    case ST_WAIT:
                        gbuf[i]     = 0.0f; // Wait state, same as closed
                        if (fEnvelope < sFadeIn.fThresh)
                            nState      = ST_CLOSED;
                        break;
                    default:
                        gain[i]     = 1.0f;
                        break;
                }
            }

            // Copy data from buffer and shift buffer
            dsp::copy(gain, &gbuf[-sFadeOut.nSamples], to_do);
            dsp::move(pGainBuf, &pGainBuf[to_do], BUF_SIZE + nLookahead - to_do);

            // Update pointers
            count          -= to_do;
            env            += to_do;
            gain           += to_do;
            src            += to_do;
        }
    }

    void Depopper::dump_fade(IStateDumper *v, const char *name, const fade_t *fade)
    {
        v->start_object(name, fade, sizeof(fade_t));
        {
            v->write("enMode", fade->enMode);
            v->write("fThresh", fade->fThresh);
            v->write("fTime", fade->fTime);
            v->write("nSamples", fade->nSamples);
            v->writev("fPoly", fade->fPoly, 4);
        }
        v->end_object();
    }

    void Depopper::dump(IStateDumper *v) const
    {
        v->write("nSampleRate", nSampleRate);
        v->write("nState", nState);
        v->write("fMaxLookahead", fMaxLookahead);
        v->write("nLookahead", nLookahead);
        v->write("fAttack", fAttack);
        v->write("fRelease", fRelease);
        v->write("fEnvelope", fEnvelope);
        v->write("bReconfigure", bReconfigure);

        v->write("fTauAttack", fTauAttack);
        v->write("fTauRelease", fTauRelease);
        v->write("nCounter", nCounter);

        dump_fade(v, "sFadeIn", &sFadeIn);
        dump_fade(v, "sFadeOut", &sFadeOut);

        v->write("pGainBuf", pGainBuf);
        v->write("pData", pData);
    }

} /* namespace lsp */
