/*
 * expander.cpp
 *
 *  Created on: 4 нояб. 2016 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/colors.h>
#include <core/util/Color.h>
#include <plugins/expander.h>

#define EXP_BUF_SIZE           0x1000
#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    //-------------------------------------------------------------------------
    // Expander base class

    expander_base::expander_base(const plugin_metadata_t &metadata, bool sc, size_t mode): plugin_t(metadata)
    {
        nMode           = mode;
        bSidechain      = sc;
        vChannels       = NULL;
        vCurve          = NULL;
        vTime           = NULL;
        bPause          = false;
        bClear          = false;
        bMSListen       = false;
        fInGain         = 1.0f;
        bUISync         = true;

        pBypass         = NULL;
        pInGain         = NULL;
        pOutGain        = NULL;
        pPause          = NULL;
        pClear          = NULL;
        pMSListen       = NULL;

        pData           = NULL;
        pIDisplay       = NULL;
    }

    expander_base::~expander_base()
    {
    }

    void expander_base::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);
        size_t channels = (nMode == EM_MONO) ? 1 : 2;

        // Allocate channels
        vChannels       = new channel_t[channels];
        if (vChannels == NULL)
            return;

        // Allocate temporary buffers
        size_t buf_size         = EXP_BUF_SIZE * sizeof(float);
        size_t curve_size       = (expander_base_metadata::CURVE_MESH_SIZE) * sizeof(float);
        size_t history_size     = (expander_base_metadata::TIME_MESH_SIZE) * sizeof(float);
        size_t allocate         = buf_size * channels * 5 + curve_size + history_size + DEFAULT_ALIGN;
        uint8_t *ptr            = new uint8_t[allocate];
        if (ptr == NULL)
            return;
        pData                   = ptr;
        ptr                     = ALIGN_PTR(ptr, DEFAULT_ALIGN);
        vCurve                  = reinterpret_cast<float *>(ptr);
        ptr                    += curve_size;
        vTime                   = reinterpret_cast<float *>(ptr);
        ptr                    += history_size;

        // Initialize channels
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c = &vChannels[i];

            if (!c->sSC.init(channels, expander_base_metadata::REACTIVITY_MAX))
                return;

            c->vIn              = reinterpret_cast<float *>(ptr);
            ptr                += buf_size;
            c->vOut             = reinterpret_cast<float *>(ptr);
            ptr                += buf_size;
            c->vSc              = reinterpret_cast<float *>(ptr);
            ptr                += buf_size;
            c->vEnv             = reinterpret_cast<float *>(ptr);
            ptr                += buf_size;
            c->vGain            = reinterpret_cast<float *>(ptr);
            ptr                += buf_size;
            c->bScListen        = false;
            c->nSync            = S_ALL;
            c->nScType          = SCT_INTERNAL;
            c->fMakeup          = 1.0f;
            c->fDryGain         = 1.0f;
            c->fWetGain         = 0.0f;
            c->fDotIn           = 0.0f;
            c->fDotOut          = 0.0f;

            c->pIn              = NULL;
            c->pOut             = NULL;
            c->pSC              = NULL;

            for (size_t j=0; j<G_TOTAL; ++j)
                c->pGraph[j]        = NULL;

            for (size_t j=0; j<M_TOTAL; ++j)
                c->pMeter[j]        = NULL;

            c->pScType          = NULL;
            c->pScMode          = NULL;
            c->pScLookahead     = NULL;
            c->pScListen        = NULL;
            c->pScSource        = NULL;
            c->pScReactivity    = NULL;
            c->pScPreamp        = NULL;

            c->pMode            = NULL;
            c->pAttackLvl       = NULL;
            c->pReleaseLvl      = NULL;
            c->pAttackTime      = NULL;
            c->pReleaseTime     = NULL;
            c->pRatio           = NULL;
            c->pKnee            = NULL;
            c->pMakeup          = NULL;
            c->pDryGain         = NULL;
            c->pWetGain         = NULL;
            c->pCurve           = NULL;
            c->pReleaseOut      = NULL;
        }

        lsp_assert(ptr < &pData[allocate]);

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
                vChannels[i].pSC        =   vPorts[port_id++];
            }
        }

        // Common ports
        lsp_trace("Binding common ports");
        TRACE_PORT(vPorts[port_id]);
        pBypass                 =   vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pInGain                 =   vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOutGain                =   vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pPause                  =   vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pClear                  =   vPorts[port_id++];
        if (nMode == EM_MS)
        {
            TRACE_PORT(vPorts[port_id]);
            pMSListen               =   vPorts[port_id++];
        }

        // Sidechain ports
        lsp_trace("Binding sidechain ports");
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c = &vChannels[i];

            if ((i > 0) && (nMode == EM_STEREO))
            {
                channel_t *sc       = &vChannels[0];
                c->pScType          = sc->pScType;
                c->pScSource        = sc->pScSource;
                c->pScMode          = sc->pScMode;
                c->pScLookahead     = sc->pScLookahead;
                c->pScListen        = sc->pScListen;
                c->pScReactivity    = sc->pScReactivity;
                c->pScPreamp        = sc->pScPreamp;
            }
            else
            {
                if (bSidechain)
                {
                    TRACE_PORT(vPorts[port_id]);
                    c->pScType          =   vPorts[port_id++];
                }
                TRACE_PORT(vPorts[port_id]);
                c->pScMode          =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pScLookahead     =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pScListen        =   vPorts[port_id++];
                if (nMode != EM_MONO)
                {
                    TRACE_PORT(vPorts[port_id]);
                    c->pScSource        =   vPorts[port_id++];
                }
                TRACE_PORT(vPorts[port_id]);
                c->pScReactivity    =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pScPreamp        =   vPorts[port_id++];
            }
        }

        // Expander ports
        lsp_trace("Binding expander ports");
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c = &vChannels[i];

            if ((i > 0) && (nMode == EM_STEREO))
            {
                channel_t *sc       = &vChannels[0];

                c->pMode            = sc->pMode;
                c->pAttackLvl       = sc->pAttackLvl;
                c->pAttackTime      = sc->pAttackTime;
                c->pReleaseLvl      = sc->pReleaseLvl;
                c->pReleaseTime     = sc->pReleaseTime;
                c->pRatio           = sc->pRatio;
                c->pKnee            = sc->pKnee;
                c->pMakeup          = sc->pMakeup;
                c->pDryGain         = sc->pDryGain;
                c->pWetGain         = sc->pWetGain;
            }
            else
            {
                TRACE_PORT(vPorts[port_id]);
                c->pMode            =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pAttackLvl       =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pAttackTime      =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pReleaseLvl      =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pReleaseTime     =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pRatio           =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pKnee            =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pMakeup          =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pDryGain         =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pWetGain         =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pReleaseOut      =   vPorts[port_id++];

                // Skip meters visibility controls
                TRACE_PORT(vPorts[port_id]);
                port_id++;
                TRACE_PORT(vPorts[port_id]);
                port_id++;
                TRACE_PORT(vPorts[port_id]);
                port_id++;

                TRACE_PORT(vPorts[port_id]);
                c->pCurve           =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pGraph[G_SC]     =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pGraph[G_ENV]    =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pGraph[G_GAIN]   =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pMeter[M_SC]     =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pMeter[M_CURVE]  =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pMeter[M_ENV]    =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                c->pMeter[M_GAIN]   =   vPorts[port_id++];
            }
        }

        // Bind history
        lsp_trace("Binding history ports");
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c = &vChannels[i];

            // Skip meters visibility controls
            TRACE_PORT(vPorts[port_id]);
            port_id++;
            TRACE_PORT(vPorts[port_id]);
            port_id++;

            // Bind ports
            TRACE_PORT(vPorts[port_id]);
            c->pGraph[G_IN]     =   vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pGraph[G_OUT]    =   vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pMeter[M_IN]     =   vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pMeter[M_OUT]    =   vPorts[port_id++];
        }

        // Initialize curve (logarithmic) in range of -72 .. +24 db
        float delta = (expander_base_metadata::CURVE_DB_MAX - expander_base_metadata::CURVE_DB_MIN) / (expander_base_metadata::CURVE_MESH_SIZE-1);
        for (size_t i=0; i<expander_base_metadata::CURVE_MESH_SIZE; ++i)
            vCurve[i]   = db_to_gain(expander_base_metadata::CURVE_DB_MIN + delta * i);

        // Initialize time points
        delta       = expander_base_metadata::TIME_HISTORY_MAX / (expander_base_metadata::TIME_MESH_SIZE - 1);
        for (size_t i=0; i<expander_base_metadata::TIME_MESH_SIZE; ++i)
            vTime[i]    = expander_base_metadata::TIME_HISTORY_MAX - i*delta;
    }

    void expander_base::destroy()
    {
        if (vChannels != NULL)
        {
            size_t channels = (nMode == EM_MONO) ? 1 : 2;
            for (size_t i=0; i<channels; ++i)
            {
                vChannels[i].sSC.destroy();
                vChannels[i].sDelay.destroy();
            }

            delete [] vChannels;
            vChannels = NULL;
        }

        if (pData != NULL)
        {
            delete [] pData;
            pData = NULL;
        }

        if (pIDisplay != NULL)
        {
            pIDisplay->detroy();
            pIDisplay   = NULL;
        }
    }

    void expander_base::update_sample_rate(long sr)
    {
        size_t samples_per_dot  = seconds_to_samples(sr, expander_base_metadata::TIME_HISTORY_MAX / expander_base_metadata::TIME_MESH_SIZE);
        size_t channels = (nMode == EM_MONO) ? 1 : 2;

        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c = &vChannels[i];
            c->sBypass.init(sr);
            c->sExp.set_sample_rate(sr);
            c->sSC.set_sample_rate(sr);
            c->sDelay.init(millis_to_samples(fSampleRate, compressor_base_metadata::LOOKAHEAD_MAX));

            for (size_t j=0; j<G_TOTAL; ++j)
                c->sGraph[j].init(expander_base_metadata::TIME_MESH_SIZE, samples_per_dot);
            c->sGraph[G_GAIN].fill(1.0f);
        }
    }

    void expander_base::update_settings()
    {
        size_t channels = (nMode == EM_MONO) ? 1 : 2;
        bool bypass     = pBypass->getValue() >= 0.5f;

        // Global parameters
        bPause          = pPause->getValue() >= 0.5f;
        bClear          = pClear->getValue() >= 0.5f;
        bMSListen       = (pMSListen != NULL) ? pMSListen->getValue() >= 0.5f : false;
        fInGain         = pInGain->getValue();
        float out_gain  = pOutGain->getValue();

        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c    = &vChannels[i];

            // Update bypass settings
            c->sBypass.set_bypass(bypass);

            // Update sidechain settings
            c->nScType      = (c->pScType != NULL) ? c->pScType->getValue() : SCT_INTERNAL;
            c->bScListen    = c->pScListen->getValue() >= 0.5f;

            c->sSC.set_gain(c->pScPreamp->getValue());
            c->sSC.set_mode((c->pScMode != NULL) ? c->pScMode->getValue() : SCM_RMS);
            c->sSC.set_source((c->pScSource != NULL) ? c->pScSource->getValue() : SCS_MIDDLE);
            c->sSC.set_reactivity(c->pScReactivity->getValue());
            c->sSC.set_stereo_mode(((nMode == EM_MS) && (c->nScType != SCT_EXTERNAL)) ? SCSM_MIDSIDE : SCSM_STEREO);

            // Update delay
            c->sDelay.set_delay(millis_to_samples(fSampleRate, (c->pScLookahead != NULL) ? c->pScLookahead->getValue() : 0));

            // Update expander settings
            float attack    = c->pAttackLvl->getValue();
            float release   = c->pReleaseLvl->getValue() * attack;
            float makeup    = c->pMakeup->getValue();
            bool upward     = c->pMode->getValue() >= 0.5f;

            c->sExp.set_threshold(attack, release);
            c->sExp.set_timings(c->pAttackTime->getValue(), c->pReleaseTime->getValue());
            c->sExp.set_ratio(c->pRatio->getValue());
            c->sExp.set_knee(c->pKnee->getValue());
            c->sExp.set_mode((upward) ? EM_UPWARD : EM_DOWNWARD);
            if (c->pReleaseOut != NULL)
                c->pReleaseOut->setValue(release);
            c->sGraph[G_GAIN].set_method((upward) ? MM_MAXIMUM : MM_MINIMUM);

            // Check modification flag
            if (c->sExp.modified())
            {
                c->sExp.update_settings();
                c->nSync           |= S_CURVE;
            }

            // Update gains
            c->fDryGain         = c->pDryGain->getValue() * out_gain;
            c->fWetGain         = c->pWetGain->getValue() * out_gain;
            if (c->fMakeup != makeup)
            {
                c->fMakeup          = makeup;
                c->nSync           |= S_CURVE;
            }
        }
    }

    void expander_base::ui_activated()
    {
        size_t channels     = (nMode == EM_MONO) ? 1 : 2;
        for (size_t i=0; i<channels; ++i)
            vChannels[i].nSync     = S_CURVE;
        bUISync             = true;
    }

    void expander_base::process(size_t samples)
    {
        size_t channels = (nMode == EM_MONO) ? 1 : 2;

        float *in_buf[2];   // Input buffer
        float *out_buf[2];  // Output buffer
        float *sc_buf[2];   // Sidechain source
        const float *in[2]; // Buffet to pass to sidechain

        // Prepare audio channels
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c        = &vChannels[i];

            // Initialize pointers
            in_buf[i]           = c->pIn->getBuffer<float>();
            out_buf[i]          = c->pOut->getBuffer<float>();
            sc_buf[i]           = (c->pSC != NULL) ? c->pSC->getBuffer<float>() : in_buf[i];
        }

        // Perform expansion
        size_t left = samples;
        while (left > 0)
        {
            // Detemine number of samples to process
            size_t to_process = (left > EXP_BUF_SIZE) ? EXP_BUF_SIZE : left;

            // Prepare audio channels
            if (nMode == EM_MONO)
                dsp::mul_k3(vChannels[0].vIn, in_buf[0], fInGain, to_process);
            else if (nMode == EM_MS)
            {
                dsp::lr_to_ms(vChannels[0].vIn, vChannels[1].vIn, in_buf[0], in_buf[1], to_process);
                dsp::mul_k2(vChannels[0].vIn, fInGain, to_process);
                dsp::mul_k2(vChannels[1].vIn, fInGain, to_process);
            }
            else
            {
                dsp::mul_k3(vChannels[0].vIn, in_buf[0], fInGain, to_process);
                dsp::mul_k3(vChannels[1].vIn, in_buf[1], fInGain, to_process);
            }

            // Perform sidechain processing
            for (size_t i=0; i<channels; ++i)
            {
                channel_t *c        = &vChannels[i];

                // Update input graph
                c->sGraph[G_IN].process(c->vIn, to_process);
                c->pMeter[M_IN]->setValue(dsp::abs_max(c->vIn, to_process));

                // Do expansion
                in[0]   = (c->nScType == SCT_EXTERNAL) ? sc_buf[0] : vChannels[0].vIn;
                if (channels > 1)
                    in[1]   = (c->nScType == SCT_EXTERNAL) ? sc_buf[1] : vChannels[1].vIn;
                c->sSC.process(c->vSc, in, to_process);
                c->sExp.process(c->vGain, c->vEnv, c->vSc, to_process);
            }

            // Apply gain to each channel and process meters
            for (size_t i=0; i<channels; ++i)
            {
                channel_t *c        = &vChannels[i];

                c->sDelay.process(c->vIn, c->vIn, to_process); // Add delay to original signal
                dsp::mul3(c->vOut, c->vGain, c->vIn, to_process);

                // Process graph outputs
                if ((i == 0) || (nMode != EM_STEREO))
                {
                    c->sGraph[G_SC].process(c->vSc, to_process);                        // Sidechain signal
                    c->pMeter[M_SC]->setValue(dsp::abs_max(c->vSc, to_process));

                    c->sGraph[G_GAIN].process(c->vGain, to_process);                    // Gain reduction signal
                    c->pMeter[M_GAIN]->setValue(dsp::abs_max(c->vGain, to_process));

                    c->sGraph[G_ENV].process(c->vEnv, to_process);                      // Envelope signal
                    c->pMeter[M_ENV]->setValue(dsp::abs_max(c->vEnv, to_process));
                }
            }

            // Form output signal
            if (nMode == EM_MS)
            {
                channel_t *cm       = &vChannels[0];
                channel_t *cs       = &vChannels[1];

                dsp::mix2(cm->vOut, cm->vIn, cm->fMakeup * cm->fWetGain, cm->fDryGain, to_process);
                dsp::mix2(cs->vOut, cs->vIn, cs->fMakeup * cs->fWetGain, cs->fDryGain, to_process);

                cm->sGraph[G_OUT].process(cm->vOut, to_process);
                cm->pMeter[M_OUT]->setValue(dsp::abs_max(cm->vOut, to_process));
                cs->sGraph[G_OUT].process(cs->vOut, to_process);
                cs->pMeter[M_OUT]->setValue(dsp::abs_max(cs->vOut, to_process));

                if (!bMSListen)
                    dsp::ms_to_lr(cm->vOut, cs->vOut, cm->vOut, cs->vOut, to_process);
                if (cm->bScListen)
                    dsp::copy(cm->vOut, cm->vSc, to_process);
                if (cs->bScListen)
                    dsp::copy(cs->vOut, cs->vSc, to_process);
            }
            else
            {
                for (size_t i=0; i<channels; ++i)
                {
                    // Mix dry/wet signal or copy sidechain signal
                    channel_t *c        = &vChannels[i];
                    if (c->bScListen)
                        dsp::copy(c->vOut, c->vSc, to_process);
                    else
                        dsp::mix2(c->vOut, c->vIn, c->fMakeup * c->fWetGain, c->fDryGain, to_process);

                    c->sGraph[G_OUT].process(c->vOut, to_process);                      // Output signal
                    c->pMeter[M_OUT]->setValue(dsp::abs_max(c->vOut, to_process));
                }
            }

            // Final metering
            for (size_t i=0; i<channels; ++i)
            {
                // Apply bypass
                vChannels[i].sBypass.process(out_buf[i], in_buf[i], vChannels[i].vOut, to_process);

                in_buf[i]          += to_process;
                out_buf[i]         += to_process;
                sc_buf[i]          += to_process;
            }

            left       -= to_process;
        }

        if ((!bPause) || (bClear) || (bUISync))
        {
            // Process mesh requests
            for (size_t i=0; i<channels; ++i)
            {
                // Get channel
                channel_t *c        = &vChannels[i];

                for (size_t j=0; j<G_TOTAL; ++j)
                {
                    // Check that port is bound
                    if (c->pGraph[j] == NULL)
                        continue;

                    // Clear data if requested
                    if (bClear)
                        dsp::fill_zero(c->sGraph[j].data(), expander_base_metadata::TIME_MESH_SIZE);

                    // Get mesh
                    mesh_t *mesh    = c->pGraph[j]->getBuffer<mesh_t>();
                    if ((mesh != NULL) && (mesh->isEmpty()))
                    {
                        // Fill mesh with new values
                        dsp::copy(mesh->pvData[0], vTime, expander_base_metadata::TIME_MESH_SIZE);
                        dsp::copy(mesh->pvData[1], c->sGraph[j].data(), expander_base_metadata::TIME_MESH_SIZE);
                        mesh->data(2, expander_base_metadata::TIME_MESH_SIZE);
                    }
                } // for j
            }

            bUISync         = false;
        }

        // Output expander curves for each channel
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c       = &vChannels[i];

            // Output expansion curve
            if (c->pCurve != NULL)
            {
                mesh_t *mesh            = c->pCurve->getBuffer<mesh_t>();
                if ((c->nSync & S_CURVE) && (mesh != NULL) && (mesh->isEmpty()))
                {
                    // Copy frequency points
                    dsp::copy(mesh->pvData[0], vCurve, expander_base_metadata::CURVE_MESH_SIZE);
                    c->sExp.curve(mesh->pvData[1], vCurve, expander_base_metadata::CURVE_MESH_SIZE);
                    if (c->fMakeup != 1.0f)
                        dsp::mul_k2(mesh->pvData[1], c->fMakeup, expander_base_metadata::CURVE_MESH_SIZE);

                    // Mark mesh containing data
                    mesh->data(2, expander_base_metadata::CURVE_MESH_SIZE);
                    c->nSync &= ~S_CURVE;
                }
            }

            // Update meter
            if ((c->pMeter[M_ENV] != NULL) && (c->pMeter[M_CURVE] != NULL))
            {
                c->fDotIn   = c->pMeter[M_ENV]->getValue();
                c->fDotOut  = c->sExp.curve(c->fDotIn) * c->fMakeup;
                c->pMeter[M_CURVE]->setValue(c->fDotOut);
            }
        }

        // Request for redraw
        if (pWrapper != NULL)
            pWrapper->query_display_draw();
    }

    bool expander_base::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        // Check proportions
        if (height > width)
            height  = width;

        // Init canvas
        if (!cv->init(width, height))
            return false;
        width   = cv->width();
        height  = cv->height();

        // Clear background
        bool bypassing = vChannels[0].sBypass.bypassing();
        cv->set_color_rgb((bypassing) ? CV_DISABLED : CV_BACKGROUND);
        cv->paint();

        float zx    = 1.0f/GAIN_AMP_M_72_DB;
        float zy    = 1.0f/GAIN_AMP_M_72_DB;
        float dx    = width/(logf(GAIN_AMP_P_24_DB)-logf(GAIN_AMP_M_72_DB));
        float dy    = height/(logf(GAIN_AMP_M_72_DB)-logf(GAIN_AMP_P_24_DB));

        // Draw horizontal and vertical lines
        cv->set_line_width(1.0);
        cv->set_color_rgb((bypassing) ? CV_SILVER: CV_YELLOW, 0.5f);
        for (float i=GAIN_AMP_M_72_DB; i<GAIN_AMP_P_24_DB; i *= GAIN_AMP_P_24_DB)
        {
            float ax = dx*(logf(i*zx));
            float ay = height + dy*(logf(i*zy));
            cv->line(ax, 0, ax, height);
            cv->line(0, ay, width, ay);
        }

        // Draw 1:1 line
        cv->set_line_width(2.0);
        cv->set_color_rgb(CV_GRAY);
        {
            float ax1 = dx*(logf(GAIN_AMP_M_72_DB*zx));
            float ax2 = dx*(logf(GAIN_AMP_P_24_DB*zx));
            float ay1 = height + dy*(logf(GAIN_AMP_M_72_DB*zy));
            float ay2 = height + dy*(logf(GAIN_AMP_P_24_DB*zy));
            cv->line(ax1, ay1, ax2, ay2);
        }

        // Draw axis
        cv->set_color_rgb((bypassing) ? CV_SILVER : CV_WHITE);
        {
            float ax = dx*(logf(GAIN_AMP_0_DB*zx));
            float ay = height + dy*(logf(GAIN_AMP_0_DB*zy));
            cv->line(ax, 0, ax, height);
            cv->line(0, ay, width, ay);
        }

        // Reuse display
        pIDisplay           = float_buffer_t::reuse(pIDisplay, 4, width);
        float_buffer_t *b   = pIDisplay;
        if (b == NULL)
            return false;

        size_t channels = ((nMode == EM_MONO) || (nMode == EM_STEREO)) ? 1 : 2;
        static uint32_t c_colors[] = {
                CV_MIDDLE_CHANNEL, CV_MIDDLE_CHANNEL,
                CV_MIDDLE_CHANNEL, CV_MIDDLE_CHANNEL,
                CV_LEFT_CHANNEL, CV_RIGHT_CHANNEL,
                CV_MIDDLE_CHANNEL, CV_SIDE_CHANNEL
               };

        bool aa = cv->set_anti_aliasing(true);
        cv->set_line_width(2);

        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c    = &vChannels[i];

            for (size_t j=0; j<width; ++j)
            {
                size_t k        = (j*expander_base_metadata::CURVE_MESH_SIZE)/width;
                b->v[0][j]      = vCurve[k];
            }
            c->sExp.curve(b->v[1], b->v[0], width);
            if (c->fMakeup != 1.0f)
                dsp::mul_k2(b->v[1], c->fMakeup, width);

            dsp::fill(b->v[2], 0.0f, width);
            dsp::fill(b->v[3], height, width);
            dsp::axis_apply_log1(b->v[2], b->v[0], zx, dx, width);
            dsp::axis_apply_log1(b->v[3], b->v[1], zy, dy, width);

            // Draw mesh
            uint32_t color = (bypassing || !(active())) ? CV_SILVER : c_colors[nMode*2 + i];
            cv->set_color_rgb(color);
            cv->draw_lines(b->v[2], b->v[3], width);
        }

        // Draw dot
        if (active())
        {
            for (size_t i=0; i<channels; ++i)
            {
                channel_t *c    = &vChannels[i];

                uint32_t color = (bypassing) ? CV_SILVER : c_colors[nMode*2 + i];
                Color c1(color), c2(color);
                c2.alpha(0.9);

                float ax = dx*(logf(c->fDotIn*zx));
                float ay = height + dy*(logf(c->fDotOut*zy));

                cv->radial_gradient(ax, ay, c1, c2, 12);
                cv->set_color_rgb(0);
                cv->circle(ax, ay, 4);
                cv->set_color_rgb(color);
                cv->circle(ax, ay, 3);
            }
        }

        cv->set_anti_aliasing(aa);

        return true;
    }

    //-------------------------------------------------------------------------
    // Expander derivatives
    expander_mono::expander_mono() : expander_base(metadata, false, EM_MONO)
    {
    }

    expander_stereo::expander_stereo() : expander_base(metadata, false, EM_STEREO)
    {
    }

    expander_lr::expander_lr() : expander_base(metadata, false, EM_LR)
    {
    }

    expander_ms::expander_ms() : expander_base(metadata, false, EM_MS)
    {
    }

    sc_expander_mono::sc_expander_mono() : expander_base(metadata, true, EM_MONO)
    {
    }

    sc_expander_stereo::sc_expander_stereo() : expander_base(metadata, true, EM_STEREO)
    {
    }

    sc_expander_lr::sc_expander_lr() : expander_base(metadata, true, EM_LR)
    {
    }

    sc_expander_ms::sc_expander_ms() : expander_base(metadata, true, EM_MS)
    {
    }
}





