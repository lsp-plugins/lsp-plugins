/*
 * Compressor.cpp
 *
 *  Created on: 16 сент. 2016 г.
 *      Author: sadko
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
        fAttack         = 0.0f;
        fRelease        = 0.0f;
        fKnee           = 0.0f;
        fRatio          = 1.0f;
        fEnvelope       = 0.0f;

        // Pre-calculated parameters
        fTauAttack      = 0.0f;
        fTauRelease     = 0.0f;
        vHermite[0]     = 0.0f;
        vHermite[1]     = 0.0f;
        vHermite[2]     = 0.0f;
        fXRatio         = 0.0f;
        fKS             = 0.0f;
        fKE             = 0.0f;
        fLogTH          = 0.0f;

        // Additional parameters
        nSampleRate     = 0;
        bUpward         = false;
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

        // Calculate interpolation parameters
        float ratio     = 1.0f / fRatio;
        fKS             = fAttackThresh * fKnee;     // Knee start
        fKE             = fAttackThresh / fKnee;     // Knee end
        float th        = fAttackThresh;             // Attack threshold

        fXRatio         = ratio;
        float log_ks    = logf(fKS);
        float log_ke    = logf(fKE);
        fLogTH          = logf(th);

        if (bUpward)
            interpolation::hermite_quadratic(vHermite, log_ke, log_ke, 1.0f, log_ks, fXRatio);
        else
            interpolation::hermite_quadratic(vHermite, log_ks, log_ks, 1.0f, log_ke, fXRatio);

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
        if (bUpward)
        {
            for (size_t i=0; i<dots; ++i)
            {
                float x     = *(in++);
                if (x < 0.0f)
                    x       = -x;
                if (x < FLOAT_SAT_N_INF)
                    x       = FLOAT_SAT_N_INF;

                if (x <= fKE)
                {
                    float lx    = logf(x);
                    *out = (x < fKS) ?
                            expf(fXRatio*(lx - fLogTH) + fLogTH) :
                            expf((vHermite[0]*lx + vHermite[1])*lx + vHermite[2]);
                }
                else
                    *out        = x;
                out++;
            }
        }
        else
        {
            for (size_t i=0; i<dots; ++i)
            {
                float x     = *(in++);
                if (x < 0.0f)
                    x       = -x;
                if (x >= fKS)
                {
                    float lx    = logf(x);
                    x   = (x > fKE) ?
                        expf(fXRatio*(lx - fLogTH) + fLogTH) :
                        expf((vHermite[0]*lx + vHermite[1])*lx + vHermite[2]);
                    *out        = x;
                }
                else
                    *out        = x;
                out++;
            }
        }
    }

    float Compressor::curve(float in)
    {
        if (in < 0.0f)
            in      = -in;
        if (bUpward)
        {
            if (in < FLOAT_SAT_N_INF)
                in      = FLOAT_SAT_N_INF;

            if (in <= fKE)
            {
                float lx    = logf(in);
                return (in < fKS) ?
                    expf(fXRatio*(lx - fLogTH) + fLogTH) :
                    expf((vHermite[0]*lx + vHermite[1])*lx + vHermite[2]);
            }
        }
        else
        {
            if (in >= fKS)
            {
                float lx    = logf(in);
                return (in > fKE) ?
                    expf(fXRatio*(lx - fLogTH) + fLogTH) :
                    expf((vHermite[0]*lx + vHermite[1])*lx + vHermite[2]);
            }
        }

        return in;
    }

    void Compressor::reduction(float *out, const float *in, size_t dots)
    {
        if (bUpward)
        {
            for (size_t i=0; i<dots; ++i)
            {
                float x     = *(in++);
                if (x < 0.0f)
                    x       = -x;
                if (x < FLOAT_SAT_N_INF)
                    x       = FLOAT_SAT_N_INF;

                if (x <= fKE)
                {
                    float lx    = logf(x);
                    x   = (x < fKS) ?
                            expf((fXRatio-1.0f)*(lx-fLogTH)) :
                            expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
                    *out        = x;
                }
                else
                    *out        = 1.0f;
                out++;
            }
        }
        else
        {
            for (size_t i=0; i<dots; ++i)
            {
                float x     = *(in++);
                if (x < 0.0f)
                    x       = -x;
                if (x >= fKS)
                {
                    float lx    = logf(x);
                    *out = (x > fKE) ?
                        expf((fXRatio-1.0f)*(lx-fLogTH)) :
                        expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
                }
                else
                    *out        = 1.0f;
                out++;
            }
        }
    }

    float Compressor::reduction(float in)
    {
        if (in < 0.0f)
            in      = -in;
        if (bUpward)
        {
            if (in < FLOAT_SAT_N_INF)
                in      = FLOAT_SAT_N_INF;

            if (in <= fKE)
            {
                float lx    = logf(in);
                return (in < fKS) ?
                    expf((fXRatio-1.0f)*(lx-fLogTH)) :
                    expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
            }
        }
        else
        {
            if (in >= fKS)
            {
                float lx    = logf(in);
                return (in > fKE) ?
                    expf((fXRatio-1.0f)*(lx-fLogTH)) :
                    expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
            }
        }

        return 1.0f;
    }
} /* namespace lsp */
