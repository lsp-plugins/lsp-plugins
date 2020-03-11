/*
 * osclilloscope.cpp
 *
 *  Created on: 1 Mar 2020
 *      Author: crocoduck
 */

#include <plugins/oscilloscope.h>
#include <core/debug.h>

#define TRACE_PORT(p)       lsp_trace("  port id=%s", (p)->metadata()->id);
#define SHIFT_BUF_LIM_SIZE  196608

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

        if (vChannels != NULL)
        {
            for (size_t ch = 0; ch < nChannels; ++ch)
            {
                vChannels[ch].sShiftBuffer.destroy();
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

        // 2x nChannels X Mesh Buffers (x, y for each channel) + 1X Default Abscissa Buffer.
        nMeshSize = oscilloscope_base_metadata::SCOPE_MESH_SIZE;
        size_t samples = 2 * nChannels * nMeshSize + nMeshSize;

        float *ptr = alloc_aligned<float>(pData, samples);
        if (ptr == NULL)
            return;

        lsp_guard_assert(float *save = ptr);

        nBuffersCapacity = SHIFT_BUF_LIM_SIZE;

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            if (!c->sShiftBuffer.init(nBuffersCapacity))
            {
                return;
            }

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
        }

        while (samples > 0)
        {
            size_t to_do = (samples < nBuffersCapacity) ? samples : nBuffersCapacity;

            for (size_t ch = 0; ch < nChannels; ++ch)
            {
                vChannels[ch].sShiftBuffer.append(vChannels[ch].vIn, to_do);

                size_t bufferSize = vChannels[ch].sShiftBuffer.size();

                if (bufferSize >= nMeshSize)
                {
                    mesh_t *mesh = vChannels[ch].pMesh->getBuffer<mesh_t>();

                    if (mesh == NULL)
                        continue;

                    if (!mesh->isEmpty())
                        continue;

                    get_plottable_data(vChannels[ch].vAbscissa, vChannels[ch].sShiftBuffer.head(), nMeshSize, bufferSize);
                    get_plottable_data(vChannels[ch].vOrdinate, vChannels[ch].sShiftBuffer.head(), nMeshSize, bufferSize);

                    vChannels[ch].sShiftBuffer.shift(bufferSize);

                    dsp::copy(mesh->pvData[0], vDflAbscissa, nMeshSize);
                    dsp::copy(mesh->pvData[1], vChannels[ch].vOrdinate, nMeshSize);
                    mesh->data(2, nMeshSize);
                }
            }

            for (size_t ch = 0; ch < nChannels; ++ch)
            {
                //vChannels[ch].sBypass.process(vChannels[ch].vOut, vChannels[ch].vIn, vChannels[ch].vBuffer, to_do);

                vChannels[ch].vIn   += to_do;
                vChannels[ch].vOut  += to_do;
            }

            samples -= to_do;
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
