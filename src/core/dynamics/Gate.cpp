/*
 * Gate.cpp
 *
 *  Created on: 7 нояб. 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/interpolation.h>
#include <core/debug.h>
#include <core/dynamics/Gate.h>
#include <math.h>

namespace lsp
{
    
    Gate::Gate()
    {
        for (size_t i=0; i<2; ++i)
        {
            curve_t *c = &sCurves[i];

            c->fThreshold   = 0.0f;
            c->fZone        = 1.0f;
            c->fZS          = 0.0f;
            c->fZE          = 0.0f;
            c->fLogZS       = 0.0f;
            c->fLogZE       = 0.0f;
            c->vHermite[0]  = 0.0f;
            c->vHermite[1]  = 0.0f;
            c->vHermite[2]  = 0.0f;
            c->vHermite[3]  = 0.0f;
        }

        fAttack         = 0.0f;
        fRelease        = 0.0f;
        fTauAttack      = 0.0f;
        fTauRelease     = 0.0f;
        fReduction      = 0.0f;
        fEnvelope       = 0.0f;

        // Additional parameters
        nSampleRate     = 0;
        nCurve          = 0;
        bUpdate         = true;
    }
    
    Gate::~Gate()
    {
    }

    void Gate::update_settings()
    {
        // Update settings if necessary
        fTauAttack      = 1.0f - expf(logf(1.0f - M_SQRT1_2) / (millis_to_samples(nSampleRate, fAttack)));
        fTauRelease     = 1.0f - expf(logf(1.0f - M_SQRT1_2) / (millis_to_samples(nSampleRate, fRelease)));

        // Calculate interpolation parameters
        for (size_t i=0; i<2; ++i)
        {
            curve_t *c      = &sCurves[i];

            c->fZS          = c->fThreshold * c->fZone;
            c->fZE          = c->fThreshold;
            c->fLogZS       = logf(c->fZS);
            c->fLogZE       = logf(c->fZE);

//            if ((c->fZE - c->fZS) > 0.97f)
//            {
//                c->vHermite[0] = 0.0f;
//                c->vHermite[1] = 0.0f;
//                c->vHermite[2] = (c->fLogZE - logf(fReduction))/(c->fLogZE - c->fLogZS);
//                c->vHermite[3] = c->fLogZE;
//            }
//            else
//            {
                interpolation::hermite_cubic(
                        c->vHermite,
                        c->fLogZS, c->fLogZS + logf(fReduction), 1.0f,
                        c->fLogZE, c->fLogZE, 1.0f
                    );
//            }
        }

        // Reset update flag
        bUpdate         = false;
    }

    void Gate::curve(float *out, const float *in, size_t dots, bool hyst)
    {
        curve_t *c      = &sCurves[(hyst) ? 1 : 0];

        for (size_t i=0; i<dots; ++i)
        {
            float x     = *(in++);
            if (x < 0.0f)
                x       = -x;

            if (x > c->fZS)
            {
                if (x < c->fZE)
                {
                    float lx    = logf(x);
                    x           = expf(((c->vHermite[0]*lx + c->vHermite[1])*lx + c->vHermite[2])*lx + c->vHermite[3]);
                }
                *out    = x;
            }
            else
                *out    = fReduction * x;
            out++;
        }
    }

    float Gate::curve(float in, bool hyst)
    {
        curve_t *c      = &sCurves[(hyst) ? 1 : 0];
        if (in < 0.0f)
            in      = -in;

        if (in > c->fZS)
        {
            if (in < c->fZE)
            {
                float lx    = logf(in);
                in      = expf(((c->vHermite[0]*lx + c->vHermite[1])*lx + c->vHermite[2])*lx + c->vHermite[3]);
            }
            return in;
        }

        return fReduction * in;
    }

    void Gate::process(float *out, float *env, const float *in, size_t samples)
    {
        // Calculate envelope of gate
        for (size_t i=0; i<samples; ++i)
        {
            float s         = *(in++);

            fEnvelope       += (s > fEnvelope) ? fTauAttack * (s - fEnvelope) : fTauRelease * (s - fEnvelope);

            // Update result
            if (env != NULL)
                env[i]          = fEnvelope;
            out[i]          = amplification(fEnvelope);

            // Change state
            curve_t *c      = &sCurves[nCurve];
            if (fEnvelope > c->fZS)
            {
                if (fEnvelope < c->fZE)
                {
                    float lx    = logf(fEnvelope);
                    out[i]      = expf(((c->vHermite[0]*lx + c->vHermite[1])*lx + c->vHermite[2] - 1.0f)*lx + c->vHermite[3]);
                }
                else
                {
                    nCurve      = 1;
                    out[i]      = 1.0f;
                }
            }
            else
            {
                nCurve      = 0;
                out[i]      = fReduction;
            }
        }
    }

    float Gate::process(float *env, float s)
    {
        curve_t *c      = &sCurves[nCurve];

        fEnvelope      += (s > fEnvelope) ? fTauAttack * (s - fEnvelope) : fTauRelease * (s - fEnvelope);
        s               = amplification(fEnvelope);

        if (fEnvelope > c->fZE)
            nCurve          = 1;
        else if (fEnvelope < c->fZS)
            nCurve          = 0;

        if (env != NULL)
            *env = fEnvelope;

        return s;
    }

    void Gate::amplification(float *out, const float *in, size_t dots, bool hyst)
    {
        curve_t *c      = &sCurves[(hyst) ? 1 : 0];

        for (size_t i=0; i<dots; ++i)
        {
            float x     = *(in++);
            if (x < 0.0f)
                x       = -x;

            if (x > c->fZS)
            {
                if (x < c->fZE)
                {
                    float lx    = logf(x);
                    *out        = expf(((c->vHermite[0]*lx + c->vHermite[1])*lx + c->vHermite[2] - 1.0f)*lx + c->vHermite[3]);
                }
                else
                    *out    = 1.0f;
            }
            else
                *out    = fReduction;
            out++;
        }
    }

    float Gate::amplification(float in)
    {
        curve_t *c      = &sCurves[nCurve];

        if (in < 0.0f)
            in          = -in;

        if (in > c->fZS)
        {
            if (in < c->fZE)
            {
                float lx    = logf(in);
                return expf(((c->vHermite[0]*lx + c->vHermite[1])*lx + c->vHermite[2] - 1.0f)*lx + c->vHermite[3]);
            }
            return 1.0f;
        }

        return fReduction;
    }

    float Gate::amplification(float in, bool hyst)
    {
        curve_t *c      = &sCurves[(hyst) ? 1 : 0];

        if (in < 0.0f)
            in          = -in;

        if (in > c->fZS)
        {
            if (in < c->fZE)
            {
                float lx    = logf(in);
                return expf(((c->vHermite[0]*lx + c->vHermite[1])*lx + c->vHermite[2] - 1.0f)*lx + c->vHermite[3]);
            }
            return 1.0f;
        }

        return fReduction;
    }

} /* namespace lsp */
