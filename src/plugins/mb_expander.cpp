/*
 * mb_expander.cpp
 *
 *  Created on: 27 дек. 2019 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <plugins/mb_expander.h>

#define MBE_BUFFER_SIZE         0x1000
#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    mb_expander_base::mb_expander_base(const plugin_metadata_t &metadata, bool sc, size_t mode):
        plugin_t(metadata)
    {
        nMode           = mode;
        bSidechain      = sc;
        bEnvUpdate      = true;
        bModern         = true;
        nEnvBoost       = mb_expander_base_metadata::FB_DEFAULT;
        vChannels       = NULL;
        fInGain         = GAIN_AMP_0_DB;
        fDryGain        = GAIN_AMP_M_INF_DB;
        fWetGain        = GAIN_AMP_0_DB;
        fZoom           = GAIN_AMP_0_DB;
        pData           = NULL;
        vTr             = NULL;
        vPFc             = NULL;
        vRFc            = NULL;
        vFreqs          = NULL;
        vCurve          = NULL;
        vIndexes        = NULL;
        pIDisplay       = NULL;
        vSc[0]          = NULL;
        vSc[1]          = NULL;
        vBuffer         = NULL;
        vEnv            = NULL;

        pBypass         = NULL;
        pMode           = NULL;
        pInGain         = NULL;
        pDryGain        = NULL;
        pWetGain        = NULL;
        pOutGain        = NULL;
        pReactivity     = NULL;
        pShiftGain      = NULL;
        pZoom           = NULL;
        pEnvBoost       = NULL;
    }

    mb_expander_base::~mb_expander_base()
    {
        // TODO
    }

    void mb_expander_base::init(IWrapper *wrapper)
    {
        // Initialize plugin
        plugin_t::init(wrapper);

        // Determine number of channels
        size_t channels     = (nMode == MBEM_MONO) ? 1 : 2;

        // Allocate channels
        vChannels       = new channel_t[channels];
        if (vChannels == NULL)
            return;

        // Initialize analyzer
        size_t an_cid       = 0;
        if (!sAnalyzer.init(2*channels, mb_expander_base_metadata::FFT_RANK))
            return;

        sAnalyzer.set_rank(mb_expander_base_metadata::FFT_RANK);
        sAnalyzer.set_activity(false);
        sAnalyzer.set_envelope(envelope::WHITE_NOISE);
        sAnalyzer.set_window(mb_expander_base_metadata::FFT_WINDOW);
        sAnalyzer.set_rate(mb_expander_base_metadata::FFT_REFRESH_RATE);

        size_t filter_mesh_size = ALIGN_SIZE(mb_expander_base_metadata::FFT_MESH_POINTS * sizeof(float), DEFAULT_ALIGN);

        // Allocate float buffer data
        size_t to_alloc =
                // Global buffers
                2 * filter_mesh_size + // vTr (both complex and real)
                2 * filter_mesh_size + // vFc (both complex and real)
                2 * filter_mesh_size + // vSig (both complex and real)
                mb_expander_base_metadata::CURVE_MESH_SIZE * sizeof(float) + // Curve
                mb_expander_base_metadata::FFT_MESH_POINTS * sizeof(float) + // vFreqs array
                mb_expander_base_metadata::FFT_MESH_POINTS * sizeof(uint32_t) + // vIndexes array
                MBE_BUFFER_SIZE * sizeof(float) + // Global vBuffer for band signal processing
                MBE_BUFFER_SIZE * sizeof(float) + // Global vEnv for band signal processing
                (
                    MBE_BUFFER_SIZE * sizeof(float) + // Global vSc[] for each channel
                    2 * filter_mesh_size + // vTr of each channel
                    filter_mesh_size + // vTrMem of each channel
                    MBE_BUFFER_SIZE * sizeof(float) + // vBuffer for each channel
                    MBE_BUFFER_SIZE * sizeof(float) + // vScBuffer for each channel
                    ((bSidechain) ? MBE_BUFFER_SIZE * sizeof(float) : 0) + // vExtScBuffer for each channel
                    // Band buffers
                    (
                        MBE_BUFFER_SIZE * sizeof(float) + // vVCA of each band
                        mb_expander_base_metadata::FFT_MESH_POINTS * 2 * sizeof(float) // vTr transfer function for each band
                    ) * mb_expander_base_metadata::BANDS_MAX
                );

        uint8_t *ptr    = alloc_aligned<uint8_t>(pData, to_alloc);
        if (ptr == NULL)
            return;
        lsp_guard_assert(uint8_t *save   = ptr);

        // Remember the pointer to frequencies buffer
        vTr             = reinterpret_cast<float *>(ptr);
        ptr            += filter_mesh_size * 2;
        vPFc             = reinterpret_cast<float *>(ptr);
        ptr            += filter_mesh_size * 2;
        vRFc            = reinterpret_cast<float *>(ptr);
        ptr            += filter_mesh_size * 2;
        vFreqs          = reinterpret_cast<float *>(ptr);
        ptr            += mb_expander_base_metadata::FFT_MESH_POINTS * sizeof(float);
        vCurve          = reinterpret_cast<float *>(ptr);
        ptr            += mb_expander_base_metadata::CURVE_MESH_SIZE * sizeof(float);
        vIndexes        = reinterpret_cast<uint32_t *>(ptr);
        ptr            += mb_expander_base_metadata::FFT_MESH_POINTS * sizeof(uint32_t);
        vSc[0]          = reinterpret_cast<float *>(ptr);
        ptr            += MBE_BUFFER_SIZE * sizeof(float);
        if (channels > 1)
        {
            vSc[1]          = reinterpret_cast<float *>(ptr);
            ptr            += MBE_BUFFER_SIZE * sizeof(float);
        }
        else
            vSc[1]          = NULL;
        vBuffer         = reinterpret_cast<float *>(ptr);
        ptr            += MBE_BUFFER_SIZE * sizeof(float);
        vEnv            = reinterpret_cast<float *>(ptr);
        ptr            += MBE_BUFFER_SIZE * sizeof(float);

        // Initialize filters according to number of bands
        if (sFilters.init(mb_expander_base_metadata::BANDS_MAX * channels) != STATUS_OK)
            return;
        size_t filter_cid = 0;

        // Initialize channels
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c    = &vChannels[i];

            if (!c->sEnvBoost[0].init(NULL))
                return;
            if (bSidechain)
            {
                if (!c->sEnvBoost[1].init(NULL))
                    return;
            }

            c->nPlanSize    = 0;
            c->vIn          = NULL;
            c->vOut         = NULL;
            c->vScIn        = NULL;

            c->vBuffer      = reinterpret_cast<float *>(ptr);
            ptr            += MBE_BUFFER_SIZE * sizeof(float);
            c->vScBuffer    = reinterpret_cast<float *>(ptr);
            ptr            += MBE_BUFFER_SIZE * sizeof(float);
            c->vExtScBuffer = NULL;
            if (bSidechain)
            {
                c->vExtScBuffer = reinterpret_cast<float *>(ptr);
                ptr            += MBE_BUFFER_SIZE * sizeof(float);
            }
            c->vTr          = reinterpret_cast<float *>(ptr);
            ptr            += 2 * filter_mesh_size;
            c->vTrMem       = reinterpret_cast<float *>(ptr);
            ptr            += filter_mesh_size;

            c->nAnInChannel = an_cid++;
            c->nAnOutChannel= an_cid++;
            c->bInFft       = false;
            c->bOutFft      = false;

            c->pIn          = NULL;
            c->pOut         = NULL;
            c->pScIn        = NULL;
            c->pFftIn       = NULL;
            c->pFftInSw     = NULL;
            c->pFftOut      = NULL;
            c->pFftOutSw    = NULL;

            c->pAmpGraph    = NULL;
            c->pInLvl       = NULL;
            c->pOutLvl      = NULL;

            // Initialize bands
            for (size_t j=0; j<mb_expander_base_metadata::BANDS_MAX; ++j)
            {
                exp_band_t *b  = &c->vBands[j];

                if (!b->sSC.init(channels, mb_expander_base_metadata::REACTIVITY_MAX))
                    return;
                if (!b->sPassFilter.init(NULL))
                    return;
                if (!b->sRejFilter.init(NULL))
                    return;
                if (!b->sAllFilter.init(NULL))
                    return;

                // Initialize sidechain equalizers
                b->sEQ[0].init(2, 6);
                b->sEQ[0].set_mode(EQM_IIR);
                if (channels > 1)
                {
                    b->sEQ[1].init(2, 6);
                    b->sEQ[1].set_mode(EQM_IIR);
                }

                b->vVCA         = reinterpret_cast<float *>(ptr);
                ptr            += MBE_BUFFER_SIZE * sizeof(float);
                b->vTr          = reinterpret_cast<float *>(ptr);
                ptr            += mb_expander_base_metadata::FFT_MESH_POINTS * sizeof(float) * 2;

                b->fScPreamp    = GAIN_AMP_0_DB;

                b->fFreqStart   = 0.0f;
                b->fFreqEnd     = 0.0f;

                b->fFreqHCF     = 0.0f;
                b->fFreqLCF     = 0.0f;
                b->fMakeup      = GAIN_AMP_0_DB;
                b->fEnvLevel    = GAIN_AMP_0_DB;
                b->fGainLevel   = GAIN_AMP_0_DB;
                b->bEnabled     = j < mb_expander_base_metadata::BANDS_DFL;
                b->bCustHCF     = false;
                b->bCustLCF     = false;
                b->bMute        = false;
                b->bSolo        = false;
                b->bExtSc       = false;
                b->nFilterID    = filter_cid++;

                b->pExtSc       = NULL;
                b->pScSource    = NULL;
                b->pScMode      = NULL;
                b->pScLook      = NULL;
                b->pScReact     = NULL;
                b->pScPreamp    = NULL;
                b->pScLpfOn     = NULL;
                b->pScHpfOn     = NULL;
                b->pScLcfFreq   = NULL;
                b->pScHcfFreq   = NULL;
                b->pScFreqChart = NULL;

                b->pMode        = NULL;
                b->pEnable      = NULL;
                b->pSolo        = NULL;
                b->pMute        = NULL;
                b->pAttLevel    = NULL;
                b->pRelLevel    = NULL;
                b->pRelTime     = NULL;
                b->pRatio       = NULL;
                b->pKnee        = NULL;
                b->pMakeup      = NULL;
                b->pFreqEnd     = NULL;
                b->pEnvLvl      = NULL;
                b->pCurveLvl    = NULL;
            }

            // Initialize split
            for (size_t j=0; j<mb_expander_base_metadata::BANDS_MAX-1; ++j)
            {
                split_t *s      = &c->vSplit[j];

                s->bEnabled     = false;
                s->fFreq        = 0.0f;

                s->pEnabled     = NULL;
                s->pFreq        = NULL;
            }
        }

        lsp_assert(ptr <= &save[to_alloc]);

        // Bind ports
        size_t port_id              = 0;

        // Input ports
        lsp_trace("Binding input ports");
        for (size_t i=0; i<channels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pIn        =   vPorts[port_id++];
        }

        // Input ports
        lsp_trace("Binding output ports");
        for (size_t i=0; i<channels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pOut       =   vPorts[port_id++];
        }

        // Input ports
        if (bSidechain)
        {
            lsp_trace("Binding sidechain ports");
            for (size_t i=0; i<channels; ++i)
            {
                TRACE_PORT(vPorts[port_id]);
                vChannels[i].pScIn      =   vPorts[port_id++];
            }
        }

        // Common ports
        lsp_trace("Binding common ports");
        TRACE_PORT(vPorts[port_id]);
        pBypass                 = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMode                   = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pInGain                 = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOutGain                = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDryGain                = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pWetGain                = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pReactivity             = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pShiftGain              = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pZoom                   = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pEnvBoost               = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        port_id++;         // Skip band selector

        lsp_trace("Binding channel ports");
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c    = &vChannels[i];
            // TODO
        }

        lsp_trace("Binding meters");
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c    = &vChannels[i];
            // TODO
        }

        // Split frequencies
        lsp_trace("Binding split frequencies");
        for (size_t i=0; i<channels; ++i)
        {
            // TODO
        }

        // Expander bands
        lsp_trace("Binding expander bands");
        for (size_t i=0; i<channels; ++i)
        {
            // TODO
        }

        // Initialize curve (logarithmic) in range of -72 .. +24 db
        float delta = (mb_expander_base_metadata::CURVE_DB_MAX - mb_expander_base_metadata::CURVE_DB_MIN) / (mb_expander_base_metadata::CURVE_MESH_SIZE-1);
        for (size_t i=0; i<mb_expander_base_metadata::CURVE_MESH_SIZE; ++i)
            vCurve[i]   = db_to_gain(mb_expander_base_metadata::CURVE_DB_MIN + delta * i);
    }

    void mb_expander_base::destroy()
    {
        // Determine number of channels
        size_t channels     = (nMode == MBEM_MONO) ? 1 : 2;

        // Destroy channels
        if (vChannels != NULL)
        {
            for (size_t i=0; i<channels; ++i)
            {
                channel_t *c    = &vChannels[i];

                c->sEnvBoost[0].destroy();
                c->sEnvBoost[1].destroy();
                c->sDelay.destroy();

                c->vBuffer      = NULL;

                for (size_t i=0; i<mb_expander_base_metadata::BANDS_MAX; ++i)
                {
                    exp_band_t *b  = &c->vBands[i];

                    b->sEQ[0].destroy();
                    b->sEQ[1].destroy();
                    b->sSC.destroy();
                    b->sDelay.destroy();

                    b->sPassFilter.destroy();
                    b->sRejFilter.destroy();
                    b->sAllFilter.destroy();
                }
            }

            delete [] vChannels;
            vChannels       = NULL;
        }

        // Destroy dynamic filters
        sFilters.destroy();

        // Destroy data
        if (pData != NULL)
            free_aligned(pData);

        // Destroy analyzer
        sAnalyzer.destroy();

        // Destroy plugin
        plugin_t::destroy();
    }

    void mb_expander_base::update_settings()
    {
        // TODO
    }

    void mb_expander_base::update_sample_rate(long sr)
    {
        // Determine number of channels
        size_t channels     = (nMode == MBEM_MONO) ? 1 : 2;
        size_t max_delay    = millis_to_samples(sr, expander_base_metadata::LOOKAHEAD_MAX);

        // Update analyzer's sample rate
        sAnalyzer.set_sample_rate(sr);
        sFilters.set_sample_rate(sr);
        bEnvUpdate          = true;

        // Update channels
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c = &vChannels[i];
            c->sBypass.init(sr);
            c->sDelay.init(max_delay);

            // Update bands
            for (size_t j=0; j<mb_expander_base_metadata::BANDS_MAX; ++j)
            {
                exp_band_t *b   = &c->vBands[j];

                b->sSC.set_sample_rate(sr);
                b->sExp.set_sample_rate(sr);
                b->sDelay.init(max_delay);

                b->sPassFilter.set_sample_rate(sr);
                b->sRejFilter.set_sample_rate(sr);
                b->sAllFilter.set_sample_rate(sr);

                b->sEQ[0].set_sample_rate(sr);
                if (channels > 1)
                    b->sEQ[1].set_sample_rate(sr);
            }

            c->nPlanSize        = 0; // Force to rebuild plan
        }
    }

    void mb_expander_base::ui_activated()
    {
        size_t channels     = (nMode == MBEM_MONO) ? 1 : 2;

        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c        = &vChannels[i];

            for (size_t j=0; j<c->nPlanSize; ++j)
            {
                exp_band_t *b       = c->vPlan[j];
                b->nSync            = S_ALL;
            }
        }
    }

    void mb_expander_base::process(size_t samples)
    {
        // TODO
    }

    bool mb_expander_base::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        // TODO
        return false;
    }

    //-------------------------------------------------------------------------
    // Expander derivatives
    mb_expander_mono::mb_expander_mono() : mb_expander_base(metadata, false, MBEM_MONO)
    {
    }

    mb_expander_stereo::mb_expander_stereo() : mb_expander_base(metadata, false, MBEM_STEREO)
    {
    }

    mb_expander_lr::mb_expander_lr() : mb_expander_base(metadata, false, MBEM_LR)
    {
    }

    mb_expander_ms::mb_expander_ms() : mb_expander_base(metadata, false, MBEM_MS)
    {
    }

    sc_mb_expander_mono::sc_mb_expander_mono() : mb_expander_base(metadata, true, MBEM_MONO)
    {
    }

    sc_mb_expander_stereo::sc_mb_expander_stereo() : mb_expander_base(metadata, true, MBEM_STEREO)
    {
    }

    sc_mb_expander_lr::sc_mb_expander_lr() : mb_expander_base(metadata, true, MBEM_LR)
    {
    }

    sc_mb_expander_ms::sc_mb_expander_ms() : mb_expander_base(metadata, true, MBEM_MS)
    {
    }
}

