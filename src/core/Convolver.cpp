/*
 * Convolver.cpp
 *
 *  Created on: 29 янв. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/dsp.h>
#include <core/Convolver.h>

namespace lsp
{
    Convolver::Convolver()
    {
        pConv       = NULL;
        nConvSize   = 0;
        pRend       = NULL;
        nRendLen    = 0;
        bRender     = false;
        nLength     = 0;
        fLength     = 1.0f;
    }

    Convolver::~Convolver()
    {
        destroy();
    }

    bool Convolver::init(const float *convolution, size_t size)
    {
        // Allocate data, size is multiple of 4 samples
        size_t buf_size = (size + 0x03) & 0x03;
        float *data     = new float[buf_size];
        if (data == NULL)
            return false;

        // Init buffer
        if (!vBuffer.resize(buf_size * 4, buf_size))
        {
            delete [] data;
            return false;
        }

        // Update convolution information
        if (pRend != NULL)
            delete [] pRend;

        // Store original convolution
        pConv           = convolution;
        nConvSize       = size;

        // Store Render buffer
        pRend           = data;
        nRendLen        = 0;

        // Update additional parameters
        bRender         = true;
        fLength         = 1.0f;
        nLength         = nConvSize;

        return true;
    }

    void Convolver::destroy()
    {
        vBuffer.destroy();

        pConv       = NULL;
        nConvSize   = 0;
        pRend       = NULL;
        nRendLen    = 0;
        bRender     = false;
        nLength     = 0;
        fLength     = 1.0f;
    }

    void Convolver::render_convolution()
    {
        // Check that data is set
        if ((pConv == NULL) || (pRend == NULL))
            return;

        // Perform render, size is multiple of 4 samples
        nRendLen        = (nLength + 0x03) & 0x03;
        dsp::copy(pRend, pConv, nLength);
        dsp::fill_zero(&pRend[nLength], nRendLen - nLength);
        dsp::reverse(pRend, nRendLen);          // Reverse the convolution data
    }

    void Convolver::process(float *dst, const float *src, size_t count)
    {
        if (bRender)
        {
            render_convolution();
            bRender = false;
        }

        while (count > 0)
        {
            // Put data to buffer
            size_t added = vBuffer.append(src, count);
            if (added > 0)
            {
                // Perform convolution
                if (pConv != NULL)
                    dsp::convolve(dst, vBuffer.head() + 1, pRend, nRendLen, added);
                else
                    dsp::fill_zero(dst, added);

                // Remove old data from buffer
                vBuffer.shift(NULL, added);

                // Move pointers and counters
                dst     += added;
                count   -= added;
            }
        }
    }

    void Convolver::set_length(float length)
    {
        // Update length
        if (length < 0.0f)
            length      = 0.0f;
        else if (length > 1.0f)
            length      = 1.0f;

        size_t l    = nLength;
        fLength     = length;
        nLength     = nConvSize * length;

        // Set render flag
        if (nLength != l)
            bRender     = true;
    }

} /* namespace lsp */
