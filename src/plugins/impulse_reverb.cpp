/*
 * impulse_reverb.cpp
 *
 *  Created on: 13 фев. 2017 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>

#include <plugins/impulse_reverb.h>
#include <core/debug.h>
#include <core/status.h>
#include <core/fade.h>

#include <stdlib.h>
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

    void impulse_reverb_base::IRLoader::init(impulse_reverb_base *base, af_descriptor_t *descr)
    {
        pCore       = base;
        pDescr      = descr;
    }

    impulse_reverb_base::IRLoader::~IRLoader()
    {
        pCore       = NULL;
        pDescr      = NULL;
    }

    int impulse_reverb_base::IRLoader::run()
    {
        return pCore->load(pDescr);
    }

    impulse_reverb_base::IRConfigurator::IRConfigurator(impulse_reverb_base *base)
    {
        pCore       = base;
        for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
            sReconfig.bRender[i]    = false;
        for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
        {
            sReconfig.nFile[i]      = 0;
            sReconfig.nTrack[i]     = 0;
            sReconfig.nRank[i]      = 0;
        }
    }

    impulse_reverb_base::IRConfigurator::~IRConfigurator()
    {
        pCore       = NULL;
    }

    int impulse_reverb_base::IRConfigurator::run()
    {
        return pCore->reconfigure(&sReconfig);
    }

    //-------------------------------------------------------------------------
    impulse_reverb_base::impulse_reverb_base(const plugin_metadata_t &metadata, size_t inputs):
        plugin_t(metadata), sConfigurator(this)
    {
        nInputs         = inputs;
        nReconfigReq    = 0;
        nReconfigResp   = -1;

        pBypass         = NULL;
        pRank           = NULL;
        pDry            = NULL;
        pWet            = NULL;
        pOutGain        = NULL;
        pPredelay       = NULL;

        pData           = NULL;
        pExecutor       = NULL;
    }

    impulse_reverb_base::~impulse_reverb_base()
    {
    }

    void impulse_reverb_base::destroy_file(af_descriptor_t *af)
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

        // Forget port
        af->pFile       = NULL;
    }

    void impulse_reverb_base::destroy_convolver(convolver_t *cv)
    {
        cv->sDelay.destroy();

        if (cv->pCurr != NULL)
        {
            lsp_trace("Destroying pCurr=%p", cv->pSwap);
            cv->pCurr->destroy();
            delete cv->pCurr;
            cv->pCurr    = NULL;
        }

        if (cv->pSwap != NULL)
        {
            lsp_trace("Destroying pSwap=%p", cv->pSwap);
            cv->pSwap->destroy();
            delete cv->pSwap;
            cv->pSwap    = NULL;
        }

        cv->vBuffer     = NULL;
    }

    void impulse_reverb_base::destroy_channel(channel_t *c)
    {
        c->sPlayer.destroy(false);
        c->sEqualizer.destroy();
        c->vOut     = NULL;
        c->vBuffer  = NULL;
    }

    size_t impulse_reverb_base::get_fft_rank(size_t rank)
    {
        return impulse_reverb_base_metadata::FFT_RANK_MIN + rank;
    }

    void impulse_reverb_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        // Remember executor service
        pExecutor       = wrapper->get_executor();
        lsp_trace("Executor = %p", pExecutor);

        // Allocate buffer data
        size_t tmp_buf_size = TMP_BUF_SIZE * sizeof(float);
        size_t thumbs_size  = impulse_reverb_base_metadata::MESH_SIZE * sizeof(float);
        size_t alloc        = tmp_buf_size * (impulse_reverb_base_metadata::CONVOLVERS + 2) +
                              thumbs_size * impulse_reverb_base_metadata::TRACKS_MAX * impulse_reverb_base_metadata::FILES;
        pData               = new uint8_t[alloc + DEFAULT_ALIGN];
        if (pData == NULL)
            return;
        uint8_t *ptr    = ALIGN_PTR(pData, DEFAULT_ALIGN);

        // Initialize inputs
        for (size_t i=0; i<2; ++i)
        {
            input_t *in     = &vInputs[i];
            in->vIn         = NULL;
            in->pIn         = NULL;
            in->pPan        = NULL;
        }

        // Initialize files
        for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
        {
            af_descriptor_t *f    = &vFiles[i];

            f->pCurr        = NULL;
            f->pSwap        = NULL;
            f->pSwapSample  = NULL;
            f->pCurrSample  = NULL;

            for (size_t j=0; j<impulse_reverb_base_metadata::TRACKS_MAX; ++j)
            {
                f->vThumbs[j]   = reinterpret_cast<float *>(ptr);
                ptr            += thumbs_size;
            }

            f->fNorm        = 1.0f;
            f->bRender      = false;
            f->nStatus      = STATUS_UNSPECIFIED;
            f->bSync        = true;
            f->bSwap        = false;
            f->fHeadCut     = 0.0f;
            f->fTailCut     = 0.0f;
            f->fFadeIn      = 0.0f;
            f->fFadeOut     = 0.0f;
            f->bReverse     = false;

            // Initialize loader
            f->sLoader.init(this, f);

            f->pFile        = NULL;
            f->pHeadCut     = NULL;
            f->pTailCut     = NULL;
            f->pFadeIn      = NULL;
            f->pFadeOut     = NULL;
            f->pListen      = NULL;
            f->pReverse     = NULL;
            f->pStatus      = NULL;
            f->pLength      = NULL;
            f->pThumbs      = NULL;
        }

        // Initialize convolvers
        for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
        {
            convolver_t *cv     = &vConvolvers[i];

            cv->pCurr           = NULL;
            cv->pSwap           = NULL;
            cv->nRank           = 0;
            cv->nRankReq        = 0;
            cv->nSource         = 0;
            cv->nFileReq        = 0;
            cv->nTrackReq       = 0;

            cv->vBuffer         = reinterpret_cast<float *>(ptr);
            ptr                += tmp_buf_size;

            cv->fPanIn[0]       = 1.0f;
            cv->fPanIn[1]       = 0.0f;
            cv->fPanOut[0]      = 1.0f;
            cv->fPanOut[1]      = 0.0f;

            cv->pMakeup         = NULL;
            cv->pPanIn          = NULL;
            cv->pPanOut         = NULL;
            cv->pFile           = NULL;
            cv->pTrack          = NULL;
            cv->pPredelay       = NULL;
            cv->pMute           = NULL;
            cv->pActivity       = NULL;
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

        // Bind common ports
        lsp_trace("Binding common ports");
        TRACE_PORT(vPorts[port_id]);
        pBypass     = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);            // Skip file selector
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

        // Bind impulse file ports
        for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
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
            f->pReverse     = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            f->pStatus      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            f->pLength      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            f->pThumbs      = vPorts[port_id++];
        }

        // Bind convolver ports
        for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
        {
            lsp_trace("Binding convolution #%d ports", int(i));
            convolver_t *c  = &vConvolvers[i];

            if (nInputs > 1)    // Input panning
            {
                TRACE_PORT(vPorts[port_id]);
                c->pPanIn       = vPorts[port_id++];
            }

            TRACE_PORT(vPorts[port_id]);
            c->pFile        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pTrack       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pMakeup      = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pMute        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pActivity    = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pPredelay    = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pPanOut      = vPorts[port_id++];
        }

        // Bind wet processing ports
        lsp_trace("Binding wet processing ports");
        size_t port         = port_id;
        for (size_t i=0; i<2; ++i)
        {
            channel_t *c        = &vChannels[i];

            TRACE_PORT(vPorts[port_id]);
            c->pWetEq           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pLowCut          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pLowFreq         = vPorts[port_id++];

            for (size_t j=0; j<impulse_reverb_base_metadata::EQ_BANDS; ++j)
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

    void impulse_reverb_base::destroy()
    {
        // Destroy files
        for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
            destroy_file(&vFiles[i]);

        // Destroy convolvers
        for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
            destroy_convolver(&vConvolvers[i]);

        // Destroy output channels
        for (size_t i=0; i<2; ++i)
            destroy_channel(&vChannels[i]);

        // Delete all allocated data
        if (pData != NULL)
        {
            delete [] pData;
            pData           = NULL;
        }
    }

    void impulse_reverb_base::ui_activated()
    {
        // Force file contents to be synchronized with UI
        for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
            vFiles[i].bSync     = true;
    }

    void impulse_reverb_base::update_settings()
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

        // Adjust channel setup
        for (size_t i=0; i<2; ++i)
        {
            channel_t *c        = &vChannels[i];
            c->sBypass.set_bypass(bypass);
            c->sPlayer.set_gain(out_gain);

            // Update equalization parameters
            Equalizer *eq               = &c->sEqualizer;
            equalizer_mode_t eq_mode    = (c->pWetEq->getValue() >= 0.5f) ? EQM_IIR : EQM_BYPASS;
            eq->set_mode(eq_mode);

            if (eq_mode != EQM_BYPASS)
            {
                filter_params_t fp;
                size_t band     = 0;

                // Set-up parametric equalizer
                while (band < impulse_reverb_base_metadata::EQ_BANDS)
                {
                    if (band == 0)
                    {
                        fp.fFreq        = band_freqs[band];
                        fp.fFreq2       = fp.fFreq;
                        fp.nType        = FLT_MT_LRX_LOSHELF;
                    }
                    else if (band == (impulse_reverb_base_metadata::EQ_BANDS - 1))
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

        // Apply panning to each convolver
        for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
        {
            convolver_t *cv         = &vConvolvers[i];
            float makeup            = cv->pMakeup->getValue() * wet_gain;
            if (nInputs == 1)
            {
                cv->fPanIn[0]       = 1.0f;
                cv->fPanIn[1]       = 0.0f;
            }
            else
            {
                float pan           = cv->pPanIn->getValue();
                cv->fPanIn[0]       = (100.0f - pan) * 0.005f;
                cv->fPanIn[1]       = (100.0f + pan) * 0.005f;
            }

            float pan           = cv->pPanOut->getValue();
            cv->fPanOut[0]      = (100.0f - pan) * 0.005f * makeup;
            cv->fPanOut[1]      = (100.0f + pan) * 0.005f * makeup;

            // Set pre-delay
            cv->sDelay.set_delay(millis_to_samples(fSampleRate, predelay + cv->pPredelay->getValue()));

            // Analyze source
            size_t file         = (cv->pMute->getValue() < 0.5f) ? cv->pFile->getValue() : 0;
            size_t track        = cv->pTrack->getValue();
            if ((file != cv->nFileReq) || (track != cv->nTrackReq) || (rank != cv->nRankReq))
            {
                nReconfigReq        ++;
                cv->nFileReq        = file;
                cv->nTrackReq       = track;
                cv->nRankReq        = rank;
            }
        }

        // Apply changes to files
        for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
        {
            af_descriptor_t *f  = &vFiles[i];

            // Check that file parameters have changed
            float head_cut      = f->pHeadCut->getValue();
            float tail_cut      = f->pTailCut->getValue();
            float fade_in       = f->pFadeIn->getValue();
            float fade_out      = f->pFadeOut->getValue();
            bool reverse        = f->pReverse->getValue() >= 0.5f;
            if ((f->fHeadCut != head_cut) ||
                (f->fTailCut != tail_cut) ||
                (f->fFadeIn  != fade_in ) ||
                (f->fFadeOut != fade_out) ||
                (f->bReverse != reverse))
            {
                f->fHeadCut         = head_cut;
                f->fTailCut         = tail_cut;
                f->fFadeIn          = fade_in;
                f->fFadeOut         = fade_out;
                f->bReverse         = reverse;
                f->bRender          = true;
                nReconfigReq        ++;
            }

            // Listen button pressed?
            if (f->pListen->getValue() >= 0.5f)
            {
                size_t n_c = (f->pCurrSample != NULL) ? f->pCurrSample->channels() : 0;
                if (n_c > 0)
                {
                    for (size_t j=0; j<2; ++j)
                        vChannels[j].sPlayer.play(i, j % n_c, 1.0f, 0);
                }
            }
        }
    }

    void impulse_reverb_base::update_sample_rate(long sr)
    {
        for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
            vConvolvers[i].sDelay.init(millis_to_samples(sr, impulse_reverb_base_metadata::PREDELAY_MAX * 4.0f));

        for (size_t i=0; i<2; ++i)
        {
            vChannels[i].sBypass.init(sr);
            vChannels[i].sEqualizer.set_sample_rate(sr);
        }
    }

    void impulse_reverb_base::sync_offline_tasks()
    {
        bool ldrs_idle = true; // Indicator that all loaders are currently in idle state

        for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
        {
            // Get descriptor
            af_descriptor_t *f  = &vFiles[i];
            if (f->pFile == NULL)
                continue;

            // Get related file path
            path_t *path        = f->pFile->getBuffer<path_t>();
            if (path == NULL)
                continue;

            // Do not allow launch loaders when reconfiguration is active
            if (sConfigurator.idle())
            {
                if ((path->pending()) && (f->sLoader.idle())) // There is pending request for file reload
                {
                    // Try to submit task
                    if (pExecutor->submit(&f->sLoader))
                    {
                        lsp_trace("successfully submitted load task");
                        f->nStatus      = STATUS_LOADING;
                        path->accept();
                    }
                }
                else if ((path->accepted()) && (f->sLoader.completed())) // The reload request has been processed
                {
                    // Swap file data
                    AudioFile *fd   = f->pSwap;
                    f->pSwap        = f->pCurr;
                    f->pCurr        = fd;

                    // Update file status and set re-rendering flag
                    f->nStatus      = f->sLoader.code();
                    f->bRender      = true;
                    nReconfigReq    ++;

                    // Now we surely can commit changes and reset task state
                    path->commit();
                    f->sLoader.reset();
                }
            }

            // Ensure that there are no active loader tasks
            if (!f->sLoader.idle())
                ldrs_idle = false;
        }

        // Do not allow launch reconfiguration when loaders are active
        if (!ldrs_idle)
            return;

        if ((sConfigurator.idle()) && (nReconfigReq != nReconfigResp))
        {
            // Remember render state
            for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
                sConfigurator.set_render(i, vFiles[i].bRender);
            for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
            {
                sConfigurator.set_file(i, vConvolvers[i].nFileReq);
                sConfigurator.set_track(i, vConvolvers[i].nTrackReq);
                sConfigurator.set_rank(i, vConvolvers[i].nRankReq);
            }

            // Try to submit task
            if (pExecutor->submit(&sConfigurator))
            {
                lsp_trace("successfully submitted configuration task");

                // Clear render state and reconfiguration request
                nReconfigResp   = nReconfigReq;
                for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
                    vFiles[i].bRender   = false;
            }
        }
        else if (sConfigurator.completed())
        {
            // Update samples
            for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
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

                // Bind sample player
                for (size_t j=0; j<2; ++j)
                {
                    channel_t *c = &vChannels[j];
                    c->sPlayer.bind(i, f->pCurrSample, false);
                }
            }

            // Update convolvers
            for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
            {
                convolver_t *c  = &vConvolvers[i];
                Convolver *cv   = c->pCurr;
                c->pCurr        = c->pSwap;
                c->pSwap        = cv;
            }

            // Reset configurator
            sConfigurator.reset();
        }
    }

    void impulse_reverb_base::process(size_t samples)
    {
        //---------------------------------------------------------------------
        // Stage 1: process reconfiguration requests and file events
        sync_offline_tasks();

        //---------------------------------------------------------------------
        // Stage 2: perform convolution
        // Get pointers to data channels
        for (size_t i=0; i<nInputs; ++i)
            vInputs[i].vIn      = vInputs[i].pIn->getBuffer<float>();

        for (size_t i=0; i<2; ++i)
            vChannels[i].vOut   = vChannels[i].pOut->getBuffer<float>();

        // Process samples
        while (samples > 0)
        {
            // Determine number of samples to process
            size_t to_do        = TMP_BUF_SIZE;
            if (to_do > samples)
                to_do               = samples;

            // Clear temporary channel buffer
            dsp::fill_zero(vChannels[0].vBuffer, to_do);
            dsp::fill_zero(vChannels[1].vBuffer, to_do);

            // Call convolvers
            for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
            {
                convolver_t *c      = &vConvolvers[i];

                // Prepare input buffer: apply panning if present
                if (nInputs == 1)
                    dsp::copy(c->vBuffer, vInputs[0].vIn, to_do);
                else
                    dsp::mix_copy2(c->vBuffer, vInputs[0].vIn, vInputs[1].vIn, c->fPanIn[0], c->fPanIn[1], to_do);

                // Do processing
                if (c->pCurr != NULL)
                    c->pCurr->process(c->vBuffer, c->vBuffer, to_do);
                else
                    dsp::fill_zero(c->vBuffer, to_do);
                c->sDelay.process(c->vBuffer, c->vBuffer, to_do);

                // Apply processed signal to output channels
                dsp::scale_add3(vChannels[0].vBuffer, c->vBuffer, c->fPanOut[0], to_do);
                dsp::scale_add3(vChannels[1].vBuffer, c->vBuffer, c->fPanOut[1], to_do);
            }

            // Now apply equalization, bypass control and players
            for (size_t i=0; i<2; ++i)
            {
                channel_t *c        = &vChannels[i];

                // Apply equalization
                c->sEqualizer.process(c->vBuffer, c->vBuffer, to_do);

                // Pass dry sound to output channels
                if (nInputs == 1)
                    dsp::scale_add3(c->vBuffer, vInputs[0].vIn, c->fDryPan[0], to_do);
                else
                    dsp::mix_add2(c->vBuffer, vInputs[0].vIn, vInputs[1].vIn, c->fDryPan[0], c->fDryPan[1], to_do);

                // Apply player and bypass
                c->sPlayer.process(c->vBuffer, c->vBuffer, to_do);
                c->sBypass.process(c->vOut, vInputs[i%nInputs].vIn, c->vBuffer, to_do);

                // Update pointers
                c->vOut            += to_do;
            }

            for (size_t i=0; i<nInputs; ++i)
                vInputs[i].vIn     += to_do;

            samples            -= to_do;
        }

        //---------------------------------------------------------------------
        // Stage 3: output parameters
        for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
        {
            // Output information about the convolver
            convolver_t *c          = &vConvolvers[i];
            c->pActivity->setValue(c->pCurr != NULL);
        }

        for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
        {
            af_descriptor_t *af     = &vFiles[i];

            // Output information about the file
            size_t length           = (af->pCurr != NULL) ? af->pCurr->samples() : 0;
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
                    dsp::copy(mesh->pvData[j], af->vThumbs[j], impulse_reverb_base_metadata::MESH_SIZE);
                mesh->data(channels, impulse_reverb_base_metadata::MESH_SIZE);
            }
            else
                mesh->data(0, 0);
            af->bSync           = false;
        }
    }

    status_t impulse_reverb_base::load(af_descriptor_t *descr)
    {
        lsp_trace("descr = %p", descr);

        // Collect garbage
        AudioFile *af       = descr->pSwap;
        if (af != NULL)
        {
            lsp_trace("Destroying file pSwap = %p", descr->pSwap);
            descr->pSwap    = NULL;
            af->destroy();
            delete af;
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
        af   = new AudioFile();
        if (af == NULL)
            return STATUS_NO_MEM;

        // Try to load file
        float convLengthMaxSeconds = impulse_reverb_base_metadata::CONV_LENGTH_MAX * 0.001f;
        status_t status = af->load(fname, convLengthMaxSeconds);
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
        lsp_trace("Setting pSwap = %p", descr->pSwap);

        return STATUS_OK;
    }

    status_t impulse_reverb_base::reconfigure(const reconfig_t *cfg)
    {
        // Perform garbage collection
        for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
        {
            convolver_t *c      = &vConvolvers[i];
            Convolver *cv       = c->pSwap;
            if (cv == NULL)
                continue;

            lsp_trace("Destroying convolver pSwap=%p for channel %d (pCurr=%p)", c->pSwap, int(i), c->pCurr);
            c->pSwap            = NULL;
            cv->destroy();
            delete cv;
        }

        for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
        {
            af_descriptor_t *f  = &vFiles[i];
            Sample *s           = f->pSwapSample;
            if (s == NULL)
                continue;

            lsp_trace("Destroying sample swapSample=%p file=%d (currSample=%p)", f->pSwapSample, int(i), f->pCurrSample);
            f->pSwapSample  = NULL;

            s->destroy();
            delete s;
        }

        // Re-render files (if needed)
        for (size_t i=0; i<impulse_reverb_base_metadata::FILES; ++i)
        {
            // Do we need to re-render file?
            if (!cfg[i].bRender)
                continue;

            // Get audio file
            af_descriptor_t *f  = &vFiles[i];

            // Allocate new sample
            Sample *s           = new Sample();
            if (s == NULL)
                return STATUS_NO_MEM;
            f->pSwapSample      = s;
            f->bSwap            = true;
            lsp_trace("Allocated swapSample=%p file=%d (currSample=%p)", s, int(i), f->pCurrSample);

            AudioFile *af       = f->pCurr;
            if (af == NULL)
                continue;

            ssize_t flen        = af->samples();
            size_t channels     = (af->channels() < impulse_reverb_base_metadata::TRACKS_MAX) ?
                                    af->channels() : impulse_reverb_base_metadata::TRACKS_MAX;

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
                if (f->bReverse)
                    dsp::reverse2(dst, &src[tail_cut], fsamples);
                else
                    dsp::copy(dst, &src[head_cut], fsamples);
                fade_in(dst, dst, millis_to_samples(fSampleRate, f->fFadeIn), fsamples);
                fade_out(dst, dst, millis_to_samples(fSampleRate, f->fFadeOut), fsamples);

                // Now render thumbnail
                src                 = dst;
                dst                 = f->vThumbs[i];
                for (size_t k=0; k<impulse_reverb_base_metadata::MESH_SIZE; ++k)
                {
                    size_t first    = (k * fsamples) / impulse_reverb_base_metadata::MESH_SIZE;
                    size_t last     = ((k + 1) * fsamples) / impulse_reverb_base_metadata::MESH_SIZE;
                    if (first < last)
                        dst[k]          = dsp::abs_max(&src[first], last - first);
                    else
                        dst[k]          = fabs(src[first]);
                }

                // Normalize graph if possible
                if (f->fNorm != 1.0f)
                    dsp::scale2(dst, f->fNorm, impulse_reverb_base_metadata::MESH_SIZE);
            }
        }

        // Randomize phase of the convolver
        uint32_t phase  = seed_addr(this);
        phase           = ((phase << 16) | (phase >> 16)) & 0x7fffffff;
        uint32_t step   = 0x80000000 / (impulse_reverb_base_metadata::CONVOLVERS + 1);

        // OK, files have been rendered, now need to commutate
        for (size_t i=0; i<impulse_reverb_base_metadata::CONVOLVERS; ++i)
        {
            convolver_t *c      = &vConvolvers[i];

            // Check that routing has changed
            size_t file     = cfg->nFile[i];
            size_t track    = cfg->nTrack[i];
            if ((file <= 0) || (file > impulse_reverb_base_metadata::FILES))
            {
                c->nSource  = 0;
                c->nRank    = cfg->nRank[i];
                continue;
            }
            else
                file --;

            // Analyze sample
            Sample *s       = (vFiles[file].bSwap) ? vFiles[file].pSwapSample : vFiles[file].pCurrSample;
            if ((s == NULL) || (!s->valid()) || (s->channels() <= track))
                continue;

            // Now we can create convolver
            Convolver *cv   = new Convolver();
            if (!cv->init(s->getBuffer(track), s->length(), cfg->nRank[i], float((phase + i*step)& 0x7fffffff)/float(0x80000000)))
            {
                cv->destroy();
                delete cv;
                return STATUS_NO_MEM;
            }

            lsp_trace("Allocated convolver pSwap=%p for channel %d (pCurr=%p)", cv, int(i), c->pCurr);
            c->pSwap        = cv;
        }

        return STATUS_OK;
    }

    //-------------------------------------------------------------------------

    impulse_reverb_mono::impulse_reverb_mono(): impulse_reverb_base(metadata, 1)
    {
    }

    impulse_reverb_mono::~impulse_reverb_mono()
    {
    }

    impulse_reverb_stereo::impulse_reverb_stereo(): impulse_reverb_base(metadata, 2)
    {
    }

    impulse_reverb_stereo::~impulse_reverb_stereo()
    {
    }

} /* namespace lsp */
