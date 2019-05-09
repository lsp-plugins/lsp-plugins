/*
 * room_builder.cpp
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>

#include <core/files/Model3DFile.h>

#include <plugins/room_builder.h>

#define TMP_BUF_SIZE            4096
#define CONV_RANK               10
#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    //-------------------------------------------------------------------------
    // 3D Scene loader
    room_builder_base::SceneLoader::~SceneLoader()
    {
    }


    void room_builder_base::SceneLoader::init(room_builder_base *base)
    {
        pCore   = base;
        sScene.clear();
    }

    void room_builder_base::SceneLoader::destroy()
    {
        sScene.destroy();
    }

    status_t room_builder_base::SceneLoader::run()
    {
        // Clear scene
        sScene.clear();

        // Check state
        if (pCore->p3DFile == NULL)
            return STATUS_UNKNOWN_ERR;

        // Get path
        path_t *path = pCore->p3DFile->getBuffer<path_t>();
        if (path == NULL)
            return STATUS_UNKNOWN_ERR;

        // Get file name
        const char *fname = path->get_path();
        if (strlen(fname) <= 0)
            return STATUS_UNSPECIFIED;

        // Load the scene file
        return Model3DFile::load(&sScene, fname, true);
    }

    //-------------------------------------------------------------------------
    room_builder_base::room_builder_base(const plugin_metadata_t &metadata, size_t inputs):
        plugin_t(metadata)
    {
        nInputs         = inputs;
        nReconfigReq    = 0;
        nReconfigResp   = 0;

        nSceneStatus    = STATUS_UNSPECIFIED;
        fSceneProgress  = 0.0f;

        pBypass         = NULL;
        pRank           = NULL;
        pDry            = NULL;
        pWet            = NULL;
        pOutGain        = NULL;
        pPredelay       = NULL;
        p3DFile         = NULL;
        p3DStatus       = NULL;
        p3DProgress     = NULL;

        pData           = NULL;
        pExecutor       = NULL;
    }

    room_builder_base::~room_builder_base()
    {
    }

    void room_builder_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        // Remember executor service
        pExecutor       = wrapper->get_executor();
        lsp_trace("Executor = %p", pExecutor);

        // Allocate memory
        size_t tmp_buf_size = TMP_BUF_SIZE * sizeof(float);
        size_t alloc        = tmp_buf_size * (impulse_reverb_base_metadata::CONVOLVERS + 2);
        uint8_t *ptr        = alloc_aligned<uint8_t>(pData, alloc);
        if (pData == NULL)
            return;

        // Initialize 3D loader
        s3DLoader.init(this);

        // Initialize inputs
        for (size_t i=0; i<2; ++i)
        {
            input_t *in     = &vInputs[i];
            in->vIn         = NULL;
            in->pIn         = NULL;
            in->pPan        = NULL;
        }

        // Initialize output channels
        for (size_t i=0; i<2; ++i)
        {
            channel_t *c    = &vChannels[i];

            if (!c->sPlayer.init(impulse_reverb_base_metadata::FILES, 32))
                return;
            if (!c->sEqualizer.init(impulse_reverb_base_metadata::EQ_BANDS + 2, CONV_RANK))
                return;
            c->sEqualizer.set_mode(EQM_BYPASS);

            c->fDryPan[0]   = 0.0f;
            c->fDryPan[1]   = 0.0f;

            c->vOut         = NULL;
            c->vBuffer      = reinterpret_cast<float *>(ptr);
            ptr            += tmp_buf_size;

            c->pOut         = NULL;

            c->pWetEq       = NULL;
            c->pLowCut      = NULL;
            c->pLowFreq     = NULL;
            c->pHighCut     = NULL;
            c->pHighFreq    = NULL;

            for (size_t j=0; j<impulse_reverb_base_metadata::EQ_BANDS; ++j)
                c->pFreqGain[j]     = NULL;
        }

        // Bind ports
        size_t port_id = 0;

        lsp_trace("Binding audio ports");
        for (size_t i=0; i<nInputs; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vInputs[i].pIn      = vPorts[port_id++];
        }
        for (size_t i=0; i<2; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pOut   = vPorts[port_id++];
        }

        // Bind controlling ports
        lsp_trace("Binding common ports");
        TRACE_PORT(vPorts[port_id]);
        pBypass             = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);            // Skip view selector
        port_id++;
        TRACE_PORT(vPorts[port_id]);            // FFT rank
        pRank       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);            // Pre-delay
        pPredelay   = vPorts[port_id++];

        for (size_t i=0; i<nInputs; ++i)        // Panning ports
        {
            TRACE_PORT(vPorts[port_id]);
            vInputs[i].pPan     = vPorts[port_id++];
        }

        TRACE_PORT(vPorts[port_id]);
        pDry        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pWet        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOutGain    = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        p3DFile     = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        p3DStatus   = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        p3DProgress = vPorts[port_id++];
    }

    void room_builder_base::destroy()
    {
        sScene.destroy();
        s3DLoader.destroy();

        if (pData != NULL)
        {
            free_aligned(pData);
            pData       = NULL;
        }
    }

    size_t room_builder_base::get_fft_rank(size_t rank)
    {
        return room_builder_base_metadata::FFT_RANK_MIN + rank;
    }

    void room_builder_base::update_settings()
    {
        float out_gain      = pOutGain->getValue();
        float dry_gain      = pDry->getValue() * out_gain;
        float wet_gain      = pWet->getValue() * out_gain;
        bool bypass         = pBypass->getValue() >= 0.5f;
        float predelay      = pPredelay->getValue();
        size_t rank         = get_fft_rank(pRank->getValue());

        // Adjust volume of dry channel
        if (nInputs == 1)
        {
            float pan               = vInputs[0].pPan->getValue();
            vChannels[0].fDryPan[0] = (100.0f - pan) * 0.005f * dry_gain;
            vChannels[0].fDryPan[1] = 0.0f;
            vChannels[1].fDryPan[0] = (100.0f + pan) * 0.005f * dry_gain;
            vChannels[1].fDryPan[1] = 0.0f;
        }
        else
        {
            float pan_l             = vInputs[0].pPan->getValue();
            float pan_r             = vInputs[1].pPan->getValue();

            vChannels[0].fDryPan[0] = (100.0f - pan_l) * 0.005f * dry_gain;
            vChannels[0].fDryPan[1] = (100.0f - pan_r) * 0.005f * dry_gain;
            vChannels[1].fDryPan[0] = (100.0f + pan_l) * 0.005f * dry_gain;
            vChannels[1].fDryPan[1] = (100.0f + pan_r) * 0.005f * dry_gain;
        }
    }

    void room_builder_base::update_sample_rate(long sr)
    {
        for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
            vConvolvers[i].sDelay.init(millis_to_samples(sr, impulse_reverb_base_metadata::PREDELAY_MAX * 4.0f));

        for (size_t i=0; i<2; ++i)
        {
            vChannels[i].sBypass.init(sr);
            vChannels[i].sEqualizer.set_sample_rate(sr);
        }
    }

    void room_builder_base::process(size_t samples)
    {
        //---------------------------------------------------------------------
        // Stage 1: process reconfiguration requests and file events
        sync_offline_tasks();

        //---------------------------------------------------------------------
        // Stage 3: output parameters
        if (p3DStatus != NULL)
            p3DStatus->setValue(nSceneStatus);
        if (p3DProgress != NULL)
            p3DProgress->setValue(fSceneProgress);
    }

    void room_builder_base::sync_offline_tasks()
    {
        // Check the state of input file
        path_t *path        = p3DFile->getBuffer<path_t>();
        if (path != NULL)
        {
            if ((path->pending()) && (s3DLoader.idle())) // There is pending request for 3D file reload
            {
                // Try to submit task
                if (pExecutor->submit(&s3DLoader))
                {
                    lsp_trace("successfully submitted load task");
                    nSceneStatus    = STATUS_LOADING;
                    fSceneProgress  = 0.0f;
                    path->accept();
                }
            }
            else if ((path->accepted()) && (s3DLoader.completed())) // The reload request has been processed
            {
                // Update file status and set re-rendering flag
                nSceneStatus    = s3DLoader.code();
                fSceneProgress  = 100.0f;
                sScene.swap(&s3DLoader.sScene);
                nReconfigReq    ++;

                // Now we surely can commit changes and reset task state
                lsp_trace("File loading task has completed with status %d", int(nSceneStatus));
                path->commit();
                s3DLoader.reset();
            }
        }
    }

    //-------------------------------------------------------------------------
    room_builder_mono::room_builder_mono(): room_builder_base(metadata, 1)
    {
    }

    room_builder_mono::~room_builder_mono()
    {
    }

    room_builder_stereo::room_builder_stereo(): room_builder_base(metadata, 1)
    {
    }

    room_builder_stereo::~room_builder_stereo()
    {
    }
}

