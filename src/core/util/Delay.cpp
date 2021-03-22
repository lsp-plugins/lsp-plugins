/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 06 дек. 2015 г.
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

#include <core/debug.h>
#include <core/util/Delay.h>
#include <dsp/dsp.h>

#define DELAY_GAP       0x200

namespace lsp
{
    
    Delay::Delay()
    {
        construct();
    }
    
    Delay::~Delay()
    {
        destroy();
    }

    void Delay::construct()
    {
        pBuffer     = NULL;
        nHead       = 0;
        nTail       = 0;
        nDelay      = 0;
        nSize       = 0;
    }

    bool Delay::init(size_t max_size)
    {
        size_t size     = ALIGN_SIZE(max_size + DELAY_GAP, DELAY_GAP);

        lsp_trace("max_size = %d, size = %d", int(max_size), int(size));

        float *ptr      = reinterpret_cast<float *>(::realloc(pBuffer, size * sizeof(float)));
        if (ptr == NULL)
            return false;
        pBuffer         = ptr;

        dsp::fill_zero(pBuffer, size);
        nHead           = 0;
        nTail           = 0;
        nDelay          = 0;
        nSize           = size;

        return true;
    }

    void Delay::destroy()
    {
        if (pBuffer != NULL)
        {
            ::free(pBuffer);
            pBuffer     = NULL;
        }
    }

    void Delay::process(float *dst, const float *src, size_t count)
    {
//        lsp_trace("dsp = %p, dst = %p, src = %p, count = %d", dsp, dst, src, int(count));
        size_t free_gap = nSize - nDelay;

        while (count > 0)
        {
            // Determine how many samples to process
            size_t to_do    = (count > free_gap) ? free_gap : count;

            // Push data to buffer
            for (size_t in=to_do; in > 0;)
            {
                size_t to_copy  = nSize - nHead;
                if (to_copy > in)
                    to_copy         = in;
                dsp::copy(&pBuffer[nHead], src, to_copy);
                nHead           = (nHead + to_copy) % nSize;
                src            += to_copy;
                in             -= to_copy;
            }

            // Shift data from buffer
            for (size_t out=to_do; out > 0;)
            {
                size_t to_copy  = nSize - nTail;
                if (to_copy > out)
                    to_copy         = out;
                dsp::copy(dst, &pBuffer[nTail], to_copy);
                nTail           = (nTail + to_copy) % nSize;
                dst            += to_copy;
                out            -= to_copy;
            }

            // Update number of samples
            count          -= to_do;
        }
    }

    void Delay::process(float *dst, const float *src, float gain, size_t count)
    {
//        lsp_trace("dsp = %p, dst = %p, src = %p, count = %d", dsp, dst, src, int(count));
        size_t free_gap = nSize - nDelay;

        while (count > 0)
        {
            // Determine how many samples to process
            size_t to_do    = (count > free_gap) ? free_gap : count;

            // Push data to buffer
            for (size_t in=to_do; in > 0;)
            {
                size_t to_copy  = nSize - nHead;
                if (to_copy > in)
                    to_copy         = in;
                dsp::copy(&pBuffer[nHead], src, to_copy);
                nHead           = (nHead + to_copy) % nSize;
                src            += to_copy;
                in             -= to_copy;
            }

            // Shift data from buffer
            for (size_t out=to_do; out > 0;)
            {
                size_t to_copy  = nSize - nTail;
                if (to_copy > out)
                    to_copy         = out;
                dsp::mul_k3(dst, &pBuffer[nTail], gain, to_copy);
                nTail           = (nTail + to_copy) % nSize;
                dst            += to_copy;
                out            -= to_copy;
            }

            // Update number of samples
            count          -= to_do;
        }
    }

    void Delay::process(float *dst, const float *src, const float *gain, size_t count)
    {
//        lsp_trace("dsp = %p, dst = %p, src = %p, count = %d", dsp, dst, src, int(count));
        size_t free_gap = nSize - nDelay;

        while (count > 0)
        {
            // Determine how many samples to process
            size_t to_do    = (count > free_gap) ? free_gap : count;

            // Push data to buffer
            for (size_t in=to_do; in > 0;)
            {
                size_t to_copy  = nSize - nHead;
                if (to_copy > in)
                    to_copy         = in;
                dsp::copy(&pBuffer[nHead], src, to_copy);
                nHead           = (nHead + to_copy) % nSize;
                src            += to_copy;
                in             -= to_copy;
            }

            // Shift data from buffer
            for (size_t out=to_do; out > 0;)
            {
                size_t to_copy  = nSize - nTail;
                if (to_copy > out)
                    to_copy         = out;
                dsp::mul3(dst, &pBuffer[nTail], gain, to_copy);
                nTail           = (nTail + to_copy) % nSize;
                gain           += to_copy;
                dst            += to_copy;
                out            -= to_copy;
            }

            // Update number of samples
            count          -= to_do;
        }
    }

    void Delay::process_ramping(float *dst, const float *src, size_t delay, size_t count)
    {
        // If delay does not change - use faster algorithm
        if (delay == nDelay)
        {
            process(dst, src, count);
            return;
        }
        else if (count <= 0)
            return;

        // More slower algorithm
        float delta     = float(ssize_t(delay) - ssize_t(nDelay)) / float(count);
        size_t step     = 0;
        do
        {
            pBuffer[nHead]  = *(src++);
            *(dst++)        = pBuffer[nTail];

            nHead           = (nHead + 1) % nSize;
            nTail           = (nHead + nSize - ssize_t(nDelay + delta * step)) % nSize;

            step            ++;
        } while ((--count) > 0);

        nDelay  = delay;
    }

    void Delay::process_ramping(float *dst, const float *src, float gain, size_t delay, size_t count)
    {
        // If delay does not change - use faster algorithm
        if (delay == nDelay)
        {
            process(dst, src, gain, count);
            return;
        }
        else if (count <= 0)
            return;

        // More slower algorithm
        float delta     = float(ssize_t(delay) - ssize_t(nDelay)) / float(count);
        size_t step     = 0;
        do
        {
            pBuffer[nHead]  = *(src++);
            *(dst++)        = pBuffer[nTail] * gain;

            nHead           = (nHead + 1) % nSize;
            nTail           = (nHead + nSize - ssize_t(nDelay + delta * step)) % nSize;

            step            ++;
        } while ((--count) > 0);

        nDelay  = delay;
    }

    float Delay::process(float src)
    {
        pBuffer[nHead]  = src;
        float ret       = pBuffer[nTail];
        nHead           = (nHead + 1) % nSize;
        nTail           = (nTail + 1) % nSize;

        return ret;
    }

    float Delay::process(float src, float gain)
    {
        pBuffer[nHead]  = src;
        float ret       = pBuffer[nTail] * gain;
        nHead           = (nHead + 1) % nSize;
        nTail           = (nTail + 1) % nSize;

        return ret;
    }

    void Delay::set_delay(size_t delay)
    {
        delay      %= nSize;
        nDelay      = delay;
        nTail       = (nHead + nSize - delay) % nSize;
    }

    void Delay::clear()
    {
        if (pBuffer == NULL)
            return;
        dsp::fill_zero(pBuffer, nSize);
    }

    void Delay::dump(IStateDumper *v) const
    {
        v->write("pBuffer", pBuffer);
        v->write("nHead", nHead);
        v->write("nTail", nTail);
        v->write("nDelay", nDelay);
        v->write("nSize", nSize);
    }

} /* namespace lsp */
