/*
 * latency_meter.cpp
 *
 *  Created on: 11 Jun 2017
 *      Author: crocoduck
 */

#include <core/types.h>
#include <core/debug.h>
#include <plugins/latency_meter.h>

#define TMP_BUF_SIZE    1024

namespace lsp
{
    latency_meter::latency_meter(): plugin_t(metadata)
    {
        bBypass         = true;
        bTrigger        = false;
        bFeedback       = false;
        fInGain         = 0.0f;
        fOutGain        = 0.0f;

        vBuffer         = NULL;
        pData           = NULL;

        pIn             = NULL;
        pOut            = NULL;
        pBypass         = NULL;
        pMaxLatency     = NULL;
        pPeakThreshold  = NULL;
        pAbsThreshold   = NULL;
        pInputGain      = NULL;
        pOutputGain     = NULL;
        pTrigger        = NULL;
        pLatencyScreen  = NULL;
        pLevel          = NULL;
        pFeedback       = NULL;
    }

    latency_meter::~latency_meter()
    {
    }

    void latency_meter::destroy()
    {
        if (pData != NULL)
        {
            delete [] pData;
            pData = NULL;
        }
        vBuffer     = NULL;
    }

    void latency_meter::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        size_t samples  = TMP_BUF_SIZE;
        pData           = new uint8_t[samples * sizeof(float) + DEFAULT_ALIGN];

        uint8_t *ptr    = ALIGN_PTR(pData, DEFAULT_ALIGN);
        vBuffer         = reinterpret_cast<float *>(ptr);
        ptr            += TMP_BUF_SIZE * sizeof(float);

        lsp_assert(reinterpret_cast<uint8_t *>(ptr) <= &pData[samples * sizeof(float) + DEFAULT_ALIGN]);

        size_t port_id = 0;
        pIn             = vPorts[port_id++];
        pOut            = vPorts[port_id++];
        pBypass         = vPorts[port_id++];
        pMaxLatency     = vPorts[port_id++];
        pPeakThreshold  = vPorts[port_id++];
        pAbsThreshold   = vPorts[port_id++];
        pInputGain      = vPorts[port_id++];
        pFeedback       = vPorts[port_id++];
        pOutputGain     = vPorts[port_id++];
        pTrigger        = vPorts[port_id++];
        pLatencyScreen  = vPorts[port_id++];
        pLevel          = vPorts[port_id++];

        sLatencyDetector.init();

        sLatencyDetector.set_delay_ratio(0.5f);
        sLatencyDetector.set_duration(0.050f);
        sLatencyDetector.set_op_fading(0.030f);
        sLatencyDetector.set_op_pause(0.025f);
    }

    void latency_meter::update_sample_rate(long sr)
    {
        sLatencyDetector.set_sample_rate(sr);
        sBypass.init(sr);
    }

    void latency_meter::process(size_t samples)
    {
        float *in = pIn->getBuffer<float>();
        if (in == NULL)
            return;

        // Calculate the level
        pLevel->setValue(dsp::abs_max(in, samples));

        float *out = pOut->getBuffer<float>();
        if (out == NULL)
            return;

        while (samples > 0)
        {
            size_t to_do = (samples > TMP_BUF_SIZE) ? TMP_BUF_SIZE : samples;

            dsp::mul_k3(vBuffer, in, fInGain, to_do);

            sLatencyDetector.process_in(vBuffer, vBuffer, to_do);
            if (!bFeedback)
                dsp::fill_zero(vBuffer, to_do);
            sLatencyDetector.process_out(vBuffer, vBuffer, to_do);

            dsp::mul_k2(vBuffer, fOutGain, to_do);
            sBypass.process(out, in, vBuffer, to_do);

            in         += to_do;
            out        += to_do;
            samples    -= to_do;
        }

        if (sLatencyDetector.latency_detected())
            pLatencyScreen->setValue(sLatencyDetector.get_latency_seconds() * 1000.0f); // * 1000.0f to show ms instead of s
    }

    void latency_meter::update_settings()
    {
        bBypass = pBypass->getValue() >= 0.5f;
        sBypass.set_bypass(bBypass);

        bTrigger    = pTrigger->getValue() >= 0.5f;
        bFeedback   = pFeedback->getValue() >= 0.5f;

        if (bTrigger)
        {
            sLatencyDetector.start_capture();
            pLatencyScreen->setValue(0.0f); // Showing 0 if no latency was detected. Can we have something like ----.--- instead?
        }

        sLatencyDetector.set_ip_detection(pMaxLatency->getValue() / 1000.0f);
        sLatencyDetector.set_peak_threshold(pPeakThreshold->getValue());
        sLatencyDetector.set_abs_threshold(pAbsThreshold->getValue());
        fInGain     = pInputGain->getValue();
        fOutGain    = pOutputGain->getValue();

        if (sLatencyDetector.needs_update())
            sLatencyDetector.update_settings();
    }
}
