/*
 * pop_destroyer.cpp
 *
 *  Created on: 4 июл. 2020 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>
#include <plugins/pop_destroyer.h>
#include <core/colors.h>
#include <core/util/Color.h>

#define BUFFER_SIZE     0x1000

#define TRACE_PORT(p)   lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    pop_destroyer_base::pop_destroyer_base(size_t channels, const plugin_metadata_t &meta): plugin_t(meta)
    {
        nChannels       = 0;
        vChannels       = NULL;
        nSync           = 0;
        pData           = NULL;

        pInGain         = NULL;
        pOutGain        = NULL;
        pThresh         = NULL;
        pAttack         = NULL;
        pRelease        = NULL;
        pFade           = NULL;
        pActive         = NULL;
        pBypass         = NULL;
        pMeshIn         = NULL;
        pMeshOut        = NULL;
        pMeshGain       = NULL;
        pGainVisible    = NULL;
    }

    pop_destroyer_base::~pop_destroyer_base()
    {
        destroy();
    }

    void pop_destroyer_base::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        // Allocate buffers
        float *bufs         = alloc_aligned<float>(pData, BUFFER_SIZE * nChannels);
        if (bufs == NULL)
            return;

        // Allocate channels
        vChannels       = new channel_t[nChannels];
        if (vChannels == NULL)
            return;

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = vChannels[i];

            c->vIn          = NULL;
            c->vOut         = NULL;
            c->vBuffer      = bufs;
            bufs           += BUFFER_SIZE;

            c->sDepopper.init();
            c->bInVisible   = true;
            c->bOutVisible  = true;
            c->nSync        = S_IN | S_OUT;
        }

        // Bind ports
        lsp_trace("Binding ports");
        size_t port_id      = 0;

        // Bind input audio ports
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pIn    = vPorts[port_id++];
        }

        // Bind output audio ports
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pOut   = vPorts[port_id++];
        }

        // Bind control ports
        TRACE_PORT(vPorts[port_id]);
        pOutGain        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pThresh         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pAttack         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pRelease        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFade           = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pActive         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pInGain         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMeshIn         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMeshOut        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMeshGain       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pGainVisible    = vPorts[port_id++];

        // Bind custom channel ports
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = vChannels[i];

            TRACE_PORT(vPorts[port_id]);
            c->pInVisible       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pOutVisible      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pMeterIn         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pMeterOut        = vPorts[port_id++];
        }
    }

    void pop_destroyer_base::destroy()
    {
        // Drop all channels
        if (vChannels != NULL)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = vChannels[i];
                c->sIn.destroy();
                c->sOut.destroy();
            }

            delete [] vChannels;
        }

        // Drop buffers
        if (pData != NULL)
        {
            free_aligned(pData);
            pData   = NULL;
        }
    }

    void pop_destroyer_base::ui_activated()
    {
        nSync      |= S_GAIN;
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = vChannels[i];
            if (c->bInVisible)
                c->nSync       |= S_IN;
            if (c->bOutVisible)
                c->nSync       |= S_OUT;
        }
    }

    void pop_destroyer_base::update_sample_rate(long sr)
    {
        size_t samples_per_dot  = seconds_to_samples(sr, MESH_TIME / MESH_POINTS);

        sGain.init(MESH_POINTS, samples_per_dot);
        sActive.init(sr);

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = vChannels[i];

            c->sBypass.init(sr);
            c->sDepopper.set_sample_rate(sr);
            c->sIn.init(MESH_POINTS, samples_per_dot);
            c->sOut.init(MESH_POINTS, samples_per_dot);
        }
    }

    void pop_destroyer_base::update_settings()
    {
    }

    void pop_destroyer_base::process(size_t samples)
    {
        // Bind ports
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = vChannels[i];
            c->vIn          = c->pIn->getBuffer<float>();
            c->vOut         = c->pOut->getBuffer<float>();
        }

        // TODO: replace this stuff
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = vChannels[i];
            dsp::copy(c->vOut, c->vIn, samples);
        }
    }

    bool pop_destroyer_base::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        return false;
    }

    //-------------------------------------------------------------------------
    pop_destroyer_mono::pop_destroyer_mono(): pop_destroyer_base(1, metadata)
    {
    }

    pop_destroyer_stereo::pop_destroyer_stereo(): pop_destroyer_base(2, metadata)
    {
    }
}

