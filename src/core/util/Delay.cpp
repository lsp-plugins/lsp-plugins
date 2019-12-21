/*
 * Delay.cpp
 *
 *  Created on: 06 дек. 2015 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/util/Delay.h>

#define DELAY_GAP       0x200

namespace lsp
{
    
    Delay::Delay()
    {
        pBuffer     = NULL;
        nHead       = 0;
        nTail       = 0;
        nDelay      = 0;
        nSize       = 0;
    }
    
    Delay::~Delay()
    {
        destroy();
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
        lsp_trace("delay = %d", int(delay));

        delay      %= nSize;
        nDelay      = delay;
        nTail       = (nHead + nSize - delay) % nSize;

        lsp_trace("final delay = %d, head=%d, tail=%d", int(nDelay), int(nHead), int(nTail));
    }

    void Delay::clear()
    {
        if (pBuffer == NULL)
            return;
        dsp::fill_zero(pBuffer, nSize);
    }

} /* namespace lsp */
