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
#define N_HOR_DIVISIONS     4
#define N_VER_DIVISIONS     2
#define VER_FULL_SCALE_AMP  1.0f
#define SWEEP_GEN_N_BITS    32
#define SWEEP_GEN_PEAK      1.0f

namespace lsp
{
    over_mode_t oscilloscope_base::get_oversampler_mode(size_t portValue)
    {
        switch (portValue)
        {
            case oscilloscope_base_metadata::OSC_OVS_NONE:
                return OM_NONE;
            case oscilloscope_base_metadata::OSC_OVS_2X:
                return OM_LANCZOS_2X2;
            case oscilloscope_base_metadata::OSC_OVS_3X:
                return OM_LANCZOS_3X2;
            case oscilloscope_base_metadata::OSC_OVS_4X:
                return OM_LANCZOS_4X2;
            case oscilloscope_base_metadata::OSC_OVS_6X:
                return OM_LANCZOS_6X2;
            case oscilloscope_base_metadata::OSC_OVS_8X:
            default:
                return OM_LANCZOS_8X2;
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

    trg_type_t oscilloscope_base::get_trigger_type(size_t portValue)
    {
        switch (portValue)
        {
            case oscilloscope_base_metadata::TRIGGER_TYPE_RISING_EDGE:
                return TRG_TYPE_SIMPLE_RISING_EDGE;
            case oscilloscope_base_metadata::TRIGGER_TYPE_FALLING_EDGE:
                return TRG_TYPE_SIMPLE_FALLING_EDGE;
            default:
                return TRG_TYPE_NONE;
        }
    }

    void oscilloscope_base::reset_display_buffers(channel_t *c)
    {
        // fill_zero is for DEBUG
        dsp::fill_zero(c->vDisplay_x, BUF_LIM_SIZE);
        dsp::fill_zero(c->vDisplay_y, BUF_LIM_SIZE);

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
                c->sSweepGenerator.set_amplitude(0.5f * SWEEP_GEN_PEAK);
                c->sSweepGenerator.set_dc_offset(0.5f * SWEEP_GEN_PEAK);
                c->sSweepGenerator.set_width(0.5f);
            }
            break;

            case CH_SWEEP_TYPE_SINE:
            {
                c->sSweepGenerator.set_function(FG_SINE);
                c->sSweepGenerator.set_dc_reference(DC_WAVEDC);
                c->sSweepGenerator.set_amplitude(0.5f * SWEEP_GEN_PEAK);
                c->sSweepGenerator.set_dc_offset(0.5f * SWEEP_GEN_PEAK);
            }
            break;

            case CH_SWEEP_TYPE_SAWTOOTH:
            default:
            {
                c->sSweepGenerator.set_function(FG_SAWTOOTH);
                c->sSweepGenerator.set_dc_reference(DC_WAVEDC);
                c->sSweepGenerator.set_amplitude(0.5f * SWEEP_GEN_PEAK);
                c->sSweepGenerator.set_dc_offset(0.5f * SWEEP_GEN_PEAK);
                c->sSweepGenerator.set_width(1.0f);
            }
            break;
        }

        c->sSweepGenerator.update_settings();
    }

    void oscilloscope_base::configure_oversamplers(channel_t *c)
    {
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

                c->sOversampler_x.destroy();
                c->sOversampler_y.destroy();
                c->sOversampler_ext.destroy();

                c->sPreTrgDelay.destroy();

                c->sSweepGenerator.destroy();

                c->vData_x = NULL;
                c->vData_y = NULL;
                c->vData_ext = NULL;
                c->vData_y_delay = NULL;
                c->vDisplay_x = NULL;
                c->vDisplay_y = NULL;
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

        // For each channel: 1X external data buffer + 1X x data buffer + 1X y data buffer + 1X delayed y data buffer + 1X x display buffer + 1X y display buffer
        size_t samples = nChannels * BUF_LIM_SIZE * 6;

        float *ptr = alloc_aligned<float>(pData, samples);
        if (ptr == NULL)
            return;

        lsp_guard_assert(float *save = ptr);

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            c->enMode           = CH_MODE_DFL;
            c->enSweepType      = CH_SWEEP_TYPE_DFL;
            c->enTrgInput       = CH_TRG_INPUT_DFL;

            if (!c->sOversampler_x.init())
                return;

            if (!c->sOversampler_y.init())
                return;

            if (!c->sOversampler_ext.init())
                return;

            if (!c->sPreTrgDelay.init(PRE_TRG_MAX_SIZE))
                return;

            // Test settings for oversampler before proper implementation
            c->enOverMode = OM_LANCZOS_8X3;
            configure_oversamplers(c);

            // Settings for the Sweep Generator
            c->sSweepGenerator.init();
            c->sSweepGenerator.set_phase_accumulator_bits(SWEEP_GEN_N_BITS);
            c->sSweepGenerator.set_phase(0.0f);
            c->sSweepGenerator.update_settings();

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

            c->enState          = CH_STATE_LISTENING;

            c->vIn_x            = NULL;
            c->vIn_y            = NULL;
            c->vIn_ext          = NULL;

            c->vOut_x           = NULL;
            c->vOut_y           = NULL;

            c->pIn_x            = NULL;
            c->pIn_y            = NULL;
            c->pIn_ext          = NULL;

            c->pOut_x           = NULL;
            c->pOut_y           = NULL;

            c->pHorDiv          = NULL;
            c->pHorPos          = NULL;

            c->pVerDiv          = NULL;
            c->pVerPos          = NULL;

            c->pTrgHys          = NULL;
            c->pTrgLev          = NULL;
            c->pTrgMode         = NULL;
            c->pTrgType         = NULL;

            c->pCoupling        = NULL;

            c->pMesh            = NULL;
        }

        lsp_assert(ptr <= &save[samples]);

        // Bind ports
        size_t port_id = 0;

        // Audio
        lsp_trace("Binding audio ports");

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pIn_x     = vPorts[port_id++];
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pIn_y     = vPorts[port_id++];
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pIn_ext   = vPorts[port_id++];
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pOut_x    = vPorts[port_id++];
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pOut_y    = vPorts[port_id++];
        }

        // Common
        lsp_trace("Binding common ports");

        // Channels
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pOvsMode = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pScpMode = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pCoupling = vPorts[port_id++];

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
            vChannels[ch].pTrgMode = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pTrgType = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pTrgInput = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            vChannels[ch].pMesh = vPorts[port_id++];
        }
    }

    void oscilloscope_base::update_settings()
    {
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            c->enOverMode = get_oversampler_mode(c->pOvsMode->getValue());
            configure_oversamplers(c);

            c->enMode = get_scope_mode(c->pScpMode->getValue());

            c->enTrgInput = get_trigger_input(c->pTrgInput->getValue());

            float verDiv = c->pVerDiv->getValue();
            float verPos = c->pVerPos->getValue();

            c->fScale = VER_FULL_SCALE_AMP / (verDiv * N_VER_DIVISIONS);
            c->fOffset = 0.01f * verPos * N_VER_DIVISIONS * verDiv;

            float horDiv = c->pHorDiv->getValue();
            float horPos = c->pHorPos->getValue();

            c->nSweepSize = N_HOR_DIVISIONS * seconds_to_samples(c->nOverSampleRate, horDiv);
            c->nSweepSize = (c->nSweepSize < BUF_LIM_SIZE) ? c->nSweepSize  : BUF_LIM_SIZE;
            c->enSweepType = get_sweep_type(c->pSweepType->getValue());
            set_sweep_generator(c);

            c->nPreTrigger = (0.01f * horPos  + 1) * (c->nSweepSize - 1) / 2;
            c->sPreTrgDelay.set_delay(c->nPreTrigger);
            c->sPreTrgDelay.clear();

            float trgLevel = c->pTrgLev->getValue();

            c->sTrigger.set_trigger_type(get_trigger_type(c->pTrgType->getValue()));
            c->sTrigger.set_trigger_threshold(0.01f * trgLevel * N_VER_DIVISIONS * verDiv);
            c->sTrigger.set_trigger_hold_samples(c->nSweepSize);
            c->sTrigger.update_settings();
        }
    }

    void oscilloscope_base::update_sample_rate(long sr)
    {
        nSampleRate = sr;

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
        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            c->vIn_x = c->pIn_x->getBuffer<float>();
            c->vIn_y = c->pIn_y->getBuffer<float>();
            c->vIn_ext = c->pIn_ext->getBuffer<float>();

            c->vOut_x = c->pOut_x->getBuffer<float>();
            c->vOut_y = c->pOut_y->getBuffer<float>();

            if ((c->vIn_x == NULL) || (c->vOut_x == NULL))
                return;

            if ((c->vIn_y == NULL) || (c->vOut_y == NULL))
                return;

            if ((c->vIn_ext == NULL))
                return;

            c->nSamplesCounter = samples;
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

            dsp::copy(c->vOut_x, c->vIn_x, samples);
            dsp::copy(c->vOut_y, c->vIn_y, samples);
        }

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            channel_t *c = &vChannels[ch];

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
                        c->sOversampler_x.upsample(c->vData_x, c->vIn_x, to_do);
                        c->sOversampler_y.upsample(c->vData_y, c->vIn_y, to_do);

                        size_t remaining = c->nSweepSize - c->nDisplayHead;
                        size_t to_copy = (to_do_upsample < remaining) ? to_do_upsample : remaining;

                        dsp::copy(&c->vDisplay_x[c->nDisplayHead], c->vData_x, to_copy);
                        dsp::copy(&c->vDisplay_y[c->nDisplayHead], c->vData_y, to_copy);

                        c->nDisplayHead += to_copy;

                        if (c->nDisplayHead >= c->nSweepSize)
                        {
                            // Plot stuff happens here

                            reset_display_buffers(c);
                        }
                    }
                    break;

                    case CH_MODE_TRIGGERED:
                    {
                        c->sOversampler_y.upsample(c->vData_y, c->vIn_y, to_do);
                        c->sPreTrgDelay.process(c->vData_y_delay, c->vData_y, to_do_upsample);
                        c->sOversampler_ext.upsample(c->vData_ext, c->vIn_ext, to_do);
                        c->nDataHead = 0;

                        float *trg_input = select_trigger_input(c->vData_ext, c->vData_y, c->enTrgInput);

                        for (size_t n = 0; n < to_do_upsample; ++n)
                        {
                            c->sTrigger.single_sample_processor(trg_input[n]);

                            switch (c->enState)
                            {
                                case CH_STATE_LISTENING:
                                {
                                    if (c->sTrigger.get_trigger_state() == TRG_STATE_FIRED)
                                    {
                                        c->sSweepGenerator.reset_phase_accumulator();
                                        c->nDataHead = n;
                                        c->enState = CH_STATE_SWEEPING;
                                    }
                                }
                                break;

                                case CH_STATE_SWEEPING:
                                {
                                    c->sSweepGenerator.process_overwrite(&c->vDisplay_x[c->nDisplayHead], 1);
                                    c->vDisplay_y[c->nDisplayHead] = c->vData_y_delay[c->nDataHead];
                                    ++c->nDataHead;
                                    ++c->nDisplayHead;

                                    if (c->nDisplayHead >= c->nSweepSize)
                                    {
                                        // Plot stuff happens here

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
}
