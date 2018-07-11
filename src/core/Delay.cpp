/*
 * Delay.cpp
 *
 *  Created on: 06 дек. 2015 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/Delay.h>

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
        size_t size = 1;
        while (size < (max_size + 1))
            size    <<= 1;

        lsp_trace("max_size = %d, size = %d", int(max_size), int(size));

        pBuffer     = new float[size];
        if (pBuffer == NULL)
            return false;

        dsp::fill_zero(pBuffer, size);
        nHead       = 0;
        nTail       = 0;
        nDelay      = 0;
        nSize       = size;

        return true;
    }

    void Delay::destroy()
    {
        if (pBuffer != NULL)
        {
            delete [] pBuffer;
            pBuffer     = NULL;
        }
    }

    void Delay::process(float *dst, const float *src, size_t count)
    {
//        lsp_trace("dsp = %p, dst = %p, src = %p, count = %d", dsp, dst, src, int(count));

        size_t in       = count, out = count;
        size_t free     = nSize - nDelay;
        size_t busy     = nDelay;

        while ((in > 0) && (out > 0))
        {
            // Put to buffer at HEAD
            while ((in > 0) && (free > 0))
            {
                size_t to_copy      = nSize - nHead;
                if (to_copy > free)
                    to_copy         = free;
                if (to_copy > in)
                    to_copy         = in;

                dsp::copy(&pBuffer[nHead], src, to_copy);
                nHead       = (nHead + to_copy) % nSize;
                in         -= to_copy;
                src        += to_copy;
                busy       += to_copy;
            }

            // Read from buffer at TAIL
            while ((out > 0) && (busy > 0))
            {
                size_t to_copy      = nSize - nTail;
                if (to_copy > busy)
                    to_copy         = busy;
                if (to_copy > out)
                    to_copy         = out;

                dsp::copy(dst, &pBuffer[nTail], to_copy);
                nTail       = (nTail + to_copy) % nSize;
                out        -= to_copy;
                dst        += to_copy;
                free       += to_copy;
            }
        }
    }

    void Delay::process(float *dst, const float *src, float gain, size_t count)
    {
//        lsp_trace("dsp = %p, dst = %p, src = %p, count = %d", dsp, dst, src, int(count));

        size_t in       = count, out = count;
        size_t free     = nSize - nDelay;
        size_t busy     = nDelay;

        while ((in > 0) && (out > 0))
        {
            // Put to buffer at HEAD
            while ((in > 0) && (free > 0))
            {
                size_t to_copy      = nSize - nHead;
                if (to_copy > free)
                    to_copy         = free;
                if (to_copy > in)
                    to_copy         = in;

                dsp::copy(&pBuffer[nHead], src, to_copy);
                nHead       = (nHead + to_copy) % nSize;
                in         -= to_copy;
                src        += to_copy;
                busy       += to_copy;
            }

            // Read from buffer at TAIL
            while ((out > 0) && (busy > 0))
            {
                size_t to_copy      = nSize - nTail;
                if (to_copy > busy)
                    to_copy         = busy;
                if (to_copy > out)
                    to_copy         = out;

                dsp::scale(dst, &pBuffer[nTail], gain, to_copy);
                nTail       = (nTail + to_copy) % nSize;
                out        -= to_copy;
                dst        += to_copy;
                free       += to_copy;
            }
        }
    }

    float Delay::process(float src)
    {
        if (nDelay == 0)
            return src;

        pBuffer[nTail]  = src;
        float ret       = pBuffer[nHead];
        nTail           = (nTail + 1) % nSize;
        nHead           = (nHead + 1) % nSize;

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

} /* namespace lsp */
