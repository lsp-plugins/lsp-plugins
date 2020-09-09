/*
 * Crossover.cpp
 *
 *  Created on: 03 авг. 2016 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <dsp/dsp.h>
#include <core/util/Crossover.h>
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

        // Construct all splits
        float step          = logf(SPEC_FREQ_MAX / SPEC_FREQ_MIN) / bands;

        for (size_t i=0; i<nSplits; ++i)
        {
            split_t *sp         = &vSplit[i];

            sp->sLPF.construct();
            sp->sHPF.construct();

            sp->nId             = i;
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

            sb->pFunc           = NULL;
            sb->pObject         = NULL;
            sb->pSubject        = NULL;
            sb->id              = i;
        }

        // Store allocated data pointer
        pData               = ptr;

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

    float Crossover::get_band_start(size_t band) const
    {
        return (band <= nSplits) ? vBands[band].fStart : -1.0f;
    }

    float Crossover::get_band_end(size_t band) const
    {
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

    void Crossover::set_sample_rate(size_t sr)
    {
        if (nSampleRate == sr)
            return;

        nSampleRate     = sr;
        nReconfigure   |= R_ALL;
    }

    bool Crossover::freq_chart(float *re, float *im, size_t band, const float *f, size_t count)
    {
        // TODO
        return false;
    }

    bool Crossover::freq_chart(float *tf, const float *f, size_t band, size_t count)
    {
        return false;
    }

    void Crossover::reconfigure()
    {
        if (!nReconfigure)
            return;

        // TODO

        nReconfigure        = 0;
    }

    void Crossover::process(const float *in, size_t samples)
    {
        reconfigure();

        // TODO
    }

} /* namespace lsp */
