/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 4 июл. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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

        fLookMax            = 0.0f;
        nLookMin            = 0;
        nLookMax            = 0;
        nLookOff            = 0;

        fRmsMax             = 0.0f;
        fRmsLength          = 0.0f;
        nRmsMin             = 0;
        nRmsMax             = 0;
        nRmsOff             = 0;
        fRmsNorm            = 0.0f;

        nCounter            = 0;
        nDelay              = 0;
        fRms                = 0.0f;

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
        pRmsBuf             = NULL;
        pData               = NULL;

        bReconfigure        = true;
    }

    void Depopper::destroy()
    {
        if (pData != NULL)
        {
            free_aligned(pData);
            pData               = NULL;
        }

        pGainBuf            = NULL;
        pRmsBuf             = NULL;
    }

    bool Depopper::init(size_t srate, float max_fade, float max_rms)
    {
        // Properties have changed?
        if ((nSampleRate == srate) &&
            (fLookMax == max_fade) &&
            (fRmsMax == max_rms))
            return true;

        destroy();

        // Update settings
        nSampleRate         = srate;
        fLookMax            = max_fade;
        fRmsMax             = max_rms;

        size_t lk_samp      = millis_to_samples(nSampleRate, fLookMax);
        size_t rms_samp     = millis_to_samples(nSampleRate, fRmsMax);

        ssize_t lk_buf      = ALIGN_SIZE(lk_samp, DEFAULT_ALIGN);
        ssize_t rms_buf     = ALIGN_SIZE(rms_samp, DEFAULT_ALIGN);

        nLookMin            = lk_buf   + rms_buf;
        nLookMax            = nLookMin + lsp_max(lk_buf*4, BUF_SIZE);
        nLookOff            = nLookMin;

        nRmsMin             = rms_buf;
        nRmsMax             = nRmsMin  + lsp_max(rms_buf*4, BUF_SIZE);
        nRmsOff             = nRmsMin;

        size_t buf_sz       = nRmsMax + nLookMax;
        float *data         = alloc_aligned<float>(pData, buf_sz);
        if (data == NULL)
            return false;

        dsp::fill_zero(data, buf_sz);

        pGainBuf            = data;
        pRmsBuf             = &pGainBuf[nLookMax];

        nState              = ST_CLOSED;
        bReconfigure        = true;

        return true;
    }

    void Depopper::calc_fade(fade_t *fade, bool in)
    {
        float time          = millis_to_samples(nSampleRate, fade->fTime);
        fade->nDelay        = millis_to_samples(nSampleRate, fade->fDelay);
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

        calc_fade(&sFadeIn, true);
        calc_fade(&sFadeOut, false);

        nRmsLen             = millis_to_samples(nSampleRate, fRmsLength);
        nLookCount          = sFadeOut.nSamples + nRmsLen;
        fRmsNorm            = 1.0f / nRmsLen;

        // Recompute RMS value
        fRms                = dsp::h_sum(&pRmsBuf[nRmsOff - nRmsLen], nRmsLen);

        bReconfigure        = false;
    }

    depopper_mode_t Depopper::set_fade_in_mode(depopper_mode_t mode)
    {
        depopper_mode_t old = sFadeIn.enMode;
        if (mode == old)
            return old;

        sFadeIn.enMode      = mode;
        bReconfigure        = true;

        return old;
    }

    depopper_mode_t Depopper::set_fade_out_mode(depopper_mode_t mode)
    {
        depopper_mode_t old = sFadeOut.enMode;
        if (mode == old)
            return old;

        sFadeOut.enMode     = mode;
        bReconfigure        = true;

        return old;
    }

    float Depopper::set_fade_in_time(float time)
    {
        float old           = lsp_max(0.0f, sFadeIn.fTime);
        if (old == time)
            return old;

        sFadeIn.fTime       = time;
        bReconfigure        = true;

        return old;
    }

    float Depopper::set_fade_out_time(float time)
    {
        float old           = lsp_limit(sFadeOut.fTime, 0.0f, fLookMax);
        if (old == time)
            return old;

        sFadeOut.fTime      = time;
        bReconfigure        = true;

        return old;
    }

    float Depopper::set_fade_in_threshold(float thresh)
    {
        float old           = lsp_max(0.0f, sFadeIn.fThresh);
        if (old == thresh)
            return old;

        sFadeIn.fThresh     = thresh;
        bReconfigure        = true;

        return old;
    }

    float Depopper::set_fade_out_threshold(float thresh)
    {
        float old           = lsp_max(0.0f, sFadeOut.fThresh);
        if (old == thresh)
            return old;

        sFadeOut.fThresh    = thresh;
        bReconfigure        = true;

        return old;
    }

    float Depopper::set_fade_in_delay(float delay)
    {
        float old           = lsp_max(0.0f, sFadeIn.fDelay);
        if (old == delay)
            return old;

        sFadeIn.fDelay      = delay;
        bReconfigure        = true;

        return old;
    }

    float Depopper::set_fade_out_delay(float delay)
    {
        float old           = lsp_max(0.0f, sFadeOut.fThresh);
        if (old == delay)
            return old;

        sFadeOut.fDelay     = delay;
        bReconfigure        = true;

        return old;
    }

    float Depopper::set_rms_length(float length)
    {
        length              = lsp_limit(length, 0.0f, fRmsMax);
        float old           = fRmsLength;
        if (old == length)
            return old;

        fRmsLength          = length;
        bReconfigure        = true;

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
                gain        = 0.0f;
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
        *dst    = 0.0f;                 // Closed
        dst    -= samples + nRmsLen;    // Roll-back pointer by number of samples + RMS estimation

        // Apply fade-out patch
        for (ssize_t x = sFadeOut.nSamples - samples; x < sFadeOut.nSamples; ++x)
            *(dst++)   *= crossfade(&sFadeOut, x);

        // Fill rest samples with zeros
        dsp::fill_zero(dst, nRmsLen);
    }

    float Depopper::calc_rms(float s)
    {
        // Need to shift the buffer ?
        if (nRmsOff >= nRmsMax)
        {
            dsp::move(pRmsBuf, &pRmsBuf[nRmsOff-nRmsMin], nRmsMin);
            nRmsOff     = nRmsMin;

            // Recompute RMS value
            fRms        = dsp::h_sum(&pRmsBuf[nRmsOff - nRmsLen], nRmsLen);
        }
        else if ((nRmsOff & 0x1f) == 0) // Recompute RMS each 32 samples
            fRms        = dsp::h_sum(&pRmsBuf[nRmsOff - nRmsLen], nRmsLen);

        s                   = s*s;
        float g             = pRmsBuf[nRmsOff - nRmsLen];
        fRms                = fabs(fRms + s - g);
        pRmsBuf[nRmsOff++]  = s;
        return sqrtf(fRms * fRmsNorm);
    }

    void Depopper::process(float *env, float *gain, const float *src, size_t count)
    {
        // Reconfigure if needed
        reconfigure();

        while (count > 0)
        {
            // Need to shift gain buffer?
            size_t can_do   = nLookMax - nLookOff;
            if (can_do <= 0)
            {
                dsp::move(pGainBuf, &pGainBuf[nLookOff-nLookMin], nLookMin);
                nLookOff    = nLookMin;
                can_do      = nLookMax - nLookOff;
            }

            // Estimate number of samples and gain buffer position
            float *gbuf     = &pGainBuf[nLookOff];
            size_t to_do    = (count > can_do) ? can_do : count;

            // Process each sample
            for (size_t i=0; i<to_do; ++i)
            {
                float s         = calc_rms(src[i]);
                env[i]          = s;

                switch (nState)
                {
                    case ST_CLOSED:
                        gbuf[i]     = 0.0f; // Still closed
                        if (s < sFadeIn.fThresh) // Can fade in?
                            break;

                        // Open the fade in
                        nCounter    = 0;
                        nDelay      = sFadeIn.nDelay;
                        nState      = ST_FADE;
                        gbuf[i]     = crossfade(&sFadeIn, nCounter++);
                        break;

                    case ST_FADE:
                        // Compute gain
                        gbuf[i]     = crossfade(&sFadeIn, nCounter++);

                        // Fall-off below threshold ?
                        if (s < sFadeOut.fThresh)
                        {
                            if ((--nDelay) <= 0)
                            {
                                apply_fadeout(&gbuf[i], nCounter);
                                nCounter    = 0;
                                nState      = ST_WAIT;
                            }
                        }
                        else
                        {
                            nDelay      = sFadeIn.nDelay;       // Reset delay
                            if (nCounter >= sFadeIn.nSamples)   // Fade has been completed?
                                nState      = ST_OPENED;
                        }
                        break;

                    case ST_OPENED:
                        gbuf[i]     = 1.0f; // Opened
                        if (nCounter < sFadeOut.nSamples) // Increment counter
                            nCounter++;

                        // Fall-off before threshold ?
                        if (s < sFadeOut.fThresh)
                        {
                            apply_fadeout(&gbuf[i], nCounter);
                            nState      = ST_WAIT;
                            nDelay      = sFadeOut.nDelay;
                        }
                        break;

                    case ST_WAIT:
                        gbuf[i]     = 0.0f; // Wait state, same as closed
                        if ((--nDelay) <= 0)
                            nState      = ST_CLOSED;
                        break;

                    default:
                        gbuf[i]     = 1.0f;
                        break;
                }
            }

            // Copy data from buffer and shift buffer
            dsp::copy(gain, &gbuf[-nLookCount], to_do);

            // Update pointers
            nLookOff       += to_do;
            count          -= to_do;
            env            += to_do;
            gain           += to_do;
            src            += to_do;
        }
    }

    void Depopper::dump_fade(IStateDumper *v, const char *name, const fade_t *fade)
    {
        v->begin_object(name, fade, sizeof(fade_t));
        {
            v->write("enMode", fade->enMode);
            v->write("fThresh", fade->fThresh);
            v->write("fTime", fade->fTime);
            v->write("fDelay", fade->fDelay);
            v->write("nSamples", fade->nSamples);
            v->write("nDelay", fade->nDelay);
            v->writev("fPoly", fade->fPoly, 4);
        }
        v->end_object();
    }

    void Depopper::dump(IStateDumper *v) const
    {
        v->write("nSampleRate", nSampleRate);
        v->write("nState", nState);

        v->write("fLookMax", fLookMax);
        v->write("nLookMin", nLookMin);
        v->write("nLookMax", nLookMax);
        v->write("nLookOff", nLookOff);
        v->write("nLookCount", nLookCount);

        v->write("fRmsMax", fRmsMax);
        v->write("fRmsLength", fRmsLength);
        v->write("nRmsMin", nRmsMin);
        v->write("nRmsMax", nRmsMax);
        v->write("nRmsOff", nRmsOff);
        v->write("nRmsLen", nRmsLen);
        v->write("fRmsNorm", fRmsNorm);

        v->write("nCounter", nCounter);
        v->write("nDelay", nDelay);
        v->write("fRms", fRms);

        dump_fade(v, "sFadeIn", &sFadeIn);
        dump_fade(v, "sFadeOut", &sFadeOut);

        v->write("pGainBuf", pGainBuf);
        v->write("pRmsBuf", pRmsBuf);
        v->write("pData", pData);

        v->write("bReconfigure", bReconfigure);
    }

} /* namespace lsp */
