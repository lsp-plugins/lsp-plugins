/*
 * Limiter.cpp
 *
 *  Created on: 25 нояб. 2016 г.
 *      Author: sadko
 */

#include <core/dsp.h>
#include <core/debug.h>
#include <core/interpolation.h>
#include <core/Limiter.h>

#define BUF_GRANULARITY         8192
#define GAIN_LOWERING           0.891250938134 /* 0.944060876286 */

namespace lsp
{
    Limiter::Limiter()
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
        nMode           = LM_COMPRESSOR;
        nThresh         = 0;
        vGainBuf        = NULL;
        vTmpBuf         = NULL;
    }

    Limiter::~Limiter()
    {
    }

    bool Limiter::init(size_t max_sr, float max_lookahead)
    {
        nMaxLookahead       = millis_to_samples(max_sr, max_lookahead);
        vGainBuf            = new float[nMaxLookahead*4 + BUF_GRANULARITY];
        if (vGainBuf == NULL)
            return false;

        vTmpBuf             = new float[BUF_GRANULARITY];
        if (vTmpBuf == NULL)
            return false;

        if (!sDelay.init(nMaxLookahead + BUF_GRANULARITY))
            return false;

        nMaxSampleRate      = max_sr;
        fMaxLookahead       = max_lookahead;
        return true;
    }

    void Limiter::destroy()
    {
        sDelay.destroy();

        if (vGainBuf != NULL)
        {
            delete [] vGainBuf;
            vGainBuf = NULL;
        }

        if (vTmpBuf != NULL)
        {
            delete [] vTmpBuf;
            vTmpBuf = NULL;
        }
    }

    void Limiter::update_settings()
    {
        // Update delay settings
        if (nUpdate & UP_SR)
            sDelay.clear();
        if (nUpdate & (UP_SR | UP_MODE))
            dsp::fill_one(vGainBuf, nMaxLookahead*3 + BUF_GRANULARITY);

        nLookahead          = millis_to_samples(nSampleRate, fLookahead);
        sDelay.set_delay(nLookahead);

        if (nUpdate & (UP_SR | UP_MODE | UP_THRESH))
            nThresh             = nLookahead;

        // Check that mode change has triggered
        if (nUpdate & UP_MODE)
        {
            // Clear state for the limiter
            switch (nMode)
            {
                case LM_COMPRESSOR:
                {
                    sComp.fKS           = 0.0f;
                    sComp.fKE           = 0.0f;
                    sComp.fTauAttack    = 0.0f;
                    sComp.fTauRelease   = 0.0f;
                    sComp.fEnvelope     = 0.0f;
                    sComp.fAmp          = 0.0f;
                    sComp.nCountdown    = 0;
                    sComp.fSample       = 0.0f;
                    sComp.vHermite[0]   = 0.0f;
                    sComp.vHermite[1]   = 0.0f;
                    sComp.vHermite[2]   = 0.0f;
                    break;
                }
                case LM_HERM_THIN:
                case LM_HERM_WIDE:
                case LM_HERM_TAIL:
                case LM_HERM_DUCK:
                {
                    sSat.nAttack        = 0;
                    sSat.nPlane         = 0;
                    sSat.nRelease       = 0;
                    sSat.nMiddle        = 0;

                    sSat.vAttack[0]     = 0.0f;
                    sSat.vAttack[1]     = 0.0f;
                    sSat.vAttack[2]     = 0.0f;
                    sSat.vAttack[3]     = 0.0f;
                    sSat.vRelease[0]    = 0.0f;
                    sSat.vRelease[1]    = 0.0f;
                    sSat.vRelease[2]    = 0.0f;
                    sSat.vRelease[3]    = 0.0f;

                    break;
                }

                case LM_EXP_THIN:
                case LM_EXP_WIDE:
                case LM_EXP_TAIL:
                case LM_EXP_DUCK:
                {
                    sExp.nAttack        = 0;
                    sExp.nPlane         = 0;
                    sExp.nRelease       = 0;
                    sExp.nMiddle        = 0;

                    sExp.vAttack[0]     = 0.0f;
                    sExp.vAttack[1]     = 0.0f;
                    sExp.vAttack[2]     = 0.0f;
                    sExp.vAttack[3]     = 0.0f;
                    sExp.vRelease[0]    = 0.0f;
                    sExp.vRelease[1]    = 0.0f;
                    sExp.vRelease[2]    = 0.0f;
                    sExp.vRelease[3]    = 0.0f;

                    break;
                }

                default:
                    break;
            }
        }

        // Update state
        switch (nMode)
        {
            case LM_COMPRESSOR:
            {
                sComp.fKS           = fThreshold * fKnee;
                sComp.fKE           = fThreshold / fKnee;
                sComp.fTauAttack    = 1.0f - expf(-10.0f * M_SQRT2 / (millis_to_samples(nSampleRate, fAttack)));
                sComp.fTauRelease   = 1.0f - expf(logf(1.0f - M_SQRT1_2) / (millis_to_samples(nSampleRate, fRelease)));
                sComp.fAmp          = 1.0f / nLookahead;
                lsp_trace("attack=%f, release=%f, tau_attack=%f, tau_release=%f", fAttack, fRelease, sComp.fTauAttack, sComp.fTauRelease);

                float log_ks    = logf(sComp.fKS);
                float log_ke    = logf(sComp.fKE);
                interpolation::hermite_quadratic(sComp.vHermite, log_ks, log_ks, 1.0f, log_ke, 0.0f);

                lsp_trace("ks=%f, ke=%f, thresh=%f, hermite={%f, %f, %f}",
                    sComp.fKS, sComp.fKE, fThreshold,
                    sComp.vHermite[0], sComp.vHermite[1], sComp.vHermite[2]
                );
                break;
            }

            case LM_HERM_THIN:
            case LM_HERM_WIDE:
            case LM_HERM_TAIL:
            case LM_HERM_DUCK:
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
                    sSat.nAttack        = attack;
                    sSat.nPlane         = attack;
                }
                else if (nMode == LM_HERM_WIDE)
                {
                    sSat.nAttack        = (attack >> 1);
                    sSat.nPlane         = (attack + (release >> 1));
                }
                else if (nMode == LM_HERM_TAIL)
                {
                    sSat.nAttack        = (attack >> 1);
                    sSat.nPlane         = attack;
                }
                else if (nMode == LM_HERM_DUCK)
                {
                    sSat.nAttack        = attack;
                    sSat.nPlane         = (attack + (release >> 1));
                }

                sSat.nRelease       = attack + release + 1;
                sSat.nMiddle        = attack;

                interpolation::hermite_cubic(sSat.vAttack, -1.0f, 0.0f, 0.0f, sSat.nAttack, 1.0f, 0.0f);
                interpolation::hermite_cubic(sSat.vRelease, sSat.nPlane, 1.0f, 0.0f, sSat.nRelease, 0.0f, 0.0f);

//                #ifdef LSP_DEBUG
//                printf("sat:\n");
//                for (ssize_t i=0; i<sSat.nRelease; ++i)
//                    printf("%.6f\n", sat(i));
//                #endif /* LSP_DEBUG */

                break;
            }

            case LM_EXP_THIN:
            case LM_EXP_WIDE:
            case LM_EXP_TAIL:
            case LM_EXP_DUCK:
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
                    sExp.nAttack        = attack;
                    sExp.nPlane         = attack;
                }
                else if (nMode == LM_EXP_WIDE)
                {
                    sExp.nAttack        = (attack >> 1);
                    sExp.nPlane         = (attack + (release >> 1));
                }
                else if (nMode == LM_EXP_TAIL)
                {
                    sExp.nAttack        = (attack >> 1);
                    sExp.nPlane         = attack;
                }
                else if (nMode == LM_EXP_DUCK)
                {
                    sExp.nAttack        = attack;
                    sExp.nPlane         = (attack + (release >> 1));
                }

                sExp.nRelease       = attack + release + 1;
                sExp.nMiddle        = attack;

                interpolation::exponent(sExp.vAttack, -1.0f, 0.0f, sExp.nAttack, 1.0f, 2.0f / attack);
                interpolation::exponent(sExp.vRelease, sExp.nPlane, 1.0f, sExp.nRelease, 0.0f, 2.0f / release);

//                #ifdef LSP_DEBUG
//                printf("exp:\n");
//                for (ssize_t i=0; i<sExp.nRelease; ++i)
//                    printf("%.6f\n", exp(i));
//                #endif /* LSP_DEBUG */

                break;
            }

            case LM_LINE_THIN:
            case LM_LINE_WIDE:
            case LM_LINE_TAIL:
            case LM_LINE_DUCK:
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
                    sLine.nAttack       = attack;
                    sLine.nPlane        = attack;
                }
                else if (nMode == LM_LINE_WIDE)
                {
                    sLine.nAttack       = (attack >> 1);
                    sLine.nPlane        = (attack + (release >> 1));
                }
                else if (nMode == LM_LINE_TAIL)
                {
                    sLine.nAttack       = (attack >> 1);
                    sLine.nPlane        = attack;
                }
                else if (nMode == LM_LINE_DUCK)
                {
                    sLine.nAttack       = attack;
                    sLine.nPlane        = (attack + (release >> 1));
                }

                sLine.nRelease      = attack + release + 1;
                sLine.nMiddle       = attack;

                interpolation::linear(sLine.vAttack, -1.0f, 0.0f, sExp.nAttack, 1.0f);
                interpolation::linear(sLine.vRelease, sLine.nPlane, 1.0f, sExp.nRelease, 0.0f);

                #ifdef LSP_DEBUG
                printf("line:\n");
                for (ssize_t i=0; i<sLine.nRelease; ++i)
                    printf("%.6f\n", line(i));
                #endif /* LSP_DEBUG */

                break;
            }

            default:
                break;
        }

        // Clear the update flag
        nUpdate         = 0;
    }

    inline float Limiter::reduction(float env)
    {
        if (env < sComp.fKS)
            return 1.0f;
        else if (env > sComp.fKE)
            return fThreshold / env;

        float lx    = logf(env);
        return expf((sComp.vHermite[0]*lx + sComp.vHermite[1] - 1.0f)*lx + sComp.vHermite[2]);
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

    void Limiter::process_compressor(float *dst, float *gain, const float *src, const float *sc, size_t samples)
    {
        // Do some stuff
        for (size_t i=0; i<samples; ++i)
        {
            // Get lookup sample
            float ls    = sc[i];
            if (ls < 0.0f)
                ls          = -ls;

            // Get delayed sample
            float ds    = sDelay.process(src[i]);
            float ads   = (ds < 0.0f) ? -ds : ds;

            // Check that limiter has triggered (overwrite sidechain signal)
            if (sComp.nCountdown > 0)
            {
                if (sComp.fSample <= ls)
                {
//                    lsp_trace("clipping: sample=%f, level=%f", sComp.fSample, ls);
                    sComp.fSample       = ls;
                    sComp.nCountdown    = nLookahead;
                }
                else
                {
                    ls                  = sComp.fSample * (1.0f + sComp.fAmp*(nLookahead - sComp.nCountdown));
                    sComp.nCountdown    --;
                }
            }
            else if (ls >= fThreshold)
            {
                sComp.nCountdown      = sDelay.get_delay();
                sComp.fSample         = ls;
            }

            // Calculate envelope and reduction
            sComp.fEnvelope     = (ls >= sComp.fEnvelope) ?
                                    sComp.fTauAttack * (ls - sComp.fEnvelope) :
                                    sComp.fTauRelease * (ls - sComp.fEnvelope);
            float r             = reduction(sComp.fEnvelope);

            // Prevent from overloading and store processed sample
            if ((r*ads) >= fThreshold)
            {
                sComp.fEnvelope     = sComp.fKE; // Force env to be of specified size
                r                   = fThreshold / ads;
            }

            gain[i]             = r;
            dst[i]              = r*ds;
        }
    }

    void Limiter::apply_sat_patch(float *dst, float amp)
    {
        ssize_t t = 0;

        // Attack part
        while (t < sSat.nAttack)
        {
            float x     = t++;
            *(dst++)   *= 1.0f - amp * (((sSat.vAttack[0]*x + sSat.vAttack[1])*x + sSat.vAttack[2])*x + sSat.vAttack[3]);
        }

        // Peak part
        while (t < sSat.nPlane)
        {
            *(dst++)   *= 1.0f - amp;
            t++;
        }

        // Release part
        while (t < sSat.nRelease)
        {
            float x     = t++;
            *(dst++)   *= 1.0f - amp * (((sSat.vRelease[0]*x + sSat.vRelease[1])*x + sSat.vRelease[2])*x + sSat.vRelease[3]);
        }
    }

    void Limiter::apply_exp_patch(float *dst, float amp)
    {
        ssize_t t = 0;

        // Attack part
        while (t < sExp.nAttack)
            *(dst++)   *= 1.0f - amp * (sExp.vAttack[0] + sExp.vAttack[1] * expf(sExp.vAttack[2] * (t++)));

        // Peak part
        while (t < sExp.nPlane)
        {
            *(dst++)   *= 1.0f - amp;
            t++;
        }

        // Release part
        while (t < sExp.nRelease)
            *(dst++)   *= 1.0f - amp * (sExp.vRelease[0] + sExp.vRelease[1] * expf(sExp.vRelease[2] * (t++)));
    }

    void Limiter::apply_line_patch(float *dst, float amp)
    {
        ssize_t t = 0;

        // Attack part
        while (t < sLine.nAttack)
            *(dst++)   *= 1.0f - amp * (sLine.vAttack[0] * (t++) + sLine.vAttack[1]);

        // Peak part
        while (t < sLine.nPlane)
        {
            *(dst++)   *= 1.0f - amp;
            t++;
        }

        // Release part
        while (t < sLine.nRelease)
            *(dst++)   *= 1.0f - amp * (sLine.vRelease[0] * (t++) + sLine.vRelease[1]);
    }

    void Limiter::process_hermite(float *dst, float *gain, const float *src, const float *sc, size_t samples)
    {
        float *gbuf     = &vGainBuf[nMaxLookahead];

        while (samples > 0)
        {
            size_t to_do    = (samples > BUF_GRANULARITY) ? BUF_GRANULARITY : samples;

            // Fill gain buffer
            dsp::fill_one(&gbuf[nMaxLookahead*3], to_do);
            dsp::abs(vTmpBuf, sc, to_do);

            float thresh    = 1.0f;

            // Repeat until there are no peaks
            while (true)
            {
                // Find LIMITER_PEAKS_MAX peaks
                peak_t vPeaks[LIMITER_PEAKS_MAX];
                size_t nPeaks   = 0;
                float left      = 0.0f;
                float s         = vTmpBuf[0] * gbuf[0];

                for (size_t i=1; i<=to_do; ++i)
                {
                    float right     = (i < to_do) ? vTmpBuf[i] * gbuf[i] : 0.0f;
                    if (s > fThreshold)
                    {
                        // Check that it is a peak
                        if ((s > left) && (s >= right))
                        {
                            peak_t *p;

                            if (nPeaks >= LIMITER_PEAKS_MAX)
                            {
                                for (size_t j=0; j<LIMITER_PEAKS_MAX; ++j)
                                {
                                    if (vPeaks[j].fValue < s)
                                        p = & vPeaks[j];
                                }
                            }
                            else
                                p = & vPeaks[nPeaks++];

                            p->nTime        = i-1;
                            p->fValue       = s;
                        }
                    }
                    left        = s;
                    s           = right;
                }

                // Check that there are no peaks left
                if (nPeaks <= 0)
                    break;

                // Apply modifications to the buffer
                for (size_t i=0; i<nPeaks; ++i)
                {
                    peak_t *p       = &vPeaks[i];

                    ssize_t off     = p->nTime - sSat.nMiddle;
                    s               = vTmpBuf[p->nTime] * gbuf[p->nTime];
                    if (s > fThreshold)
                    {
                        left            = (s - (fKnee * fThreshold * thresh - 0.000001))/ s;
                        apply_sat_patch(&gbuf[off], left);
                    }
                }

                // Lower gain each time at -0.5 dB
                thresh     *=       GAIN_LOWERING;
            }

            // Copy gain value and shift gain buffer
            dsp::copy(gain, &vGainBuf[nMaxLookahead - nLookahead], to_do);
            dsp::move(vGainBuf, &vGainBuf[to_do], nMaxLookahead*4);

            // Apply gain to delayed signal
            sDelay.process(dst, src, to_do);

            // Decrement number of samples and update pointers
            dst            += to_do;
            gain           += to_do;
            src            += to_do;
            sc             += to_do;
            samples        -= to_do;
        }
    }

    void Limiter::process_exp(float *dst, float *gain, const float *src, const float *sc, size_t samples)
    {
        float *gbuf     = &vGainBuf[nMaxLookahead];

        while (samples > 0)
        {
            size_t to_do    = (samples > BUF_GRANULARITY) ? BUF_GRANULARITY : samples;

            // Fill gain buffer
            dsp::fill_one(&gbuf[nMaxLookahead*3], to_do);
            dsp::abs(vTmpBuf, sc, to_do);

            float thresh = 1.0f;

            // Repeat until there are no peaks
            while (true)
            {
                // Find LIMITER_PEAKS_MAX peaks
                peak_t vPeaks[LIMITER_PEAKS_MAX];
                size_t nPeaks   = 0;
                float left      = 0.0f;
                float s         = vTmpBuf[0] * gbuf[0];

                for (size_t i=1; i<=to_do; ++i)
                {
                    float right     = (i < to_do) ? vTmpBuf[i] * gbuf[i] : 0.0f;
                    if (s > fThreshold)
                    {
                        // Check that it is a peak
                        if ((s > left) && (s >= right))
                        {
                            peak_t *p;

                            if (nPeaks >= LIMITER_PEAKS_MAX)
                            {
                                for (size_t j=0; j<LIMITER_PEAKS_MAX; ++j)
                                {
                                    if (vPeaks[j].fValue < s)
                                        p = & vPeaks[j];
                                }
                            }
                            else
                                p = & vPeaks[nPeaks++];

                            p->nTime        = i-1;
                            p->fValue       = s;
                        }
                    }
                    left        = s;
                    s           = right;
                }

                // Check that there are no peaks left
                if (nPeaks <= 0)
                    break;

                // Apply modifications to the buffer
                for (size_t i=0; i<nPeaks; ++i)
                {
                    peak_t *p       = &vPeaks[i];

                    ssize_t off     = p->nTime - sSat.nMiddle;
                    s               = vTmpBuf[p->nTime] * gbuf[p->nTime];
                    if (s > fThreshold)
                    {
                        left            = (s - (fKnee * fThreshold * thresh - 0.000001))/ s;
                        apply_exp_patch(&gbuf[off], left);
                    }
                }

                // Lower gain each time at -0.5 dB
                thresh     *=       GAIN_LOWERING;
            }

            // Copy gain value and shift gain buffer
            dsp::copy(gain, &vGainBuf[nMaxLookahead - nLookahead], to_do);
            dsp::move(vGainBuf, &vGainBuf[to_do], nMaxLookahead*4);

            // Apply gain to delayed signal
            sDelay.process(dst, src, to_do);

            // Decrement number of samples and update pointers
            dst            += to_do;
            gain           += to_do;
            src            += to_do;
            sc             += to_do;
            samples        -= to_do;
        }
    }

    void Limiter::process_line(float *dst, float *gain, const float *src, const float *sc, size_t samples)
    {
        float *gbuf     = &vGainBuf[nMaxLookahead];

        while (samples > 0)
        {
            size_t to_do    = (samples > BUF_GRANULARITY) ? BUF_GRANULARITY : samples;

            // Fill gain buffer
            dsp::fill_one(&gbuf[nMaxLookahead*3], to_do);
            dsp::abs(vTmpBuf, sc, to_do);

            float thresh = 1.0f;

            // Repeat until there are no peaks
            while (true)
            {
                // Find LIMITER_PEAKS_MAX peaks
                peak_t vPeaks[LIMITER_PEAKS_MAX];
                size_t nPeaks   = 0;
                float left      = 0.0f;
                float s         = vTmpBuf[0] * gbuf[0];

                for (size_t i=1; i<=to_do; ++i)
                {
                    float right     = (i < to_do) ? vTmpBuf[i] * gbuf[i] : 0.0f;
                    if (s > fThreshold)
                    {
                        // Check that it is a peak
                        if ((s > left) && (s >= right))
                        {
                            peak_t *p;

                            if (nPeaks >= LIMITER_PEAKS_MAX)
                            {
                                for (size_t j=0; j<LIMITER_PEAKS_MAX; ++j)
                                {
                                    if (vPeaks[j].fValue < s)
                                        p = & vPeaks[j];
                                }
                            }
                            else
                                p = & vPeaks[nPeaks++];

                            p->nTime        = i-1;
                            p->fValue       = s;
                        }
                    }
                    left        = s;
                    s           = right;
                }

                // Check that there are no peaks left
                if (nPeaks <= 0)
                    break;

                // Apply modifications to the buffer
                for (size_t i=0; i<nPeaks; ++i)
                {
                    peak_t *p       = &vPeaks[i];

                    ssize_t off     = p->nTime - sSat.nMiddle;
                    s               = vTmpBuf[p->nTime] * gbuf[p->nTime];
                    if (s > fThreshold)
                    {
                        left            = (s - (fKnee * fThreshold * thresh - 0.000001))/ s;
                        apply_line_patch(&gbuf[off], left);
                    }
                }

                // Lower gain each time at -0.5 dB
                thresh     *=       GAIN_LOWERING;
            }

            // Copy gain value and shift gain buffer
            dsp::copy(gain, &vGainBuf[nMaxLookahead - nLookahead], to_do);
            dsp::move(vGainBuf, &vGainBuf[to_do], nMaxLookahead*4);

            // Apply gain to delayed signal
            sDelay.process(dst, src, to_do);

            // Decrement number of samples and update pointers
            dst            += to_do;
            gain           += to_do;
            src            += to_do;
            sc             += to_do;
            samples        -= to_do;
        }
    }

    void Limiter::process(float *dst, float *gain, const float *src, const float *sc, size_t samples)
    {
        switch (nMode)
        {
            case LM_COMPRESSOR:
                process_compressor(dst, gain, src, sc, samples);
                break;
            case LM_HERM_THIN:
            case LM_HERM_WIDE:
            case LM_HERM_TAIL:
            case LM_HERM_DUCK:
                process_hermite(dst, gain, src, sc, samples);
                break;

            case LM_EXP_THIN:
            case LM_EXP_WIDE:
            case LM_EXP_TAIL:
            case LM_EXP_DUCK:
                process_exp(dst, gain, src, sc, samples);
                break;

            case LM_LINE_THIN:
            case LM_LINE_WIDE:
            case LM_LINE_TAIL:
            case LM_LINE_DUCK:
                process_line(dst, gain, src, sc, samples);
                break;

            default:
                dsp::fill_one(gain, samples);
                dsp::copy(dst, src, samples);
                break;
        }

        // Fix-up gain to prevent from overloading
        if (nThresh <= 0)
            return;

        while ((samples > 0) && (nThresh > 0))
        {
            size_t to_do = (samples > nThresh) ? nThresh : samples;

            float max = 0.0f;

            for (size_t i=0; i<to_do; ++i)
            {
                float lvl = dst[i] * gain[i];
                if (lvl < 0.0f)
                    lvl = - lvl;
                if (lvl > max)
                    max = lvl;
            }

            if (max > fThreshold)
                dsp::scale(gain, gain, (fThreshold - 0.000001f)/max, to_do);

            // Move pointers
            nThresh    -= to_do;
            samples    -= to_do;
            dst        += to_do;
            gain       += to_do;
        }
    }

} /* namespace lsp */
