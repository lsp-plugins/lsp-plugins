/*
 * impulse_responses.cpp
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>

#include <plugins/impulse_responses.h>
#include <core/debug.h>
#include <core/status.h>
#include <core/fade.h>

#include <string.h>

#define TMP_BUF_SIZE            4096
#define CONV_RANK               10
#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    static float band_freqs[] =
    {
        73.0f,
        156.0f,
        332.0f,
        707.0f,
        1507.0f,
        3213.0f,
        6849.0f
    };

    impulse_responses_base::IRLoader::IRLoader(impulse_responses_base *base, af_descriptor_t *descr)
    {
        pCore       = base;
        pDescr      = descr;
    }

    impulse_responses_base::IRLoader::~IRLoader()
    {
        pCore       = NULL;
        pDescr      = NULL;
    }

    int impulse_responses_base::IRLoader::run()
    {
        return pCore->load(pDescr);
    }

    impulse_responses_base::IRConfigurator::IRConfigurator(impulse_responses_base *base)
    {
        pCore       = base;
        for (size_t i=0; i<impulse_responses_base_metadata::TRACKS_MAX; ++i)
        {
            sReconfig[i].bRender    = false;
            sReconfig[i].nSource    = 0;
            sReconfig[i].nRank      = 0;
        }
    }

    impulse_responses_base::IRConfigurator::~IRConfigurator()
    {
        pCore       = NULL;
    }

    int impulse_responses_base::IRConfigurator::run()
    {
        return pCore->reconfigure(sReconfig);
    }

    //-------------------------------------------------------------------------
    impulse_responses_base::impulse_responses_base(const plugin_metadata_t &metadata, size_t channels):
        plugin_t(metadata), sConfigurator(this)
    {
        nChannels       = channels;
        vChannels       = NULL;
        vFiles          = NULL;
        pExecutor       = NULL;
        nReconfigReq    = 0;
        nReconfigResp   = -1;
        fGain           = 1.0f;

        pBypass         = NULL;
        pRank           = NULL;
        pDry            = NULL;
        pWet            = NULL;
        pOutGain        = NULL;

        pData           = NULL;
    }

    impulse_responses_base::~impulse_responses_base()
    {
    }

    void impulse_responses_base::destroy_file(af_descriptor_t *af)
    {
        // Destroy sample
        if (af->pSwapSample != NULL)
        {
            af->pSwapSample->destroy();
            delete af->pSwapSample;
            af->pSwapSample = NULL;
        }
        if (af->pCurrSample != NULL)
        {
            af->pCurrSample->destroy();
            delete af->pCurrSample;
            af->pCurrSample = NULL;
        }

        // Destroy current file
        if (af->pCurr != NULL)
        {
            af->pCurr->destroy();
            delete af->pCurr;
            af->pCurr    = NULL;
        }

        if (af->pSwap != NULL)
        {
            af->pSwap->destroy();
            delete af->pSwap;
            af->pSwap    = NULL;
        }

        // Destroy loader
        if (af->pLoader != NULL)
        {
            delete af->pLoader;
            af->pLoader     = NULL;
        }

        // Forget port
        af->pFile       = NULL;
    }

    void impulse_responses_base::destroy_channel(channel_t *c)
    {
        if (c->pCurr != NULL)
        {
            c->pCurr->destroy();
            delete c->pCurr;
            c->pCurr    = NULL;
        }

        if (c->pSwap != NULL)
        {
            c->pSwap->destroy();
            delete c->pSwap;
            c->pSwap    = NULL;
        }

        c->sDelay.destroy();
        c->sPlayer.destroy(false);
        c->sEqualizer.destroy();
    }

    size_t impulse_responses_base::get_fft_rank(size_t rank)
    {
        return impulse_responses_base_metadata::FFT_RANK_MIN + rank;
    }

    void impulse_responses_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        // Remember executor service
        pExecutor       = wrapper->get_executor();
        lsp_trace("Executor = %p", pExecutor);

        // Allocate buffer data
        size_t tmp_buf_size = TMP_BUF_SIZE * sizeof(float);
        size_t thumbs_size  = impulse_responses_base_metadata::MESH_SIZE * sizeof(float);
        size_t thumbs_perc  = thumbs_size * impulse_responses_base_metadata::TRACKS_MAX;
        size_t alloc        = (tmp_buf_size + thumbs_perc) * nChannels;
        pData               = new uint8_t[alloc + DEFAULT_ALIGN];
        if (pData == NULL)
            return;
        uint8_t *ptr    = ALIGN_PTR(pData, DEFAULT_ALIGN);

        // Allocate channels
        vChannels       = new channel_t[nChannels];
        if (vChannels == NULL)
            return;

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];

            if (!c->sPlayer.init(nChannels, 32))
                return;
            if (!c->sEqualizer.init(impulse_responses_base_metadata::EQ_BANDS + 2, CONV_RANK))
                return;
            c->sEqualizer.set_mode(EQM_BYPASS);

            c->pCurr        = NULL;
            c->pSwap        = NULL;

            c->vIn          = NULL;
            c->vOut         = NULL;
            c->vBuffer      = reinterpret_cast<float *>(ptr);
            ptr            += tmp_buf_size;

            c->fDryGain     = 0.0f;
            c->fWetGain     = 1.0f;
            c->nSource      = 0;
            c->nSourceReq   = 0;
            c->nRank        = 0;
            c->nRankReq     = 0;

            c->pIn          = NULL;
            c->pOut         = NULL;

            c->pSource      = NULL;
            c->pMakeup      = NULL;
            c->pActivity    = NULL;
            c->pPredelay    = NULL;

            c->pWetEq       = NULL;
            c->pLowCut      = NULL;
            c->pLowFreq     = NULL;
            c->pHighCut     = NULL;
            c->pHighFreq    = NULL;

            for (size_t j=0; j<impulse_responses_base_metadata::EQ_BANDS; ++j)
                c->pFreqGain[j]     = NULL;
        }

        // Allocate files
        vFiles          = new af_descriptor_t[nChannels];
        if (vFiles == NULL)
            return;

        for (size_t i=0; i<nChannels; ++i)
        {
            af_descriptor_t *f    = &vFiles[i];

            f->pCurr        = NULL;
            f->pSwap        = NULL;
            f->pSwapSample  = NULL;
            f->pCurrSample  = NULL;

            for (size_t j=0; j<impulse_responses_base_metadata::TRACKS_MAX; ++j, ptr += thumbs_size)
                f->vThumbs[j]   = reinterpret_cast<float *>(ptr);

            f->fNorm        = 1.0f;
            f->bRender      = false;
            f->nStatus      = STATUS_UNSPECIFIED;
            f->bSync        = true;
            f->bSwap        = false;
            f->fHeadCut     = 0.0f;
            f->fTailCut     = 0.0f;
            f->fFadeIn      = 0.0f;
            f->fFadeOut     = 0.0f;

            f->pLoader      = new IRLoader(this, f);
            if (f->pLoader == NULL)
                return;
            f->pFile        = NULL;
            f->pHeadCut     = NULL;
            f->pTailCut     = NULL;
            f->pFadeIn      = NULL;
            f->pFadeOut     = NULL;
            f->pListen      = NULL;
            f->pStatus      = NULL;
            f->pLength      = NULL;
            f->pThumbs      = NULL;
        }

        // Bind ports
        size_t port_id = 0;

        lsp_trace("Binding audio ports");
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pIn    = vPorts[port_id++];
        }
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pOut   = vPorts[port_id++];
        }

        // Bind common ports
        lsp_trace("Binding common ports");
        TRACE_PORT(vPorts[port_id]);
        pBypass     = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pRank       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDry        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pWet        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOutGain    = vPorts[port_id++];

        // Skip file selector
        if (nChannels > 1)
        {
            TRACE_PORT(vPorts[port_id]);
            port_id++;
        }

        // Bind impulse file ports
        for (size_t i=0; i<nChannels; ++i)
        {
            lsp_trace("Binding impulse file #%d ports", int(i));
            af_descriptor_t *f  = &vFiles[i];

            TRACE_PORT(vPorts[port_id]);
            f->pFile        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            f->pHeadCut     = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            f->pTailCut     = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            f->pFadeIn      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            f->pFadeOut     = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            f->pListen      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            f->pStatus      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            f->pLength      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            f->pThumbs      = vPorts[port_id++];
        }

        // Bind convolution ports
        for (size_t i=0; i<nChannels; ++i)
        {
            lsp_trace("Binding convolution #%d ports", int(i));
            channel_t *c    = &vChannels[i];

            TRACE_PORT(vPorts[port_id]);
            c->pSource      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pMakeup      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pActivity    = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pPredelay    = vPorts[port_id++];
        }

        // Bind wet processing ports
        lsp_trace("Binding wet processing ports");
        size_t port         = port_id;
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c        = &vChannels[i];

            TRACE_PORT(vPorts[port_id]);
            c->pWetEq           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pLowCut          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pLowFreq         = vPorts[port_id++];

            for (size_t j=0; j<impulse_responses_base_metadata::EQ_BANDS; ++j)
            {
                TRACE_PORT(vPorts[port_id]);
                c->pFreqGain[j]     = vPorts[port_id++];
            }

            TRACE_PORT(vPorts[port_id]);
            c->pHighCut         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pHighFreq        = vPorts[port_id++];

            port_id         = port;
        }

    }

    void impulse_responses_base::destroy()
    {
        // Drop buffers
        if (vChannels != NULL)
        {
            for (size_t i=0; i<nChannels; ++i)
                destroy_channel(&vChannels[i]);
            delete [] vChannels;
            vChannels       = NULL;
        }

        if (vFiles != NULL)
        {
            for (size_t i=0; i<nChannels; ++i)
                destroy_file(&vFiles[i]);

            delete [] vFiles;
            vFiles          = NULL;
        }

        if (pData != NULL)
        {
            delete [] pData;
            pData           = NULL;
        }
    }

    void impulse_responses_base::ui_activated()
    {
        // Force file contents to be synchronized with UI
        for (size_t i=0; i<nChannels; ++i)
            vFiles[i].bSync     = true;
    }

    void impulse_responses_base::update_settings()
    {
        fGain               = pOutGain->getValue();

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c        = &vChannels[i];
            af_descriptor_t *f  = &vFiles[i];

            c->fDryGain         = pDry->getValue() * fGain;
            c->fWetGain         = pWet->getValue() * c->pMakeup->getValue() * fGain;

            // Update delay and bypass configuration
            c->sPlayer.set_gain(fGain);
            c->sDelay.set_delay(millis_to_samples(fSampleRate, c->pPredelay->getValue()));
            c->sBypass.set_bypass(pBypass->getValue() >= 0.5f);

            // Check that file parameters have changed
            float head_cut      = f->pHeadCut->getValue();
            float tail_cut      = f->pTailCut->getValue();
            float fade_in       = f->pFadeIn->getValue();
            float fade_out      = f->pFadeOut->getValue();
            if ((f->fHeadCut != head_cut) ||
                (f->fTailCut != tail_cut) ||
                (f->fFadeIn  != fade_in ) ||
                (f->fFadeOut != fade_out))
            {
                f->fHeadCut         = head_cut;
                f->fTailCut         = tail_cut;
                f->fFadeIn          = fade_in;
                f->fFadeOut         = fade_out;
                f->bRender          = true;
                nReconfigReq        ++;
            }

            // Listen button pressed?
            if (f->pListen->getValue() >= 0.5f)
            {
                size_t n_c = (f->pCurrSample != NULL) ? f->pCurrSample->channels() : 0;
                if (n_c > 0)
                {
                    for (size_t j=0; j<nChannels; ++j)
                        vChannels[j].sPlayer.play(i, j%n_c, 1.0f, 0);
                }
            }

            size_t source       = c->pSource->getValue();
            size_t rank         = get_fft_rank(pRank->getValue());
            if ((source != c->nSourceReq) || (rank != c->nRankReq))
            {
                nReconfigReq        ++;
                c->nSourceReq       = source;
                c->nRankReq         = rank;
            }

            // Get path
            path_t *path        = f->pFile->getBuffer<path_t>();
            if ((path != NULL) && (path->pending()) && (f->pLoader->idle()))
            {
                // Try to submit task
                if (pExecutor->submit(f->pLoader))
                {
                    lsp_trace("successfully submitted load task");
                    f->nStatus      = STATUS_LOADING;
                    path->accept();
                }
            }

            // Update equalization parameters
            Equalizer *eq               = &c->sEqualizer;
            equalizer_mode_t eq_mode    = (c->pWetEq->getValue() >= 0.5f) ? EQM_IIR : EQM_BYPASS;
            eq->set_mode(eq_mode);

            if (eq_mode != EQM_BYPASS)
            {
                filter_params_t fp;
                size_t band     = 0;

                // Set-up parametric equalizer
                while (band < impulse_responses_base_metadata::EQ_BANDS)
                {
                    if (band == 0)
                    {
                        fp.fFreq        = band_freqs[band];
                        fp.fFreq2       = fp.fFreq;
                        fp.nType        = FLT_MT_LRX_LOSHELF;
                    }
                    else if (band == (impulse_responses_base_metadata::EQ_BANDS - 1))
                    {
                        fp.fFreq        = band_freqs[band-1];
                        fp.fFreq2       = fp.fFreq;
                        fp.nType        = FLT_MT_LRX_HISHELF;
                    }
                    else
                    {
                        fp.fFreq        = band_freqs[band-1];
                        fp.fFreq2       = band_freqs[band];
                        fp.nType        = FLT_MT_LRX_LADDERPASS;
                    }

                    fp.fGain        = c->pFreqGain[band]->getValue();
                    fp.nSlope       = 2;
                    fp.fQuality     = 0.0f;

                    // Update filter parameters
                    eq->set_params(band++, &fp);
                }

                // Setup hi-pass filter
                size_t hp_slope = c->pLowCut->getValue() * 2;
                fp.nType        = (hp_slope > 0) ? FLT_BT_BWC_HIPASS : FLT_NONE;
                fp.fFreq        = c->pLowFreq->getValue();
                fp.fFreq2       = fp.fFreq;
                fp.fGain        = 1.0f;
                fp.nSlope       = hp_slope;
                fp.fQuality     = 0.0f;
                eq->set_params(band++, &fp);

                // Setup low-pass filter
                size_t lp_slope = c->pHighCut->getValue() * 2;
                fp.nType        = (lp_slope > 0) ? FLT_BT_BWC_LOPASS : FLT_NONE;
                fp.fFreq        = c->pHighFreq->getValue();
                fp.fFreq2       = fp.fFreq;
                fp.fGain        = 1.0f;
                fp.nSlope       = lp_slope;
                fp.fQuality     = 0.0f;
                eq->set_params(band++, &fp);
            }
        }
    }

    void impulse_responses_base::update_sample_rate(long sr)
    {
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c = &vChannels[i];

            c->sBypass.init(sr);
            c->sDelay.init(millis_to_samples(sr, impulse_responses_base_metadata::PREDELAY_MAX));
            c->sEqualizer.set_sample_rate(sr);
        }
    }

    void impulse_responses_base::process(size_t samples)
    {
        //---------------------------------------------------------------------
        // Stage 1: process reconfiguration requests and file events
        if (sConfigurator.idle())
        {
            // Check that reconfigure is pending
            if (nReconfigReq != nReconfigResp)
            {
                // Remember render state
                for (size_t i=0; i<nChannels; ++i)
                    sConfigurator.set_render(i, vFiles[i].bRender);
                for (size_t i=0; i<nChannels; ++i)
                {
                    sConfigurator.set_source(i, vChannels[i].nSourceReq);
                    sConfigurator.set_rank(i, vChannels[i].nRankReq);
                }

                // Try to submit task
                if (pExecutor->submit(&sConfigurator))
                {
                    lsp_trace("successfully submitted configuration task");

                    // Clear render state and reconfiguration request
                    nReconfigResp   = nReconfigReq;
                    for (size_t i=0; i<nChannels; ++i)
                        vFiles[i].bRender   = false;
                }
            }
            else
            {
                // Process file requests
                for (size_t i=0; i<nChannels; ++i)
                {
                    // Get descriptor
                    af_descriptor_t *af     = &vFiles[i];
                    if (af->pFile == NULL)
                        continue;

                    // Get path and check task state
                    path_t *path = af->pFile->getBuffer<path_t>();
                    if ((path != NULL) && (path->accepted()) && (af->pLoader->completed()))
                    {
                        // Swap file data
                        AudioFile *fd   = af->pSwap;
                        af->pSwap       = af->pCurr;
                        af->pCurr       = fd;

                        // Update file status and set re-rendering flag
                        af->nStatus     = af->pLoader->code();
                        af->bRender     = true;
                        nReconfigReq    ++;

                        // Now we surely can commit changes and reset task state
                        path->commit();
                        af->pLoader->reset();
                    }
                }
            }
        }
        else if (sConfigurator.completed())
        {
            // Update samples
            for (size_t i=0; i<nChannels; ++i)
            {
                af_descriptor_t *f = &vFiles[i];
                if (f->bSwap)
                {
                    Sample *s           = f->pCurrSample;
                    f->pCurrSample      = f->pSwapSample;
                    f->pSwapSample      = s;
                    f->bSwap            = false;
                }

                f->bSync = true;
            }

            // Update convolvers
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = &vChannels[i];
                Convolver *cv   = c->pCurr;
                c->pCurr        = c->pSwap;
                c->pSwap        = cv;

                // Bind sample player
                for (size_t j=0; j<nChannels; ++j)
                {
                    af_descriptor_t *f = &vFiles[j];
                    c->sPlayer.bind(j, f->pCurrSample, false);
                }
            }

            // Reset configurator
            sConfigurator.reset();
        }

        //---------------------------------------------------------------------
        // Stage 2: perform convolution
        // Get pointers to data channels
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];

            c->vIn          = c->pIn->getBuffer<float>();
            c->vOut         = c->pOut->getBuffer<float>();
        }

        // Process samples
        while (samples > 0)
        {
            size_t to_do        = TMP_BUF_SIZE;
            if (to_do > samples)
                to_do               = samples;

            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c    = &vChannels[i];

                // Do processing
                if (c->pCurr != NULL)
                    c->pCurr->process(c->vBuffer, c->vIn, to_do);
                else
                    dsp::fill_zero(c->vBuffer, to_do);
                c->sEqualizer.process(c->vBuffer, c->vBuffer, to_do); // Process wet signal with equalizer
                c->sDelay.process(c->vBuffer, c->vBuffer, to_do);
                dsp::mix2(c->vBuffer, c->vIn, c->fWetGain, c->fDryGain, to_do);
                c->sPlayer.process(c->vBuffer, c->vBuffer, to_do);
                c->sBypass.process(c->vOut, c->vIn, c->vBuffer, to_do);

                // Update pointers
                c->vIn             += to_do;
                c->vOut            += to_do;
            }

            samples            -= to_do;
        }

        //---------------------------------------------------------------------
        // Stage 3: output parameters

        for (size_t i=0; i<nChannels; ++i)
        {
            af_descriptor_t *af     = &vFiles[i];
            channel_t *c            = &vChannels[i];

            // Output information about the file
            c->pActivity->setValue(c->pCurr != NULL);
            size_t length           = (af->pCurr != NULL) ? af->pCurr->samples() : 0;
//            lsp_trace("channels=%d, samples=%d, timing=%f",
//                    int((af->pCurr != NULL) ? af->pCurr->channels() : -1),
//                    int(length),
//                    samples_to_millis(fSampleRate, length)
//                );
            af->pLength->setValue(samples_to_millis(fSampleRate, length));
            af->pStatus->setValue(af->nStatus);

            // Store file dump to mesh
            mesh_t *mesh        = af->pThumbs->getBuffer<mesh_t>();
//            lsp_trace("i=%d, mesh=%p, is_empty=%d, bSync=%d", int(i), mesh, int(mesh->isEmpty()), int(af->bSync));
            if ((mesh == NULL) || (!mesh->isEmpty()) || (!af->bSync))
                continue;

            size_t channels     = (af->pCurrSample != NULL) ? af->pCurrSample->channels() : 0;
//            lsp_trace("curr_sample=%p, channels=%d", af->pCurrSample, int(channels));
            if (channels > 0)
            {
                // Copy thumbnails
                for (size_t j=0; j<channels; ++j)
                    dsp::copy(mesh->pvData[j], af->vThumbs[j], impulse_responses_base_metadata::MESH_SIZE);
                mesh->data(channels, impulse_responses_base_metadata::MESH_SIZE);
            }
            else
                mesh->data(0, 0);
            af->bSync           = false;
        }
    }

    status_t impulse_responses_base::load(af_descriptor_t *descr)
    {
        lsp_trace("descr = %p", descr);

        // Remove swap data
        if (descr->pSwap != NULL)
        {
            descr->pSwap->destroy();
            delete descr->pSwap;
            descr->pSwap    = NULL;
        }

        // Check state
        if ((descr == NULL) || (descr->pFile == NULL))
            return STATUS_UNKNOWN_ERR;

        // Get path
        path_t *path = descr->pFile->getBuffer<path_t>();
        if (path == NULL)
            return STATUS_UNKNOWN_ERR;

        // Get file name
        const char *fname = path->get_path();
        if (strlen(fname) <= 0)
            return STATUS_UNSPECIFIED;

        // Load audio file
        AudioFile *af   = new AudioFile();
        if (af == NULL)
            return STATUS_NO_MEM;

        // Try to load file
        float convLengthMaxSeconds = impulse_reverb_base_metadata::CONV_LENGTH_MAX * 0.001f;
        status_t status = af->load(fname,  convLengthMaxSeconds);
        if (status != STATUS_OK)
        {
            af->destroy();
            delete af;
            lsp_trace("load failed: status=%d (%s)", status, get_status(status));
            return status;
        }

        // Try to resample
        status  = af->resample(fSampleRate);
        if (status != STATUS_OK)
        {
            af->destroy();
            delete af;
            lsp_trace("resample failed: status=%d (%s)", status, get_status(status));
            return status;
        }

        // Determine the normalizing factor
        size_t channels         = af->channels();
        float max = 0.0f;

        for (size_t i=0; i<channels; ++i)
        {
            // Determine the maximum amplitude
            float a_max = dsp::abs_max(af->channel(i), af->samples());
            if (max < a_max)
                max     = a_max;
        }
        descr->fNorm    = (max != 0.0f) ? 1.0f / max : 1.0f;

        // File was successfully loaded, report to caller
        descr->pSwap    = af;

        return STATUS_OK;
    }

    status_t impulse_responses_base::reconfigure(const reconfig_t *cfg)
    {
        // Re-render files (if needed)
        for (size_t i=0; i<nChannels; ++i)
        {
            // Do we need to re-render file?
            if (!cfg[i].bRender)
                continue;

            // Get audio file
            af_descriptor_t *f  = &vFiles[i];
            AudioFile *af       = f->pCurr;

            // Destroy swap sample
            if (f->pSwapSample != NULL)
            {
                f->pSwapSample->destroy();
                delete f->pSwapSample;
                f->pSwapSample  = NULL;
            }

            Sample *s           = new Sample();
            if (s == NULL)
                return STATUS_NO_MEM;
            f->pSwapSample      = s;
            f->bSwap            = true;

            if (af == NULL)
                continue;

            ssize_t flen        = af->samples();
            size_t channels     = (af->channels() < impulse_responses_base_metadata::TRACKS_MAX) ?
                                    af->channels() : impulse_responses_base_metadata::TRACKS_MAX;

            // Buffer is present, file is present, check boundaries
            size_t head_cut     = millis_to_samples(fSampleRate, f->fHeadCut);
            size_t tail_cut     = millis_to_samples(fSampleRate, f->fTailCut);
            ssize_t fsamples    = flen - head_cut - tail_cut;
            if (fsamples <= 0)
            {
                s->setLength(0);
                continue;
            }

            // Now ensure that we have enough space for sample
            if (!s->init(channels, flen, fsamples))
                return STATUS_NO_MEM;

            // Copy data to temporary buffer and apply fading
            for (size_t i=0; i<channels; ++i)
            {
                float *dst = s->getBuffer(i);
                const float *src = af->channel(i);

                // Copy sample data and apply fading
                dsp::copy(dst, &src[head_cut], fsamples);
                fade_in(dst, dst, millis_to_samples(fSampleRate, f->fFadeIn), fsamples);
                fade_out(dst, dst, millis_to_samples(fSampleRate, f->fFadeOut), fsamples);

                // Now render thumbnail
                src                 = dst;
                dst                 = f->vThumbs[i];
                for (size_t k=0; k<impulse_responses_base_metadata::MESH_SIZE; ++k)
                {
                    size_t first    = (k * fsamples) / impulse_responses_base_metadata::MESH_SIZE;
                    size_t last     = ((k + 1) * fsamples) / impulse_responses_base_metadata::MESH_SIZE;
                    if (first < last)
                        dst[k]          = dsp::abs_max(&src[first], last - first);
                    else
                        dst[k]          = fabs(src[first]);
                }

                // Normalize graph if possible
                if (f->fNorm != 1.0f)
                    dsp::scale2(dst, f->fNorm, impulse_responses_base_metadata::MESH_SIZE);
            }
        }

        // Randomize phase of the convolver
        uint32_t phase  = seed_addr(this);
        phase           = ((phase << 16) | (phase >> 16)) & 0x7fffffff;
        uint32_t step   = 0x80000000 / (impulse_reverb_base_metadata::CONVOLVERS + 1);

        // OK, files have been rendered, now need to commutate
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];

            // Check that we need to free previous convolver
            if (c->pSwap != NULL)
            {
                c->pSwap->destroy();
                delete c->pSwap;
                c->pSwap = NULL;
            }

            // Check that routing has changed
            size_t ch   = cfg[i].nSource;
            if ((ch--) == 0)
            {
                c->nSource  = 0;
                c->nRank    = cfg[i].nRank;
                continue;
            }

            // Apply new routing
            size_t track    = ch % impulse_responses_base_metadata::TRACKS_MAX;
            size_t file     = ch / impulse_responses_base_metadata::TRACKS_MAX;
            if (file >= nChannels)
                continue;

            // Analyze sample
            Sample *s       = (vFiles[file].bSwap) ? vFiles[file].pSwapSample : vFiles[file].pCurrSample;
            if ((s == NULL) || (!s->valid()) || (s->channels() <= track))
                continue;

            // Now we can create convolver
            Convolver *cv   = new Convolver();
            if (!cv->init(s->getBuffer(track), s->length(), cfg[i].nRank, float((phase + i*step)& 0x7fffffff)/float(0x80000000)))
                return STATUS_NO_MEM;
            c->pSwap        = cv;
        }

        return STATUS_OK;
    }

    //-------------------------------------------------------------------------

    impulse_responses_mono::impulse_responses_mono(): impulse_responses_base(metadata, 1)
    {
    }

    impulse_responses_mono::~impulse_responses_mono()
    {
    }

    impulse_responses_stereo::impulse_responses_stereo(): impulse_responses_base(metadata, 2)
    {
    }

    impulse_responses_stereo::~impulse_responses_stereo()
    {
    }

} /* namespace lsp */
