/*
 * Limiter.cpp
 *
 *  Created on: 25 нояб. 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/debug.h>
#include <core/interpolation.h>
#include <core/dynamics/Limiter.h>

#define BUF_GRANULARITY         8192
#define GAIN_LOWERING           0.891250938134 /* 0.944060876286 */
#define MIN_LIMITER_RELEASE     5.0f

namespace lsp
{
    Limiter::Limiter()
    {
        construct();
    }

    Limiter::~Limiter()
    {
        destroy();
    }

    void Limiter::construct()
    {
        fThreshold      = 1.0f;
        fLookahead      = 0.0f;
        fMaxLookahead   = 0.0f;
        fAttack         = 0.0f;
        fRelease        = 0.0f;
        fKnee           = GAIN_AMP_M_6_DB;
        nMaxLookahead   = 0;
        nLookahead      = 0;
        nMaxSampleRate  = 0;
        nSampleRate     = 0;
        nUpdate         = UP_ALL;
        nMode           = LM_HERM_THIN;
        nThresh         = 0;

        sALR.fAttack    = 10.0f;
        sALR.fRelease   = 50.0f;
        sALR.fEnvelope  = 0.0f;
        sALR.bEnable    = false;

        vGainBuf        = NULL;
        vTmpBuf         = NULL;
        vData           = NULL;
    }

    bool Limiter::init(size_t max_sr, float max_lookahead)
    {
        nMaxLookahead       = millis_to_samples(max_sr, max_lookahead);
        size_t alloc        = nMaxLookahead*4 + BUF_GRANULARITY*2;
        float *ptr          = alloc_aligned<float>(vData, alloc, DEFAULT_ALIGN);
        if (ptr == NULL)
            return false;

        vGainBuf            = ptr;
        ptr                += nMaxLookahead*4 + BUF_GRANULARITY;
        vTmpBuf             = ptr;
        ptr                += BUF_GRANULARITY;

        lsp_assert(reinterpret_cast<uint8_t *>(ptr) <= &vData[alloc*sizeof(float) + DEFAULT_ALIGN]);

        dsp::fill_one(vGainBuf, nMaxLookahead*4 + BUF_GRANULARITY);
        dsp::fill_zero(vTmpBuf, BUF_GRANULARITY);

        if (!sDelay.init(nMaxLookahead + BUF_GRANULARITY))
            return false;

        nMaxSampleRate      = max_sr;
        fMaxLookahead       = max_lookahead;
        return true;
    }

    void Limiter::destroy()
    {
        sDelay.destroy();

        if (vData != NULL)
        {
            free_aligned(vData);
            vData = NULL;
        }

        vGainBuf    = NULL;
        vTmpBuf     = NULL;
    }

    float Limiter::set_attack(float attack)
    {
        float old = fAttack;
        if (attack == old)
            return old;

        fAttack         = attack;
        nUpdate        |= UP_OTHER;
        return old;
    }

    float Limiter::set_release(float release)
    {
        float old = fRelease;
        if (release == old)
            return old;

        fRelease        = release;
        nUpdate        |= UP_OTHER;
        return old;
    }

    float Limiter::set_threshold(float thresh)
    {
        float old = fThreshold;
        if (old == thresh)
            return old;

        fThreshold      = thresh;
        nUpdate        |= UP_THRESH | UP_ALR;
        return old;
    }

    float Limiter::set_lookahead(float lk_ahead)
    {
        float old = fLookahead;
        if (lk_ahead > fMaxLookahead)
            lk_ahead = fMaxLookahead;
        if (old == lk_ahead)
            return old;

        fLookahead      = lk_ahead;
        nUpdate        |= UP_LK;

        return old;
    }

    float Limiter::set_knee(float knee)
    {
        float old = fKnee;
        if (knee > 1.0f)
            knee = 1.0f;
        if (old == knee)
            return old;

        fKnee           = knee;
        nUpdate        |= UP_OTHER;
        return old;
    }

    float Limiter::set_alr_attack(float attack)
    {
        float old = sALR.fAttack;
        if (attack == old)
            return old;

        sALR.fAttack    = attack;
        nUpdate        |= UP_ALR;
        return old;
    }

    float Limiter::set_alr_release(float release)
    {
        float old = sALR.fRelease;
        if (release == old)
            return old;

        sALR.fRelease   = release;
        nUpdate        |= UP_ALR;
        return old;
    }

    bool Limiter::set_alr(bool enable)
    {
        bool old        = sALR.bEnable;
        sALR.bEnable    = enable;
        if (!enable)
            sALR.fEnvelope  = 0.0f;
        return old;
    }

    void Limiter::reset_sat(sat_t *sat)
    {
        sat->nAttack        = 0;
        sat->nPlane         = 0;
        sat->nRelease       = 0;
        sat->nMiddle        = 0;

        sat->vAttack[0]     = 0.0f;
        sat->vAttack[1]     = 0.0f;
        sat->vAttack[2]     = 0.0f;
        sat->vAttack[3]     = 0.0f;
        sat->vRelease[0]    = 0.0f;
        sat->vRelease[1]    = 0.0f;
        sat->vRelease[2]    = 0.0f;
        sat->vRelease[3]    = 0.0f;
    }

    void Limiter::reset_exp(exp_t *exp)
    {
        exp->nAttack        = 0;
        exp->nPlane         = 0;
        exp->nRelease       = 0;
        exp->nMiddle        = 0;

        exp->vAttack[0]     = 0.0f;
        exp->vAttack[1]     = 0.0f;
        exp->vAttack[2]     = 0.0f;
        exp->vAttack[3]     = 0.0f;
        exp->vRelease[0]    = 0.0f;
        exp->vRelease[1]    = 0.0f;
        exp->vRelease[2]    = 0.0f;
        exp->vRelease[3]    = 0.0f;
    }

    void Limiter::reset_line(line_t *line)
    {
        line->nAttack       = 0;
        line->nPlane        = 0;
        line->nRelease      = 0;
        line->nMiddle       = 0;

        line->vAttack[0]    = 0.0f;
        line->vAttack[1]    = 0.0f;
        line->vRelease[0]   = 0.0f;
        line->vRelease[1]   = 0.0f;
    }

    void Limiter::init_sat(sat_t *sat)
    {
        ssize_t attack      = millis_to_samples(nSampleRate, fAttack);
        ssize_t release     = millis_to_samples(nSampleRate, fRelease);
        if (attack > ssize_t(nLookahead))
            attack              = nLookahead;
        else if (attack < 8)
            attack              = 8;
        if (release > ssize_t(nLookahead*2))
            release             = nLookahead*2;
        else if (release < 8)
            release             = 8;

        if (nMode == LM_HERM_THIN)
        {
            sat->nAttack        = attack;
            sat->nPlane         = attack;
        }
        else if (nMode == LM_HERM_TAIL)
        {
            sat->nAttack        = (attack >> 1);
            sat->nPlane         = attack;
        }
        else if (nMode == LM_HERM_DUCK)
        {
            sat->nAttack        = attack;
            sat->nPlane         = (attack + (release >> 1));
        }
        else // LM_HERM_WIDE by default
        {
            sat->nAttack        = (attack >> 1);
            sat->nPlane         = (attack + (release >> 1));
        }

        sat->nRelease       = attack + release + 1;
        sat->nMiddle        = attack;

        interpolation::hermite_cubic(sat->vAttack, -1.0f, 0.0f, 0.0f, sat->nAttack, 1.0f, 0.0f);
        interpolation::hermite_cubic(sat->vRelease, sat->nPlane, 1.0f, 0.0f, sat->nRelease, 0.0f, 0.0f);
    }

    void Limiter::init_exp(exp_t *exp)
    {
        ssize_t attack      = millis_to_samples(nSampleRate, fAttack);
        ssize_t release     = millis_to_samples(nSampleRate, fRelease);
        if (attack > ssize_t(nLookahead))
            attack              = nLookahead;
        else if (attack < 8)
            attack              = 8;
        if (release > ssize_t(nLookahead*2))
            release             = nLookahead*2;
        else if (release < 8)
            release             = 8;

        if (nMode == LM_EXP_THIN)
        {
            exp->nAttack        = attack;
            exp->nPlane         = attack;
        }
        else if (nMode == LM_EXP_TAIL)
        {
            exp->nAttack        = (attack >> 1);
            exp->nPlane         = attack;
        }
        else if (nMode == LM_EXP_DUCK)
        {
            exp->nAttack        = attack;
            exp->nPlane         = (attack + (release >> 1));
        }
        else // LM_EXP_WIDE by default
        {
            exp->nAttack        = (attack >> 1);
            exp->nPlane         = (attack + (release >> 1));
        }

        exp->nRelease       = attack + release + 1;
        exp->nMiddle        = attack;

        interpolation::exponent(exp->vAttack, -1.0f, 0.0f, exp->nAttack, 1.0f, 2.0f / attack);
        interpolation::exponent(exp->vRelease, exp->nPlane, 1.0f, exp->nRelease, 0.0f, 2.0f / release);
    }

    void Limiter::init_line(line_t *line)
    {
        ssize_t attack      = millis_to_samples(nSampleRate, fAttack);
        ssize_t release     = millis_to_samples(nSampleRate, fRelease);
        if (attack > ssize_t(nLookahead))
            attack              = nLookahead;
        else if (attack < 8)
            attack              = 8;
        if (release > ssize_t(nLookahead*2))
            release             = nLookahead*2;
        else if (release < 8)
            release             = 8;

        if (nMode == LM_LINE_THIN)
        {
            line->nAttack       = attack;
            line->nPlane        = attack;
        }
        else if (nMode == LM_LINE_TAIL)
        {
            line->nAttack       = (attack >> 1);
            line->nPlane        = attack;
        }
        else if (nMode == LM_LINE_DUCK)
        {
            line->nAttack       = attack;
            line->nPlane        = (attack + (release >> 1));
        }
        else // LM_LINE_WIDE by default
        {
            line->nAttack       = (attack >> 1);
            line->nPlane        = (attack + (release >> 1));
        }

        line->nRelease      = attack + release + 1;
        line->nMiddle       = attack;

        interpolation::linear(line->vAttack, -1.0f, 0.0f, line->nAttack, 1.0f);
        interpolation::linear(line->vRelease, line->nPlane, 1.0f, line->nRelease, 0.0f);
    }

    void Limiter::update_settings()
    {
        if (nUpdate == 0)
            return;

        // Update delay settings
        if (nUpdate & UP_SR)
            sDelay.clear();
        if (nUpdate & UP_SR)
            dsp::fill_one(vGainBuf, nMaxLookahead*3 + BUF_GRANULARITY);

        nLookahead          = millis_to_samples(nSampleRate, fLookahead);
        sDelay.set_delay(nLookahead);

        if (nUpdate & (UP_SR | UP_MODE | UP_THRESH))
            nThresh             = nLookahead;

        // Update automatic level regulation
        if (nUpdate & UP_ALR)
        {
            float thresh        = fThreshold * GAIN_AMP_M_6_DB;
            sALR.fKS            = thresh * (M_SQRT2 - 1.0f);
            sALR.fKE            = thresh;
            sALR.fGain          = thresh * M_SQRT1_2;
            interpolation::hermite_quadratic(sALR.vHermite, sALR.fKS, sALR.fKS, 1.0f, thresh, 0.0f);

            float att           = millis_to_samples(nSampleRate, sALR.fAttack);
            float rel           = millis_to_samples(nSampleRate, sALR.fRelease);

            sALR.fTauAttack     = (att < 1.0f)  ? 1.0f : 1.0f - expf(logf(1.0f - M_SQRT1_2) / att);
            sALR.fTauRelease    = (rel < 1.0f)  ? 1.0f : 1.0f - expf(logf(1.0f - M_SQRT1_2) / rel);
        }

        // Check that mode change has triggered
        if (nUpdate & UP_MODE)
        {
            // Clear state for the limiter
            switch (nMode)
            {
                case LM_HERM_THIN:
                case LM_HERM_WIDE:
                case LM_HERM_TAIL:
                case LM_HERM_DUCK:
                    reset_sat(&sSat);
                    break;

                case LM_EXP_THIN:
                case LM_EXP_WIDE:
                case LM_EXP_TAIL:
                case LM_EXP_DUCK:
                    reset_exp(&sExp);
                    break;

                case LM_LINE_THIN:
                case LM_LINE_WIDE:
                case LM_LINE_TAIL:
                case LM_LINE_DUCK:
                    reset_line(&sLine);
                    break;

                default:
                    break;
            }
        }

        // Update state
        switch (nMode)
        {
            case LM_HERM_THIN:
            case LM_HERM_WIDE:
            case LM_HERM_TAIL:
            case LM_HERM_DUCK:
                init_sat(&sSat);
                break;

            case LM_EXP_THIN:
            case LM_EXP_WIDE:
            case LM_EXP_TAIL:
            case LM_EXP_DUCK:
                init_exp(&sExp);
                break;

            case LM_LINE_THIN:
            case LM_LINE_WIDE:
            case LM_LINE_TAIL:
            case LM_LINE_DUCK:
                init_line(&sLine);
                break;

            default:
                break;
        }

        // Clear the update flag
        nUpdate         = 0;
    }

    inline float Limiter::sat(ssize_t n)
    {
        if (n < sSat.nAttack)
        {
            if (n < 0)
                return 0.0f;
            float x = n;
            return (((sSat.vAttack[0]*x + sSat.vAttack[1])*x + sSat.vAttack[2])*x + sSat.vAttack[3]);
        }
        else if (n > sSat.nPlane)
        {
            if (n > sSat.nRelease)
                return 0.0f;

            float x = n;
            return (((sSat.vRelease[0]*x + sSat.vRelease[1])*x + sSat.vRelease[2])*x + sSat.vRelease[3]);
        }

        return 1.0f;
    }

    inline float Limiter::exp(ssize_t n)
    {
        if (n < sExp.nAttack)
        {
            if (n < 0)
                return 0.0f;
            return sExp.vAttack[0] + sExp.vAttack[1] * expf(sExp.vAttack[2] * n);
        }
        else if (n > sExp.nPlane)
        {
            if (n > sExp.nRelease)
                return 0.0f;

            return sExp.vRelease[0] + sExp.vRelease[1] * expf(sExp.vRelease[2] * n);
        }

        return 1.0f;
    }

    inline float Limiter::line(ssize_t n)
    {
        if (n < sLine.nAttack)
        {
            if (n < 0)
                return 0.0f;
            return sLine.vAttack[0] * n + sLine.vAttack[1];
        }
        else if (n > sLine.nPlane)
        {
            if (n > sLine.nRelease)
                return 0.0f;

            return sLine.vRelease[0] * n + sLine.vRelease[1];
        }

        return 1.0f;
    }

    void Limiter::apply_sat_patch(sat_t *sat, float *dst, float amp)
    {
        ssize_t t = 0;

        // Attack part
        while (t < sat->nAttack)
        {
            float x     = t++;
            *(dst++)   *= 1.0f - amp * (((sat->vAttack[0]*x + sat->vAttack[1])*x + sat->vAttack[2])*x + sat->vAttack[3]);
        }

        // Peak part
        while (t < sat->nPlane)
        {
            *(dst++)   *= 1.0f - amp;
            t++;
        }

        // Release part
        while (t < sat->nRelease)
        {
            float x     = t++;
            *(dst++)   *= 1.0f - amp * (((sat->vRelease[0]*x + sat->vRelease[1])*x + sat->vRelease[2])*x + sat->vRelease[3]);
        }
    }

    void Limiter::apply_exp_patch(exp_t *exp, float *dst, float amp)
    {
        ssize_t t = 0;

        // Attack part
        while (t < exp->nAttack)
            *(dst++)   *= 1.0f - amp * (exp->vAttack[0] + exp->vAttack[1] * expf(exp->vAttack[2] * (t++)));

        // Peak part
        while (t < exp->nPlane)
        {
            *(dst++)   *= 1.0f - amp;
            t++;
        }

        // Release part
        while (t < exp->nRelease)
            *(dst++)   *= 1.0f - amp * (exp->vRelease[0] + exp->vRelease[1] * expf(exp->vRelease[2] * (t++)));
    }

    void Limiter::apply_line_patch(line_t *line, float *dst, float amp)
    {
        ssize_t t = 0;

        // Attack part
        while (t < line->nAttack)
            *(dst++)   *= 1.0f - amp * (line->vAttack[0] * (t++) + line->vAttack[1]);

        // Peak part
        while (t < line->nPlane)
        {
            *(dst++)   *= 1.0f - amp;
            t++;
        }

        // Release part
        while (t < line->nRelease)
            *(dst++)   *= 1.0f - amp * (line->vRelease[0] * (t++) + line->vRelease[1]);
    }

    void Limiter::process_alr(float *gbuf, const float *sc, size_t samples)
    {
        for (size_t i=0; i<samples; ++i)
        {
            float d     = sc[i] - sALR.fEnvelope;
            float k     = (d > 0.0f) ? sALR.fTauAttack : sALR.fTauRelease;
            float e     = (sALR.fEnvelope += k * d);

            if (e <= sALR.fKS)
                gbuf[i]     = 1.0f;
            else
            {
                float dg    = (e >= sALR.fKE) ? sALR.fGain :
                              (sALR.vHermite[0]*e + sALR.vHermite[1])*e + sALR.vHermite[2];

                gbuf[i]  = dg / e;
            }
        }
    }

    void Limiter::process(float *dst, float *gain, const float *src, const float *sc, size_t samples)
    {
        // Force settings update if there are any
        update_settings();

        float *gbuf     = &vGainBuf[nMaxLookahead];

        while (samples > 0)
        {
            size_t to_do    = (samples > BUF_GRANULARITY) ? BUF_GRANULARITY : samples;

            // Fill gain buffer
            dsp::fill_one(&gbuf[nMaxLookahead*3], to_do);
            dsp::abs_mul3(vTmpBuf, gbuf, sc, to_do);    // Apply current gain buffer to the side chain signal
            if (sALR.bEnable) // Apply ALR if necessary
            {
                process_alr(gbuf, vTmpBuf, to_do);
                dsp::abs_mul3(vTmpBuf, gbuf, sc, to_do);    // Apply gain to sidechain
            }

            float knee          = fKnee;
            size_t iterations   = 0;

            while (true)
            {
                // Find peak
                ssize_t peak    = dsp::max_index(vTmpBuf, to_do);
                float s         = vTmpBuf[peak];
                if (s <= fThreshold) // No more peaks are present
                    break;

                // Apply patch to the gain buffer
                s           = (s - (fThreshold * knee - 0.000001))/ s;
                switch (nMode)
                {
                    case LM_HERM_THIN:
                    case LM_HERM_WIDE:
                    case LM_HERM_TAIL:
                    case LM_HERM_DUCK:
                        apply_sat_patch(&sSat, &gbuf[peak - sSat.nMiddle], s);
                        break;

                    case LM_EXP_THIN:
                    case LM_EXP_WIDE:
                    case LM_EXP_TAIL:
                    case LM_EXP_DUCK:
                        apply_exp_patch(&sExp, &gbuf[peak - sExp.nMiddle], s);
                        break;

                    case LM_LINE_THIN:
                    case LM_LINE_WIDE:
                    case LM_LINE_TAIL:
                    case LM_LINE_DUCK:
                        apply_line_patch(&sLine, &gbuf[peak - sLine.nMiddle], s);
                        break;

                    default:
                        break;
                }

                // Apply new gain to sidechain
                dsp::abs_mul3(vTmpBuf, gbuf, sc, to_do);    // Apply gain to sidechain

                // Lower the knee if necessary
                if (((++iterations) % LIMITER_PEAKS_MAX) == 0)
                    knee     *=       GAIN_LOWERING;
            }

            // Copy gain value and shift gain buffer
            dsp::copy(gain, &vGainBuf[nMaxLookahead - nLookahead], to_do);
            dsp::move(vGainBuf, &vGainBuf[to_do], nMaxLookahead*4);

            // Gain will be applied to the delayed signal
            sDelay.process(dst, src, to_do);

            // Decrement number of samples and update pointers
            dst            += to_do;
            gain           += to_do;
            src            += to_do;
            sc             += to_do;
            samples        -= to_do;
        }
    }

    void Limiter::dump(IStateDumper *v, const char *name, const sat_t *sat)
    {
        v->begin_object(name, sat, sizeof(sat_t));
        {
            v->write("nAttack", sat->nAttack);
            v->write("nPlane", sat->nPlane);
            v->write("nRelease", sat->nRelease);
            v->write("nMiddle", sat->nMiddle);
            v->writev("vAttack", sat->vAttack, 4);
            v->writev("vRelease", sat->vRelease, 4);
        }
        v->end_object();
    }

    void Limiter::dump(IStateDumper *v, const char *name, const exp_t *exp)
    {
        v->begin_object(name, exp, sizeof(exp_t));
        {
            v->write("nAttack", exp->nAttack);
            v->write("nPlane", exp->nPlane);
            v->write("nRelease", exp->nRelease);
            v->write("nMiddle", exp->nMiddle);
            v->writev("vAttack", exp->vAttack, 4);
            v->writev("vRelease", exp->vRelease, 4);
        }
        v->end_object();
    }

    void Limiter::dump(IStateDumper *v, const char *name, const line_t *line)
    {
        v->begin_object(name, line, sizeof(line_t));
        {
            v->write("nAttack", line->nAttack);
            v->write("nPlane", line->nPlane);
            v->write("nRelease", line->nRelease);
            v->write("nMiddle", line->nMiddle);
            v->writev("vAttack", line->vAttack, 2);
            v->writev("vRelease", line->vRelease, 2);
        }
        v->end_object();
    }

    void Limiter::dump(IStateDumper *v) const
    {
        v->write("fThreshold", fThreshold);
        v->write("fLookahead", fLookahead);
        v->write("fMaxLookahead", fMaxLookahead);
        v->write("fAttack", fAttack);
        v->write("fRelease", fRelease);
        v->write("fKnee", fKnee);
        v->write("nMaxLookahead", nMaxLookahead);
        v->write("nLookahead", nLookahead);
        v->write("nMaxSampleRate", nMaxSampleRate);
        v->write("nSampleRate", nSampleRate);
        v->write("nUpdate", nUpdate);
        v->write("nMode", nMode);
        v->write("nThresh", nThresh);
        v->begin_object("sALR", &sALR, sizeof(alr_t));
        {
            v->write("fKS", sALR.fKS);
            v->write("fKE", sALR.fKE);
            v->write("fGain", sALR.fGain);
            v->write("fTauAttack", sALR.fTauAttack);
            v->write("fTauRelease", sALR.fTauRelease);
            v->writev("vHermite", sALR.vHermite, 3);
            v->write("fAttack", sALR.fAttack);
            v->write("fRelease", sALR.fRelease);
            v->write("fEnvelope", sALR.fEnvelope);
            v->write("bEnabled", sALR.bEnable);
        }
        v->end_object();

        v->write("vGainBuf", vGainBuf);
        v->write("vTmpBuf", vTmpBuf);
        v->write("vData", vData);

        v->write_object("sDelay", &sDelay);

        switch (nMode)
        {
            case LM_HERM_THIN:
            case LM_HERM_WIDE:
            case LM_HERM_TAIL:
            case LM_HERM_DUCK:
                dump(v, "sSat", &sSat);
                break;

            case LM_EXP_THIN:
            case LM_EXP_WIDE:
            case LM_EXP_TAIL:
            case LM_EXP_DUCK:
                dump(v, "sExp", &sExp);
                break;

            case LM_LINE_THIN:
            case LM_LINE_WIDE:
            case LM_LINE_TAIL:
            case LM_LINE_DUCK:
                dump(v, "sLine", &sLine);
                break;

            default:
                break;
        }
    }

} /* namespace lsp */
