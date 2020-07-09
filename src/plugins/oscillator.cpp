/*
 * oscillator.cpp
 *
 *  Created on: 20 Mar 2017
 *      Author: crocoduck
 */

#include <plugins/oscillator.h>
#include <core/types.h>

#include <core/debug.h>
#include <core/colors.h>
#include <core/util/Color.h>

// Maximum size of temporary buffer in samples
#define TMP_BUF_SIZE        1024

namespace lsp
{
    oscillator_mono::oscillator_mono(): plugin_t(metadata)
    {
        nMode                   = 0;
        bMeshSync               = false;
        bBypass                 = false;

        vBuffer                 = NULL;
        vTime                   = NULL;
        vDisplaySamples         = NULL;
        pData                   = NULL;
        pIDisplay               = NULL;

        pIn                     = NULL;
        pOut                    = NULL;
        pBypass                 = NULL;
        pFrequency              = NULL;
        pGain                   = NULL;
        pDCOffset               = NULL;
        pDCRefSc                = NULL;
        pInitPhase              = NULL;
        pModeSc                 = NULL;
        pOversamplerModeSc      = NULL;
        pFuncSc                 = NULL;
        pSquaredSinusoidInv     = NULL;
        pParabolicInv           = NULL;
        pRectangularDutyRatio   = NULL;
        pSawtoothWidth          = NULL;
        pTrapezoidRaiseRatio    = NULL;
        pTrapezoidFallRatio     = NULL;
        pPulsePosWidthRatio     = NULL;
        pPulseNegWidthRatio     = NULL;
        pParabolicWidth         = NULL;
        pOutputMesh             = NULL;
    }

    oscillator_mono::~oscillator_mono()
    {
    }

    void oscillator_mono::destroy()
    {
        if (pData != NULL)
        {
            delete [] pData;
            pData = NULL;
        }
        vBuffer             = NULL;
        vTime               = NULL;
        vDisplaySamples     = NULL;

        if (pIDisplay != NULL)
        {
            pIDisplay->detroy();
            pIDisplay = NULL;
        }
    }

    fg_function_t oscillator_mono::get_function(size_t function)
    {
        switch (function)
        {
            case oscillator_mono_metadata::SC_FUNC_SINE:
                return FG_SINE;
            case oscillator_mono_metadata::SC_FUNC_COSINE:
                return FG_COSINE;
            case oscillator_mono_metadata::SC_FUNC_SQUARED_SINE:
                return FG_SQUARED_SINE;
            case oscillator_mono_metadata::SC_FUNC_SQUARED_COSINE:
                return FG_SQUARED_COSINE;
            case oscillator_mono_metadata::SC_FUNC_RECTANGULAR:
                return FG_RECTANGULAR;
            case oscillator_mono_metadata::SC_FUNC_SAWTOOTH:
                return FG_SAWTOOTH;
            case oscillator_mono_metadata::SC_FUNC_TRAPEZOID:
                return FG_TRAPEZOID;
            case oscillator_mono_metadata::SC_FUNC_PULSETRAIN:
                return FG_PULSETRAIN;
            case oscillator_mono_metadata::SC_FUNC_PARABOLIC:
                return FG_PARABOLIC;
            case oscillator_mono_metadata::SC_FUNC_BL_RECTANGULAR:
                return FG_BL_RECTANGULAR;
            case oscillator_mono_metadata::SC_FUNC_BL_SAWTOOTH:
                return FG_BL_SAWTOOTH;
            case oscillator_mono_metadata::SC_FUNC_BL_TRAPEZOID:
                return FG_BL_TRAPEZOID;
            case oscillator_mono_metadata::SC_FUNC_BL_PULSETRAIN:
                return FG_BL_PULSETRAIN;
            case oscillator_mono_metadata::SC_FUNC_BL_PARABOLIC:
                return FG_BL_PARABOLIC;

            default:
                return FG_SINE;
        }
        return FG_SINE;
    }

    dc_reference_t oscillator_mono::get_dc_reference(size_t reference)
    {
        switch (reference)
        {
            case oscillator_mono_metadata::SC_DC_WAVEDC:
                return DC_WAVEDC;
            case oscillator_mono_metadata::SC_DC_ZERO:
                return DC_ZERO;
            default:
                return DC_WAVEDC;
        }
    }

    over_mode_t oscillator_mono::get_oversampling_mode(size_t mode)
    {
        switch (mode)
        {
            case oscillator_mono_metadata::SC_OVS_2X:
                return OM_LANCZOS_2X2;
            case oscillator_mono_metadata::SC_OVS_3X:
                return OM_LANCZOS_3X2;
            case oscillator_mono_metadata::SC_OVS_4X:
                return OM_LANCZOS_4X2;
            case oscillator_mono_metadata::SC_OVS_6X:
                return OM_LANCZOS_6X2;
            case oscillator_mono_metadata::SC_OVS_8X:
                return OM_LANCZOS_8X2;

            case oscillator_mono_metadata::SC_OVS_NONE:
            default:
                return OM_NONE;
        }
        return OM_NONE;
    }

    void oscillator_mono::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        size_t samples  = TMP_BUF_SIZE + oscillator_mono_metadata::HISTORY_MESH_SIZE * 2;
        pData           = new uint8_t[samples * sizeof(float) + DEFAULT_ALIGN];

        uint8_t *ptr    = ALIGN_PTR(pData, DEFAULT_ALIGN);
        vBuffer         = reinterpret_cast<float *>(ptr);
        ptr            += TMP_BUF_SIZE * sizeof(float);
        vTime           = reinterpret_cast<float *>(ptr);
        ptr            += oscillator_mono_metadata::HISTORY_MESH_SIZE * sizeof(float);
        vDisplaySamples = reinterpret_cast<float *>(ptr);
        ptr            += oscillator_mono_metadata::HISTORY_MESH_SIZE * sizeof(float);

        lsp_assert(reinterpret_cast<uint8_t *>(ptr) <= &pData[samples * sizeof(float) + DEFAULT_ALIGN]);

        // Make time scale normalized between 0 and 2
        for (size_t n = 0; n < oscillator_mono_metadata::HISTORY_MESH_SIZE; ++n)
            vTime[n] = float(2 * n) / oscillator_mono_metadata::HISTORY_MESH_SIZE;

        size_t port_id = 0;
        pIn                     = vPorts[port_id++];
        pOut                    = vPorts[port_id++];
        pBypass                 = vPorts[port_id++];
        pFrequency              = vPorts[port_id++];
        pGain                   = vPorts[port_id++];
        pDCOffset               = vPorts[port_id++];
        pDCRefSc                = vPorts[port_id++];
        pInitPhase              = vPorts[port_id++];
        pModeSc                 = vPorts[port_id++];
        pOversamplerModeSc      = vPorts[port_id++];
        pFuncSc                 = vPorts[port_id++];
        pSquaredSinusoidInv     = vPorts[port_id++];
        pParabolicInv           = vPorts[port_id++];
        pRectangularDutyRatio   = vPorts[port_id++];
        pSawtoothWidth          = vPorts[port_id++];
        pTrapezoidRaiseRatio    = vPorts[port_id++];
        pTrapezoidFallRatio     = vPorts[port_id++];
        pPulsePosWidthRatio     = vPorts[port_id++];
        pPulseNegWidthRatio     = vPorts[port_id++];
        pParabolicWidth         = vPorts[port_id++];
        pOutputMesh             = vPorts[port_id++];

        sOsc.init();
    }

    void oscillator_mono::update_sample_rate(long sr)
    {
        sOsc.set_sample_rate(sr);
        sBypass.init(sr);
    }

    void oscillator_mono::ui_activated()
    {
        // Mark mesh needs to be synchronized
        bMeshSync               = true;
    }

    void oscillator_mono::process(size_t samples)
    {
        float *in = pIn->getBuffer<float>();
        if (in == NULL)
            return;

        float *out = pOut->getBuffer<float>();
        if (out == NULL)
            return;

        switch (nMode)
        {
            case oscillator_mono_metadata::SC_MODE_ADD:
            {
                while (samples > 0)
                {
                    size_t to_do = (samples > TMP_BUF_SIZE) ? TMP_BUF_SIZE : samples;

                    sOsc.process_add(vBuffer, in, to_do);

                    sBypass.process(out, in, vBuffer, to_do);

                    in         += to_do;
                    out        += to_do;
                    samples    -= to_do;
                }
            }
            break;

            case oscillator_mono_metadata::SC_MODE_MUL:
            {
                while (samples > 0)
                {
                    size_t to_do = (samples > TMP_BUF_SIZE) ? TMP_BUF_SIZE : samples;

                    sOsc.process_mul(vBuffer, in, to_do);

                    sBypass.process(out, in, vBuffer, to_do);

                    in         += to_do;
                    out        += to_do;
                    samples    -= to_do;
                }
            }
            break;

            case oscillator_mono_metadata::SC_MODE_REP:
            {
                while (samples > 0)
                {
                    size_t to_do = (samples > TMP_BUF_SIZE) ? TMP_BUF_SIZE : samples;

                    sOsc.process_overwrite(vBuffer, to_do);

                    sBypass.process(out, in, vBuffer, to_do);

                    in         += to_do;
                    out        += to_do;
                    samples    -= to_do;
                }
            }
            break;

        }

        if (bMeshSync)
        {
            mesh_t *mesh    = pOutputMesh->getBuffer<mesh_t>();

            if ((mesh != NULL) && (mesh->isEmpty()))
            {
                dsp::copy(mesh->pvData[0], vTime, oscillator_mono_metadata::HISTORY_MESH_SIZE);
                dsp::copy(mesh->pvData[1], vDisplaySamples, oscillator_mono_metadata::HISTORY_MESH_SIZE);
                mesh->data(2, oscillator_mono_metadata::HISTORY_MESH_SIZE);

                bMeshSync   = false;
            }
        }
    }

    void oscillator_mono::update_settings()
    {
        nMode = pModeSc->getValue();

        bBypass = pBypass->getValue() >= 0.5f;
        sBypass.set_bypass(bBypass);

        sOsc.set_parabolic_width(pParabolicWidth->getValue() / 100);
        sOsc.set_pulsetrain_ratios(pPulsePosWidthRatio->getValue() / 100, pPulseNegWidthRatio->getValue() / 100);
        sOsc.set_trapezoid_ratios(pTrapezoidRaiseRatio->getValue() / 100, pTrapezoidFallRatio->getValue() / 100);
        sOsc.set_width(pSawtoothWidth->getValue() / 100);
        sOsc.set_duty_ratio(pRectangularDutyRatio->getValue() / 100);
        sOsc.set_oversampler_mode(get_oversampling_mode(pOversamplerModeSc->getValue()));
        sOsc.set_function(get_function(pFuncSc->getValue()));
        sOsc.set_squared_sinusoid_inversion(pSquaredSinusoidInv->getValue() >= 0.5f);
        sOsc.set_parabolic_inversion(pParabolicInv->getValue() >= 0.5f);
        sOsc.set_phase(pInitPhase->getValue() * M_PI / 180.0f);
        sOsc.set_dc_reference(get_dc_reference(pDCRefSc->getValue()));
        sOsc.set_dc_offset(pDCOffset->getValue());
        sOsc.set_frequency(pFrequency->getValue());
        sOsc.set_amplitude(pGain->getValue());

        if (sOsc.needs_update())
        {
            sOsc.update_settings();
            bMeshSync   = true;
        }

        sOsc.get_periods(vDisplaySamples, 2, 10, oscillator_mono_metadata::HISTORY_MESH_SIZE);

        // Always query drawing on settings update
        if (pWrapper != NULL)
            pWrapper->query_display_draw();
    }

    bool oscillator_mono::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        // Check proportions
        if (height > (R_GOLDEN_RATIO * width))
            height  = R_GOLDEN_RATIO * width;

        // Init canvas
        if (!cv->init(width, height))
            return false;
        width       = cv->width();
        height      = cv->height();
        ssize_t cx  = width >> 1;
        ssize_t dx  = width >> 2;
        ssize_t cy  = height >> 1;
        ssize_t dy  = height >> 2;

        // Clear background
        cv->set_color_rgb((bBypass) ? CV_DISABLED : CV_BACKGROUND);
        cv->paint();

        // Draw axis
        cv->set_line_width(1.0);
        cv->set_color_rgb((bBypass) ? CV_SILVER : CV_YELLOW, 0.5f);
        for (size_t i=1; i<4; i += 2)
        {
            cv->line(i*dx, 0, i*dx, height);
            cv->line(0, i*dy, width, i*dy);
        }

        cv->set_color_rgb(CV_WHITE, 0.5f);
        cv->line(cx, 0, cx, height);
        cv->line(0, cy, width, cy);

        // Allocate buffer: t, f(t)
        pIDisplay           = float_buffer_t::reuse(pIDisplay, 2, width);
        float_buffer_t *b   = pIDisplay;
        if (b == NULL)
            return false;

        float ni = float(oscillator_mono_metadata::HISTORY_MESH_SIZE) / width; // Normalizing index

        for (size_t j = 0; j < width; ++j)
        {
            // Top left corner has coordinates (0; 0)
            // Bottom right corner has coordinates (width-1; height-1)
            size_t k        = j*ni;
            b->v[0][j]      = j;
            b->v[1][j]      = cy - dy * vDisplaySamples[k];
        }

        // Set colour and draw
        cv->set_color_rgb((bBypass) ? CV_SILVER : CV_MESH);
        cv->set_line_width(2);
        cv->draw_lines(b->v[0], b->v[1], width);

        return true;

    }

    void oscillator_mono::dump(IStateDumper *v) const
    {
        v->write_object("sOsc", &sOsc);
        v->write_object("sBypass", &sBypass);

        v->write("nMode", nMode);
        v->write("bMeshSync", bMeshSync);
        v->write("bBypass", bBypass);
        v->write("vBuffer", vBuffer);
        v->write("vTime", vTime);
        v->write("vDisplaySamples", vDisplaySamples);
        v->write("pData", pData);
        v->write("pIDisplay", pIDisplay);

        v->write("pIn", pIn);
        v->write("pOut", pOut);
        v->write("pBypass", pBypass);
        v->write("pFrequency", pFrequency);
        v->write("pGain", pGain);
        v->write("pDCOffset", pDCOffset);
        v->write("pDCRefSc", pDCRefSc);
        v->write("pInitPhase", pInitPhase);
        v->write("pModeSc", pModeSc);
        v->write("pOversamplerModeSc", pOversamplerModeSc);
        v->write("pFuncSc", pFuncSc);
        v->write("pSquaredSinusoidInv", pSquaredSinusoidInv);
        v->write("pParabolicInv", pParabolicInv);
        v->write("pRectangularDutyRatio", pRectangularDutyRatio);
        v->write("pSawtoothWidth", pSawtoothWidth);
        v->write("pTrapezoidRaiseRatio", pTrapezoidRaiseRatio);
        v->write("pTrapezoidFallRatio", pTrapezoidFallRatio);
        v->write("pPulsePosWidthRatio", pPulsePosWidthRatio);
        v->write("pPulseNegWidthRatio", pPulseNegWidthRatio);
        v->write("pParabolicWidth", pParabolicWidth);
        v->write("pOutputMesh", pOutputMesh);
    }
}
