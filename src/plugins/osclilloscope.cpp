/*
 * osclilloscope.cpp
 *
 *  Created on: 1 Mar 2020
 *      Author: crocoduck
 */

#include <plugins/oscilloscope.h>
#include <core/debug.h>

#define TRACE_PORT(p)       lsp_trace("  port id=%s", (p)->metadata()->id);
#define BUF_LIM_SIZE        196608
#define PRE_TRG_MAX_SIZE    196608

#define SWEEP_GEN_N_BITS    32
#define SWEEP_GEN_PEAK      1.0f // Stream min x coordinate should be -SWEEP_GEN_PEAK and max x coordinate should be +SWEEP_GEN_PEAK

#define DC_BLOCK_CUTOFF_HZ  5.0
#define DC_BLOCK_DFL_ALPHA  0.999f

#define STREAM_MAX_X        1.0f
#define STREAM_MIN_X       -1.0f
#define STREAM_MAX_Y        1.0f
#define STREAM_MIN_Y       -1.0f
#define STREAM_N_VER_DIV    4
#define STREAM_N_HOR_DIV    4
#define DECIM_PRECISION     0.1e-4 // For development, this should be calculated from screen size !!!

#define AUTO_SWEEP_TIME     1.0f

namespace lsp
{
    over_mode_t oscilloscope_base::get_oversampler_mode(size_t portValue)
    {
        switch (portValue)
        {
            case oscilloscope_base_metadata::OSC_OVS_NONE:
                return OM_NONE;
            case oscilloscope_base_metadata::OSC_OVS_2X:
                return OM_LANCZOS_2X4;
            case oscilloscope_base_metadata::OSC_OVS_3X:
                return OM_LANCZOS_3X4;
            case oscilloscope_base_metadata::OSC_OVS_4X:
                return OM_LANCZOS_4X4;
            case oscilloscope_base_metadata::OSC_OVS_6X:
                return OM_LANCZOS_6X4;
            case oscilloscope_base_metadata::OSC_OVS_8X:
            default:
                return OM_LANCZOS_8X4;
        }
    }

    oscilloscope_base::ch_mode_t oscilloscope_base::get_scope_mode(size_t portValue)
    {
        switch (portValue)
        {
            case oscilloscope_base_metadata::MODE_XY:
                return CH_MODE_XY;
            case oscilloscope_base_metadata::MODE_TRIGGERED:
                return CH_MODE_TRIGGERED;
            default:
                return CH_MODE_DFL;
        }
    }

    oscilloscope_base::ch_sweep_type_t oscilloscope_base::get_sweep_type(size_t portValue)
    {
        switch (portValue)
        {
            case oscilloscope_base_metadata::SWEEP_TYPE_SAWTOOTH:
                return CH_SWEEP_TYPE_SAWTOOTH;
            case oscilloscope_base_metadata::SWEEP_TYPE_TRIANGULAR:
                return CH_SWEEP_TYPE_TRIANGULAR;
            case oscilloscope_base_metadata::SWEEP_TYPE_SINE:
                return CH_SWEEP_TYPE_SINE;
            default:
                return CH_SWEEP_TYPE_DFL;
        }
    }

    oscilloscope_base::ch_trg_input_t oscilloscope_base::get_trigger_input(size_t portValue)
    {
        switch (portValue)
        {
            case oscilloscope_base_metadata::TRIGGER_INPUT_Y:
                return CH_TRG_INPUT_Y;
            case oscilloscope_base_metadata::TRIGGER_INPUT_EXT:
                return CH_TRG_INPUT_EXT;
            default:
                return CH_TRG_INPUT_DFL;
        }
    }

    oscilloscope_base::ch_coupling_t oscilloscope_base::get_coupling_type(size_t portValue)
    {
        switch (portValue)
        {
            case oscilloscope_base_metadata::COUPLING_AC:
                return CH_COUPLING_AC;
            case oscilloscope_base_metadata::COUPLING_DC:
                return CH_COUPLING_DC;
            default:
                return CH_COUPLING_DFL;

        }
    }

    trg_mode_t oscilloscope_base::get_trigger_mode(size_t portValue)
    {
        switch (portValue)
        {
            case oscilloscope_base_metadata::TRIGGER_MODE_SINGLE:
                return TRG_MODE_SINGLE;
            case  oscilloscope_base_metadata::TRIGGER_MODE_MANUAL:
                return TRG_MODE_MANUAL;
            case oscilloscope_base_metadata::TRIGGER_MODE_REPEAT:
                return TRG_MODE_REPEAT;
            default:
                return TRG_MODE_REPEAT;
        }
    }

    trg_type_t oscilloscope_base::get_trigger_type(size_t portValue)
    {
        switch (portValue)
        {
            case oscilloscope_base_metadata::TRIGGER_TYPE_NONE:
                return TRG_TYPE_NONE;
            case oscilloscope_base_metadata::TRIGGER_TYPE_SIMPLE_RISING_EDGE:
                return TRG_TYPE_SIMPLE_RISING_EDGE;
            case oscilloscope_base_metadata::TRIGGER_TYPE_SIMPE_FALLING_EDGE:
                return TRG_TYPE_SIMPLE_FALLING_EDGE;
            case oscilloscope_base_metadata::TRIGGER_TYPE_ADVANCED_RISING_EDGE:
                return TRG_TYPE_ADVANCED_RISING_EDGE;
            case oscilloscope_base_metadata::TRIGGER_TYPE_ADVANCED_FALLING_EDGE:
                return TRG_TYPE_ADVANCED_FALLING_EDGE;
            default:
                return TRG_TYPE_NONE;
        }
    }

    void oscilloscope_base::update_dc_block_filter(FilterBank &rFilterBank)
    {
        /* Filter Transfer Function:
         *
         *          g - g z^-1
         * H(z) = ----------------
         *          1 - a * z^-1
         *
         * With g = sDCBlockParams.fGain, a = sACBlockParams.fAlpha
         */

        rFilterBank.begin();

        biquad_x1_t *f = rFilterBank.add_chain();
        if (f == NULL)
            return;

        f->b0   = sDCBlockParams.fGain;
        f->b1   = -sDCBlockParams.fGain;
        f->b2   = 0.0f;
        f->a1   = sDCBlockParams.fAlpha;
        f->a2   = 0.0f;
        f->p0   = 0.0f;
        f->p1   = 0.0f;
        f->p2   = 0.0f;

        rFilterBank.end(true);
    }

    void oscilloscope_base::reconfigure_dc_block_filters()
    {
        double omega = 2.0 * M_PI * DC_BLOCK_CUTOFF_HZ / nSampleRate; // Normalised frequency

        double c = cos(omega);
        double g = 1.9952623149688795; // This is 10^(3/10), used to calculate the parameter alpha so that it is exactly associated to the cutoff frequency (-3 dB).
        double r = sqrt(c*c - 1.0 - 2.0 * g * c + 2.0 * g);

        double alpha1 = c + r;
        double alpha2 = c - r;

        if ((alpha1 >= 0.0) && (alpha1 < 1.0))
            sDCBlockParams.fAlpha = alpha1;
        else if ((alpha2 >= 0.0) && (alpha2 < 1.0))
            sDCBlockParams.fAlpha = alpha2;
        else
            sDCBlockParams.fAlpha = DC_BLOCK_DFL_ALPHA;

        sDCBlockParams.fGain = 0.5f * (1.0f + sDCBlockParams.fAlpha);

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            update_dc_block_filter(c->sDCBlockBank_x);
            update_dc_block_filter(c->sDCBlockBank_y);
            update_dc_block_filter(c->sDCBlockBank_ext);
        }
    }

    void oscilloscope_base::do_sweep_step(channel_t *c, float strobe_value)
    {
        c->sSweepGenerator.process_overwrite(&c->vDisplay_x[c->nDisplayHead], 1);
        c->vDisplay_y[c->nDisplayHead] = c->vData_y_delay[c->nDataHead];
        c->vDisplay_s[c->nDisplayHead] = strobe_value;
        ++c->nDataHead;
        ++c->nDisplayHead;
    }

    void oscilloscope_base::reset_display_buffers(channel_t *c)
    {
        // fill_zero is for DEBUG !!! Should not be necessary as they are constantly overwritten.
        dsp::fill_zero(c->vDisplay_x, BUF_LIM_SIZE);
        dsp::fill_zero(c->vDisplay_y, BUF_LIM_SIZE);
        dsp::fill_zero(c->vDisplay_s, BUF_LIM_SIZE);
        ///

        c->nDisplayHead = 0;
    }

    float *oscilloscope_base::select_trigger_input(float *extPtr, float* yPtr, ch_trg_input_t input)
    {
        switch (input)
        {
            case CH_TRG_INPUT_EXT:
                return extPtr;

            case CH_TRG_INPUT_Y:
            default:
                return yPtr;
        }
    }

    void oscilloscope_base::set_oversampler(Oversampler &over, over_mode_t mode)
    {
        over.set_mode(mode);
        if (over.modified())
            over.update_settings();
    }

    void oscilloscope_base::set_sweep_generator(channel_t *c)
    {
        c->sSweepGenerator.set_sample_rate(c->nOverSampleRate);
        c->sSweepGenerator.set_frequency(c->nOverSampleRate / c->nSweepSize);

        switch (c->enSweepType)
        {
            case CH_SWEEP_TYPE_TRIANGULAR:
            {
                c->sSweepGenerator.set_function(FG_SAWTOOTH);
                c->sSweepGenerator.set_dc_reference(DC_WAVEDC);
                c->sSweepGenerator.set_amplitude(SWEEP_GEN_PEAK);
                c->sSweepGenerator.set_dc_offset(0.0f);
                c->sSweepGenerator.set_width(0.5f);
            }
            break;

            case CH_SWEEP_TYPE_SINE:
            {
                c->sSweepGenerator.set_function(FG_SINE);
                c->sSweepGenerator.set_dc_reference(DC_WAVEDC);
                c->sSweepGenerator.set_amplitude(SWEEP_GEN_PEAK);
                c->sSweepGenerator.set_dc_offset(0.0f);
            }
            break;

            case CH_SWEEP_TYPE_SAWTOOTH:
            default:
            {
                c->sSweepGenerator.set_function(FG_SAWTOOTH);
                c->sSweepGenerator.set_dc_reference(DC_WAVEDC);
                c->sSweepGenerator.set_amplitude(SWEEP_GEN_PEAK);
                c->sSweepGenerator.set_dc_offset(0.0f);
                c->sSweepGenerator.set_width(1.0f);
            }
            break;
        }

        c->sSweepGenerator.update_settings();
    }

    void oscilloscope_base::configure_oversamplers(channel_t *c, over_mode_t mode)
    {
        c->enOverMode = mode;

        set_oversampler(c->sOversampler_x, c->enOverMode);
        set_oversampler(c->sOversampler_y, c->enOverMode);
        set_oversampler(c->sOversampler_ext, c->enOverMode);

        // All are set the same way, use any to get these variables
        c->nOversampling    = c->sOversampler_x.get_oversampling();
        c->nOverSampleRate  = c->nOversampling * nSampleRate;
    }

    oscilloscope_base::oscilloscope_base(const plugin_metadata_t &metadata, size_t channels): plugin_t(metadata)
    {
        nChannels           = channels;
        vChannels           = NULL;

        nSampleRate         = 0;

        pData               = NULL;

        pStrobeHistSize     = NULL;
        pXYRecordTime       = NULL;

        pChannelSelector    = NULL;

        pOvsMode            = NULL;
        pScpMode            = NULL;
        pCoupling_x         = NULL;
        pCoupling_y         = NULL;
        pCoupling_ext       = NULL;

        pSweepType          = NULL;
        pHorDiv             = NULL;
        pHorPos             = NULL;

        pVerDiv             = NULL;
        pVerPos             = NULL;

        pTrgHys             = NULL;
        pTrgLev             = NULL;
        pTrgHold            = NULL;
        pTrgMode            = NULL;
        pTrgType            = NULL;
        pTrgInput           = NULL;
        pTrgReset           = NULL;
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
                channel_t *c = &vChannels[ch];

                c->sDCBlockBank_x.destroy();
                c->sDCBlockBank_y.destroy();
                c->sDCBlockBank_ext.destroy();

                c->sOversampler_x.destroy();
                c->sOversampler_y.destroy();
                c->sOversampler_ext.destroy();

                c->sPreTrgDelay.destroy();

                c->sSweepGenerator.destroy();

                c->vTemp            = NULL;
                c->vData_x          = NULL;
                c->vData_y          = NULL;
                c->vData_ext        = NULL;
                c->vData_y_delay    = NULL;
                c->vDisplay_x       = NULL;
                c->vDisplay_y       = NULL;
                c->vDisplay_s       = NULL;
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

        /** For each channel:
         * 1X temp buffer +
         * 1X external data buffer +
         * 1X x data buffer +
         * 1X y data buffer +
         * 1X delayed y data buffer +
         * 1X x display buffer +
         * 1X y display buffer +
         * 1X strobe display buffer
         *
         * All buffers size BUF_LIM_SIZE
         */
        size_t samples = nChannels * BUF_LIM_SIZE * 8;

        float *ptr = alloc_aligned<float>(pData, samples);
        if (ptr == NULL)
            return;

        lsp_guard_assert(float *save = ptr);

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            init_state_stage(c);

            if (!c->sDCBlockBank_x.init(FILTER_CHAINS_MAX))
                return;

            if (!c->sDCBlockBank_y.init(FILTER_CHAINS_MAX))
                return;

            if (!c->sDCBlockBank_ext.init(FILTER_CHAINS_MAX))
                return;

            if (!c->sOversampler_x.init())
                return;

            if (!c->sOversampler_y.init())
                return;

            if (!c->sOversampler_ext.init())
                return;

            if (!c->sPreTrgDelay.init(PRE_TRG_MAX_SIZE))
                return;

            // Settings for the Sweep Generator
            c->sSweepGenerator.init();
            c->sSweepGenerator.set_phase_accumulator_bits(SWEEP_GEN_N_BITS);
            c->sSweepGenerator.set_phase(0.0f);
            c->sSweepGenerator.update_settings();

            c->vTemp            = ptr;
            ptr                += BUF_LIM_SIZE;

            c->vData_x          = ptr;
            ptr                += BUF_LIM_SIZE;

            c->vData_y          = ptr;
            ptr                += BUF_LIM_SIZE;

            c->vData_ext        = ptr;
            ptr                += BUF_LIM_SIZE;

            c->vData_y_delay    = ptr;
            ptr                += BUF_LIM_SIZE;

            c->vDisplay_x       = ptr;
            ptr                += BUF_LIM_SIZE;

            c->vDisplay_y       = ptr;
            ptr                += BUF_LIM_SIZE;

            c->vDisplay_s       = ptr;
            ptr                += BUF_LIM_SIZE;

            c->nDataHead            = 0;
            c->nDisplayHead         = 0;
            c->nSamplesCounter      = 0;
            c->bClearStream         = false;

            c->nPreTrigger          = 0;
            c->nSweepSize           = 0;

            c->fVerStreamScale      = 0.0f;
            c->fVerStreamOffset     = 0.0f;

            c->bAutoSweep           = true;
            c->nAutoSweepLimit      = 0;
            c->nAutoSweepCounter    = 0;

            c->enState              = CH_STATE_LISTENING;

            c->vIn_x                = NULL;
            c->vIn_y                = NULL;
            c->vIn_ext              = NULL;

            c->vOut_x               = NULL;
            c->vOut_y               = NULL;

            c->pIn_x                = NULL;
            c->pIn_y                = NULL;
            c->pIn_ext              = NULL;

            c->pOut_x               = NULL;
            c->pOut_y               = NULL;

            c->pOvsMode             = NULL;
            c->pScpMode             = NULL;
            c->pCoupling_x          = NULL;
            c->pCoupling_y          = NULL;
            c->pCoupling_ext        = NULL;

            c->pSweepType           = NULL;
            c->pHorDiv              = NULL;
            c->pHorPos              = NULL;

            c->pVerDiv              = NULL;
            c->pVerPos              = NULL;

            c->pTrgHys              = NULL;
            c->pTrgLev              = NULL;
            c->pTrgHold             = NULL;
            c->pTrgMode             = NULL;
            c->pTrgType             = NULL;
            c->pTrgInput            = NULL;
            c->pTrgReset            = NULL;

            c->pGlobalSwitch        = NULL;
            c->pFreezeSwitch        = NULL;
            c->pSoloSwitch          = NULL;
            c->pMuteSwitch          = NULL;

            c->pStream              = NULL;
        }

        lsp_assert(ptr <= &save[samples]);

        // Bind ports
        size_t port_id = 0;

        lsp_trace("Binding audio ports");

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pIn_x = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pIn_y = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pIn_ext = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pOut_x = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pOut_y = vPorts[port_id++];
        }

        // Common settings
        lsp_trace("Binding common ports");

        TRACE_PORT(vPorts[port_id]);
        pStrobeHistSize = vPorts[port_id++];

        TRACE_PORT(vPorts[port_id]);
        pXYRecordTime = vPorts[port_id++];

        // Channel selector only exists on multi-channel versions. Skip for 1X plugin.
        if (nChannels > 1)
        {
            TRACE_PORT(vPorts[port_id]);
            pChannelSelector = vPorts[port_id++];
        }

        // Global ports only exists on multi-channel versions. Skip for 1X plugin.

        if (nChannels > 1)
        {
            lsp_trace("Binding global control ports");

            TRACE_PORT(vPorts[port_id]);
            pOvsMode = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pScpMode = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pCoupling_x = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pCoupling_y = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pCoupling_ext = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pSweepType = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pHorDiv = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pHorPos = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pVerDiv = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pVerPos = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pTrgHys = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pTrgLev = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pTrgHold = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pTrgMode = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pTrgType = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pTrgInput = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            pTrgReset = vPorts[port_id++];
        }

        lsp_trace("Binding channel control ports");


        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pOvsMode = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pScpMode = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pCoupling_x = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pCoupling_y = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pCoupling_ext = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pSweepType = vPorts[port_id++];

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
            vChannels[ch].pTrgHold = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pTrgMode = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pTrgType = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pTrgInput = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pTrgReset = vPorts[port_id++];
        }

        lsp_trace("Binding channel switches ports");

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pGlobalSwitch = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pFreezeSwitch = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pSoloSwitch = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pMuteSwitch = vPorts[port_id++];
        }

        lsp_trace("Binding channel visual outputs ports");

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pStream = vPorts[port_id++];
        }
    }

    void oscilloscope_base::init_state_stage(channel_t *c)
    {
        c->nUpdate = 0;

        c->sStateStage.nPV_pScpMode = oscilloscope_base_metadata::MODE_DFL;
        c->nUpdate |= UPD_SCPMODE;

        c->sStateStage.nPV_pCoupling_x = oscilloscope_base_metadata::COUPLING_DFL;
        c->nUpdate |= UPD_ACBLOCK_X;

        c->sStateStage.nPV_pCoupling_y = oscilloscope_base_metadata::COUPLING_DFL;
        c->nUpdate |= UPD_ACBLOCK_Y;

        c->sStateStage.nPV_pCoupling_ext = oscilloscope_base_metadata::COUPLING_DFL;
        c->nUpdate |= UPD_ACBLOCK_EXT;

        c->sStateStage.nPV_pOvsMode = oscilloscope_base_metadata::OSC_OVS_DFL;
        c->nUpdate |= UPD_OVERSAMPLER_X;
        c->nUpdate |= UPD_OVERSAMPLER_Y;
        c->nUpdate |= UPD_OVERSAMPLER_EXT;

        c->sStateStage.nPV_pTrgInput = oscilloscope_base_metadata::TRIGGER_INPUT_DFL;
        c->nUpdate |= UPD_TRIGGER_INPUT;

        c->sStateStage.fPV_pVerDiv = oscilloscope_base_metadata::VERTICAL_DIVISION_DFL;
        c->sStateStage.fPV_pVerPos = oscilloscope_base_metadata::VERTICAL_POSITION_DFL;
        c->nUpdate |= UPD_TRIGGER;

        c->sStateStage.fPV_pTrgHys = oscilloscope_base_metadata::TRIGGER_HYSTERESIS_DFL;
        c->nUpdate |= UPD_TRIGGER;

        c->sStateStage.fPV_pTrgLevel = oscilloscope_base_metadata::TRIGGER_LEVEL_DFL;
        c->nUpdate |= UPD_TRIGGER;

        c->sStateStage.nPV_pTrgMode = oscilloscope_base_metadata::TRIGGER_MODE_DFL;
        c->nUpdate |= UPD_TRIGGER;

        c->sStateStage.fPV_pTrgHold = oscilloscope_base_metadata::TRIGGER_HOLD_TIME_DFL;
        c->nUpdate |= UPD_TRIGGER_HOLD;

        c->sStateStage.nPV_pTrgType = oscilloscope_base_metadata::TRIGGER_TYPE_DFL;
        c->nUpdate |= UPD_TRIGGER;

        c->sStateStage.fPV_pHorDiv = oscilloscope_base_metadata::TIME_DIVISION_DFL;
        c->nUpdate |= UPD_SWEEP_GENERATOR;

        c->sStateStage.fPV_pHorPos = oscilloscope_base_metadata::TIME_POSITION_DFL;
        c->nUpdate |= UPD_SWEEP_GENERATOR;
        c->nUpdate |= UPD_PRETRG_DELAY;

        c->sStateStage.nPV_pSweepType = oscilloscope_base_metadata::SWEEP_TYPE_DFL;
        c->nUpdate |= UPD_SWEEP_GENERATOR;

        c->sStateStage.fPV_pXYRecordTime = oscilloscope_base_metadata::XY_RECORD_TIME_DFL;
        c->nUpdate |= UPD_XY_RECORD_TIME;

        c->nUpdate |= UPD_VER_SCALES;
        c->nUpdate |= UPD_HOR_SCALES;

        // By default, this must be false.
        c->bUseGlobal = false;
    }

    void oscilloscope_base::commit_staged_state_change(channel_t *c)
    {
        if (c->nUpdate == 0)
            return;

        if (c->nUpdate & UPD_SCPMODE)
            c->enMode = get_scope_mode(c->sStateStage.nPV_pScpMode);

        if (c->nUpdate & UPD_ACBLOCK_X)
            c->enCoupling_x = get_coupling_type(c->sStateStage.nPV_pCoupling_x);

        if (c->nUpdate & UPD_ACBLOCK_Y)
            c->enCoupling_y = get_coupling_type(c->sStateStage.nPV_pCoupling_y);

        if (c->nUpdate & UPD_ACBLOCK_EXT)
            c->enCoupling_ext = get_coupling_type(c->sStateStage.nPV_pCoupling_ext);

        if (
                (c->nUpdate & UPD_OVERSAMPLER_X) ||
                (c->nUpdate & UPD_OVERSAMPLER_Y) ||
                (c->nUpdate & UPD_OVERSAMPLER_EXT)
                )
        {
            configure_oversamplers(c, get_oversampler_mode(c->sStateStage.nPV_pOvsMode));
        }

        if (c->nUpdate & UPD_XY_RECORD_TIME)
        {
            c->nXYRecordSize = seconds_to_samples(c->nOverSampleRate, 0.001f * c->sStateStage.fPV_pXYRecordTime);
            c->nXYRecordSize = (c->nXYRecordSize < BUF_LIM_SIZE) ? c->nXYRecordSize  : BUF_LIM_SIZE;
        }

        // UPD_SWEEP_GENERATOR handling is split as if also UPD_PRETRG_DELAY needs to be handled the correct order of operations is as follows.
        if (c->nUpdate & UPD_SWEEP_GENERATOR)
        {
             c->nSweepSize = STREAM_N_HOR_DIV * seconds_to_samples(c->nOverSampleRate, 0.001f * c->sStateStage.fPV_pHorDiv);
             c->nSweepSize = (c->nSweepSize < BUF_LIM_SIZE) ? c->nSweepSize  : BUF_LIM_SIZE;
        }

        if (c->nUpdate & UPD_PRETRG_DELAY)
        {
             c->nPreTrigger = 0.5f * (0.01f * c->sStateStage.fPV_pHorPos  + 1) * (c->nSweepSize - 1);
             c->nPreTrigger = (c->nPreTrigger < PRE_TRG_MAX_SIZE) ? c->nPreTrigger : PRE_TRG_MAX_SIZE;
             c->sPreTrgDelay.set_delay(c->nPreTrigger);
             c->sPreTrgDelay.clear();
        }

        if (c->nUpdate & UPD_SWEEP_GENERATOR)
        {
            c->enSweepType = get_sweep_type(c->sStateStage.nPV_pSweepType);
            set_sweep_generator(c);
        }

        if (c->nUpdate & UPD_TRIGGER_INPUT)
            c->enTrgInput = get_trigger_input(c->sStateStage.nPV_pTrgInput);

        if (c->nUpdate & UPD_TRIGGER_HOLD)
        {
            size_t minHold = c->nSweepSize;
            size_t trgHold = seconds_to_samples(c->nOverSampleRate, c->sStateStage.fPV_pTrgHold);
            trgHold = trgHold > minHold ? trgHold : minHold;
            c->sTrigger.set_trigger_hold_samples(trgHold);

            c->nAutoSweepLimit = seconds_to_samples(c->nOverSampleRate, AUTO_SWEEP_TIME);
            c->nAutoSweepLimit = (c->nAutoSweepLimit < trgHold) ? c->nAutoSweepLimit : trgHold;
            c->nAutoSweepCounter = 0;
        }

        if (c->nUpdate & UPD_HOR_SCALES)
        {
            c->fHorStreamScale = (STREAM_MAX_X - STREAM_MIN_X) / (STREAM_N_HOR_DIV * c->sStateStage.fPV_pHorDiv);
            c->fHorStreamOffset = 0.5f * (STREAM_MAX_X - STREAM_MIN_X) * (0.01f * c->sStateStage.fPV_pHorPos + 1.0f) + STREAM_MIN_X;
        }

        if (c->nUpdate & UPD_VER_SCALES)
        {
            c->fVerStreamScale = (STREAM_MAX_Y - STREAM_MIN_Y) / (STREAM_N_VER_DIV * c->sStateStage.fPV_pVerDiv);
            c->fVerStreamOffset = 0.5f * (STREAM_MAX_Y - STREAM_MIN_Y) * (0.01f * c->sStateStage.fPV_pVerPos + 1.0f) + STREAM_MIN_Y;
        }

        if (c->nUpdate & UPD_TRIGGER)
        {
            trg_mode_t trgMode = get_trigger_mode(c->sStateStage.nPV_pTrgMode);

            if ((trgMode == TRG_MODE_SINGLE) || (trgMode == TRG_MODE_MANUAL))
                c->bAutoSweep = false;
            else
                c->bAutoSweep = true;

            c->sTrigger.set_trigger_mode(trgMode);
            c->sTrigger.set_trigger_hysteresis(0.01f * c->sStateStage.fPV_pTrgHys * STREAM_N_VER_DIV * c->sStateStage.fPV_pVerDiv);
            c->sTrigger.set_trigger_type(get_trigger_type(c->sStateStage.nPV_pTrgType));
            c->sTrigger.set_trigger_threshold(0.5f * STREAM_N_VER_DIV * c->sStateStage.fPV_pVerDiv * 0.01f * c->sStateStage.fPV_pTrgLevel);
            c->sTrigger.update_settings();
        }

        if (c->nUpdate & UPD_TRGGER_RESET)
        {
            c->sTrigger.reset_single_trigger();
            c->sTrigger.activate_manual_trigger();
        }

        c->bClearStream = true;

        // Clear the update flag
        c->nUpdate = 0;
    }

    void oscilloscope_base::graph_stream(channel_t * c)
    {
        stream_t *stream = c->pStream->getBuffer<stream_t>();
        if (stream == NULL)
            return;

        if (c->bClearStream)
        {
            stream->clear();
            c->bClearStream = false;
        }

        // In-place decimation:
        size_t j = 0;

        // Number of samples changes depending on mode
        size_t query_size;
        switch (c->enMode)
        {
            case CH_MODE_XY: query_size = c->nXYRecordSize; break;
            case CH_MODE_TRIGGERED: query_size = c->nSweepSize; break;
        }

        for (size_t i = 1; i < query_size; ++i)
        {
            float dx    = c->vDisplay_x[i] - c->vDisplay_x[j];
            float dy    = c->vDisplay_y[i] - c->vDisplay_y[j];
            float s     = dx*dx + dy*dy;

            if (s < DECIM_PRECISION) // Skip point
            {
                c->vDisplay_s[j] = lsp_max(c->vDisplay_s[i], c->vDisplay_s[j]); // Keep the strobe signal
                continue;
            }

            // Add point to decimated array
            ++j;
            c->vDisplay_x[j] = c->vDisplay_x[i];
            c->vDisplay_y[j] = c->vDisplay_y[i];
        }

        size_t to_submit = j + 1; // Total number of decimated samples.

        // Apply scaling and offset:
        for (size_t i = 0; i < to_submit; ++i)
        {
            c->vDisplay_y[i] = c->fVerStreamScale * c->vDisplay_y[i] + c->fVerStreamOffset;

            // x is to be scaled and offset only in XY mode
            if (c->enMode == CH_MODE_XY)
                c->vDisplay_x[i] = c->fHorStreamScale * c->vDisplay_x[i] + c->fHorStreamOffset;
        }

        // Submit data for plotting (emit the figure data with fixed-size frames):
        for (size_t i = 0; i < to_submit; )  // nSweepSize can be as big as BUF_LIM_SIZE !!!
        {

            size_t count = stream->add_frame(to_submit - i);     // Add a frame
            stream->write_frame(0, &c->vDisplay_x[i], 0, count); // X'es
            stream->write_frame(1, &c->vDisplay_y[i], 0, count); // Y's
            stream->write_frame(2, &c->vDisplay_s[i], 0, count); // Strobe signal
            stream->commit_frame();                              // Commit the frame

            // Move the index in the source buffer
            i += count;
        }
    }

    void oscilloscope_base::update_settings()
    {
        float nXYRecordSize = pSweepType->getValue();

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            // Global controls only actually exist for mult-channel plugins. Do not use for 1X.
            if (nChannels > 1)
                c->bUseGlobal = c->pGlobalSwitch->getValue() >= 0.5f;

            size_t scpmode;
            if (c->bUseGlobal)
                scpmode = pScpMode->getValue();
            else
                scpmode = c->pScpMode->getValue();
            if (scpmode != c->sStateStage.nPV_pScpMode)
            {
                c->sStateStage.nPV_pScpMode = scpmode;
                c->nUpdate |= UPD_SCPMODE;
            }

            size_t coupling_x;
            if (c->bUseGlobal)
                coupling_x = pCoupling_x->getValue();
            else
                coupling_x = c->pCoupling_x->getValue();
            if (coupling_x != c->sStateStage.nPV_pCoupling_x)
            {
                c->sStateStage.nPV_pCoupling_x = coupling_x;
                c->nUpdate |= UPD_ACBLOCK_X;
            }

            size_t coupling_y;
            if (c->bUseGlobal)
                coupling_y = pCoupling_y->getValue();
            else
                coupling_y = c->pCoupling_y->getValue();
            if (coupling_y != c->sStateStage.nPV_pCoupling_y)
            {
                c->sStateStage.nPV_pCoupling_y = coupling_y;
                c->nUpdate |= UPD_ACBLOCK_Y;
            }

            size_t coupling_ext;
            if (c->bUseGlobal)
                coupling_ext = pCoupling_ext->getValue();
            else
                coupling_ext = c->pCoupling_ext->getValue();
            if (coupling_ext != c->sStateStage.nPV_pCoupling_ext)
            {
                c->sStateStage.nPV_pCoupling_ext = coupling_ext;
                c->nUpdate |= UPD_ACBLOCK_EXT;
            }

            size_t overmode;
            if (c->bUseGlobal)
                overmode = pOvsMode->getValue();
            else
                overmode = c->pOvsMode->getValue();
            if (overmode != c->sStateStage.nPV_pOvsMode)
            {
                c->sStateStage.nPV_pOvsMode = overmode;
                c->nUpdate |= UPD_OVERSAMPLER_X;
                c->nUpdate |= UPD_OVERSAMPLER_Y;
                c->nUpdate |= UPD_OVERSAMPLER_EXT;
                c->nUpdate |= UPD_PRETRG_DELAY;
                c->nUpdate |= UPD_SWEEP_GENERATOR;
                c->nUpdate |= UPD_TRIGGER_HOLD;
            }

            if (nXYRecordSize != c->sStateStage.fPV_pXYRecordTime)
                c->nUpdate |= UPD_XY_RECORD_TIME;

            size_t trginput;
            if (c->bUseGlobal)
                trginput = pTrgInput->getValue();
            else
                trginput = c->pTrgInput->getValue();
            if (trginput != c->sStateStage.nPV_pTrgInput)
            {
                c->sStateStage.nPV_pTrgInput = trginput;
                c->nUpdate |= UPD_TRIGGER_INPUT;
            }

            float verDiv;
            float verPos;
            if (c->bUseGlobal)
            {
                verDiv = pVerDiv->getValue();
                verPos = pVerPos->getValue();
            }
            else
            {
                verDiv = c->pVerDiv->getValue();
                verPos = c->pVerPos->getValue();
            }
            if ((verDiv != c->sStateStage.fPV_pVerDiv) || (verPos != c->sStateStage.fPV_pVerPos))
            {
                c->sStateStage.fPV_pVerDiv = verDiv;
                c->sStateStage.fPV_pVerPos = verPos;
                c->nUpdate |= UPD_VER_SCALES;
                c->nUpdate |= UPD_TRIGGER;
            }

            float trgHys;
            if (c->bUseGlobal)
                trgHys = pTrgHys->getValue();
            else
                trgHys = c->pTrgHys->getValue();
            if (trgHys != c->sStateStage.fPV_pTrgHys)
            {
                c->sStateStage.fPV_pTrgHys = trgHys;
                c->nUpdate |= UPD_TRIGGER;
            }

            float trgLevel;
            if (c->bUseGlobal)
                trgLevel = pTrgLev->getValue();
            else
                trgLevel = c->pTrgLev->getValue();
            if (trgLevel != c->sStateStage.fPV_pTrgLevel)
            {
                c->sStateStage.fPV_pTrgLevel = trgLevel;
                c->nUpdate |= UPD_TRIGGER;
            }

            size_t trgmode;
            if (c->bUseGlobal)
                trgmode = pTrgMode->getValue();
            else
                trgmode = c->pTrgMode->getValue();
            if (trgmode != c->sStateStage.nPV_pTrgMode)
            {
                c->sStateStage.nPV_pTrgMode = trgmode;
                c->nUpdate |= UPD_TRIGGER;
            }

            float trghold;
            if (c->bUseGlobal)
                trghold = pTrgHold->getValue();
            else
                trghold = c->pTrgHold->getValue();
            if (trghold != c->sStateStage.fPV_pTrgHold)
            {
                c->sStateStage.fPV_pTrgHold = trghold;
                c->nUpdate |= UPD_TRIGGER_HOLD;
            }

            size_t trgtype;
            if (c->bUseGlobal)
                trgtype = pTrgType->getValue();
            else
                trgtype = c->pTrgType->getValue();
            if (trgtype != c->sStateStage.nPV_pTrgType)
            {
                c->sStateStage.nPV_pTrgType = trgtype;
                c->nUpdate |= UPD_TRIGGER;
            }

            if (c->bUseGlobal)
            {
                if (pTrgReset->getValue() >= 0.5f)
                    c->nUpdate |= UPD_TRGGER_RESET;
            }
            else
            {
                if (c->pTrgReset->getValue() >= 0.5f)
                    c->nUpdate |= UPD_TRGGER_RESET;
            }

            float horDiv;
            if (c->bUseGlobal)
                horDiv = pHorDiv->getValue();
            else
                horDiv = c->pHorDiv->getValue();
            if (horDiv != c->sStateStage.fPV_pHorDiv)
            {
                c->sStateStage.fPV_pHorDiv = horDiv;
                c->nUpdate |= UPD_HOR_SCALES;
                c->nUpdate |= UPD_PRETRG_DELAY;
                c->nUpdate |= UPD_SWEEP_GENERATOR;
                c->nUpdate |= UPD_TRIGGER_HOLD;
            }

            float horPos;
            if (c->bUseGlobal)
                horPos = pHorPos->getValue();
            else
                horPos = c->pHorPos->getValue();
            if (horPos != c->sStateStage.fPV_pHorPos)
            {
                c->sStateStage.fPV_pHorPos = horPos;
                c->nUpdate |= UPD_HOR_SCALES;
                c->nUpdate |= UPD_PRETRG_DELAY;
                c->nUpdate |= UPD_SWEEP_GENERATOR;
            }

            size_t sweeptype;
            if (c->bUseGlobal)
                sweeptype = pSweepType->getValue();
            else
                sweeptype = c->pSweepType->getValue();
            if (sweeptype != c->sStateStage.nPV_pSweepType)
            {
                c->sStateStage.nPV_pSweepType = sweeptype;
                c->nUpdate |= UPD_SWEEP_GENERATOR;
            }

        }
    }

    void oscilloscope_base::update_sample_rate(long sr)
    {
        nSampleRate = sr;

        reconfigure_dc_block_filters();

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            c->sOversampler_x.set_sample_rate(sr);
            c->sOversampler_x.update_settings();

            c->sOversampler_y.set_sample_rate(sr);
            c->sOversampler_y.update_settings();

            c->sOversampler_ext.set_sample_rate(sr);
            c->sOversampler_ext.update_settings();

            c->nOverSampleRate = c->nOversampling * nSampleRate;

            c->sSweepGenerator.set_sample_rate(sr);
            c->sSweepGenerator.update_settings();
        }
    }

    void oscilloscope_base::process(size_t samples)
    {
        // Prepare channels
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            c->vIn_x = c->pIn_x->getBuffer<float>();
            c->vIn_y = c->pIn_y->getBuffer<float>();
            c->vIn_ext = c->pIn_ext->getBuffer<float>();

            c->vOut_x = c->pOut_x->getBuffer<float>();
            c->vOut_y = c->pOut_y->getBuffer<float>();

            if ((c->vIn_x == NULL) || (c->vIn_y == NULL))
                return;

            if ((c->vIn_ext == NULL))
                return;

            c->nSamplesCounter = samples;
        }

        // Bypass signal
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            if (c->vOut_x != NULL)
                dsp::copy(c->vOut_x, c->vIn_x, samples);
            if (c->vOut_y != NULL)
                dsp::copy(c->vOut_y, c->vIn_y, samples);
        }

        // Process each channel
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            commit_staged_state_change(c);

            while (c->nSamplesCounter > 0)
            {
                size_t requested        = c->nOversampling * c->nSamplesCounter;
                size_t availble         = BUF_LIM_SIZE;
                size_t to_do_upsample   = (requested < availble) ? requested : availble;
                size_t to_do            = to_do_upsample / c->nOversampling;

                switch (c->enMode)
                {
                    case CH_MODE_XY:
                    {
                        if (c->enCoupling_x == CH_COUPLING_AC)
                        {
                            c->sDCBlockBank_x.process(c->vTemp, c->vIn_x, to_do);
                            c->sOversampler_x.upsample(c->vData_x, c->vTemp, to_do);
                        }
                        else
                        {
                            c->sOversampler_x.upsample(c->vData_x, c->vIn_x, to_do);
                        }

                        if (c->enCoupling_y == CH_COUPLING_AC)
                        {
                            c->sDCBlockBank_y.process(c->vTemp, c->vIn_y, to_do);
                            c->sOversampler_y.upsample(c->vData_y, c->vTemp, to_do);
                        }
                        else
                        {
                            c->sOversampler_y.upsample(c->vData_y, c->vIn_y, to_do);
                        }

                        for (size_t n = 0; n < to_do_upsample; ++n)
                        {
                            if (c->nDisplayHead >= c->nXYRecordSize)
                            {
                                // Plot time!
                                graph_stream(c);

                                reset_display_buffers(c);
                            }

                            c->vDisplay_x[c->nDisplayHead] = c->vData_x[n];
                            c->vDisplay_y[c->nDisplayHead] = c->vData_y[n];

                            // This ensures the strobe has nXYRecordSize period and 0 phase shift.
                            if (c->nDisplayHead == 0)
                                c->vDisplay_s[c->nDisplayHead] = 1.0f;
                            else
                                c->vDisplay_s[c->nDisplayHead] = 0.0f;

                            ++c->nDisplayHead;
                        }

                    }
                    break;

                    case CH_MODE_TRIGGERED:
                    {
                        if (c->enCoupling_y == CH_COUPLING_AC)
                        {
                            c->sDCBlockBank_y.process(c->vTemp, c->vIn_y, to_do);
                            c->sOversampler_y.upsample(c->vData_y, c->vTemp, to_do);
                        }
                        else
                        {
                            c->sOversampler_y.upsample(c->vData_y, c->vIn_y, to_do);
                        }
                        c->sPreTrgDelay.process(c->vData_y_delay, c->vData_y, to_do_upsample);

                        if (c->enCoupling_ext == CH_COUPLING_AC)
                        {
                            c->sDCBlockBank_ext.process(c->vTemp, c->vIn_ext, to_do);
                            c->sOversampler_ext.upsample(c->vData_ext, c->vTemp, to_do);
                        }
                        else
                        {
                            c->sOversampler_ext.upsample(c->vData_ext, c->vIn_ext, to_do);
                        }

                        c->nDataHead = 0;

                        float *trg_input = select_trigger_input(c->vData_ext, c->vData_y, c->enTrgInput);

                        for (size_t n = 0; n < to_do_upsample; ++n)
                        {
                            c->sTrigger.single_sample_processor(trg_input[n]);

                            switch (c->enState)
                            {
                                case CH_STATE_LISTENING:
                                {
                                    if (c->bAutoSweep)
                                    {
                                        if (c->nAutoSweepCounter > c->nAutoSweepLimit)
                                        {
                                            c->enState = CH_STATE_SWEEPING;
                                            do_sweep_step(c, 1.0f);
                                            c->nAutoSweepCounter = 0;
                                        }

                                        ++c->nAutoSweepCounter;
                                    }

                                    if (c->sTrigger.get_trigger_state() == TRG_STATE_FIRED)
                                    {
                                        c->sSweepGenerator.reset_phase_accumulator();
                                        c->nDataHead = n;
                                        c->enState = CH_STATE_SWEEPING;
                                        do_sweep_step(c, 1.0f);
                                        c->nAutoSweepCounter = 0;
                                    }
                                }
                                break;

                                case CH_STATE_SWEEPING:
                                {
                                    do_sweep_step(c, 0.0f);

                                    if (c->nDisplayHead >= c->nSweepSize)
                                    {
                                        // Plot time!
                                        graph_stream(c);

                                        reset_display_buffers(c);
                                        c->enState = CH_STATE_LISTENING;
                                    }
                                }
                                break;
                            }

                        }
                    }
                    break;
                }

                c->vIn_x            += to_do;
                c->vIn_y            += to_do;
                c->vIn_ext          += to_do;
                c->vOut_x           += to_do;
                c->vOut_y           += to_do;
                c->nSamplesCounter  -= to_do;
            }
        }
    }

    // THIS MUST BE UPDATED !!!
    void oscilloscope_base::dump(IStateDumper *v) const
    {
        v->begin_object("sDCBlockParams", &sDCBlockParams, sizeof(sDCBlockParams));
        {
            v->write("fAlpha", sDCBlockParams.fAlpha);
            v->write("fGain", sDCBlockParams.fGain);
        }
        v->end_object();

        v->write("nChannels", nChannels);

        v->begin_array("vChannels", vChannels, nChannels);
        for (size_t i = 0; i < nChannels; ++i)
        {
            const channel_t *c = &vChannels[i];

            v->begin_object(c, sizeof(channel_t));
            {
                v->write("enMode", &c->enMode);
                v->write("enSweepType", &c->enSweepType);
                v->write("enTrgInput", &c->enTrgInput);
                v->write("enCoupling_x", &c->enCoupling_x);
                v->write("enCoupling_y", &c->enCoupling_y);
                v->write("enCoupling_ext", &c->enCoupling_ext);

                v->write_object("sDCBlockBank_x", &c->sDCBlockBank_x);
                v->write_object("sDCBlockBank_y", &c->sDCBlockBank_y);
                v->write_object("sDCBlockBank_ext", &c->sDCBlockBank_ext);

                v->write("enOverMode", &c->enOverMode);
                v->write("nOversampling", &c->nOversampling);
                v->write("nOverSampleRate", &c->nOverSampleRate);

                v->write_object("sOversampler_x", &c->sOversampler_x);
                v->write_object("sOversampler_y", &c->sOversampler_y);
                v->write_object("sOversampler_ext", &c->sOversampler_ext);

                v->write_object("sPreTrgDelay", &c->sPreTrgDelay);

                v->write_object("sTrigger", &c->sTrigger);

                v->write_object("sSweepGenerator", &c->sSweepGenerator);

                v->write("vTemp", &c->vTemp);
                v->write("vData_x", &c->vData_x);
                v->write("vData_y", &c->vData_y);
                v->write("vData_ext", &c->vData_ext);
                v->write("vData_y_delay", &c->vData_y_delay);
                v->write("vDisplay_x", &c->vDisplay_x);
                v->write("vDisplay_y", &c->vDisplay_y);
                v->write("vDisplay_s", &c->vDisplay_s);

                v->write("nDataHead", &c->nDataHead);
                v->write("nDisplayHead", &c->nDisplayHead);
                v->write("nSamplesCounter", &c->nSamplesCounter);
                v->write("bClearStream", &c->bClearStream);

                v->write("nPreTrigger", &c->nPreTrigger);
                v->write("nSweepSize", &c->nSweepSize);

                v->write("fVerStreamScale", &c->fVerStreamScale);
                v->write("fVerStreamOffset", &c->fVerStreamOffset);

                v->write("nXYRecordSize", &c->nXYRecordSize);
                v->write("fHorStreamScale", &c->fHorStreamScale);
                v->write("fHorStreamOffset", &c->fHorStreamOffset);

                v->write("bAutoSweep", &c->bAutoSweep);
                v->write("nAutoSweepLimit", &c->nAutoSweepLimit);
                v->write("nAutoSweepCounter", &c->nAutoSweepCounter);

                v->write("enState", &c->enState);

                v->write("nUpdate", &c->nUpdate);

                v->begin_object("sStateStage", &c->sStateStage, sizeof(c->sStateStage));
                {
                    v->write("nPV_pScpMode", &c->sStateStage.nPV_pScpMode);

                    v->write("nPV_pCoupling_x", &c->sStateStage.nPV_pCoupling_x);
                    v->write("nPV_pCoupling_y", &c->sStateStage.nPV_pCoupling_y);
                    v->write("nPV_pCoupling_ext", &c->sStateStage.nPV_pCoupling_ext);
                    v->write("nPV_pOvsMode", &c->sStateStage.nPV_pOvsMode);

                    v->write("nPV_pTrgInput", &c->sStateStage.nPV_pTrgInput);
                    v->write("fPV_pVerDiv", &c->sStateStage.fPV_pVerDiv);
                    v->write("fPV_pVerPos", &c->sStateStage.fPV_pVerPos);
                    v->write("fPV_pTrgLevel", &c->sStateStage.fPV_pTrgLevel);
                    v->write("fPV_pTrgHys", &c->sStateStage.fPV_pTrgHys);
                    v->write("nPV_pTrgMode", &c->sStateStage.nPV_pTrgMode);
                    v->write("fPV_pTrgHold", &c->sStateStage.fPV_pTrgHold);
                    v->write("nPV_pTrgType", &c->sStateStage.nPV_pTrgType);

                    v->write("fPV_pHorDiv", &c->sStateStage.fPV_pHorDiv);
                    v->write("fPV_pHorPos", &c->sStateStage.fPV_pHorPos);

                    v->write("nPV_pSweepType", &c->sStateStage.nPV_pSweepType);

                    v->write("fPV_pXYRecordTime", &c->sStateStage.fPV_pXYRecordTime);
                }
                v->end_object();

                v->write("bUseGlobal", &c->bUseGlobal);

                v->write("vIn_x", &c->vIn_x);
                v->write("vIn_y", &c->vIn_y);
                v->write("vIn_ext", &c->vIn_ext);

                v->write("vOut_x", &c->vOut_x);
                v->write("vOut_y", &c->vOut_y);

                v->write("pIn_x", &c->pIn_x);
                v->write("pIn_y", &c->pIn_y);
                v->write("pIn_ext", &c->pIn_ext);

                v->write("pOut_x", &c->pOut_x);
                v->write("pOut_y", &c->pOut_y);

                v->write("pOvsMode", &c->pOvsMode);
                v->write("pScpMode", &c->pScpMode);
                v->write("pCoupling_x", &c->pCoupling_x);
                v->write("pCoupling_y", &c->pCoupling_y);
                v->write("pCoupling_ext", &c->pCoupling_ext);

                v->write("pSweepType", &c->pSweepType);
                v->write("pHorDiv", &c->pHorDiv);
                v->write("pHorPos", &c->pHorPos);

                v->write("pVerDiv", &c->pVerDiv);
                v->write("pVerPos", &c->pVerPos);

                v->write("pTrgHys", &c->pTrgHys);
                v->write("pTrgLev", &c->pTrgLev);
                v->write("pTrgHold", &c->pTrgHold);
                v->write("pTrgMode", &c->pTrgMode);
                v->write("pTrgType", &c->pTrgType);
                v->write("pTrgInput", &c->pTrgInput);
                v->write("pTrgReset", &c->pTrgReset);

                v->write("pGlobalSwitch", &c->pGlobalSwitch);
                v->write("pFreezeSwitch", &c->pFreezeSwitch);
                v->write("pSoloSwitch", &c->pSoloSwitch);
                v->write("pMuteSwitch", &c->pMuteSwitch);

                v->write("pStream", &c->pStream);
            }
            v->end_object();
        }
        v->end_array();

        v->write("nSampleRate", nSampleRate);
        v->write("pData", pData);

        v->write("pStrobeHistSize", pStrobeHistSize);
        v->write("pXYRecordTime", pXYRecordTime);

        v->write("pChannelSelector", pChannelSelector);

        v->write("pOvsMode", pOvsMode);
        v->write("pScpMode", pScpMode);
        v->write("pCoupling_x", pCoupling_x);
        v->write("pCoupling_y", pCoupling_y);
        v->write("pCoupling_ext", pCoupling_ext);

        v->write("pSweepType", pSweepType);
        v->write("pHorDiv", pHorDiv);
        v->write("pHorPos", pHorPos);

        v->write("pVerDiv", pVerDiv);
        v->write("pVerPos", pVerPos);

        v->write("pTrgHys", pTrgHys);
        v->write("pTrgLev", pTrgLev);
        v->write("pTrgHold", pTrgHold);
        v->write("pTrgMode", pTrgMode);
        v->write("pTrgType", pTrgType);
        v->write("pTrgInput", pTrgInput);
        v->write("pTrgReset", pTrgReset);
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

    oscilloscope_x4::oscilloscope_x4(): oscilloscope_base(metadata, 4)
    {
    }

    oscilloscope_x4::~oscilloscope_x4()
    {
    }
}
