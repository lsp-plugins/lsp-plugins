/*
 * Crossover.cpp
 *
 *  Created on: 03 авг. 2016 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <dsp/dsp.h>
#include <core/util/Crossover.h>
#include <core/sugar.h>
#include <core/stdlib/math.h>

namespace lsp
{
    Crossover::Crossover()
    {
        construct();
    }

    Crossover::~Crossover()
    {
        destroy();
    }

    void Crossover::construct()
    {
        nReconfigure    = 0;
        nSplits         = 0;
        nBufSize        = 0;
        nSampleRate     = DEFAULT_SAMPLE_RATE;

        vBands          = NULL;
        vSplit          = NULL;
        vPlan           = NULL;
        nPlanSize       = 0;

        vLpfBuf         = NULL;
        vHpfBuf         = NULL;

        pData           = NULL;
    }

    void Crossover::destroy()
    {
        if (vSplit != NULL)
        {
            for (size_t i=0; i<nSplits; ++i)
            {
                split_t *sp     = &vSplit[i];
                sp->sLPF.destroy();
                sp->sHPF.destroy();
            }
        }

        free_aligned(pData);
        construct();
    }

    bool Crossover::init(size_t bands, size_t buf_size)
    {
        if (bands < 1)
            return false;

        size_t xbuf_size    = ALIGN_SIZE(buf_size * sizeof(float), DEFAULT_ALIGN);
        size_t band_size    = ALIGN_SIZE(bands * sizeof(band_t), DEFAULT_ALIGN);
        size_t split_size   = ALIGN_SIZE((bands - 1) * sizeof(split_t), DEFAULT_ALIGN);
        size_t plan_size    = ALIGN_SIZE((bands - 1) * sizeof(split_t *), DEFAULT_ALIGN);
        size_t to_alloc     = band_size +
                              split_size +
                              plan_size +
                              xbuf_size * 2;

        // Allocate buffers
        uint8_t *data       = NULL;
        uint8_t *ptr        = alloc_aligned<uint8_t>(data, to_alloc);
        if (ptr == NULL)
            return false;
        lsp_guard_assert(uint8_t *save   = ptr);

        // Distribute the allocated space
        vBands              = reinterpret_cast<band_t *>(ptr);
        ptr                += band_size;
        vSplit              = reinterpret_cast<split_t *>(ptr);
        ptr                += split_size;
        vPlan               = reinterpret_cast<split_t **>(ptr);
        ptr                += plan_size;
        vLpfBuf             = reinterpret_cast<float *>(ptr);
        ptr                += xbuf_size;
        vHpfBuf             = reinterpret_cast<float *>(ptr);
        ptr                += xbuf_size;

        // Initialize fields, keep sample_rate unchanged
        nReconfigure        = R_ALL;
        nSplits             = bands - 1;
        nBufSize            = buf_size;
        nPlanSize           = 0;

        // Store allocated data pointer
        pData               = data;

        // Construct all splits
        float step          = logf(SPEC_FREQ_MAX / SPEC_FREQ_MIN) / bands;

        for (size_t i=0; i<nSplits; ++i)
        {
            split_t *sp         = &vSplit[i];

            // Initialize filters
            sp->sLPF.construct();
            sp->sHPF.construct();

            if (!sp->sLPF.init(bands-1, 0))
            {
                destroy();
                return false;
            }
            sp->sLPF.set_sample_rate(nSampleRate);

            if (!sp->sHPF.init(NULL))
            {
                destroy();
                return false;
            }
            sp->sHPF.set_sample_rate(nSampleRate);

            // Set IIR mode for each filter
            sp->sLPF.set_mode(EQM_IIR);

            // Initialize split point parameters
            sp->nBandId         = i + 1; // Band N+1 is attached to split point N
            sp->nSlope          = 0;
            sp->fFreq           = SPEC_FREQ_MIN * expf((i+1) * step);
        }

        // Construct all bands
        for (size_t i=0; i<=nSplits; ++i)
        {
            band_t *sb          = &vBands[i];

            sb->fGain           = GAIN_AMP_0_DB;
            sb->fStart          = (i == 0) ? SPEC_FREQ_MIN : vSplit[i-1].fFreq;
            sb->fEnd            = vSplit[i].fFreq;
            sb->bEnabled        = false;
            sb->pStart          = NULL;
            sb->pEnd            = NULL;

            sb->pFunc           = NULL;
            sb->pObject         = NULL;
            sb->pSubject        = NULL;
            sb->nId             = i;
        }

        lsp_assert(ptr <= &save[to_alloc]);

        return true;
    }

    void Crossover::set_slope(size_t sp, size_t slope)
    {
        if (sp >= nSplits)
            return;
        if (slope == vSplit[sp].nSlope)
            return;

        vSplit[sp].nSlope   = slope;
        nReconfigure       |= R_SPLIT;
    }

    ssize_t Crossover::get_slope(size_t sp) const
    {
        return (sp < nSplits) ? vSplit[sp].nSlope : -1;
    }

    void Crossover::set_frequency(size_t sp, float freq)
    {
        if (sp >= nSplits)
            return;
        if (freq == vSplit[sp].fFreq)
            return;

        vSplit[sp].fFreq    = freq;
        nReconfigure       |= R_SPLIT;
    }

    float Crossover::get_frequency(size_t sp) const
    {
        return (sp < nSplits) ? vSplit[sp].fFreq : -1.0f;
    }

    void Crossover::set_gain(size_t band, float gain)
    {
        if (band >= nSplits)
            return;
        if (gain == vBands[band].fGain)
            return;

        vBands[band].fGain = gain;
        if (!vBands[band].bEnabled)
            nReconfigure       |= R_GAIN;
    }

    float Crossover::get_gain(size_t band) const
    {
        return (band <= nSplits) ? vBands[band].fGain: -1.0f;
    }

    float Crossover::get_band_start(size_t band)
    {
        reconfigure();
        return (band <= nSplits) ? vBands[band].fStart : -1.0f;
    }

    float Crossover::get_band_end(size_t band)
    {
        reconfigure();
        return (band <= nSplits) ? vBands[band].fEnd : -1.0f;
    }

    bool Crossover::set_handler(size_t band, crossover_func_t func, void *object, void *subject)
    {
        if (band > nSplits)
            return false;

        band_t *b       = &vBands[band];
        b->pFunc        = func;
        b->pObject      = object;
        b->pSubject     = subject;

        return true;
    }

    bool Crossover::unset_handler(size_t band)
    {
        if (band > nSplits)
            return false;

        band_t *b       = &vBands[band];
        b->pFunc        = NULL;
        b->pObject      = NULL;
        b->pSubject     = NULL;
        return true;
    }

    bool Crossover::band_active(size_t band)
    {
        if (band > nSplits)
            return false;
        else if (band == 0)
            return true;

        reconfigure();
        return vBands[band].bEnabled;
    }

    void Crossover::set_sample_rate(size_t sr)
    {
        if (nSampleRate == sr)
            return;

        nSampleRate     = sr;
        for (size_t i=0; i<nSplits; ++i)
        {
            vSplit[i].sLPF.set_sample_rate(sr);
            vSplit[i].sHPF.set_sample_rate(sr);
        }

        nReconfigure   |= R_ALL;
    }

    void Crossover::reconfigure()
    {
        if (!nReconfigure)
            return;

        // Form the plan and reset band state
        nPlanSize       = 0;
        for (size_t i=0; i<nSplits; ++i)
        {
            if (vSplit[i].nSlope > 0)
                vPlan[nPlanSize++]  = &vSplit[i];
        }
        for (size_t i=0; i<=nSplits; ++i)
            vBands[i].bEnabled  = false;

        // Sort split bands in ascending order
        for (ssize_t si=0, n=nPlanSize; si < n-1; ++si)
            for (ssize_t sj=si+1; sj < n; ++sj)
                if (vPlan[sj]->fFreq < vPlan[si]->fFreq)
                    swap(vPlan[si], vPlan[sj]);

        band_t *left        = &vBands[0];
        left->fStart        = SPEC_FREQ_MIN;
        left->bEnabled      = true;
        left->pStart        = NULL;

        // Configure LPF and HPF bands
        for (size_t i=0; i<nPlanSize; ++i)
        {
            split_t *sp         = vPlan[i];
            band_t *right       = &vBands[sp->nBandId];

            left->fEnd          = sp->fFreq;
            left->pEnd          = sp;
            right->fStart       = sp->fFreq;
            right->pStart       = sp;
            right->bEnabled     = true;

            // Set LPF parameters
            size_t filter_id    = 0;
            filter_params_t fp;

            fp.nType            = FLT_BT_LRX_LOPASS;
            fp.fFreq            = sp->fFreq;
            fp.fFreq2           = sp->fFreq;
            fp.fGain            = left->fGain;
            fp.nSlope           = sp->nSlope;
            fp.fQuality         = 0.0f;

            sp->sLPF.set_params(filter_id++, &fp);

            // Append APF filters
            for (size_t j=i+1; j<nPlanSize; ++j)
            {
                split_t *xsp        = vPlan[j];

                fp.nType            = FLT_BT_LRX_ALLPASS;
                fp.fFreq            = sp->fFreq;
                fp.fFreq2           = sp->fFreq;
                fp.fGain            = GAIN_AMP_0_DB;
                fp.nSlope           = xsp->nSlope;
                fp.fQuality         = 0.0f;

                sp->sLPF.set_params(filter_id++, &fp);
            }

            // Disable all other filters in the chain
            while (filter_id < nSplits)
            {
                fp.nType            = FLT_NONE;
                fp.fFreq            = 0;
                fp.fFreq2           = 0;
                fp.fGain            = GAIN_AMP_0_DB;
                fp.nSlope           = 0;
                fp.fQuality         = 0.0f;

                sp->sLPF.set_params(filter_id++, &fp);
            }

            // Set HPF parameters
            fp.nType            = FLT_BT_LRX_HIPASS;
            fp.fFreq            = sp->fFreq;
            fp.fFreq2           = sp->fFreq;
            fp.fGain            = (i < (nPlanSize-1)) ? GAIN_AMP_0_DB : right->fGain;
            fp.nSlope           = sp->nSlope;
            fp.fQuality         = 0.0f;

            sp->sHPF.update(nSampleRate, &fp);
            sp->sHPF.rebuild();

            // Move to next band
            left                = right;
        }

        // Update frequency of the last band
        left->fEnd          = nSampleRate * 0.5f;
        left->pEnd          = NULL;

        // DEBUG BEGIN
        lsp_trace("Execution plan:");
        for (size_t i=0; i<nPlanSize; ++i)
        {
            split_t *sp         = vPlan[i];
            lsp_trace("  split point #%d: this=%p, band=%d, freq=%.2f, slope=%d",
                int(i), sp, int(sp->nBandId), sp->fFreq, int(sp->nSlope)
            );
        }
        lsp_trace("Bands:");
        for (size_t i=0; i<=nSplits; ++i)
        {
            band_t *b           = &vBands[i];
            lsp_trace("  band #%d: this=%p, enabled=%s, gain=%f, start=%.2f, end=%.2f, start=%p, end=%p",
                            int(i), b, (b->bEnabled) ? "true " : "false",
                            b->fGain, b->fStart, b->fEnd, b->pStart, b->pEnd
                        );
        }
        // DEBUG END

        // Reset reconfiguration flag
        nReconfigure        = 0;
    }

    void Crossover::process(const float *in, size_t samples)
    {
        reconfigure();

        while (samples > 0)
        {
            size_t to_do        = lsp_min(samples, nBufSize);
            band_t *left        = &vBands[0];
            const float *src    = in;

            if (nPlanSize > 0)
            {
                // Process each band except last
                for (size_t i=0; i<nPlanSize; ++i)
                {
                    split_t *sp         = vPlan[i];
                    band_t *right       = &vBands[sp->nBandId];

                    // Perform split first
                    if (left->pFunc != NULL)
                        sp->sLPF.process(vLpfBuf, src, to_do);
                    sp->sHPF.process(vHpfBuf, src, to_do);

                    // Now call handlers
                    if (left->pFunc != NULL)
                        left->pFunc(left->pObject, left->pSubject, left->nId, vLpfBuf, to_do);

                    src                 = vHpfBuf;
                    left                = right;
                }

                // Process last band
                if (left->pFunc != NULL)
                    left->pFunc(left->pObject, left->pSubject, left->nId, vHpfBuf, to_do);
            }
            else if (left->pFunc != NULL)
            {
                dsp::mul_k3(vLpfBuf, src, vBands[0].fGain, to_do);
                left->pFunc(left->pObject, left->pSubject, left->nId, vLpfBuf, to_do);
            }

            // Update pointers
            in                 += to_do;
            samples            -= to_do;
        }
    }

    bool Crossover::freq_chart(size_t band, float *re, float *im, const float *f, size_t count)
    {
        // Valid index of the band?
        if (band > nSplits)
            return false;

        // Reconfigure
        reconfigure();

        // Band is enabled ?
        band_t *b       = &vBands[band];
        if (!b->bEnabled)
        {
            dsp::fill_zero(re, count);
            dsp::fill_zero(im, count);
        }
        else if (b->pEnd == NULL)
            b->pStart->sHPF.freq_chart(re, im, f, count);
        else if (b->pStart == NULL)
            b->pEnd->sLPF.freq_chart(re, im, f, count);
        else
        {
            // Compute frequency chart with chunks of maximum nBufSize size
            while (count > 0)
            {
                size_t to_do    = lsp_min(count, nBufSize);

                // Apply frequency chart
                b->pStart->sHPF.freq_chart(re, im, f, to_do);
                b->pEnd->sLPF.freq_chart(vLpfBuf, vHpfBuf, f, to_do);
                dsp::complex_mul2(re, im, vLpfBuf, vHpfBuf, to_do);

                // Update pointers
                re             += to_do;
                im             += to_do;
                f              += to_do;
                count          -= to_do;
            }
        }

        return true;
    }

    bool Crossover::freq_chart(size_t band, float *c, const float *f, size_t count)
    {
        // Valid index of the band?
        if (band > nSplits)
            return false;

        // Reconfigure
        reconfigure();

        // Band is enabled ?
        band_t *b       = &vBands[band];
        if (!b->bEnabled)
            dsp::pcomplex_fill_ri(c, 0.0f, 0.0f, count);
        else if (b->pEnd == NULL)
            b->pStart->sHPF.freq_chart(c, f, count);
        else if (b->pStart == NULL)
            b->pEnd->sLPF.freq_chart(c, f, count);
        else
        {
            // Compute frequency chart with chunks of maximum nBufSize size
            while (count > 0)
            {
                // We can go out of vLpfBuf because vHpfBuf is there after it
                size_t to_do    = lsp_min(count, nBufSize);

                // Apply frequency chart
                b->pStart->sHPF.freq_chart(c, f, to_do);
                b->pEnd->sLPF.freq_chart(vLpfBuf, f, to_do);
                dsp::pcomplex_mul2(c, vLpfBuf, to_do);

                // Update pointers
                c              += to_do * 2;
                f              += to_do;
                count          -= to_do;
            }
        }

        return true;
    }

    void Crossover::dump(IStateDumper *v) const
    {
        // TODO
    }

} /* namespace lsp */
