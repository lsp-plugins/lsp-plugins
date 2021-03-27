/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 16 сент. 2016 г.
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

#include <dsp/dsp.h>
#include <core/interpolation.h>
#include <core/dynamics/Compressor.h>
#include <math.h>

namespace lsp
{
    Compressor::Compressor()
    {
        // Basic parameters
        fAttackThresh   = 0.0f;
        fReleaseThresh  = 0.0f;
        fBoostThresh    = GAIN_AMP_M_72_DB;
        fAttack         = 0.0f;
        fRelease        = 0.0f;
        fKnee           = 0.0f;
        fRatio          = 1.0f;
        fEnvelope       = 0.0f;

        // Pre-calculated parameters
        fTauAttack      = 0.0f;
        fTauRelease     = 0.0f;
        fXRatio         = 0.0f;

        fLogTH          = 0.0f;
        fKS             = 0.0f;
        fKE             = 0.0f;
        vHermite[0]     = 0.0f;
        vHermite[1]     = 0.0f;
        vHermite[2]     = 0.0f;

        fBLogTH         = 0.0f;
        vBHermite[0]    = 0.0f;
        vBHermite[1]    = 0.0f;
        vBHermite[2]    = 0.0f;
        fBKS            = 0.0f;
        fBKE            = 0.0f;
        fBoost          = 1.0f;

        // Additional parameters
        nSampleRate     = 0;
        nMode           = CM_DOWNWARD;
        bUpdate         = true;
    }

    Compressor::~Compressor()
    {
    }

    void Compressor::update_settings()
    {
        // Update settings if necessary
        fTauAttack      = 1.0f - expf(logf(1.0f - M_SQRT1_2) / (millis_to_samples(nSampleRate, fAttack)));
        fTauRelease     = 1.0f - expf(logf(1.0f - M_SQRT1_2) / (millis_to_samples(nSampleRate, fRelease)));

        // Configure according to the mode
        float ratio     = 1.0f / fRatio;
        fKS             = fAttackThresh * fKnee;        // Knee start
        fKE             = fAttackThresh / fKnee;        // Knee end
        fXRatio         = ratio;

        float log_ks    = logf(fKS);
        float log_ke    = logf(fKE);
        fLogTH          = logf(fAttackThresh);

        // Mode-dependent configuration
        switch (nMode)
        {
            case CM_UPWARD:
            {
                fBKS            = fBoostThresh * fKnee;         // Boost knee start
                fBKE            = fBoostThresh / fKnee;         // Boost knee end
                fBLogTH         = logf(fBoostThresh);

                float boost     = (fXRatio-1.0)*(fBLogTH - fLogTH);
                fBoost          = expf(boost);
                float log_bks   = logf(fBKS);
                float log_bke   = logf(fBKE);

                interpolation::hermite_quadratic(vHermite, log_ks, log_ks, 1.0f, log_ke, 2.0f - fXRatio);
                interpolation::hermite_quadratic(vBHermite, log_bks, log_bks, 1.0f, log_bke, fXRatio);

                break;
            }

            case CM_BOOSTING:
            {
                fBKS            = fBoostThresh * fKnee;         // Boost knee start
                fBKE            = fBoostThresh / fKnee;         // Boost knee end
                fBLogTH         = logf(fBoostThresh);

                float boost     = (fXRatio-1.0)*(fBLogTH - fLogTH);
                fBoost          = expf(boost);
                float log_bks   = logf(fBKS);
                float log_bke   = logf(fBKE);

                interpolation::hermite_quadratic(vHermite, log_ks, log_ks, 1.0f, log_ke, 2.0f - fXRatio);
                interpolation::hermite_quadratic(vBHermite, log_bks, log_bks, 1.0f, log_bke, fXRatio);

                break;
            }

            case CM_DOWNWARD:
            default:
                interpolation::hermite_quadratic(vHermite, log_ks, log_ks, 1.0f, log_ke, fXRatio);
                break;
        }

        // Reset update flag
        bUpdate         = false;
    }

    void Compressor::process(float *out, float *env, const float *in, size_t samples)
    {
        // Calculate envelope of compressor
        for (size_t i=0; i<samples; ++i)
        {
            float s         = *(in++);

            if (fEnvelope > fReleaseThresh)
                fEnvelope       += (s > fEnvelope) ? fTauAttack * (s - fEnvelope) : fTauRelease * (s - fEnvelope);
            else
                fEnvelope       += fTauAttack * (s - fEnvelope);

            out[i]          = fEnvelope;
        }

        // Copy envelope to array if specified
        if (env != NULL)
            dsp::copy(env, out, samples);

        // Now calculate compressor's curve
        reduction(out, out, samples);
    }

    float Compressor::process(float *env, float s)
    {
        if (fEnvelope > fReleaseThresh)
            fEnvelope       += (s > fEnvelope) ? fTauAttack * (s - fEnvelope) : fTauRelease * (s - fEnvelope);
        else
            fEnvelope       += fTauAttack * (s - fEnvelope);

        if (env != NULL)
            *env    = fEnvelope;

        return reduction(fEnvelope);
    }

    void Compressor::curve(float *out, const float *in, size_t dots)
    {
        if (nMode == CM_DOWNWARD)
        {
            for (size_t i=0; i<dots; ++i)
            {
                float x     = fabs(in[i]);

                if (x > fKS)
                {
                    float lx    = logf(x);
                    out[i]      = (x >= fKE) ?
                        expf(fXRatio*(lx - fLogTH) + fLogTH) :
                        expf((vHermite[0]*lx + vHermite[1])*lx + vHermite[2]);
                }
                else
                    out[i]      = x;
            }
        }
        else
        {
            for (size_t i=0; i<dots; ++i)
            {
                float x     = fabs(in[i]);
                float lx    = logf(x);
                float g1    = 1.0f, g2 = 1.0f;
                if (x > fBKS)
                {
                    g1  = (x >= fBKE) ?
                        expf((fXRatio - 1.0f)*(lx-fBLogTH)) :
                        expf((vBHermite[0]*lx + vBHermite[1] - 1.0f)*lx + vBHermite[2]);
                }

                if (x > fKS)
                {
                    g2  = (x >= fKE) ?
                        expf((1.0f - fXRatio)*(lx-fLogTH)) :
                        expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
                }

                out[i]      = x * g1 * g2 * fBoost;
            }
        }
    }

    float Compressor::curve(float in)
    {
        float x     = fabs(in);

        if (nMode == CM_DOWNWARD)
        {
            if (x > fKS)
            {
                float lx    = logf(x);
                return (x >= fKE) ?
                    expf(fXRatio*(lx - fLogTH) + fLogTH) :
                    expf((vHermite[0]*lx + vHermite[1])*lx + vHermite[2]);
            }
        }
        else
        {
            float lx    = logf(x);
            float g1    = 1.0f, g2 = 1.0f;
            if (x > fBKS)
            {
                g1  = (x >= fBKE) ?
                    expf((fXRatio - 1.0f)*(lx-fBLogTH)) :
                    expf((vBHermite[0]*lx + vBHermite[1] - 1.0f)*lx + vBHermite[2]);
            }

            if (x > fKS)
            {
                g2  = (x >= fKE) ?
                    expf((1.0f - fXRatio)*(lx-fLogTH)) :
                    expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
            }

            return x * g1 * g2 * fBoost;
        }

        return x;
    }

    void Compressor::reduction(float *out, const float *in, size_t dots)
    {
        if (nMode == CM_DOWNWARD)
        {
            for (size_t i=0; i<dots; ++i)
            {
                float x     = fabs(in[i]);
                if (x > fKS)
                {
                    float lx    = logf(x);
                    out[i]  = (x >= fKE) ?
                        expf((fXRatio-1.0f)*(lx-fLogTH)) :
                        expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
                }
                else
                    out[i]      = 1.0f;
            }
        }
        else
        {
            for (size_t i=0; i<dots; ++i)
            {
                float x     = fabs(in[i]);
                float lx    = logf(x);
                float g1    = 1.0f, g2 = 1.0f;
                if (x > fBKS)
                {
                    g1  = (x >= fBKE) ?
                        expf((fXRatio - 1.0f)*(lx-fBLogTH)) :
                        expf((vBHermite[0]*lx + vBHermite[1] - 1.0f)*lx + vBHermite[2]);
                }

                if (x > fKS)
                {
                    g2  = (x >= fKE) ?
                        expf((1.0f - fXRatio)*(lx-fLogTH)) :
                        expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
                }

                out[i]      = g1 * g2 * fBoost;
            }
        }
    }

    float Compressor::reduction(float in)
    {
        float x     = fabs(in);

        if (nMode == CM_DOWNWARD)
        {
            if (x > fKS)
            {
                float lx    = logf(x);
                return (x >= fKE) ?
                    expf((fXRatio-1.0f)*(lx-fLogTH)) :
                    expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
            }
        }
        else
        {
            float lx    = logf(x);
            float g1    = 1.0f, g2 = 1.0f;
            if (x > fBKS)
            {
                g1  = (x >= fBKE) ?
                    expf((fXRatio - 1.0f)*(lx-fBLogTH)) :
                    expf((vBHermite[0]*lx + vBHermite[1] - 1.0f)*lx + vBHermite[2]);
            }

            if (x > fKS)
            {
                g2  = (x >= fKE) ?
                    expf((1.0f - fXRatio)*(lx-fLogTH)) :
                    expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
            }

            return g1 * g2 * fBoost;
        }

        return 1.0f;
    }

    void Compressor::dump(IStateDumper *v) const
    {
        v->write("fAttackThresh", fAttackThresh);
        v->write("fReleaseThresh", fReleaseThresh);
        v->write("fBoostThresh", fBoostThresh);
        v->write("fAttack", fAttack);
        v->write("fRelease", fRelease);
        v->write("fKnee", fKnee);
        v->write("fRatio", fRatio);
        v->write("fEnvelope", fEnvelope);
        v->write("fTauAttack", fTauAttack);
        v->write("fTauRelease", fTauRelease);
        v->write("fXRatio", fXRatio);
        v->write("fLogTH", fLogTH);
        v->write("fKS", fKS);
        v->write("fKE", fKE);
        v->writev("vHermite", vHermite, 3);
        v->write("fBLogTH", fBLogTH);
        v->write("fBKS", fBKS);
        v->write("fBKE", fBKE);
        v->writev("vBHermite", vBHermite, 3);
        v->write("fBoost", fBoost);
        v->write("nSampleRate", nSampleRate);
        v->write("nMode", nMode);
        v->write("bUpdate", bUpdate);
    }

} /* namespace lsp */
