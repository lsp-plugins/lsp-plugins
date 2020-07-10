/*
 * surge_filter.cpp
 *
 *  Created on: 4 июл. 2020 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>
#include <core/colors.h>
#include <core/util/Color.h>
#include <plugins/surge_filter.h>

#define BUFFER_SIZE     0x1000

#define TRACE_PORT(p)   lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    surge_filter_base::surge_filter_base(size_t channels, const plugin_metadata_t &meta): plugin_t(meta)
    {
        nChannels       = channels;
        vChannels       = NULL;
        vBuffer         = NULL;
        vTimePoints     = 0;
        fGainIn         = 1.0f;
        fGainOut        = 1.0f;
        bGainVisible    = false;
        pData           = NULL;
        pIDisplay       = NULL;

        pModeIn         = NULL;
        pModeOut        = NULL;
        pGainIn         = NULL;
        pGainOut        = NULL;
        pThreshOn       = NULL;
        pThreshOff      = NULL;
        pAttack         = NULL;
        pRelease        = NULL;
        pFadeIn         = NULL;
        pFadeOut        = NULL;
        pActive         = NULL;
        pBypass         = NULL;
        pMeshIn         = NULL;
        pMeshOut        = NULL;
        pMeshGain       = NULL;
        pGainVisible    = NULL;
        pGainMeter      = NULL;
    }

    surge_filter_base::~surge_filter_base()
    {
        destroy();
    }

    void surge_filter_base::init(IWrapper *wrapper)
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

            c->sBypass.construct();

            c->vIn          = NULL;
            c->vOut         = NULL;
            c->vBuffer      = bufs;
            bufs           += BUFFER_SIZE;

            c->bInVisible   = true;
            c->bOutVisible  = true;
        }

        // Initialize de-popper
        sDepopper.construct();
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
        pModeIn         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pModeOut        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pGainIn         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pThreshOn       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pThreshOff      = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pAttack         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pRelease        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFadeIn         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFadeOut        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pActive         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pGainOut        = vPorts[port_id++];
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
            vTimePoints[i]  = MESH_TIME - i*delta;
    }

    void surge_filter_base::destroy()
    {
        // Drop all channels
        if (vChannels != NULL)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = &vChannels[i];
                c->sDelay.destroy();
                c->sDryDelay.destroy();
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

        // Drop inline display buffer
        if (pIDisplay != NULL)
        {
            pIDisplay->detroy();
            pIDisplay   = NULL;
        }
    }

    void surge_filter_base::update_sample_rate(long sr)
    {
        size_t samples_per_dot  = seconds_to_samples(sr, MESH_TIME / MESH_POINTS);
        size_t max_delay        = millis_to_samples(sr, FADEOUT_MAX);

        sDepopper.init(sr, FADEOUT_MAX);
        sGain.init(MESH_POINTS, samples_per_dot);
        sActive.init(sr);

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];

            c->sBypass.init(sr);
            c->sDelay.init(max_delay);
            c->sDryDelay.init(max_delay);
            c->sIn.init(MESH_POINTS, samples_per_dot);
            c->sOut.init(MESH_POINTS, samples_per_dot);
        }
    }

    void surge_filter_base::update_settings()
    {
        bool bypass     = pBypass->getValue() >= 0.5f;
        fGainIn         = pGainIn->getValue();
        fGainOut        = pGainOut->getValue();
        bGainVisible    = pGainVisible->getValue() >= 0.5f;

        // Change depopper state
        sDepopper.set_fade_in_mode(depopper_mode_t(pModeIn->getValue()));
        sDepopper.set_fade_in_threshold(pThreshOn->getValue());
        sDepopper.set_fade_in_time(pFadeIn->getValue());
        sDepopper.set_fade_out_mode(depopper_mode_t(pModeOut->getValue()));
        sDepopper.set_fade_out_threshold(pThreshOff->getValue());
        sDepopper.set_fade_out_time(pFadeOut->getValue());
        sDepopper.set_attack(pAttack->getValue());
        sDepopper.set_release(pRelease->getValue());
        sDepopper.reconfigure();

        size_t latency  = sDepopper.latency();

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];

            c->sBypass.set_bypass(bypass);
            c->sDelay.set_delay(latency);
            c->sDryDelay.set_delay(latency);
            c->bInVisible   = c->pInVisible->getValue();
            c->bOutVisible  = c->pOutVisible->getValue();
        }

        // Report actual latency
        set_latency(latency);
    }

    void surge_filter_base::process(size_t samples)
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
                dsp::mul_k3(vChannels[0].vBuffer, vChannels[0].vIn, fGainIn, to_process);
                dsp::mul_k3(vChannels[1].vBuffer, vChannels[1].vIn, fGainIn, to_process);

                // Process input graph
                vChannels[0].sIn.process(vChannels[0].vBuffer, to_process);
                vChannels[1].sIn.process(vChannels[1].vBuffer, to_process);

                // Apply meter values
                vChannels[0].pMeterIn->setValue(dsp::abs_max(vChannels[0].vBuffer, to_process));
                vChannels[1].pMeterIn->setValue(dsp::abs_max(vChannels[1].vBuffer, to_process));

                // Compute control signal
                dsp::pamax3(vBuffer, vChannels[0].vBuffer, vChannels[1].vBuffer, to_process);
            }
            else
            {
                // Apply input gain
                dsp::mul_k3(vChannels[0].vBuffer, vChannels[0].vIn, fGainIn, to_process);

                // Process input graph and meter
                vChannels[0].sIn.process(vChannels[0].vBuffer, to_process);
                vChannels[0].pMeterIn->setValue(dsp::abs_max(vChannels[0].vBuffer, to_process));

                // Compute control signal
                dsp::abs2(vBuffer, vChannels[0].vBuffer, to_process);
            }

            // Process the gain reduction control
            sDepopper.process(vBuffer, vBuffer, to_process);
            pGainMeter->setValue(dsp::abs_min(vBuffer, to_process));
            sGain.process(vBuffer, to_process);

            // Apply reduction to the signal
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = &vChannels[i];

                // Apply delay to compensate latency and output gain
                c->sDelay.process(c->vBuffer, c->vBuffer, to_process);
                c->sDryDelay.process(c->vOut, c->vIn, to_process);
                dsp::fmmul_k3(c->vBuffer, vBuffer, fGainOut, to_process);
                c->sBypass.process(c->vOut, c->vOut, c->vBuffer, to_process);

                // Process output graph and meter
                c->sOut.process(c->vBuffer, to_process);
                c->pMeterOut->setValue(dsp::abs_max(c->vBuffer, to_process));

                // Update pointers
                c->vIn         += to_process;
                c->vOut        += to_process;
            }

            // Update number of samples left
            nleft      -= to_process;
        }

        // Sync gain mesh
        mesh_t *mesh    = pMeshGain->getBuffer<mesh_t>();
        if ((mesh != NULL) && (mesh->isEmpty()) && (bGainVisible))
        {
            dsp::copy(mesh->pvData[0], vTimePoints, MESH_POINTS);
            dsp::copy(mesh->pvData[1], sGain.data(), MESH_POINTS);
            mesh->data(2, MESH_POINTS);
        }

        // Sync input mesh
        mesh            = pMeshIn->getBuffer<mesh_t>();
        if ((mesh != NULL) && (mesh->isEmpty()))
        {
            dsp::copy(mesh->pvData[0], vTimePoints, MESH_POINTS);

            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = &vChannels[i];
                if (c->bInVisible)
                {
                    dsp::copy(mesh->pvData[i+1], c->sIn.data(), MESH_POINTS);
                }
                else
                    dsp::fill_zero(mesh->pvData[i+1], MESH_POINTS);
            }

            mesh->data(nChannels + 1, MESH_POINTS);
        }

        // Sync output mesh
        mesh            = pMeshOut->getBuffer<mesh_t>();
        if ((mesh != NULL) && (mesh->isEmpty()))
        {
            dsp::copy(mesh->pvData[0], vTimePoints, MESH_POINTS);

            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = &vChannels[i];
                if (c->bOutVisible)
                    dsp::copy(mesh->pvData[i+1], c->sOut.data(), MESH_POINTS);
                else
                    dsp::fill_zero(mesh->pvData[i+1], MESH_POINTS);
            }

            mesh->data(nChannels + 1, MESH_POINTS);
        }

        // Query inline display for draw
        bool query_draw = bGainVisible;
        if (!query_draw)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = &vChannels[i];
                query_draw      = (c->bInVisible) || (c->bOutVisible);
                if (query_draw)
                    break;
            }
        }

        if (query_draw)
            pWrapper->query_display_draw();
    }

    bool surge_filter_base::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        // Check proportions
        if (height > (R_GOLDEN_RATIO * width))
            height  = R_GOLDEN_RATIO * width;

        // Init canvas
        if (!cv->init(width, height))
            return false;
        width   = cv->width();
        height  = cv->height();

        // Clear background
        bool bypassing = vChannels[0].sBypass.bypassing();
        cv->set_color_rgb((bypassing) ? CV_DISABLED : CV_BACKGROUND);
        cv->paint();

        // Calc axis params
        float zy    = 1.0f/GAIN_AMP_M_72_DB;
        float dx    = -float(width/MESH_TIME);
        float dy    = height/logf(GAIN_AMP_M_72_DB/GAIN_AMP_P_24_DB);

        // Draw axis
        cv->set_line_width(1.0);

        // Draw vertical lines
        cv->set_color_rgb(CV_YELLOW, 0.5f);
        for (float i=1.0; i < (MESH_TIME-0.1); i += 1.0f)
        {
            float ax = width + dx*i;
            cv->line(ax, 0, ax, height);
        }

        // Draw horizontal lines
        cv->set_color_rgb(CV_WHITE, 0.5f);
        for (float i=GAIN_AMP_M_48_DB; i<GAIN_AMP_P_24_DB; i *= GAIN_AMP_P_24_DB)
        {
            float ay = height + dy*(logf(i*zy));
            cv->line(0, ay, width, ay);
        }

        // Allocate buffer: t, f1(t), x, y
        pIDisplay           = float_buffer_t::reuse(pIDisplay, 4, width);
        float_buffer_t *b   = pIDisplay;
        if (b == NULL)
            return false;

        // Draw input signal
        static uint32_t cin_colors[] = {
                CV_MIDDLE_CHANNEL_IN, CV_MIDDLE_CHANNEL_IN,
                CV_LEFT_CHANNEL_IN, CV_RIGHT_CHANNEL_IN
               };
        static uint32_t c_colors[] = {
                CV_MIDDLE_CHANNEL, CV_MIDDLE_CHANNEL,
                CV_LEFT_CHANNEL, CV_RIGHT_CHANNEL
               };
        bool bypass         = vChannels[0].sBypass.bypassing();
        float r             = MESH_POINTS/float(width);

        for (size_t j=0; j<width; ++j)
        {
            size_t k        = r*j;
            b->v[0][j]      = vTimePoints[k];
        }

        // Draw input channels
        cv->set_line_width(2.0f);
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];
            if (!c->bInVisible)
                continue;

            // Initialize values
            float *ft       = c->sIn.data();
            for (size_t j=0; j<width; ++j)
                b->v[1][j]      = ft[size_t(r*j)];

            // Initialize coords
            dsp::fill(b->v[2], width, width);
            dsp::fill(b->v[3], height, width);
            dsp::fmadd_k3(b->v[2], b->v[0], dx, width);
            dsp::axis_apply_log1(b->v[3], b->v[1], zy, dy, width);

            // Draw channel
            cv->set_color_rgb((bypass) ? CV_SILVER : cin_colors[(nChannels-1)*2 + i]);
            cv->draw_lines(b->v[2], b->v[3], width);
        }

        // Draw output channels
        cv->set_line_width(2.0f);
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];
            if (!c->bOutVisible)
                continue;

            // Initialize values
            float *ft       = c->sOut.data();
            for (size_t j=0; j<width; ++j)
                b->v[1][j]      = ft[size_t(r*j)];

            // Initialize coords
            dsp::fill(b->v[2], width, width);
            dsp::fill(b->v[3], height, width);
            dsp::fmadd_k3(b->v[2], b->v[0], dx, width);
            dsp::axis_apply_log1(b->v[3], b->v[1], zy, dy, width);

            // Draw channel
            cv->set_color_rgb((bypass) ? CV_SILVER : c_colors[(nChannels-1)*2 + i]);
            cv->draw_lines(b->v[2], b->v[3], width);
        }

        // Draw function (if present)
        if (bGainVisible)
        {
            float *ft       = sGain.data();
            for (size_t j=0; j<width; ++j)
                b->v[1][j]      = ft[size_t(r*j)];

            // Initialize coords
            dsp::fill(b->v[2], width, width);
            dsp::fill(b->v[3], height, width);
            dsp::fmadd_k3(b->v[2], b->v[0], dx, width);
            dsp::axis_apply_log1(b->v[3], b->v[1], zy, dy, width);

            // Draw channel
            cv->set_color_rgb((bypass) ? CV_SILVER : CV_BRIGHT_BLUE);
            cv->draw_lines(b->v[2], b->v[3], width);
        }

        return true;
    }

    void surge_filter_base::dump(IStateDumper *v) const
    {
        v->write("nChannels", nChannels);
        v->start_array("vChannels", vChannels, nChannels);
        for (size_t i=0; i<nChannels; ++i)
        {
            const channel_t *c = &vChannels[i];
            v->start_object(c, sizeof(channel_t));
            {
                v->write("vIn", c->vIn);
                v->write("vOut", c->vOut);
                v->write("vBuffer", c->vBuffer);
                v->write_object("sBypass", &c->sBypass);
                v->write_object("sIn", &c->sIn);
                v->write_object("sOut", &c->sOut);
                v->write("bInVisible", c->bInVisible);
                v->write("bOutVisible", c->bOutVisible);

                v->write("pIn", c->pIn);
                v->write("pOut", c->pOut);
                v->write("pInVisible", c->pInVisible);
                v->write("pOutVisible", c->pOutVisible);
                v->write("pMeterIn", c->pMeterIn);
                v->write("pMeterOut", c->pMeterOut);
            }
            v->end_object();
        }
        v->end_array();

        v->write("vBuffer", vBuffer);
        v->write("vTimePoints", vTimePoints);
        v->write("fGainIn", fGainIn);
        v->write("fGainOut", fGainOut);
        v->write("bGainVisible", bGainVisible);
        v->write("pData", pData);
        v->write("pIDisplay", pIDisplay);

        v->write_object("sGain", &sGain);
        v->write_object("sActive", &sActive);
        v->write_object("sDepopper", &sDepopper);

        v->write("pModeIn", pModeIn);
        v->write("pModeOut", pModeOut);
        v->write("pGainIn", pGainIn);
        v->write("pGainOut", pGainOut);
        v->write("pThreshOn", pThreshOn);
        v->write("pThreshOff", pThreshOff);
        v->write("pAttack", pAttack);
        v->write("pRelease", pRelease);
        v->write("pFadeIn", pFadeIn);
        v->write("pFadeOut", pFadeOut);
        v->write("pActive", pActive);
        v->write("pBypass", pBypass);
        v->write("pMeshIn", pMeshIn);
        v->write("pMeshOut", pMeshOut);
        v->write("pMeshGain", pMeshGain);
        v->write("pGainVisible", pGainVisible);
        v->write("pGainMeter", pGainMeter);
    }

    //-------------------------------------------------------------------------
    surge_filter_mono::surge_filter_mono(): surge_filter_base(1, metadata)
    {
    }

    surge_filter_stereo::surge_filter_stereo(): surge_filter_base(2, metadata)
    {
    }
}

