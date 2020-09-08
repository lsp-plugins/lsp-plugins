/*
 * Crossover.cpp
 *
 *  Created on: 03 авг. 2016 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <dsp/dsp.h>
#include <core/util/Crossover.h>

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
        nBands          = 0;
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
        free_aligned(pData);
        construct();
    }

    bool Crossover::init(size_t bands, size_t buf_size)
    {
        // TODO

        return true;
    }

    void Crossover::set_slope(size_t sp, size_t slope)
    {
        if (sp >= (nBands - 1))
            return;
        if (slope == vSplit[sp].nSlope)
            return;

        vSplit[sp].nSlope   = slope;
        nReconfigure       |= R_SPLIT;
    }

    ssize_t Crossover::get_slope(size_t sp) const
    {
        return (sp < (nBands - 1)) ? vSplit[sp].nSlope : -1;
    }

    void Crossover::set_frequency(size_t sp, float freq)
    {
        if (sp >= (nBands - 1))
            return;
        if (freq == vSplit[sp].fFreq)
            return;

        vSplit[sp].fFreq    = freq;
        nReconfigure       |= R_SPLIT;
    }

    float Crossover::get_frequency(size_t sp) const
    {
        return (sp < (nBands - 1)) ? vSplit[sp].fFreq : -1.0f;
    }

    void Crossover::set_gain(size_t band, float gain)
    {
        if (band >= nBands)
            return;
        if (gain == vBands[band].fGain)
            return;

        vBands[band].fGain = gain;
        if (!vBands[band].bEnabled)
            nReconfigure       |= R_GAIN;
    }

    float Crossover::get_gain(size_t band) const
    {
        return (band < nBands) ? vBands[band].fGain: -1.0f;
    }

    float Crossover::get_band_start(size_t band) const
    {
        return (band < nBands) ? vBands[band].fStart : -1.0f;
    }

    float Crossover::get_band_end(size_t band) const
    {
        return (band < nBands) ? vBands[band].fEnd : -1.0f;
    }

    void Crossover::set_handler(size_t band, crossover_func_t func, void *object, void *subject)
    {
        if (band >= nBands)
            return;

        band_t *b       = &vBands[band];
        b->pFunc        = func;
        b->pObject      = object;
        b->pSubject     = subject;
    }

    void Crossover::unset_handler(size_t band)
    {
        if (band >= nBands)
            return;

        band_t *b       = &vBands[band];
        b->pFunc        = NULL;
        b->pObject      = NULL;
        b->pSubject     = NULL;
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
