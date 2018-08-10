/*
 * Expander.cpp
 *
 *  Created on: 2 нояб. 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/interpolation.h>
#include <core/dynamics/Expander.h>
#include <math.h>

namespace lsp
{
    
    Expander::Expander()
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
        fLogKS          = 0.0f;
        fLogKE          = 0.0f;
        fLogTH          = 0.0f;

        // Additional parameters
        nSampleRate     = 0;
        bUpdate         = true;
        bUpward         = true;
    }
    
    Expander::~Expander()
    {
    }

    void Expander::update_settings()
    {
        // Update settings if necessary
        fTauAttack      = 1.0f - expf(logf(1.0f - M_SQRT1_2) / (millis_to_samples(nSampleRate, fAttack)));
        fTauRelease     = 1.0f - expf(logf(1.0f - M_SQRT1_2) / (millis_to_samples(nSampleRate, fRelease)));

        // Calculate interpolation parameters
        fLogKS          = logf(fAttackThresh * fKnee);      // Knee start
        fLogKE          = logf(fAttackThresh / fKnee);      // Knee end
        fLogTH          = logf(fAttackThresh);              // Attack threshold

        if (bUpward)
            interpolation::hermite_quadratic(vHermite, fLogKS, fLogKS, 1.0f, fLogKE, fRatio);
        else
            interpolation::hermite_quadratic(vHermite, fLogKE, fLogKE, 1.0f, fLogKS, fRatio);

        // Reset update flag
        bUpdate         = false;
    }

    void Expander::process(float *out, float *env, const float *in, size_t samples)
    {
        // Calculate envelope of expander
        for (size_t i=0; i<samples; ++i)
        {
            float s         = *(in++);

            fEnvelope       += (s > fEnvelope) ? fTauAttack * (s - fEnvelope) : fTauRelease * (s - fEnvelope);
            out[i]          = fEnvelope;
        }

        // Copy envelope to array if specified
        if (env != NULL)
            dsp::copy(env, out, samples);

        // Now calculate expander curve
        amplification(out, out, samples);
    }

    float Expander::process(float *env, float s)
    {
        if (fEnvelope > fReleaseThresh)
            fEnvelope       += (s > fEnvelope) ? fTauAttack * (s - fEnvelope) : fTauRelease * (s - fEnvelope);
        else
            fEnvelope       += fTauAttack * (s - fEnvelope);

        if (env != NULL)
            *env    = fEnvelope;

        return amplification(fEnvelope);
    }

    void Expander::curve(float *out, const float *in, size_t dots)
    {
        if (bUpward)
        {
            for (size_t i=0; i<dots; ++i)
            {
                float x     = *(in++);
                if (x < 0.0f)
                    x       = -x;
                if (x > FLOAT_SAT_P_INF)
                    x       = FLOAT_SAT_P_INF;

                float lx    = logf(x);
                if (lx >= fLogKS)
                {
                    x   = (lx > fLogKE) ?
                        expf(fRatio*(lx - fLogTH) + fLogTH) :
                        expf((vHermite[0]*lx + vHermite[1])*lx + vHermite[2]);
                    *out        = x;
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

                float lx    = logf(x);
                if (lx <= fLogKE)
                {
                    x   = (lx < fLogKS) ?
                        expf(fRatio*(lx - fLogTH) + fLogTH) :
                        expf((vHermite[0]*lx + vHermite[1])*lx + vHermite[2]);
                    *out        = x;
                }
                else
                    *out        = x;
                out++;
            }
        }
    }

    float Expander::curve(float in)
    {
        if (in < 0.0f)
            in      = -in;

        if (bUpward)
        {
            if (in > FLOAT_SAT_P_INF)
                in      = FLOAT_SAT_P_INF;

            float lx    = logf(in);
            if (lx >= fLogKS)
                in = (lx > fLogKE) ?
                    expf(fRatio*(lx - fLogTH) + fLogTH) :
                    expf((vHermite[0]*lx + vHermite[1])*lx + vHermite[2]);
        }
        else
        {
            float lx    = logf(in);
            if (lx <= fLogKE)
                in = (lx < fLogKS) ?
                    expf(fRatio*(lx - fLogTH) + fLogTH) :
                    expf((vHermite[0]*lx + vHermite[1])*lx + vHermite[2]);
        }

        return in;
    }

    void Expander::amplification(float *out, const float *in, size_t dots)
    {
        if (bUpward)
        {
            for (size_t i=0; i<dots; ++i)
            {
                float x     = *(in++);
                if (x < 0.0f)
                    x       = -x;
                if (x > FLOAT_SAT_P_INF)
                    x       = FLOAT_SAT_P_INF;

                float lx    = logf(x);
                if (lx >= fLogKS)
                {
                    *out    = (lx > fLogKE) ?
                        expf((fRatio - 1.0f)*(lx - fLogTH)) :
                        expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
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

                float lx    = logf(x);
                if (lx <= fLogKE)
                {
                    *out    = (lx < fLogKS) ?
                        expf((fRatio - 1.0f)*(lx - fLogTH)) :
                        expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
                }
                else
                    *out        = 1.0f;
                out++;
            }
        }
    }

    float Expander::amplification(float in)
    {
        if (in < 0.0f)
            in      = -in;

        if (bUpward)
        {
            if (in > FLOAT_SAT_P_INF)
                in      = FLOAT_SAT_P_INF;

            float lx    = logf(in);
            if (lx >= fLogKS)
                return (lx > fLogKE) ?
                    expf((fRatio - 1.0f)*(lx - fLogTH)) :
                    expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);

        }
        else
        {
            float lx    = logf(in);
            if (lx <= fLogKE)
                return (lx < fLogKS) ?
                    expf((fRatio - 1.0f)*(lx - fLogTH)) :
                    expf((vHermite[0]*lx + vHermite[1] - 1.0f)*lx + vHermite[2]);
        }

        return 1.0f;
    }

} /* namespace lsp */
