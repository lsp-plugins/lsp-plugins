/*
 * osclilloscope.cpp
 *
 *  Created on: 1 Mar 2020
 *      Author: crocoduck
 */

#include <plugins/oscilloscope.h>
#include <core/debug.h>

#define TRACE_PORT(p)       lsp_trace("  port id=%s", (p)->metadata()->id);
#define BUF_LIM_SIZE        196608
#define PRE_TRG_MAX_SIZE    196608
#define N_HOR_DIVISIONS     4
#define N_VER_DIVISIONS     2
#define VER_FULL_SCALE_AMP  1.0f

namespace lsp
{
    void oscilloscope_base::calculate_output(float *dst, float *src, size_t count, ch_output_mode_t mode)
    {
        switch (mode)
        {
            case CH_OUTPUT_MODE_MUTE: dsp::fill_zero(dst, count); break;
            default:
            case CH_OUTPUT_MODE_COPY: dsp::copy(dst, src, count); break;
        }
    }

    bool oscilloscope_base::fill_display_buffers(channel_t *c, float *xBuf, float *yBuf, size_t bufSize)
    {
        size_t remaining = c->nSweepSize - c->nDisplayHead;
        size_t to_copy = (bufSize < remaining) ? bufSize : remaining;

        dsp::copy(&c->vDisplay_x[c->nDisplayHead], c->vData_x, to_copy);
        dsp::copy(&c->vDisplay_y[c->nDisplayHead], c->vData_y, to_copy);

        c->nDisplayHead += to_copy;

        return c->nDisplayHead >= c->nSweepSize;
    }

    void oscilloscope_base::reset_display_buffers(channel_t *c)
    {
        c->nDisplayHead = 0;
    }

    float *oscilloscope_base::select_trigger_input(float *extPtr, float* yPtr, ch_trg_input_t input)
    {
        switch (input)
        {
            case CH_TRG_INPUT_EXT: return extPtr;
            default:
            case CH_TRG_INPUT_Y: return yPtr;
        }
    }

//    void oscilloscope_base::route_display_data(float *raw_data, float *proc_data, float *display, size_t count, ch_mode_t mode)
//    {
//        switch (mode)
//        {
//            case CH_MODE_XY: dsp::copy(display, raw_data, count); break;
//            default:
//            case CH_MODE_DFL:
//            case CH_MODE_TRIGGERED: dsp::copy(display, proc_data, count); break;
//        }
//    }

    void oscilloscope_base::set_oversampler(Oversampler &over, over_mode_t mode)
    {
        over.set_mode(mode);
        if (over.modified())
            over.update_settings();
    }

//    void oscilloscope_base::get_plottable_data(float *dst, float *src, size_t dstCount, size_t srcCount)
//    {
//        dsp::fill_zero(dst, dstCount);
//
//        float decimationStep = float(srcCount) / float(dstCount);
//
//        if (decimationStep == 1.0f) // Nothing to do
//        {
//            dsp::copy(dst, src, srcCount);
//        }
//        else if (decimationStep < 1.0f) // Zero filling upsampling.
//        {
//            size_t plotDataHead = 0;
//
//            for (size_t n = 0; n < srcCount; ++n)
//            {
//                dst[plotDataHead] = src[n];
//                plotDataHead  += (1.0f / decimationStep);
//
//                if (plotDataHead >= dstCount)
//                    break;
//            }
//        }
//        else // Decimation downsampling
//        {
//            size_t plotDataHead         = 0;
//            size_t plotDataDownLimit    = 0;
//            size_t plotDataRange        = decimationStep - 1.0f;
//
//            for (size_t n = 0; n < dstCount; ++n)
//            {
//                plotDataHead       = dsp::abs_max_index(&src[plotDataDownLimit], plotDataRange) + plotDataDownLimit;
//                dst[n]             = src[plotDataHead];
//                plotDataDownLimit += decimationStep;
//
//                if (plotDataDownLimit >= srcCount)
//                    break;
//
//                size_t samplesAhead = srcCount - plotDataDownLimit;
//                plotDataRange       = (plotDataRange > samplesAhead) ? samplesAhead : plotDataRange;
//            }
//        }
//    }

    oscilloscope_base::oscilloscope_base(const plugin_metadata_t &metadata, size_t channels): plugin_t(metadata)
    {
        nChannels           = channels;
        vChannels           = NULL;

        nSampleRate         = 0;

//        nCaptureSize        = 0;
//
//        nMeshSize           = 0;
//
//        vTemp               = NULL;
//
//        vDflAbscissa        = NULL;

        pBypass             = NULL;

        pData               = NULL;
    }

    oscilloscope_base::~oscilloscope_base()
    {
    }

    void oscilloscope_base::destroy()
    {
        free_aligned(pData);
        pData = NULL;
//        vTemp = NULL;
//        vDflAbscissa = NULL;

        if (vChannels != NULL)
        {
            for (size_t ch = 0; ch < nChannels; ++ch)
            {
                channel_t *c = &vChannels[ch];

                c->sOversampler_x.destroy();
                c->sOversampler_y.destroy();
                c->sOversampler_ext.destroy();

                c->sPreTrgDelay.destroy();

                c->sSweepGenerator.destroy();

                c->vData_x = NULL;
                c->vData_y = NULL;
                c->vData_ext = NULL;
                c->vData_y_delay = NULL;
                c->vDisplay_x = NULL;
                c->vDisplay_y = NULL;
            }

            delete [] vChannels;
            vChannels = NULL;
        }
    }

    void oscilloscope_base::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        vChannels = new channel_t[nChannels];
        if (vChannels == NULL)
            return;

        // For each channel: 1X external data buffer + 1X x data buffer + 1X y data buffer + 1X delayed y data buffer + 1X x display buffer + 1X y display buffer
//        nCaptureSize = BUF_LIM_SIZE;
//        nMeshSize = oscilloscope_base_metadata::SCOPE_MESH_SIZE;
//        size_t samples = (2 * nChannels * nMeshSize) + (nChannels * nCaptureSize) + (nChannels * nCaptureSize) + nCaptureSize + nMeshSize;
        size_t samples = nChannels * BUF_LIM_SIZE * 6;

        float *ptr = alloc_aligned<float>(pData, samples);
        if (ptr == NULL)
            return;

        lsp_guard_assert(float *save = ptr);

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            if (!c->sOversampler_x.init())
                return;

            if (!c->sOversampler_y.init())
                return;

            if (!c->sOversampler_ext.init())
                return;

            if (!c->sPreTrgDelay.init(PRE_TRG_MAX_SIZE))
                return;

            // Test settings for oversampler before proper implementation
            c->enOverMode = OM_LANCZOS_8X3;

            set_oversampler(c->sOversampler_x, c->enOverMode);
            set_oversampler(c->sOversampler_y, c->enOverMode);
            set_oversampler(c->sOversampler_ext, c->enOverMode);

            // All are set the same way, use any to get these variables
            c->nOversampling = c->sOversampler_x.get_oversampling();
            c->nOverSampleRate = c->nOversampling * nSampleRate;

//            if (!c->sShiftBuffer.init(nCaptureSize))
//                return;

//            c->nSamplesCounter = 0;
//            c->nBufferScanningHead = 0;
//            c->nBufferCopyHead = 0;
//            c->nBufferCopyCount = 0;
//
//            c->vAbscissa = ptr;
//            ptr += nMeshSize;
//
//            c->vOrdinate = ptr;
//            ptr += nMeshSize;
//
//            c->vOutput = ptr;
//            ptr += nCaptureSize;
//
//            c->vSweep = ptr;
//            ptr += nCaptureSize;

            c->vData_x = ptr;
            ptr += BUF_LIM_SIZE;

            c->vData_y = ptr;
            ptr += BUF_LIM_SIZE;

            c->vData_ext = ptr;
            ptr += BUF_LIM_SIZE;

            c->vData_y_delay = ptr;
            ptr += BUF_LIM_SIZE;

            c->vDisplay_x = ptr;
            ptr += BUF_LIM_SIZE;

            c->vDisplay_y = ptr;
            ptr += BUF_LIM_SIZE;

            c->enState      = CH_STATE_LISTENING;

            c->vIn_x        = NULL;
            c->vIn_y        = NULL;
            c->vIn_ext      = NULL;

            c->vOut_x       = NULL;
            c->vOut_y       = NULL;

            c->pIn_x        = NULL;
            c->pIn_y        = NULL;
            c->pIn_ext      = NULL;

            c->pOut_x       = NULL;
            c->pOut_y       = NULL;

            c->pHorDiv      = NULL;
            c->pHorPos      = NULL;

            c->pVerDiv      = NULL;
            c->pVerPos      = NULL;

            c->pTrgHys      = NULL;
            c->pTrgLev      = NULL;
            c->pTrgMode     = NULL;
            c->pTrgType     = NULL;

            c->pCoupling    = NULL;

            c->pMesh        = NULL;
        }

        lsp_assert(ptr <= &save[samples]);

//        vTemp = ptr;
//        ptr += nCaptureSize;
//
//        vDflAbscissa = ptr;
//        ptr += nMeshSize;
//
//        lsp_assert(ptr <= &save[samples]);
//
//        // Fill default abscissa
//        for (size_t n = 0; n < nMeshSize; ++n)
//            vDflAbscissa[n] = float(2 * n) / nMeshSize;

        // Bind ports
        size_t port_id = 0;

        // Audio
        lsp_trace("Binding audio ports");

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pIn_x = vPorts[port_id++];
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pIn_y = vPorts[port_id++];
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pIn_ext = vPorts[port_id++];
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pOut_x = vPorts[port_id++];
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pOut_y = vPorts[port_id++];
        }

        // Common
        lsp_trace("Binding common ports");
        pBypass = vPorts[port_id++];

        // Channels
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pHorDiv = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pHorPos = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pVerDiv = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pVerPos = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pTrgHys = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pTrgLev = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pTrgMode = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pTrgType = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pCoupling = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pMesh = vPorts[port_id++];
        }
    }

    void oscilloscope_base::update_settings()
    {
        bool bPassValue = pBypass->getValue() >= 0.5f;

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            c->sBypass.set_bypass(bPassValue);

            float verDiv = c->pVerDiv->getValue();
            float verPos = c->pVerPos->getValue();

            c->fScale = VER_FULL_SCALE_AMP / (verDiv * N_VER_DIVISIONS);
            c->fOffset = 0.01f * verPos * N_VER_DIVISIONS * verDiv;

            float horDiv = c->pHorDiv->getValue();
            float horPos = c->pHorPos->getValue();

            c->nSweepSize = N_HOR_DIVISIONS * seconds_to_samples(c->nOverSampleRate, horDiv);
            c->nPreTrigger = (0.01f * horPos  + 1) * (c->nSweepSize - 1) / 2;

            c->sPreTrgDelay.set_delay(c->nPreTrigger);
//            c->nPostTrigger = c->nSweepSize - c->nPreTrigger - 1;

            float trgLevel = c->pTrgLev->getValue();

//            c->sTrigger.set_post_trigger_samples(c->nPostTrigger);
            c->sTrigger.set_trigger_type(TRG_TYPE_SIMPLE_RISING_EDGE);
            c->sTrigger.set_trigger_threshold(0.01f * trgLevel * N_VER_DIVISIONS * verDiv);
            c->sTrigger.update_settings();
        }
    }

    void oscilloscope_base::update_sample_rate(long sr)
    {
        nSampleRate = sr;

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            c->sBypass.init(sr);

            c->sOversampler_x.set_sample_rate(sr);
            c->sOversampler_y.set_sample_rate(sr);
            c->sOversampler_ext.set_sample_rate(sr);

            c->nOverSampleRate = c->nOversampling * nSampleRate;
        }
    }

    void oscilloscope_base::process(size_t samples)
    {
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            c->vIn_x = c->pIn_x->getBuffer<float>();
            c->vIn_y = c->pIn_y->getBuffer<float>();
            c->vIn_ext = c->pIn_ext->getBuffer<float>();

            c->vOut_x = c->pOut_x->getBuffer<float>();
            c->vOut_y = c->pOut_y->getBuffer<float>();

            if ((c->vIn_x == NULL) || (c->vOut_x == NULL))
                return;

            if ((c->vIn_y == NULL) || (c->vOut_y == NULL))
                return;

            if ((c->vIn_ext == NULL))
                return;

            c->nSamplesCounter = samples;
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            calculate_output(c->vOut_x, c->vIn_x, samples, c->enOutputMode);
            calculate_output(c->vOut_y, c->vIn_y, samples, c->enOutputMode);
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            while (c->nSamplesCounter > 0)
            {
                size_t requested        = c->nOversampling * c->nSamplesCounter;
                size_t availble         = BUF_LIM_SIZE;
                size_t to_do_upsample   = (requested < availble) ? requested : availble;
                size_t to_do            = to_do_upsample / c->nOversampling;

                switch (c->enMode)
                {
                    case CH_MODE_XY:
                    {
                        c->sOversampler_x.upsample(c->vData_x, c->vIn_x, to_do);
                        c->sOversampler_y.upsample(c->vData_y, c->vIn_y, to_do);

                        if (fill_display_buffers(c, c->vData_x, c->vData_y, to_do_upsample))
                        {
                            // Plot stuff happens here

                            reset_display_buffers(c);
                        }
                    }
                    break;

                    case CH_MODE_TRIGGERED:
                    {
                        c->sOversampler_y.upsample(c->vData_y, c->vIn_y, to_do);
                        c->sPreTrgDelay.process(c->vData_y_delay, c->vData_y, to_do_upsample);
                        c->sOversampler_ext.upsample(c->vData_ext, c->vIn_ext, to_do);

                        float *trg_input = select_trigger_input(c->vData_ext, c->vData_y, c->enTrgInput);

                        for (size_t n = 0; n < to_do_upsample; ++n)
                        {
                            c->sTrigger.single_sample_processor(trg_input[n]);

                            switch (c->enState)
                            {
                                case CH_STATE_LISTENING:
                                {
                                    if (c->sTrigger.get_trigger_state() == TRG_STATE_FIRED)
                                    {
                                        c->sSweepGenerator.reset();
                                        c->sSweepGenerator.sweep(c->vData_x, c->nSweepSize);
                                        c->enState = CH_STATE_SWEEPING;
                                    }
                                }
                                break;

                                case CH_STATE_SWEEPING:
                                {
                                    if (fill_display_buffers(c, c->vData_x, c->vData_y_delay, to_do_upsample))
                                    {
                                        // Plot stuff happens here

                                        reset_display_buffers(c);
                                    }
                                }
                                break;
                            }
                        }
                    }
                    break;
                }

                c->vIn_x            += to_do;
                c->vIn_y            += to_do;
                c->vIn_ext          += to_do;
                c->vOut_x           += to_do;
                c->vOut_y           += to_do;
                c->nSamplesCounter  -= to_do;
            }
        }
    }

    oscilloscope_x1::oscilloscope_x1(): oscilloscope_base(metadata, 1)
    {
    }

    oscilloscope_x1::~oscilloscope_x1()
    {
    }

    oscilloscope_x2::oscilloscope_x2(): oscilloscope_base(metadata, 2)
    {
    }

    oscilloscope_x2::~oscilloscope_x2()
    {
    }
}
