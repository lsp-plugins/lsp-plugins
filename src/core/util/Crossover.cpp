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
    ICrossHandler::ICrossHandler()
    {
    }

    ICrossHandler::~ICrossHandler()
    {
    }

    void ICrossHandler::process(size_t band_id, float *out, const float *in, size_t samples)
    {
        if (out != in)
            dsp::copy(out, in, samples);
    }

    Crossover::Crossover()
    {
        nBands          = 0;
        nBufSize        = 0;
        nSlope          = 1;
        nSampleRate     = DEFAULT_SAMPLE_RATE;
        vBands          = NULL;
        vSplit          = NULL;
        vTasks          = NULL;
        vBuffers        = NULL;
        pHandler        = NULL;
    }

    Crossover::~Crossover()
    {
        destroy();
    }

    bool Crossover::init(size_t bands, size_t buf_size)
    {
        destroy();

        vBands          = new band_t[bands];
        if (vBands == NULL)
            return false;

        vSplit          = new splitpoint_t[bands-1];
        if (vSplit == NULL)
        {
            delete [] vBands;
            vBands          = NULL;
            return false;
        }

        buf_size        = ALIGN_SIZE(buf_size, DEFAULT_ALIGN);
        vBuffers        = new float[(bands + 2) * buf_size];
        if (vBuffers == NULL)
        {
            delete [] vBands;
            delete [] vSplit;
            vBands          = NULL;
            vSplit          = NULL;
            return false;
        }

        vTasks          = new task_t[bands-1];
        if (vTasks == NULL)
        {
            delete [] vBands;
            delete [] vSplit;
            delete [] vBuffers;
            vBands          = NULL;
            vSplit          = NULL;
            vBuffers        = NULL;
            return false;
        }

        // Initialize additional parameters
        nBands          = bands;
        nBufSize        = buf_size;
        nSlope          = 1;
        pHandler        = NULL;

        // Initialize split points
        for (size_t i=0; i<(nBands-1); ++i)
        {
            splitpoint_t *sp    = &vSplit[i];
            if (!sp->sLoPass.init(NULL))
            {
                destroy();
                return false;
            }
            if (!sp->sHiPass.init(NULL))
            {
                destroy();
                return false;
            }
            sp->pLoBand     = &vBands[i];       // Bind lo-band
            sp->pHiBand     = &vBands[i+1];     // Bind hi-band
            sp->fFreq       = 1.0f;
            sp->bChanged    = true;
        }

        // Initialize bands
        float *pbuf     = vBuffers;
        pbuf           += buf_size*2;           // Reserve buffer for frequency chart processing
        for (size_t i=0; i<nBands; ++i)
        {
            band_t *b       = &vBands[i];
            b->pLeft        = (i > 0) ? &vSplit[i-1] : NULL;
            b->pRight       = (i < (nBands-1)) ? &vSplit[i] : NULL;
            b->fAmp         = 1.0f;
            b->vBuffer      = pbuf;
            pbuf           += buf_size;
        }

        // Create processing plan
        build_tasks();

        // Reconfigure filter
        reconfigure();

        return true;
    }

    void Crossover::build_tasks()
    {
        size_t n_tasks = 0;
        if (nBands <= 1)
        {
            lsp_trace("SPLIT PLAN:");
            lsp_trace("  single channel processing");
            return;
        }

        // Create first split task
        task_t *t       = &vTasks[n_tasks++];
        t->vInBuf       = NULL;

        build_task(n_tasks, 0, nBands, t);

        // Trace the schema
#ifdef LSP_TRACE
        lsp_trace("SPLIT PLAN:");
        for (size_t i=0; i<(nBands-1); ++i)
        {
            t           = &vTasks[i];
            lsp_trace("  Task #%02d:", int(i));

            if (t->vLoBuf == t->vInBuf)
            {
                lsp_trace("    1: %p --HP-> %p", t->vInBuf, t->vHiBuf);
                lsp_trace("    2: %p --LP-> %p", t->vInBuf, t->vLoBuf);
            }
            else
            {
                lsp_trace("    1: %p --LP-> %p", t->vInBuf, t->vLoBuf);
                lsp_trace("    2: %p --HP-> %p", t->vInBuf, t->vHiBuf);
            }
        }
#endif /* LSP_TRACE */
    }

    void Crossover::build_task(size_t &n_tasks, size_t left, size_t bands, task_t *task)
    {
        // Determine the split index
        size_t center   = 1;
        while ((center << 1) < bands)
            center <<= 1;

        size_t lband    = left + center - 1;

        // Now we know the ID of split point
        splitpoint_t *sp= &vSplit[lband];
        task->pSplit    = sp;
        task->vLoBuf    = vBands[lband].vBuffer;
        task->vHiBuf    = vBands[lband + 1].vBuffer;

        // Create split point from left
        size_t lbands   = center;
        size_t rbands   = bands - lbands;

        if (lbands > 1)
        {
            task_t *lt      = &vTasks[n_tasks++];
            lt->vInBuf      = task->vLoBuf;
            build_task(n_tasks, left, lbands, lt);
        }

        // Create split point from right
        if (rbands > 1)
        {
            task_t *lt      = &vTasks[n_tasks++];
            lt->vInBuf      = task->vHiBuf;
            build_task(n_tasks, lband + 1, rbands, lt);
        }
    }

    void Crossover::destroy()
    {
        if (vBuffers != NULL)
        {
            delete [] vBuffers;
            vBuffers    = NULL;
        }

        if (vSplit != NULL)
        {
            for (size_t i=0; i<(nBands-1); ++i)
            {
                splitpoint_t *sp = &vSplit[i];
                sp->sHiPass.destroy();
                sp->sLoPass.destroy();
                sp->pLoBand     = NULL;
                sp->pHiBand     = NULL;
            }
            delete [] vSplit;
            vSplit      = NULL;
        }

        if (vBands != NULL)
        {
            for (size_t i=0; i<(nBands-1); ++i)
            {
                vBands[i].pLeft     = NULL;
                vBands[i].pRight    = NULL;
                vBands[i].vBuffer   = NULL;
            }
            delete [] vBands;
            vBands      = NULL;
        }
    }

    void Crossover::set_slope(size_t slope)
    {
        nSlope      = slope;
        for (size_t i=0; i<(nBands-1); ++i)
            vSplit[i].bChanged      = true;
    }

    bool Crossover::set_frequency(size_t sp, float freq)
    {
        size_t points   = nBands - 1;
        if (sp >= points)
            return false;

        vSplit[sp].fFreq        = freq;
        vSplit[sp].bChanged     = true;

        return true;
    }

    bool Crossover::set_gain(size_t band, float gain)
    {
        if (band >= nBands)
            return false;
        vBands[band].fAmp       = gain;
        return true;
    }

    void Crossover::reconfigure()
    {
        filter_params_t fp;
        fp.fGain        = 1.0f;
        fp.fQuality     = 0.0f;
        fp.nSlope       = nSlope;

        // Process each split point
        size_t points   = nBands - 1;
        for (size_t i=0; i<points; ++i)
        {
            splitpoint_t *sp = &vSplit[i];
            if (!sp->bChanged)
                continue;

            // Set-up frequency
            fp.fFreq    = sp->fFreq;
            fp.fFreq2   = sp->fFreq;

            // Update lo-pass filter
            fp.nType    = FLT_BT_LRX_LOPASS;
            sp->sLoPass.update(nSampleRate, &fp);

            // Update hi-pass filter
            fp.nType    = FLT_BT_LRX_HIPASS;
            sp->sHiPass.update(nSampleRate, &fp);

            // Clear changed flag
            sp->bChanged= false;
        }
    }

    void Crossover::process(float *out, const float *in, size_t samples)
    {
        // Test for single channel processing
        if (nBands <= 1)
        {
            // Apply single channel processing
            if (pHandler != NULL)
                pHandler->process(0, out, in, samples);
            else
                dsp::scale3(out, in, vBands[0].fAmp, samples);

            // And return
            return;
        }

        // Multi-channel processing
        vTasks[0].vInBuf    = in;

        while (samples > 0)
        {
            // Determine number of samples to process
            size_t to_process   = (samples > nBufSize) ? nBufSize : samples;

            // Split input buffer into bands
            for (size_t i=0; i<(nBands-1); ++i)
            {
                band_t *left        = &vBands[i];
                band_t *right       = &vBands[i+1];
                splitpoint_t *sp    = &vSplit[i];

                sp->sHiPass.process(right->vBuffer, in, to_process);
                sp->sLoPass.process(left->vBuffer, in, to_process);
                in                  = right->vBuffer;
            }

            // Call handler for individual processing
            if (pHandler != NULL)
            {
                for (size_t i=0; i<nBands; ++i)
                {
                    band_t *b   = &vBands[i];
                    pHandler->process(i, b->vBuffer, b->vBuffer, to_process);
                }
            }

            // Mix output buffer
            if ((out != NULL) && (nBands > 0))
            {
                dsp::fill_zero(out, to_process);
                for (size_t i=0; i<nBands; ++i)
                    dsp::scale_add3(out, vBands[i].vBuffer, vBands[i].fAmp, to_process);

                out        += to_process;
            }

            // Move pointers
            vTasks[0].vInBuf   += to_process;
            samples            -= to_process;
        }
    }

    bool Crossover::freq_chart(float *re, float *im, const float *f, size_t count)
    {
//        dsp::fill_one(re, count);
//        dsp::fill_zero(im, count);
//        return true;

        // For single band the frequency chart is simple
        if (nBands <= 1)
        {
            dsp::fill(re, vBands[0].fAmp, count);
            dsp::fill_zero(im, count);
            return true;
        }

        // Maximum number of items that can be processed
        size_t max_items    = nBufSize >> 1;
        size_t points       = nBands - 1;
        dsp::fill_zero(re, count);
        dsp::fill_zero(im, count);

        float *tmp_re       = vBuffers;
        float *tmp_im       = &vBuffers[max_items];

        while (count > 0)
        {
            size_t to_process   = (count > max_items) ? max_items : count;

            dsp::fill_one(tmp_re, to_process);
            dsp::fill_zero(tmp_im, to_process);

            for (size_t i=0; i<points; ++i)
            {
                band_t *left        = &vBands[i];
                band_t *right       = &vBands[i+1];
                splitpoint_t *sp    = &vSplit[i];

                // Calculate frequency charts
                sp->sLoPass.freq_chart(left->vBuffer, &left->vBuffer[max_items], f, to_process);
                dsp::complex_mul3(left->vBuffer, &left->vBuffer[max_items], left->vBuffer, &left->vBuffer[max_items], tmp_re, tmp_im, to_process);

                sp->sHiPass.freq_chart(right->vBuffer, &right->vBuffer[max_items], f, to_process);
                dsp::complex_mul3(right->vBuffer, &right->vBuffer[max_items], right->vBuffer, &right->vBuffer[max_items], tmp_re, tmp_im, to_process);

                dsp::copy(tmp_re, right->vBuffer, to_process);
                dsp::copy(tmp_im, &right->vBuffer[max_items], to_process);
            }

            // Now summarize all frequency bands
            for (size_t i=0; i<nBands; ++i)
            {
                band_t *b       = &vBands[i];

                dsp::scale_add3(re, b->vBuffer, b->fAmp, to_process);
                dsp::scale_add3(im, &b->vBuffer[max_items], b->fAmp, to_process);
            }

            // Decrement counter and update pointers
            f                  += to_process;
            re                 += to_process;
            im                 += to_process;
            count              -= to_process;
        }

        return true;
    }

} /* namespace lsp */
