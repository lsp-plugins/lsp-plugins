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
        nChannels       = channels;
        vChannels       = NULL;
        vBuffer         = NULL;
        vTimePoints     = 0;
        fGainIn         = 1.0f;
        fGainOut        = 1.0f;
        bGainVisible    = false;
        nSync           = 0;
        pData           = NULL;

        pGainIn         = NULL;
        pGainOut        = NULL;
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
        pGainMeter      = NULL;
    }

    pop_destroyer_base::~pop_destroyer_base()
    {
        destroy();
    }

    void pop_destroyer_base::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        // Allocate buffers
        size_t meshbuf      = ALIGN_SIZE(MESH_POINTS * sizeof(float), DEFAULT_ALIGN);
        float *bufs         = alloc_aligned<float>(pData, BUFFER_SIZE * 3);
        if (bufs == NULL)
            return;

        // Allocate channels
        vChannels       = new channel_t[nChannels];
        if (vChannels == NULL)
            return;
        vBuffer         = bufs;
        bufs           += BUFFER_SIZE;
        vTimePoints     = bufs;
        bufs           += meshbuf/sizeof(float);

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];

            c->vIn          = NULL;
            c->vOut         = NULL;
            c->vBuffer      = bufs;
            bufs           += BUFFER_SIZE;

            c->bInVisible   = true;
            c->bOutVisible  = true;
            c->nSync        = S_IN | S_OUT;
        }

        // Initialize de-popper
        sDepopper.init();
        sGain.set_method(MM_MINIMUM);

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
        pBypass         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pGainOut        = vPorts[port_id++];
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
        pGainIn         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMeshIn         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMeshOut        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMeshGain       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pGainVisible    = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pGainMeter      = vPorts[port_id++];

        // Bind custom channel ports
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];

            TRACE_PORT(vPorts[port_id]);
            c->pInVisible       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pOutVisible      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pMeterIn         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pMeterOut        = vPorts[port_id++];
        }

        // Initialize time points
        float delta     = MESH_TIME / (MESH_POINTS - 1);
        for (size_t i=0; i<MESH_POINTS; ++i)
            vTimePoints[i]  = MESH_POINTS - i*delta;
    }

    void pop_destroyer_base::destroy()
    {
        // Drop all channels
        if (vChannels != NULL)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = &vChannels[i];
                c->sIn.destroy();
                c->sOut.destroy();
            }

            delete [] vChannels;
            vChannels = NULL;
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
            channel_t *c    = &vChannels[i];
            if (c->bInVisible)
                c->nSync       |= S_IN;
            if (c->bOutVisible)
                c->nSync       |= S_OUT;
        }
    }

    void pop_destroyer_base::update_sample_rate(long sr)
    {
        size_t samples_per_dot  = seconds_to_samples(sr, MESH_TIME / MESH_POINTS);

        sDepopper.set_sample_rate(sr);
        sGain.init(MESH_POINTS, samples_per_dot);
        sActive.init(sr);

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];

            c->sBypass.init(sr);
            c->sIn.init(MESH_POINTS, samples_per_dot);
            c->sOut.init(MESH_POINTS, samples_per_dot);
        }
    }

    void pop_destroyer_base::update_settings()
    {
        bool bypass     = pBypass->getValue() >= 0.5f;
        fGainIn         = pGainIn->getValue();
        fGainOut        = pGainOut->getValue();
        bGainVisible    = pGainVisible->getValue() >= 0.5f;

        sDepopper.set_threshold(pThresh->getValue());
        sDepopper.set_attack(pAttack->getValue());
        sDepopper.set_release(pRelease->getValue());
        sDepopper.set_fade_time(pFade->getValue());

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];

            c->sBypass.set_bypass(bypass);
            c->bInVisible   = c->pInVisible->getValue();
            c->bOutVisible  = c->pOutVisible->getValue();
        }
    }

    void pop_destroyer_base::process(size_t samples)
    {
        // Bind ports
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];
            c->vIn          = c->pIn->getBuffer<float>();
            c->vOut         = c->pOut->getBuffer<float>();
        }

        for (size_t nleft=samples; nleft > 0; )
        {
            size_t to_process = (nleft > BUFFER_SIZE) ? BUFFER_SIZE : nleft;

            // Perform main processing
            if (nChannels > 1)
            {
                // Apply input gain
                dsp::mul_k3(vChannels[0].vOut, vChannels[0].vIn, fGainIn, to_process);
                dsp::mul_k3(vChannels[1].vOut, vChannels[1].vIn, fGainIn, to_process);

                // Process input graph
                vChannels[0].sIn.process(vChannels[0].vOut, to_process);
                vChannels[1].sIn.process(vChannels[1].vOut, to_process);

                // Apply meter values
                vChannels[0].pMeterIn->setValue(dsp::abs_max(vChannels[0].vOut, to_process));
                vChannels[1].pMeterIn->setValue(dsp::abs_max(vChannels[1].vOut, to_process));

                // Compute control signal
                dsp::pamax3(vBuffer, vChannels[0].vOut, vChannels[1].vOut, to_process);
            }
            else
            {
                // Apply input gain
                dsp::mul_k3(vChannels[0].vOut, vChannels[0].vIn, fGainIn, to_process);

                // Process input graph
                vChannels[0].sIn.process(vChannels[0].vOut, to_process);

                // Apply meter values
                vChannels[0].pMeterIn->setValue(dsp::abs_max(vChannels[0].vOut, to_process));

                // Compute control signal
                dsp::abs2(vBuffer, vChannels[0].vOut, to_process);
            }

            // Process the gain reduction control
            sDepopper.process(vBuffer, vBuffer, to_process);
            pGainMeter->setValue(dsp::abs_min(vBuffer, to_process));
            sGain.process(vBuffer, to_process);

            // Apply reduction to the signal
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = &vChannels[i];

                // Apply output gain
                dsp::fmmul_k3(c->vOut, c->vBuffer, fGainOut, to_process);

                // Process output graph
                c->sOut.process(c->vOut, to_process);

                // Process output meter
                c->pOut->setValue(dsp::abs_max(c->vOut, to_process));

                // Update pointers
                c->vIn         += to_process;
                c->vOut        += to_process;
            }

            // Update number of samples left
            nleft      -= to_process;
        }

        // Sync gain mesh
        mesh_t *mesh    = pGainOut->getBuffer<mesh_t>();
        bool sync       = ((bGainVisible) && (nSync & S_GAIN));
        if ((mesh != NULL) && (mesh->isEmpty()) && (sync))
        {
            dsp::copy(mesh->pvData[0], vTimePoints, MESH_POINTS);
            dsp::copy(mesh->pvData[1], sGain.data(), MESH_POINTS);
            mesh->data(2, MESH_POINTS);

            nSync          &= ~S_GAIN;
        }

        // Sync input mesh
        mesh            = pMeshIn->getBuffer<mesh_t>();
        sync            = (vChannels[0].bInVisible) && (vChannels[0].nSync & S_IN);
        if ((!sync) && (nChannels > 0))
            sync            = (vChannels[1].bInVisible) && (vChannels[1].nSync & S_IN);
        if ((mesh != NULL) && (mesh->isEmpty()) && (sync))
        {
            dsp::copy(mesh->pvData[0], vTimePoints, MESH_POINTS);

            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = &vChannels[i];
                if (c->bInVisible)
                    dsp::copy(mesh->pvData[1], c->sIn.data(), MESH_POINTS);
                else
                    dsp::fill_zero(mesh->pvData[1], MESH_POINTS);
                c->nSync       &= ~S_IN;
            }

            mesh->data(nChannels + 1, MESH_POINTS);
        }

        // Sync output mesh
        mesh            = pMeshOut->getBuffer<mesh_t>();
        sync            = (vChannels[0].bOutVisible) && (vChannels[0].nSync & S_OUT);
        if ((!sync) && (nChannels > 0))
            sync            = (vChannels[1].bOutVisible) && (vChannels[1].nSync & S_OUT);
        if ((mesh != NULL) && (mesh->isEmpty()) && (sync))
        {
            dsp::copy(mesh->pvData[0], vTimePoints, MESH_POINTS);

            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = &vChannels[i];
                if (c->bOutVisible)
                    dsp::copy(mesh->pvData[1], c->sOut.data(), MESH_POINTS);
                else
                    dsp::fill_zero(mesh->pvData[1], MESH_POINTS);
                c->nSync       &= ~S_OUT;
            }

            mesh->data(nChannels + 1, MESH_POINTS);
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

