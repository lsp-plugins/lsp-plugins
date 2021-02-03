/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 1 дек. 2020 г.
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

#include <core/util/DynamicDelay.h>
#include <core/sugar.h>
#include <dsp/dsp.h>

#define BUF_SIZE        0x400

namespace lsp
{
    DynamicDelay::DynamicDelay()
    {
        construct();
    }

    DynamicDelay::~DynamicDelay()
    {
        destroy();
    }

    void DynamicDelay::construct()
    {
        vDelay      = NULL;
        nHead       = 0;
        nCapacity   = 0;
        nMaxDelay   = 0;
        pData       = NULL;
    }

    void DynamicDelay::destroy()
    {
        if (pData != NULL)
        {
            free_aligned(pData);
            vDelay      = NULL;
            nHead       = 0;
            nCapacity   = 0;
            nMaxDelay   = 0;
            pData       = NULL;
        }
    }

    status_t DynamicDelay::init(size_t max_size)
    {
        size_t delay        = max_size + 1;
        size_t buf_sz       = delay - (delay % BUF_SIZE) + BUF_SIZE * 2;
        size_t alloc        = buf_sz * sizeof(float);

        uint8_t *data       = NULL;
        uint8_t *ptr        = alloc_aligned<uint8_t>(data, alloc);
        if (ptr == NULL)
            return STATUS_NO_MEM;

        if (pData != NULL)
            free_aligned(pData);

        vDelay              = reinterpret_cast<float *>(ptr);
        ptr                += buf_sz * sizeof(float);

        nHead               = 0;
        nCapacity           = buf_sz;
        nMaxDelay           = max_size;
        pData               = data;

        return STATUS_OK;
    }

    void DynamicDelay::clear()
    {
        dsp::fill_zero(vDelay, nCapacity);
        nHead               = 0;
    }

    void DynamicDelay::process(float *out, const float *in, const float *delay, const float *fgain, const float *fdelay, size_t samples)
    {
        for (size_t i=0; i < samples; ++i)
        {
            ssize_t shift   = lsp_limit(ssize_t(delay[i]), 0, nMaxDelay);   // Delay
            ssize_t tail    = nHead - shift;
            if (tail < 0)
                tail           += nCapacity;
            size_t feed     = tail  + lsp_limit(fdelay[i], 0, shift);       // Feedback delay
            if (feed > nCapacity)
                feed           -= nCapacity;

            vDelay[nHead]   = in[i];            // Save input sample to buffer
            float s         = vDelay[tail];     // Read delayed sample
            vDelay[feed]   += s * fgain[i];     // Add feedback to the buffer
            out[i]          = vDelay[tail];     // Read the final sample to output buffer

            // Update head pointer
            if ((++nHead) >= nCapacity)
                nHead = 0;
        }
    }

    /**
     * Copy the contents of the dynamic delay
     * @param s delay to copy contents from
     */
    void DynamicDelay::copy(DynamicDelay *s)
    {
        // Estimate the amount of samples to copy
        size_t count    = lsp_min(nCapacity, s->nCapacity);
        ssize_t dt      = nCapacity - count;    // Position of destination tail
        ssize_t st      = s->nHead - count;     // Position of source tail
        if (st < 0)
            st         += s->nCapacity;

        // Perform data copy
        size_t tail     = s->nCapacity - st;
        if (tail < count)
        {
            dsp::copy(&vDelay[dt], &s->vDelay[st], tail);
            dsp::copy(&vDelay[dt + tail], s->vDelay, count - tail);
        }
        else
            dsp::copy(&vDelay[dt], &s->vDelay[st], count);

        // Clear the rest samples
        dsp::fill_zero(vDelay, dt);

        // Reset head to first sample
        nHead           = 0;
    }

    void DynamicDelay::swap(DynamicDelay *d)
    {
        ::swap(vDelay, d->vDelay);
        ::swap(nHead, d->nHead);
        ::swap(nCapacity, d->nCapacity);
        ::swap(nMaxDelay, d->nMaxDelay);
        ::swap(pData, d->pData);
    }

    void DynamicDelay::dump(IStateDumper *v) const
    {
        v->write("vDelay", vDelay);
        v->write("nHead", nHead);
        v->write("nCapacity", nCapacity);
        v->write("nMaxDelay", nMaxDelay);
        v->write("pData", pData);
    }
}


