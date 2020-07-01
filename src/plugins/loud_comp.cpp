/*
 * loud_comp.cpp
 *
 *  Created on: 29 июн. 2020 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>
#include <plugins/loud_comp.h>

#include <generated/iso226/fletcher_munson.h>
#include <generated/iso226/robinson_dadson.h>
#include <generated/iso226/iso226-2003.h>

#define BUF_SIZE            0x1000

#define TRACE_PORT(p) lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    static const freq_curve_t *freq_curves[]=
    {
        &iso226_2003_curve,
        &fletcher_munson_curve,
        &robinson_dadson_curve
    };

    //-------------------------------------------------------------------------
    // Loudness compensator

    loud_comp_base::loud_comp_base(const plugin_metadata_t &mdata, size_t channels): plugin_t(mdata)
    {
        nChannels       = channels;
        fGain           = 0.0f;
        nMode           = 0;
        nRank           = FFT_RANK_MIN;
        fVolume         = -1.0f;
        vChannels[0]    = NULL;
        vChannels[1]    = NULL;
        vFreqApply      = NULL;
        vFreqMesh       = NULL;
        vAmpMesh        = NULL;
        bSyncMesh       = false;
        pData           = NULL;
        pIDisplay       = NULL;

        pBypass         = NULL;
        pGain           = NULL;
        pMode           = NULL;
        pRank           = NULL;
        pVolume         = NULL;
        pMesh           = NULL;
    }

    loud_comp_base::~loud_comp_base()
    {
        destroy();
    }

    void loud_comp_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        // Compute size of data to allocate
        size_t sz_channel   = ALIGN_SIZE(sizeof(channel_t), DEFAULT_ALIGN);
        size_t sz_fft       = (4 << FFT_RANK_MAX) * sizeof(float);
        size_t sz_sync      = ALIGN_SIZE(sizeof(float) * CURVE_MESH_SIZE, DEFAULT_ALIGN);
        size_t sz_buf       = BUF_SIZE * sizeof(float);

        // Total amount of data to allocate
        size_t sz_alloc     = (sz_channel + sz_buf*2) * nChannels + sz_fft + sz_sync*2;
        uint8_t *ptr        = alloc_aligned<uint8_t>(pData, sz_alloc);
        if (ptr == NULL)
            return;

        // Allocate channels
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c        = reinterpret_cast<channel_t *>(ptr);
            ptr                += sz_channel;

            c->sDelay.init(1 << (FFT_RANK_MAX - 1));

            c->vIn              = NULL;
            c->vOut             = NULL;
            c->vDry             = NULL;
            c->vBuffer          = NULL;

            c->pIn              = NULL;
            c->pOut             = NULL;
            c->pMeterIn         = NULL;
            c->pMeterOut        = NULL;

            vChannels[i]        = c;
        }

        // Allocate buffers
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c        = vChannels[i];
            c->vDry             = reinterpret_cast<float *>(ptr);
            ptr                += sz_buf;
            c->vBuffer          = reinterpret_cast<float *>(ptr);
            ptr                += sz_buf;

            dsp::fill_zero(c->vBuffer, BUF_SIZE);
        }

        // Initialize buffers
        vFreqApply          = reinterpret_cast<float *>(ptr);
        ptr                += sz_fft;
        vFreqMesh           = reinterpret_cast<float *>(ptr);
        ptr                += sz_sync;
        vAmpMesh            = reinterpret_cast<float *>(ptr);
        ptr                += sz_sync;

        lsp_trace("Binding ports...");
        size_t port_id      = 0;

        // Bind input audio ports
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i]->pIn   = vPorts[port_id++];
        }

        // Bind output audio ports
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i]->pOut  = vPorts[port_id++];
        }

        // Bind common ports
        TRACE_PORT(vPorts[port_id]);
        pBypass             = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pGain               = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMode               = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pRank               = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pVolume             = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMesh               = vPorts[port_id++];

        // Bind input level meters
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i]->pMeterIn  = vPorts[port_id++];
        }

        // Bind output level meters
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i]->pMeterOut = vPorts[port_id++];
        }
    }

    void loud_comp_base::destroy()
    {
        if (pIDisplay != NULL)
        {
            pIDisplay->detroy();
            pIDisplay   = NULL;
        }

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = vChannels[i];
            if (c == NULL)
                continue;

            c->sDelay.destroy();
            vChannels[i]    = NULL;
        }

        vFreqApply      = NULL;
        vFreqMesh       = NULL;

        if (pData != NULL)
        {
            free_aligned(pData);
            pData   = NULL;
        }
    }

    void loud_comp_base::ui_activated()
    {
        bSyncMesh           = true;
    }

    void loud_comp_base::update_sample_rate(long sr)
    {
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c        = vChannels[i];

            // Update processor settings
            c->sBypass.init(sr);
        }
    }

    void loud_comp_base::update_settings()
    {
        size_t mode         = pMode->getValue();
        size_t rank         = FFT_RANK_MIN + ssize_t(pRank->getValue());
        if (rank < FFT_RANK_MIN)
            rank                = FFT_RANK_MIN;
        else if (rank > FFT_RANK_MAX)
            rank                = FFT_RANK_MAX;
        float volume        = pVolume->getValue();

        // Need to update curve?
        if ((mode != nMode) || (rank != nRank) || (volume != fVolume))
        {
            nMode               = mode;
            nRank               = rank;
            fVolume             = volume;
            bSyncMesh           = true;

            update_response_curve();
        }

        fGain               = pGain->getValue();
        bool bypass         = pBypass->getValue() >= 0.5f;

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c        = vChannels[i];
            c->sBypass.set_bypass(bypass);

            // TODO: update settings of the spectral processor
        }
    }

    void loud_comp_base::update_response_curve()
    {
//        const freq_curve_t *c   = ((nMode > 0) && (nMode <= (sizeof(freq_curves)/sizeof(freq_curve_t *)))) ? freq_curves[nMode-1] : NULL;
        size_t freqs            = 1 << nRank;
/*
        if (c != NULL)
        {
            // TODO: compute curve characteristics
        }
        else
        {*/
            float vol   = db_to_gain(fVolume);

            dsp::fill(vFreqApply, vol, freqs * 4);
//            dsp::fill(vAmpMesh, , CURVE_MESH_SIZE);
            for (size_t i=0; i<CURVE_MESH_SIZE; ++i)
                vFreqMesh[i] = i;
            dsp::fill(vAmpMesh, vol, CURVE_MESH_SIZE);
//        }
    }

    void loud_comp_base::process(size_t samples)
    {
        // Bind ports
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = vChannels[i];
            c->vIn          = c->pIn->getBuffer<float>();
            c->vOut         = c->pOut->getBuffer<float>();
            float v         = dsp::abs_max(c->vIn, samples);
            c->pMeterIn->setValue(v);
        }

        while (samples > 0)
        {
            size_t to_process   = (samples > BUF_SIZE) ? BUF_SIZE : samples;

            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = vChannels[i];
                dsp::mul_k3(c->vBuffer, c->vIn, fGain, to_process);

                // Process the signal
                c->sDelay.process(c->vDry, c->vIn, to_process);
                // TODO: add spectral processor call here
                c->pMeterOut->setValue(dsp::abs_max(c->vBuffer, to_process));

                // Apply bypass
                c->sBypass.process(c->vOut, c->vDry, c->vBuffer, to_process);

                // Update pointers
                c->vIn         += to_process;
                c->vOut        += to_process;
            }

            samples        -= to_process;
        }

        // Report latency
        set_latency(vChannels[0]->sDelay.get_delay());

        // Sync output mesh
        mesh_t *mesh = pMesh->getBuffer<mesh_t>();
        if ((bSyncMesh) && (mesh != NULL) && (mesh->isEmpty()))
        {
            // Output mesh data
            dsp::copy(mesh->pvData[0], vFreqMesh, CURVE_MESH_SIZE);
            dsp::copy(mesh->pvData[1], vAmpMesh, CURVE_MESH_SIZE);
            mesh->data(2, CURVE_MESH_SIZE);
            bSyncMesh   = NULL;
        }
    }

    bool loud_comp_base::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        // TODO
        return false;
    }

    //-------------------------------------------------------------------------
    // Different instances
    loud_comp_mono::loud_comp_mono(): loud_comp_base(metadata, 1)
    {
    }

    loud_comp_stereo::loud_comp_stereo(): loud_comp_base(metadata, 2)
    {
    }
}

