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
#define N_HOR_DIVISIONS     4

namespace lsp
{
    void oscilloscope_base::get_plottable_data(float *dst, float *src, size_t dstCount, size_t srcCount)
    {
        dsp::fill_zero(dst, dstCount);

        float decimationStep = float(srcCount) / float(dstCount);

        if (decimationStep == 1.0f) // Nothing to do
        {
            dsp::copy(dst, src, srcCount);
        }
        else if (decimationStep < 1.0f) // Zero filling upsampling.
        {
            size_t plotDataHead = 0;

            for (size_t n = 0; n < srcCount; ++n)
            {
                dst[plotDataHead] = src[n];
                plotDataHead  += (1.0f / decimationStep);

                if (plotDataHead >= dstCount)
                    break;
            }
        }
        else // Decimation downsampling
        {
            size_t plotDataHead         = 0;
            size_t plotDataDownLimit    = 0;
            size_t plotDataRange        = decimationStep - 1.0f;

            for (size_t n = 0; n < dstCount; ++n)
            {
                plotDataHead       = dsp::abs_max_index(&src[plotDataDownLimit], plotDataRange) + plotDataDownLimit;
                dst[n]             = src[plotDataHead];
                plotDataDownLimit += decimationStep;

                if (plotDataDownLimit >= srcCount)
                    break;

                size_t samplesAhead = srcCount - plotDataDownLimit;
                plotDataRange       = (plotDataRange > samplesAhead) ? samplesAhead : plotDataRange;
            }
        }
    }

    oscilloscope_base::oscilloscope_base(const plugin_metadata_t &metadata, size_t channels): plugin_t(metadata)
    {
        nChannels           = channels;
        vChannels           = NULL;

        nSampleRate         = 0;

        nCaptureSize        = 0;

        nMeshSize           = 0;

        vTemp               = NULL;

        vDflAbscissa        = NULL;

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
        vTemp = NULL;
        vDflAbscissa = NULL;

        if (vChannels != NULL)
        {
            for (size_t ch = 0; ch < nChannels; ++ch)
            {
                vChannels[ch].sOversampler.destroy();
                vChannels[ch].sShiftBuffer.destroy();
                vChannels[ch].sTrigger.destroy();
                vChannels[ch].vAbscissa = NULL;
                vChannels[ch].vOrdinate = NULL;
                vChannels[ch].vSweep    = NULL;
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

        // 2x nChannels X Mesh Buffers (x, y for each channel) + nChannels X output buffers + nChannels X sweep buffers +  1X temporary buffer + 1X Default Abscissa Buffer.
        nCaptureSize = BUF_LIM_SIZE;
        nMeshSize = oscilloscope_base_metadata::SCOPE_MESH_SIZE;
        size_t samples = (2 * nChannels * nMeshSize) + (nChannels * nCaptureSize) + (nChannels * nCaptureSize) + nCaptureSize + nMeshSize;

        float *ptr = alloc_aligned<float>(pData, samples);
        if (ptr == NULL)
            return;

        lsp_guard_assert(float *save = ptr);

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            if (!c->sOversampler.init())
                return;

            // Test settings for oversampler before proper implementation
            c->enOverMode = OM_LANCZOS_8X3;
            c->sOversampler.set_mode(c->enOverMode);
            if (c->sOversampler.modified())
                c->sOversampler.update_settings();
            c->nOversampling = c->sOversampler.get_oversampling();
            c->nOverSampleRate = c->nOversampling * nSampleRate;

            if (!c->sShiftBuffer.init(nCaptureSize))
                return;

            if (!c->sTrigger.init())
                return;

            c->nSamplesCounter = 0;
            c->nBufferScanningHead = 0;
            c->nBufferCopyHead = 0;
            c->nBufferCopyCount = 0;

            // Test settings for trigger before proper implementation.
            c->nPreTrigger = 256;
            c->nPostTrigger = 255;
            c->nSweepSize = c->nPreTrigger + c->nPostTrigger + 1;
            c->nSweepHead = 0;
            c->sTrigger.set_post_trigger_samples(c->nPostTrigger);
            c->sTrigger.set_trigger_type(TRG_TYPE_SIMPLE_RISING_EDGE);
            c->sTrigger.set_trigger_threshold(0.5f);
            c->sTrigger.update_settings();

            c->vAbscissa = ptr;
            ptr += nMeshSize;

            c->vOrdinate = ptr;
            ptr += nMeshSize;

            c->vOutput = ptr;
            ptr += nCaptureSize;

            c->vSweep = ptr;
            ptr += nCaptureSize;

            c->enState = LISTENING;

            c->vIn          = NULL;
            c->vOut         = NULL;

            c->pIn          = NULL;
            c->pOut         = NULL;

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

        vTemp = ptr;
        ptr += nCaptureSize;

        vDflAbscissa = ptr;
        ptr += nMeshSize;

        lsp_assert(ptr <= &save[samples]);

        // Fill default abscissa
        for (size_t n = 0; n < nMeshSize; ++n)
            vDflAbscissa[n] = float(2 * n) / nMeshSize;

        // Bind ports
        size_t port_id = 0;

        // Audio
        lsp_trace("Binding audio ports");
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pIn = vPorts[port_id++];
        }
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pOut = vPorts[port_id++];
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

            float horDiv = c->pHorDiv->getValue();
            float horPos = c->pHorPos->getValue();

            c->nSweepSize = N_HOR_DIVISIONS * seconds_to_samples(c->nOverSampleRate, horDiv);
            c->nPreTrigger = (0.01f * horPos  + 1) * (c->nSweepSize - 1) / 2;
            c->nPostTrigger = c->nSweepSize - c->nPreTrigger - 1;

//            size_t inertia = (horPos > 0) ? c->nPreTrigger : c->nPostTrigger;
            c->sTrigger.set_post_trigger_samples(c->nPostTrigger);
            c->sTrigger.set_trigger_type(TRG_TYPE_SIMPLE_RISING_EDGE);
            c->sTrigger.set_trigger_threshold(0.5f);
            c->sTrigger.update_settings();

//            size_t to_shift = c->nBufferScanningHead - c->nSweepSize + 1;
//
//            c->sShiftBuffer.shift(to_shift);
//            c->nBufferScanningHead -= to_shift;
//            c->nBufferCopyHead = 0;
//            c->nBufferCopyCount = 0;
//            c->nSweepHead = 0;
//            c->bDoPlot = false;
//            dsp::fill_zero(c->vSweep, c->nSweepSize);
        }
    }

    void oscilloscope_base::update_sample_rate(long sr)
    {
        nSampleRate = sr;

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            vChannels[ch].sBypass.init(sr);
            vChannels[ch].sOversampler.set_sample_rate(sr);
            vChannels[ch].nOverSampleRate = vChannels[ch].nOversampling * nSampleRate;
        }
    }

    void oscilloscope_base::process(size_t samples)
    {
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            vChannels[ch].vIn   = vChannels[ch].pIn->getBuffer<float>();
            vChannels[ch].vOut  = vChannels[ch].pOut->getBuffer<float>();

            if ((vChannels[ch].vIn == NULL) || (vChannels[ch].vOut == NULL))
                return;

            vChannels[ch].nSamplesCounter   = samples;
            vChannels[ch].bProcessComplete  = false;
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            while (c->nSamplesCounter > 0)
            {
                size_t requested        = c->nOversampling * c->nSamplesCounter;
                size_t availble         = c->sShiftBuffer.capacity() - c->sShiftBuffer.size();
                size_t to_do_upsample   = (requested < availble) ? requested : availble;
                size_t to_do            = to_do_upsample / c->nOversampling;

                c->sOversampler.upsample(vTemp, c->vIn, to_do);
                c->sShiftBuffer.append(vTemp, to_do_upsample);

                // In the future, switch with different output modes.
                dsp::fill_zero(c->vOutput, to_do);

                while (c->nBufferScanningHead < c->sShiftBuffer.size())
                {
                    c->sTrigger.single_sample_processor(*c->sShiftBuffer.head(c->nBufferScanningHead));

                    switch (c->enState)
                    {
                        case LISTENING:
                        {
                            if (c->sTrigger.get_trigger_state() == TRG_STATE_FIRED)
                            {
                                if (c->nBufferScanningHead > c->nPreTrigger)
                                {
                                    c->nBufferCopyHead  = c->nBufferScanningHead - c->nPreTrigger;
                                    c->nBufferCopyCount = c->nSweepSize;
                                    c->nSweepHead       = 0;
                                }
                                else
                                {
                                    c->nBufferCopyHead  = 0;
                                    c->nBufferCopyCount = c->nSweepSize - c->nBufferScanningHead;
                                    c->nSweepHead       = c->nPreTrigger - c->nBufferScanningHead;
                                }

                                c->enState = SWEEPING;
                            }
                            else if (c->nBufferScanningHead >= 2 * c->nSweepSize - 1)
                            {
                                c->nBufferCopyHead  = 0;
                                c->nBufferCopyCount = c->nSweepSize;
                                c->nSweepHead       = 0;
                                c->bDoPlot          = true;
                            }
                        }
                        break;

                        case SWEEPING:
                        {
                            if (c->sShiftBuffer.size() - c->nBufferCopyHead >= c->nBufferCopyCount)
                            {
                                c->bDoPlot = true;
                                c->enState = LISTENING;
                            }
                        }
                        break;
                    }

                    ++c->nBufferScanningHead;

                    if (c->bDoPlot)
                    {
                        dsp::copy(&c->vSweep[c->nSweepHead], c->sShiftBuffer.head(c->nBufferCopyHead), c->nBufferCopyCount);

                        mesh_t *mesh = c->pMesh->getBuffer<mesh_t>();

                        if (mesh != NULL)
                        {
                            if (mesh->isEmpty())
                            {
                                get_plottable_data(c->vAbscissa, c->vSweep, nMeshSize, c->nSweepSize);
                                get_plottable_data(c->vOrdinate, c->vSweep, nMeshSize, c->nSweepSize);

                                dsp::copy(mesh->pvData[0], vDflAbscissa, nMeshSize);
                                dsp::copy(mesh->pvData[1], c->vOrdinate, nMeshSize);
                                mesh->data(2, nMeshSize);
                            }
                        }

//                        c->sShiftBuffer.shift(c->nBufferScanningHead + 1);
//                        c->nBufferScanningHead = 0;

                        size_t to_shift = c->nBufferScanningHead - c->nSweepSize + 1;

                        c->sShiftBuffer.shift(to_shift);
                        c->nBufferScanningHead -= to_shift;
                        c->nBufferCopyHead = 0;
                        c->nBufferCopyCount = 0;
                        c->nSweepHead = 0;
                        c->bDoPlot = false;
                        dsp::fill_zero(c->vSweep, c->nSweepSize);
                    }
                }

                c->sBypass.process(vChannels[ch].vOut, vChannels[ch].vIn, c->vOutput, to_do);

                c->vIn              += to_do;
                c->vOut             += to_do;
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
