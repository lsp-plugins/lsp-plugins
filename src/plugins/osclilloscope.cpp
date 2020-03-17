/*
 * osclilloscope.cpp
 *
 *  Created on: 1 Mar 2020
 *      Author: crocoduck
 */

#include <plugins/oscilloscope.h>
#include <core/debug.h>

#define TRACE_PORT(p)   lsp_trace("  port id=%s", (p)->metadata()->id);
#define BUF_LIM_SIZE    196608

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

        nBuffersCapacity    = 0;

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

        // 2x nChannels X Mesh Buffers (x, y for each channel) +  1X temporary buffer + 1X Default Abscissa Buffer.
        nBuffersCapacity = BUF_LIM_SIZE;
        nMeshSize = oscilloscope_base_metadata::SCOPE_MESH_SIZE;
        size_t samples = 2 * nChannels * nMeshSize + nBuffersCapacity + nMeshSize;

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
            c->enOverMode = OM_NONE;
            c->sOversampler.set_mode(c->enOverMode);
            if (c->sOversampler.modified())
                c->sOversampler.update_settings();
            c->nOversampling = c->sOversampler.get_oversampling();
            c->nOverSampleRate = c->nOversampling * nSampleRate;

            if (!c->sShiftBuffer.init(nBuffersCapacity))
                return;

            if (!c->sTrigger.init())
                return;

            // Test settings for trigger before proper implementation.
            c->nTriggerIndex = 0;
            c->nPreTrigger = 1024;
            c->nPostTrigger = 1024;
            c->nSweepSize = c->nPreTrigger + c->nPostTrigger + 1;
            c->sTrigger.set_post_trigger_samples(c->nSweepSize);
            c->sTrigger.set_trigger_type(TRG_TYPE_SIMPLE_RISING_EDGE);
            c->sTrigger.set_trigger_threshold(0.5f);
            c->sTrigger.update_settings();

            c->vAbscissa = ptr;
            ptr += nMeshSize;

            c->vOrdinate = ptr;
            ptr += nMeshSize;

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
        ptr += nBuffersCapacity;

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
        // Bind audio ports
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            vChannels[ch].vIn   = vChannels[ch].pIn->getBuffer<float>();
            vChannels[ch].vOut  = vChannels[ch].pOut->getBuffer<float>();

            if ((vChannels[ch].vIn == NULL) || (vChannels[ch].vOut == NULL))
                return;

            vChannels[ch].nSamplesCounter = samples;
            vChannels[ch].bProcessComplete = false;
        }

        bool doLoop = true;
        bool doSweep = false;

        while (doLoop)
        {
            doLoop = false;

            for (size_t ch = 0; ch < nChannels; ++ch)
            {
                if (vChannels[ch].bProcessComplete)
                    continue;

                size_t upsampled_total = vChannels[ch].nOversampling * vChannels[ch].nSamplesCounter;
                size_t upsampled_available = (upsampled_total < nBuffersCapacity) ? upsampled_total : nBuffersCapacity;
                size_t to_do = upsampled_available / vChannels[ch].nOversampling;

                vChannels[ch].sOversampler.upsample(vTemp, vChannels[ch].vIn, to_do);
                vChannels[ch].sShiftBuffer.append(vTemp, upsampled_available);

                float *head = vChannels[ch].sShiftBuffer.head();
                size_t bufferSize = vChannels[ch].sShiftBuffer.size();

                for (size_t n = 0; n < bufferSize; ++n)
                {
                    vChannels[ch].sTrigger.single_sample_processor(head[n]);

                    trg_state_t tState = vChannels[ch].sTrigger.get_trigger_state();
                    if (tState == TRG_STATE_FIRED)
                    {
                        vChannels[ch].nTriggerIndex = n;
                        doSweep = true;
                        break;
                    }
                }

                if (!doSweep)
                    vChannels[ch].sShiftBuffer.shift(bufferSize - vChannels[ch].nPreTrigger);

                if (doSweep && (bufferSize >= vChannels[ch].nSweepSize))
                {
                    mesh_t *mesh = vChannels[ch].pMesh->getBuffer<mesh_t>();

                    if (mesh == NULL)
                        continue;

                    if (!mesh->isEmpty())
                        continue;

                    size_t copyhead = vChannels[ch].nTriggerIndex - vChannels[ch].nPreTrigger;

                    get_plottable_data(vChannels[ch].vAbscissa, &head[copyhead], nMeshSize, vChannels[ch].nSweepSize);
                    get_plottable_data(vChannels[ch].vOrdinate, &head[copyhead], nMeshSize, vChannels[ch].nSweepSize);

                    dsp::copy(mesh->pvData[0], vDflAbscissa, nMeshSize);
                    dsp::copy(mesh->pvData[1], vChannels[ch].vOrdinate, nMeshSize);
                    mesh->data(2, nMeshSize);

                    vChannels[ch].sShiftBuffer.shift(copyhead + vChannels[ch].nSweepSize);
                    doSweep = false;
                }

                vChannels[ch].vIn   += to_do;
                vChannels[ch].vOut  += to_do;
                vChannels[ch].nSamplesCounter -= to_do;

                if (vChannels[ch].nSamplesCounter <= 0)
                {
                    vChannels[ch].bProcessComplete = true;
                    doLoop = doLoop || !vChannels[ch].bProcessComplete;
                }
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
