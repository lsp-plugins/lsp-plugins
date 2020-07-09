/*
 * DynamicProcessor.cpp
 *
 *  Created on: 19 окт. 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/dynamics/DynamicProcessor.h>
#include <core/interpolation.h>
#include <core/debug.h>
#include <core/units.h>
#include <math.h>

namespace lsp
{
    DynamicProcessor::DynamicProcessor()
    {
        fInRatio        = 1.0f;
        fOutRatio       = 1.0f;
        fEnvelope       = 0.0f;
        nSampleRate     = 0.0f;
        bUpdate         = true;

        for (size_t i=0; i<DYNAMIC_PROCESSOR_DOTS; ++i)
        {
            vDots[i].fInput     = 0.0f;
            vDots[i].fOutput    = 0.0f;
            vDots[i].fKnee      = 0.0f;

            vAttackLvl[i]       = 0.0f;
            vReleaseLvl[i]      = 0.0f;
        }

        for (size_t i=0; i<DYNAMIC_PROCESSOR_RANGES; ++i)
        {
            vAttackTime[i]      = 0.0f;
            vReleaseTime[i]     = 0.0f;
        }

        for (size_t i=0; i<CT_TOTAL; ++i)
            fCount[i]           = 0;
    }

    DynamicProcessor::~DynamicProcessor()
    {
    }

    inline float DynamicProcessor::spline_amp(const spline_t *s, float lx)
    {
        if (lx <= s->fKneeStart)
            lx = s->fMakeup + s->fPreRatio * (lx - s->fThresh);
        else if (lx >= s->fKneeStop)
            lx = s->fMakeup + s->fPostRatio * (lx - s->fThresh);
        else
            lx = (s->vHermite[0]*lx + s->vHermite[1])*lx + s->vHermite[2];

        return lx;
    }

    inline float DynamicProcessor::spline_model(const spline_t *s, float lx)
    {
        if (lx <= s->fThresh)
            lx      = s->fMakeup + s->fPreRatio * (lx - s->fThresh);
        else
            lx      = s->fMakeup + s->fPostRatio * (lx - s->fThresh);

        return lx;
    }

    inline float DynamicProcessor::solve_reaction(const reaction_t *s, float x, size_t count)
    {
        float r     = s[0].fTau;
        for (size_t i=1; i<count; ++i)
            if (x >= s[i].fLevel)
                r       = s[i].fTau;
        return r;
    }

    void DynamicProcessor::sort_reactions(reaction_t *s, size_t count)
    {
        // Sort
        for (size_t i=0; i<count-1; ++i)
        {
            for (size_t j=i+1; j<count; ++j)
            {
                if (s[j].fLevel < s[i].fLevel)
                {
                    // Swap
                    float tmp       = s[i].fLevel;
                    s[i].fLevel     = s[j].fLevel;
                    s[j].fLevel     = tmp;
                    tmp             = s[i].fTau;
                    s[i].fTau       = s[j].fTau;
                    s[j].fTau       = tmp;
                }
            }
        }

        // Now process tau
        for (size_t i=0; i<count; ++i)
            s[i].fTau       = 1.0f - expf(logf(1.0f - M_SQRT1_2) / (millis_to_samples(nSampleRate, s[i].fTau)));
    }

    void DynamicProcessor::sort_splines(spline_t *s, size_t count)
    {
        if (count == 0)
            return;

        // Sort
        for (size_t i=0; i<(count-1); ++i)
        {
            for (size_t j=i+1; j<count; ++j)
            {
                if (s[j].fThresh < s[i].fThresh)
                {
                    // Swap
                    float tmp       = s[i].fThresh;
                    s[i].fThresh    = s[j].fThresh;
                    s[j].fThresh    = tmp;
                    tmp             = s[i].fMakeup;
                    s[i].fMakeup    = s[j].fMakeup;
                    s[j].fMakeup    = tmp;
                    tmp             = s[i].fKneeStart;
                    s[i].fKneeStart = s[j].fKneeStart;
                    s[j].fKneeStart = tmp;
                }
            }
        }

        for (size_t i=0; i<count; ++i)
        {
            lsp_trace("i=%d, tr=%f, mk=%f, ks=%f",
                int(i), s[i].fThresh, s[i].fMakeup, s[i].fKneeStart);
        }

        // Now we are ready to render splines
        float sub = 0.0f;

        for (size_t i=0; i<count; ++i)
        {
            // Calculate input ratio
            s[i].fPreRatio      = (i == 0) ? fInRatio - 1.0f : 0.0f;

            if ((i+1) < count)
            {
                float dx            = logf(s[i+1].fThresh / s[i].fThresh);
                float dy            = logf(s[i+1].fMakeup / s[i].fMakeup);
                s[i].fPostRatio     = dy / dx - 1.0f;
            }
            else
                s[i].fPostRatio     = (1.0f / fOutRatio) - 1.0f;

            s[i].fPostRatio    -= sub;
            sub                += s[i].fPostRatio;

            float thresh        = logf(s[i].fThresh);
            float knee          = logf(s[i].fKneeStart);
            s[i].fThresh        = thresh;
            s[i].fKneeStop      = thresh - knee;
            s[i].fKneeStart     = thresh + knee;
            s[i].fMakeup        = (i == 0) ? logf(s[i].fMakeup) - thresh : 0.0f;

            float log_y1        = s[i].fMakeup + s[i].fPreRatio * knee;
            interpolation::hermite_quadratic(s[i].vHermite, s[i].fKneeStart, log_y1, s[i].fPreRatio, s[i].fKneeStop, s[i].fPostRatio);

            lsp_trace("i=%d, tr=%f, mk=%f, rs=%f, re=%f, ks=%f, ke=%f",
                int(i), s[i].fThresh, s[i].fMakeup,
                s[i].fPreRatio, s[i].fPostRatio,
                s[i].fKneeStart, s[i].fKneeStop);
        }
    }

    bool DynamicProcessor::set_dot(size_t id, const dyndot_t *src)
    {
        if (id >= DYNAMIC_PROCESSOR_DOTS)
            return false;

        dyndot_t *dst = &vDots[id];

        if (src == NULL)
        {
            bUpdate = bUpdate ||
                (dst->fInput    >= 0.0f) ||
                (dst->fOutput   >= 0.0f) ||
                (dst->fKnee     >= 0.0f);

            dst->fInput     = -1.0f;
            dst->fOutput    = -1.0f;
            dst->fKnee      = -1.0f;
        }
        else
        {
            bUpdate = bUpdate ||
                (dst->fInput    != src->fInput) ||
                (dst->fOutput   != src->fOutput) ||
                (dst->fKnee     != src->fKnee);

            dst->fInput     = src->fInput;
            dst->fOutput    = src->fOutput;
            dst->fKnee      = src->fKnee;
        }

        return true;
    }

    bool DynamicProcessor::set_dot(size_t id, float in, float out, float knee)
    {
        if (id >= DYNAMIC_PROCESSOR_DOTS)
            return false;

        dyndot_t *dst = &vDots[id];

        bUpdate = bUpdate ||
            (dst->fInput    != in) ||
            (dst->fOutput   != out) ||
            (dst->fKnee     != knee);

        dst->fInput     = in;
        dst->fOutput    = out;
        dst->fKnee      = knee;

        return true;
    }

    void DynamicProcessor::update_settings()
    {
        // Initialize counters
        fCount[CT_SPLINES]  = 0;
        fCount[CT_ATTACK]   = 1;
        fCount[CT_RELEASE]  = 1;

        // Initialize structure
        vAttack[0].fLevel   = 0.0f;
        vAttack[0].fTau     = vAttackTime[0];
        vRelease[0].fLevel  = 0.0f;
        vRelease[0].fTau    = vReleaseTime[0];

        // Process attack and release
        for (size_t i=0; i<DYNAMIC_PROCESSOR_DOTS; ++i)
        {
            if (vAttackLvl[i] >= 0.0f)
            {
                size_t idx              = fCount[CT_ATTACK]++;
                vAttack[idx].fLevel     = vAttackLvl[i];
                vAttack[idx].fTau       = vAttackTime[i+1];
            }
            if (vReleaseLvl[i] >= 0.0f)
            {
                size_t idx              = fCount[CT_RELEASE]++;
                vRelease[idx].fLevel    = vReleaseLvl[i];
                vRelease[idx].fTau      = vReleaseTime[i+1];
            }
        }

        // Build spline list
        spline_t *s     = vSplines;
        for (size_t i=0; i<DYNAMIC_PROCESSOR_DOTS; ++i)
        {
            // Get dot
            dyndot_t *d     = &vDots[i];
            if ((d->fInput < 0) || (d->fOutput < 0) || (d->fKnee < 0))
                continue;

            // Store dot in the list
            s->fThresh      = d->fInput;
            s->fMakeup      = d->fOutput;
            s->fKneeStart   = d->fKnee;

            // Update counter and pointers
            fCount[CT_SPLINES]++;
            s++;
        }

        // Sort reactions and splines
        sort_reactions(vAttack, fCount[CT_ATTACK]);
        sort_reactions(vRelease, fCount[CT_RELEASE]);
        sort_splines(vSplines, fCount[CT_SPLINES]);
    }

    void DynamicProcessor::process(float *out, float *env, const float *in, size_t samples)
    {
        // Calculate envelope of compressor
        for (size_t i=0; i<samples; ++i)
        {
            float s     = *(in++);

            fEnvelope  += (s > fEnvelope) ?
                    (s - fEnvelope) * solve_reaction(vAttack, fEnvelope, fCount[CT_ATTACK]) :
                    (s - fEnvelope) * solve_reaction(vRelease, fEnvelope, fCount[CT_RELEASE]);

            out[i]      = fEnvelope;
        }

        // Copy envelope to array if specified
        if (env != NULL)
            dsp::copy(env, out, samples);

        // Now calculate compressor's curve
        reduction(out, out, samples);
    }

    float DynamicProcessor::process(float *env, float in)
    {
        fEnvelope  += (in > fEnvelope) ?
                (in - fEnvelope) * solve_reaction(vAttack, fEnvelope, fCount[CT_ATTACK]) :
                (in - fEnvelope) * solve_reaction(vRelease, fEnvelope, fCount[CT_RELEASE]);

        if (env != NULL)
            *env    = fEnvelope;

        return reduction(fEnvelope);
    }

    void DynamicProcessor::curve(float *out, const float *in, size_t dots)
    {
        size_t splines  = fCount[CT_SPLINES];

        for (size_t i=0; i<dots; ++i)
        {
            float x     = *(in++);
            if (x < 0.0f)
                x       = -x;
            if (x < FLOAT_SAT_N_INF)
                x       = FLOAT_SAT_N_INF;
            else if (x > FLOAT_SAT_P_INF)
                x       = FLOAT_SAT_P_INF;

            float lx    = logf(x);
            float gain  = 0.0f;

            for (size_t j=0; j<splines; ++j)
                gain       += spline_amp(&vSplines[j], lx);

            *(out++)    = expf(gain) * x;
        }
    }

    float DynamicProcessor::curve(float in)
    {
        size_t splines  = fCount[CT_SPLINES];

        if (in < 0.0f)
            in      = -in;
        if (in < FLOAT_SAT_N_INF)
            in      = FLOAT_SAT_N_INF;
        else if (in > FLOAT_SAT_P_INF)
            in      = FLOAT_SAT_P_INF;

        float lx    = logf(in);
        float gain  = 0.0f;

        for (size_t j=0; j<splines; ++j)
            gain           += spline_amp(&vSplines[j], lx);

        return expf(gain) * in;
    }

    void DynamicProcessor::model(float *out, const float *in, size_t dots)
    {
        size_t splines  = fCount[CT_SPLINES];

        for (size_t i=0; i<dots; ++i)
        {
            float x     = *(in++);
            if (x < 0.0f)
                x       = -x;
            if (x < FLOAT_SAT_N_INF)
                x       = FLOAT_SAT_N_INF;
            else if (x > FLOAT_SAT_P_INF)
                x       = FLOAT_SAT_P_INF;

            float lx    = logf(x);
            float gain  = 0.0f;

            for (size_t j=0; j<splines; ++j)
                gain       += spline_model(&vSplines[j], lx);

            *(out++)    = expf(gain) * x;
        }
    }

    float DynamicProcessor::model(float in)
    {
        size_t splines  = fCount[CT_SPLINES];

        if (in < 0.0f)
            in      = -in;
        if (in < FLOAT_SAT_N_INF)
            in      = FLOAT_SAT_N_INF;
        else if (in > FLOAT_SAT_P_INF)
            in      = FLOAT_SAT_P_INF;

        float lx    = logf(in);
        float gain  = 0.0f;

        for (size_t j=0; j<splines; ++j)
            gain           += spline_model(&vSplines[j], lx);

        return expf(gain) * in;
    }

    void DynamicProcessor::reduction(float *out, const float *in, size_t dots)
    {
        size_t splines  = fCount[CT_SPLINES];

        for (size_t i=0; i<dots; ++i)
        {
            float x     = *(in++);
            if (x < 0.0f)
                x       = -x;
            if (x < GAIN_AMP_MIN)
                x       = GAIN_AMP_MIN;
            else if (x > FLOAT_SAT_P_INF)
                x       = FLOAT_SAT_P_INF;

            float lx    = logf(x);
            float gain  = 0.0f;

            for (size_t j=0; j<splines; ++j)
                gain       += spline_amp(&vSplines[j], lx);

            *(out++)    = expf(gain);
        }
    }

    float DynamicProcessor::reduction(float in)
    {
        size_t splines  = fCount[CT_SPLINES];

        if (in < 0.0f)
            in      = -in;
        if (in < FLOAT_SAT_N_INF)
            in      = FLOAT_SAT_N_INF;
        else if (in > FLOAT_SAT_P_INF)
            in      = FLOAT_SAT_P_INF;

        float lx    = logf(in);
        float gain  = 0.0f;

        for (size_t j=0; j<splines; ++j)
            gain       += spline_amp(&vSplines[j], lx);

        return expf(gain);
    }

    void DynamicProcessor::dump(IStateDumper *v) const
    {
        v->start_array("vDots", vDots, DYNAMIC_PROCESSOR_DOTS);
        for (size_t i=0; i<DYNAMIC_PROCESSOR_DOTS; ++i)
        {
            const dyndot_t *dot = &vDots[i];
            v->start_object(dot, sizeof(dyndot_t));
            {
                v->write("fInput", dot->fInput);
                v->write("fOutput", dot->fOutput);
                v->write("fKnee", dot->fKnee);
            }
            v->end_object();
        }
        v->end_array();

        v->writev("vAttackLvl", vAttackLvl, DYNAMIC_PROCESSOR_DOTS);
        v->writev("vReleaseLvl", vReleaseLvl, DYNAMIC_PROCESSOR_DOTS);
        v->writev("vAttackTime", vAttackTime, DYNAMIC_PROCESSOR_RANGES);
        v->writev("vReleaseTime", vReleaseTime, DYNAMIC_PROCESSOR_RANGES);

        v->write("fInRatio", fInRatio);
        v->write("fOutRatio", fOutRatio);

        v->start_array("vSplines", vSplines, DYNAMIC_PROCESSOR_DOTS);
        for (size_t i=0; i<DYNAMIC_PROCESSOR_DOTS; ++i)
        {
            const spline_t *s = &vSplines[i];
            v->start_object(s, sizeof(spline_t));
            {
                v->write("fPreRatio", s->fPreRatio);
                v->write("fPostRatio", s->fPostRatio);
                v->write("fKneeStart", s->fKneeStart);
                v->write("fKneeStop", s->fKneeStop);
                v->write("fThresh", s->fThresh);
                v->write("fMakeup", s->fMakeup);
                v->writev("vHermite", s->vHermite, 4);
            }
            v->end_object();
        }
        v->end_array();

        v->start_array("vAttack", vAttack, DYNAMIC_PROCESSOR_RANGES);
        for (size_t i=0; i<DYNAMIC_PROCESSOR_RANGES; ++i)
        {
            const reaction_t *r = &vAttack[i];
            v->start_object(r, sizeof(reaction_t));
            {
                v->write("fLevel", r->fLevel);
                v->write("fTau", r->fTau);
            }
            v->end_object();
        }
        v->end_array();

        v->start_array("vRelease", vRelease, DYNAMIC_PROCESSOR_RANGES);
        for (size_t i=0; i<DYNAMIC_PROCESSOR_RANGES; ++i)
        {
            const reaction_t *r = &vRelease[i];
            v->start_object(r, sizeof(reaction_t));
            {
                v->write("fLevel", r->fLevel);
                v->write("fTau", r->fTau);
            }
            v->end_object();
        }
        v->end_array();

        v->write("fEnvelope", fEnvelope);
        v->write("nSampleRate", nSampleRate);
        v->write("bUpdate", bUpdate);
    }

} /* namespace lsp */
