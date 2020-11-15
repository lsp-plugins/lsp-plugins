/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 1 июл. 2020 г.
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

#include <core/windows.h>
#include <core/util/SpectralProcessor.h>
#include <dsp/dsp.h>

namespace lsp
{
    
    SpectralProcessor::SpectralProcessor()
    {
        construct();
    }

    SpectralProcessor::~SpectralProcessor()
    {
        destroy();
    }

    void SpectralProcessor::construct()
    {
        nRank           = 0;
        nMaxRank        = 0;
        fPhase          = 0.0f;
        pWnd            = NULL;
        pOutBuf         = NULL;
        pInBuf          = NULL;
        pFftBuf         = NULL;
        pData           = NULL;
        nOffset         = 0;
        bUpdate         = true;

        pFunc           = NULL;
        pObject         = NULL;
        pSubject        = NULL;
    }

    bool SpectralProcessor::init(size_t max_rank)
    {
        nRank           = max_rank;
        nMaxRank        = max_rank;
        fPhase          = 0.0f;
        bUpdate         = true;

        pFunc           = NULL;
        pObject         = NULL;
        pSubject        = NULL;

        // Allocate buffer
        size_t buf_sz   = sizeof(float) << max_rank;
        pWnd            = alloc_aligned<float>(pData, buf_sz * 5, DEFAULT_ALIGN);

        return true;
    }

    void SpectralProcessor::destroy()
    {
        if (pData != NULL)
        {
            free_aligned(pData);
            pData           = NULL;
        }

        nRank           = 0;
        nMaxRank        = 0;
        fPhase          = 0.0f;
        pWnd            = NULL;
        pOutBuf         = NULL;
        pInBuf          = NULL;
        pFftBuf         = NULL;
        pData           = NULL;
        bUpdate         = false;

        pFunc           = NULL;
        pObject         = NULL;
        pSubject        = NULL;
    }

    void SpectralProcessor::bind(spectral_processor_func_t func, void *object, void *subject)
    {
        pFunc           = func;
        pObject         = object;
        pSubject        = subject;
    }

    void SpectralProcessor::unbind()
    {
        pFunc           = NULL;
        pObject         = NULL;
        pSubject        = NULL;
    }

    void SpectralProcessor::update_settings()
    {
        // Distribute buffers
        size_t buf_size = 1 << nRank;

        pOutBuf         = &pWnd[buf_size];
        pInBuf          = &pOutBuf[buf_size];
        pFftBuf         = &pInBuf[buf_size];

        // Clear buffers and reset pointers
        windows::sqr_cosine(pWnd, buf_size);
        dsp::fill_zero(pOutBuf, buf_size*4);     // OutBuf + InBuf + Fft(x2)
        nOffset         = buf_size * fPhase;

        // Mark settings applied
        bUpdate         = false;
    }

    void SpectralProcessor::set_phase(float phase)
    {
        fPhase          = (phase < 0.0f) ? 0.0f : (phase > 1.0f) ? 1.0f : phase;
        bUpdate         = true;
    }

    void SpectralProcessor::set_rank(size_t rank)
    {
        if ((rank == nRank) || (rank > nMaxRank))
            return;

        nRank           = rank;
        bUpdate         = true;
    }

    void SpectralProcessor::process(float *dst, const float *src, size_t count)
    {
        // Check if we need to commit new settings
        if (bUpdate)
            update_settings();

        size_t buf_size     = 1 << nRank;
        size_t frame_size   = 1 << (nRank - 1);

        while (count > 0)
        {
            // Need to perform transformations?
            if (nOffset >= frame_size)
            {
                if (pFunc != NULL)
                {
                    // Perform FFT and processing
                    dsp::pcomplex_r2c(pFftBuf, pInBuf, buf_size);       // Convert from real to packed complex
                    dsp::packed_direct_fft(pFftBuf, pFftBuf, nRank);    // Perform direct FFT
                    pFunc(pObject, pSubject, pFftBuf, nRank);           // Call the function
                    dsp::packed_reverse_fft(pFftBuf, pFftBuf, nRank);   // Perform reverse FFT
                    dsp::pcomplex_c2r(pFftBuf, pFftBuf, buf_size);      // Unpack complex numbers
                }
                else
                    dsp::move(pFftBuf, pInBuf, buf_size);               // Copy data to FFT buffer

                // Shift input and output buffers
                dsp::move(pOutBuf, &pOutBuf[frame_size], buf_size + frame_size);  // Shift buffers
                dsp::fill_zero(&pOutBuf[frame_size], frame_size);       // Fill tail of input buffer with zeros

                // Apply window and add to the output buffer
                dsp::fmadd3(pOutBuf, pFftBuf, pWnd, buf_size);          // Apply window and

                // Reset read/write offset
                nOffset     = 0;
            }

            // Estimate number of samples to process
            size_t to_process   = frame_size - nOffset;
            if (to_process > count)
                to_process          = count;

            // Copy data
            dsp::copy(&pInBuf[frame_size + nOffset], src, to_process);
            dsp::copy(dst, &pOutBuf[nOffset], to_process);

            // Update pointers
            nOffset    += to_process;
            count      -= to_process;
            dst        += to_process;
            src        += to_process;
        }
    }

    void SpectralProcessor::dump(IStateDumper *v) const
    {
        v->write("nRank", nRank);
        v->write("nMaxRank", nMaxRank);
        v->write("fPhase", fPhase);
        v->write("pWnd", pWnd);
        v->write("pOutBuf", pOutBuf);
        v->write("pInBuf", pInBuf);
        v->write("pFftBuf", pFftBuf);
        v->write("nOffset", nOffset);
        v->write("pData", pData);
        v->write("bUpdate", bUpdate);

        v->write("pFunc", pFunc);
        v->write("pObject", pObject);
        v->write("pSubject", pSubject);
    }

} /* namespace lsp */
