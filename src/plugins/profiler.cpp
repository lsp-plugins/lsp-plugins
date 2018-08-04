/*
 * profiler.cpp
 *
 *  Created on: 30 Sep 2017
 *      Author: crocoduck
 */

#include <plugins/profiler.h>
#include <core/debug.h>

#define TMP_BUF_SIZE                1024
#define BGNOISE_INTERVAL_FACTOR     2.0f    // Multiply reactivity by this to calculate the time to spend in BGNOISEPROFILING state
#define POSTPROCESSOR_REACTIVITY    0.085   // Window Size for the Post Processor IR envelope follower [s]
#define POSTPROCESSOR_TOLERANCE     3.0     // Maximum level above the noise floor below which, if IR peaks are not found, the IR is considered concluded [dB]
#define SYNC_CHIRP_START_FREQ       1.0     // Synchronized Chirp Starting frequency [Hz]
#define SYNC_CHIRP_MAX_FREQ         23000.0 // Synchronized Chirp Final frequency [Hz]

namespace lsp
{
    profiler_mono::PreProcessor::PreProcessor(profiler_mono *base)
    {
        pCore           = base;
    }

    profiler_mono::PreProcessor::~PreProcessor()
    {
        pCore           = NULL;
    }

    int profiler_mono::PreProcessor::run()
    {
        status_t returnValue;

        // reconfigure() will call update_settings() if needed.
        returnValue     = pCore->sSyncChirpProcessor.reconfigure();

        if (returnValue != STATUS_OK)
            return returnValue;

        pCore->pActualDuration->setValue(pCore->sSyncChirpProcessor.get_chirp_duration_seconds());
        pCore->sResponseTaker.set_op_tail(pCore->sSyncChirpProcessor.get_chirp_duration_seconds());

        returnValue     = pCore->sResponseTaker.reconfigure(pCore->sSyncChirpProcessor.get_chirp());

        return returnValue;
    }

    profiler_mono::Convolver::Convolver(profiler_mono *base)
    {
        pCore           = base;
    }

    profiler_mono::Convolver::~Convolver()
    {
        pCore           = NULL;
    }

    int profiler_mono::Convolver::run()
    {
        return pCore->sSyncChirpProcessor.do_linear_convolution(pCore->sResponseTaker.get_capture(), pCore->sResponseTaker.get_capture_start());
    }

    profiler_mono::PostProcessor::PostProcessor(profiler_mono *base)
    {
        pCore           = base;
    }

    profiler_mono::PostProcessor::~PostProcessor()
    {
        pCore           = NULL;
    }

    int profiler_mono::PostProcessor::run()
    {
        status_t returnValue;

        ssize_t nIROffset = seconds_to_samples(pCore->nSampleRate, pCore->pIROffset->getValue() / 1000.0f);

        returnValue     = pCore->sSyncChirpProcessor.postprocess_linear_convolution(nIROffset, pCore->enRtAlgo, POSTPROCESSOR_REACTIVITY, POSTPROCESSOR_TOLERANCE);

        if (returnValue != STATUS_OK)
            return returnValue;

        pCore->pRTScreen->setValue(pCore->sSyncChirpProcessor.get_reverberation_time_seconds());
        pCore->pRScreen->setValue(pCore->sSyncChirpProcessor.get_reverberation_correlation());
        pCore->pILScreen->setValue(pCore->sSyncChirpProcessor.get_integration_limit_seconds());
        pCore->pRTAccuracyLed->setValue(pCore->sSyncChirpProcessor.get_background_noise_suitability());

        // Do the plot - PROTOTYPE
        for (size_t n = 0; n < profiler_mono_metadata::RESULT_MESH_SIZE; ++n)
            pCore->vDisplayAbscissa[n] = float(2 * n) / profiler_mono_metadata::RESULT_MESH_SIZE;

        size_t irQuery = (nIROffset > 0) ? pCore->sSyncChirpProcessor.get_reverberation_time_samples() : pCore->sSyncChirpProcessor.get_reverberation_time_samples() + size_t(-nIROffset);

        pCore->sSyncChirpProcessor.get_convolution_result_plottable_samples(pCore->vDisplayOrdinate, nIROffset, irQuery , profiler_mono_metadata::RESULT_MESH_SIZE, true);

        mesh_t *mesh    = pCore->pResultMesh->getBuffer<mesh_t>();

        if ((mesh != NULL) && (mesh->isEmpty()))
        {
            dsp::copy(mesh->pvData[0], pCore->vDisplayAbscissa, profiler_mono_metadata::RESULT_MESH_SIZE);
            dsp::copy(mesh->pvData[1], pCore->vDisplayOrdinate, profiler_mono_metadata::RESULT_MESH_SIZE);
            mesh->data(2, profiler_mono_metadata::RESULT_MESH_SIZE);
        }

        if (pCore->pWrapper != NULL)
            pCore->pWrapper->query_display_draw();

        return returnValue;
    }

    profiler_mono::Saver::Saver(profiler_mono *base)
    {
        pCore               = base;
    }

    profiler_mono::Saver::~Saver()
    {
        pCore               = NULL;
    }

    int profiler_mono::Saver::run()
    {
        // Doing Checks:
        if (pCore->bIRMeasured)
        {
            pCore->pIRSaveStatus->setValue(STATUS_LOADING);
            pCore->pIRSavePercent->setValue(0.0f);
        }
        else
        {
            pCore->pIRSaveStatus->setValue(STATUS_NO_DATA);
            pCore->pIRSavePercent->setValue(0.0f);
            return STATUS_NO_DATA;
        }

        // Calculate saving time. Taking mode from GUI here, so that the combo
        // item is relevant.

        float fRT               = pCore->sSyncChirpProcessor.get_reverberation_time_seconds();
        float fIL               = pCore->sSyncChirpProcessor.get_integration_limit_seconds();
        float fAll              = pCore->sSyncChirpProcessor.get_convolution_result_positive_time_length();

        float saveTime          = (fRT > fIL) ? fRT : fIL;
        bool doNlinearSave      = false;

        switch (pCore->nSaveMode)
        {
            case profiler_mono_metadata::SC_SVMODE_RT:
                saveTime        = fRT;
                break;
            case profiler_mono_metadata::SC_SVMODE_IT:
                saveTime        = fIL;
                break;
            case profiler_mono_metadata::SC_SVMODE_ALL:
                saveTime        = fAll;
                break;
            case profiler_mono_metadata::SC_SVMOD_NLINEAR:
                doNlinearSave   = true;
                break;
            case profiler_mono_metadata::SC_SVMODE_AUTO:
            default:
                break;
        }

        saveTime                = ceilf(saveTime * 10.0f) / 10.0f; // Round to the next tenth of second
        size_t saveCount        = seconds_to_samples(pCore->nSampleRate, saveTime); // This count is relative to the middle of the convolution result

        // Saving Data:
        path_t *path = pCore->pIRFileName->getBuffer<path_t>();
        if (path->accepted())
            path->commit();

        pCore->bFileSet         = false;

        status_t returnValue;

        ssize_t nIROffset = millis_to_samples(pCore->nSampleRate, pCore->pIROffset->getValue());

        // Update saveCount to account for offset
        if (nIROffset > 0)
            saveCount += size_t(nIROffset);
        else
            saveCount += size_t(-nIROffset);

        if (doNlinearSave)
            returnValue = pCore->sSyncChirpProcessor.save_nonlinear_convolution(path->get_path());
        else
            returnValue = pCore->sSyncChirpProcessor.save_linear_convolution(path->get_path(), nIROffset, saveCount);

        if (returnValue == STATUS_OK)
        {
            pCore->pIRSavePercent->setValue(100.0f);
            pCore->pIRSaveStatus->setValue(STATUS_OK);
        }
        else
        {
            pCore->pIRSavePercent->setValue(0.0f);
            pCore->pIRSaveStatus->setValue(STATUS_UNKNOWN_ERR);
        }

        return returnValue;
    }

    profiler_mono::profiler_mono(): plugin_t(metadata)
    {
        nState                  = IDLE;

        pExecutor               = NULL;
        pPreProcessor           = NULL;
        pConvolver              = NULL;
        pPostProcessor          = NULL;
        pSaver                  = NULL;

        nSampleRate             = 0;
        fLtAmplitude            = 1.0f;
        enRtAlgo                = SCP_RT_T_20;
        nWaitCounter            = 0;
        bDoReset                = false;
        bDoLatencyOnly          = false;
        bLatencyMeasured        = false;
        nLatency                = 0;
        fScpDurationPrevious    = 0.0f;
        bIRMeasured             = false;
        bFileSet                = false;
        nSaveMode               = profiler_mono_metadata::SC_SVMODE_DFL;
        bResetSaver             = false;

        bBypass                 = true;
        bCalibration            = false;
        bSkipLatencyDetection   = false;
        bPostprocess            = false;
        bPostprocessPrevious    = false;
        bLatTrigger             = false;
        bLatTriggerPrevious     = false;
        bLinTrigger             = false;
        bLinTriggerPrevious     = false;
        bFeedback               = false;

        vBuffer                 = NULL;
        vDisplayAbscissa        = NULL;
        vDisplayOrdinate        = NULL;
        pData                   = NULL;

        pIn                     = NULL;
        pOut                    = NULL;

        pBypass                 = NULL;

        pStateLEDs              = NULL;

        pCalFrequency           = NULL;
        pCalAmplitude           = NULL;
        pCalSwitch              = NULL;

        pLdMaxLatency           = NULL;
        pLdPeakThs              = NULL;
        pLdAbsThs               = NULL;
        pLdEnableSwitch         = NULL;

        pDuration               = NULL;
        pActualDuration         = NULL;

        pIROffset               = NULL;

        pLatencyScreen          = NULL;

        pRTAlgoSelector         = NULL;

        pRTScreen               = NULL;
        pRTAccuracyLed          = NULL;

        pILScreen               = NULL;

        pRScreen                = NULL;

        pLevelMeter             = NULL;

        pPostTrigger            = NULL;

        pSaveModeSelector       = NULL;

        pIRFileName             = NULL;
        pIRSaveCmd              = NULL;
        pIRSaveStatus           = NULL;
        pIRSavePercent          = NULL;

        pLatTrigger             = NULL;

        pLinTrigger             = NULL;

        pFeedback               = NULL;

        pResultMesh             = NULL;
    }

    profiler_mono::~profiler_mono()
    {
    }

    void profiler_mono::destroy()
    {
        if (pPreProcessor  != NULL)
        {
            delete pPreProcessor;
            pPreProcessor   = NULL;
        }

        if (pConvolver     != NULL)
        {
            delete pConvolver;
            pConvolver      = NULL;
        }

        if (pPostProcessor != NULL)
        {
            delete pPostProcessor;
            pPostProcessor  = NULL;
        }

        if (pSaver         != NULL)
        {
            delete pSaver;
            pSaver          = NULL;
        }

        free_aligned(pData);
        pData               = NULL;
        vBuffer             = NULL;
        vDisplayAbscissa    = NULL;
        vDisplayOrdinate    = NULL;
    }

    scp_rtcalc_t profiler_mono::get_rt_algorithm(size_t algorithm)
    {
        switch (algorithm)
        {
            case profiler_mono_metadata::SC_RTALGO_EDT_0:
                return SCP_RT_EDT_0;
            case profiler_mono_metadata::SC_RTALGO_EDT_1:
                return SCP_RT_EDT_1;
            case profiler_mono_metadata::SC_RTALGO_T_10:
                return SCP_RT_T_10;
            case profiler_mono_metadata::SC_RTALGO_T_20:
                return SCP_RT_T_20;
            case profiler_mono_metadata::SC_RTALGO_T_30:
                return SCP_RT_T_30;
            default:
                return SCP_RT_T_20;
        }
    }

    void profiler_mono::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        pExecutor               = wrapper->get_executor();

        pPreProcessor           = new PreProcessor(this);
        pConvolver              = new Convolver(this);
        pPostProcessor          = new PostProcessor(this);
        pSaver                  = new Saver(this);

        sCalOscillator.init();
        sCalOscillator.set_function(FG_SINE);
        sCalOscillator.set_dc_offset(0.0f);
        sCalOscillator.set_dc_reference(DC_ZERO);
        sCalOscillator.set_phase(0.0f);

        sLatencyDetector.init();
        sLatencyDetector.set_delay_ratio(0.5f);
        sLatencyDetector.set_duration(0.050f);
        sLatencyDetector.set_op_fading(0.030f);
        sLatencyDetector.set_op_pause(0.025f);

        sSyncChirpProcessor.init();
        sSyncChirpProcessor.set_chirp_synthesis_method(SCP_SYNTH_BANDLIMITED);
        sSyncChirpProcessor.set_chirp_initial_frequency(SYNC_CHIRP_START_FREQ);
        sSyncChirpProcessor.set_chirp_final_frequency(SYNC_CHIRP_MAX_FREQ);
        sSyncChirpProcessor.set_chirp_amplitude(profiler_mono_metadata::AMPLITUDE_DFL);
        sSyncChirpProcessor.set_fader_fading_method(SCP_FADE_RAISED_COSINES);
        sSyncChirpProcessor.set_fader_fadein(0.500f);
        sSyncChirpProcessor.set_fader_fadeout(0.020f);
        sSyncChirpProcessor.set_oversampler_mode(OM_LANCZOS_8X2);

        sResponseTaker.init();
        sResponseTaker.set_op_fading(0.030f);
        sResponseTaker.set_op_pause(0.025f);

        // 1X temporary DSP buffer + 2X buffer to plot data
        size_t samples          = TMP_BUF_SIZE + 2 * profiler_mono_metadata::RESULT_MESH_SIZE;

        float *ptr              = alloc_aligned<float>(pData, samples);
        if (ptr == NULL)
            return;

        float *save             = ptr;
        vBuffer                 = ptr; //reinterpret_cast<float *>(ptr);
        ptr                    += TMP_BUF_SIZE; // * sizeof(float);
        vDisplayAbscissa        = ptr; //reinterpret_cast<float *>(ptr);
        ptr                    += profiler_mono_metadata::RESULT_MESH_SIZE; // * sizeof(float);
        vDisplayOrdinate        = ptr; //reinterpret_cast<float *>(ptr);
        ptr                    += profiler_mono_metadata::RESULT_MESH_SIZE; // * sizeof(float);

        lsp_assert(ptr <= &save[samples]);

        size_t port_id          = 0;

        pIn                     = vPorts[port_id++];
        pOut                    = vPorts[port_id++];

        pBypass                 = vPorts[port_id++];

        pStateLEDs              = vPorts[port_id++];

        pCalFrequency           = vPorts[port_id++];
        pCalAmplitude           = vPorts[port_id++];
        pCalSwitch              = vPorts[port_id++];

        pLdMaxLatency           = vPorts[port_id++];
        pLdPeakThs              = vPorts[port_id++];
        pLdAbsThs               = vPorts[port_id++];
        pLdEnableSwitch         = vPorts[port_id++];

        pDuration               = vPorts[port_id++];
        pActualDuration         = vPorts[port_id++];

        pIROffset               = vPorts[port_id++];

        pLatencyScreen          = vPorts[port_id++];

        pRTAlgoSelector         = vPorts[port_id++];

        pRTScreen               = vPorts[port_id++];
        pRTAccuracyLed          = vPorts[port_id++];

        pILScreen               = vPorts[port_id++];

        pRScreen                = vPorts[port_id++];

        pLevelMeter             = vPorts[port_id++];

        pPostTrigger            = vPorts[port_id++];

        pSaveModeSelector       = vPorts[port_id++];

        pIRFileName             = vPorts[port_id++];
        pIRSaveCmd              = vPorts[port_id++];
        pIRSaveStatus           = vPorts[port_id++];
        pIRSavePercent          = vPorts[port_id++];

        pLatTrigger             = vPorts[port_id++];

        pLinTrigger             = vPorts[port_id++];

        pFeedback               = vPorts[port_id++];

        pResultMesh             = vPorts[port_id++];

        pStateLEDs->setValue(nState);
    }

    void profiler_mono::update_sample_rate(long sr)
    {
        nSampleRate             = sr;

        sBypass.init(sr);
        sCalOscillator.set_sample_rate(sr);
        sLatencyDetector.set_sample_rate(sr);
        sSyncChirpProcessor.set_sample_rate(sr);
        sResponseTaker.set_sample_rate(sr);
    }

    void profiler_mono::process(size_t samples)
    {
        float *in = pIn->getBuffer<float>();
        if (in == NULL)
            return;

        float *out = pOut->getBuffer<float>();
        if (out == NULL)
            return;

        pLevelMeter->setValue(dsp::abs_max(in, samples));

        if (bDoReset)
        {
            pPreProcessor->reset();
            pConvolver->reset();
            pPostProcessor->reset();
            pSaver->reset();

            sLatencyDetector.reset_capture();
            sResponseTaker.reset_capture();

            nState                  = IDLE;

            bDoReset                = false;
        }

        if (bResetSaver)
        {
            pIRSaveStatus->setValue(STATUS_UNSPECIFIED);
            bResetSaver             = false;
        }

        path_t *path = pIRFileName->getBuffer<path_t>();
        if (path->pending())
        {
            bFileSet                = true;
            path->accept();
        }

        if ((bFileSet) && (pIRSaveCmd->getValue() > 0.5f) && (bIRMeasured) && (nState == IDLE))
            nState                  = SAVING;

        while (samples > 0)
        {
            size_t to_do = (samples > TMP_BUF_SIZE) ? TMP_BUF_SIZE : samples;

            switch (nState)
            {
                case IDLE:
                {
                    dsp::fill_zero(vBuffer, to_do);
                }
                break;

                case CALIBRATION:
                {
                    if (bCalibration)
                    {
                        sCalOscillator.process_overwrite(vBuffer, to_do);
                    }
                    else
                    {
                        dsp::fill_zero(vBuffer, to_do);
                        // This state transition is here, instead than in update_settings(), to avoid
                        // forcing IDLE every time the calibrator is found off (which could happen during system test
                        // if a control is moved).
                        nState  = IDLE;
                    }
                }
                break;

                case LATENCYDETECTION:
                {
                    sLatencyDetector.process_in(vBuffer, in, to_do);

                    if (!bFeedback)
                        dsp::fill_zero(vBuffer, to_do);

                    sLatencyDetector.process_out(vBuffer, vBuffer, to_do);
                    dsp::scale2(vBuffer, fLtAmplitude, to_do);

                    if (sLatencyDetector.latency_detected())
                    {
                        bLatencyMeasured    = true;
                        nLatency            = sLatencyDetector.get_latency_samples();
                        pLatencyScreen->setValue(sLatencyDetector.get_latency_seconds() * 1000.0f); // * 1000.0f to show ms instead of s
                        sResponseTaker.set_latency_samples(nLatency);

                        if (bDoLatencyOnly)
                        {
                            nState  = IDLE;
                            bDoLatencyOnly  = false;
                        }
                        else
                            nState  = PREPROCESSING;

                        sLatencyDetector.reset_capture();
                    }
                    else if (sLatencyDetector.cycle_complete())
                    {
                        bLatencyMeasured    = false;
                        nLatency            = 0;
                        nState  = IDLE;
                        sLatencyDetector.reset_capture();
                    }

                    nWaitCounter           -= to_do;
                }
                break;

                case PREPROCESSING:
                {
                    // Check task state. If needed (first time we get here after state transition) submit the
                    // task.
                    if (pPreProcessor->idle())
                        pExecutor->submit(pPreProcessor);

                    // Advance machine status only if when (and if) the pre processing task is completed
                    if (pPreProcessor->completed())
                    {
                        if (pPreProcessor->successful())
                            nState  = WAIT;
                        else
                            nState  = IDLE;

                        pPreProcessor->reset();
                    }

                    dsp::fill_zero(vBuffer, to_do);

                    nWaitCounter   -= to_do;
                }
                break;

                case WAIT:
                {
                    if (nWaitCounter <= 0)
                    {
                        bIRMeasured = false;
                        sResponseTaker.start_capture();
                        nState      = RECORDING;
                    }

                    dsp::fill_zero(vBuffer, to_do);

                    nWaitCounter   -= to_do;
                }
                break;

                case RECORDING:
                {
                    sResponseTaker.process_in(vBuffer, in, to_do);

                    if (!bFeedback)
                        dsp::fill_zero(vBuffer, to_do);

                    sResponseTaker.process_out(vBuffer, vBuffer, to_do);

                    if (sResponseTaker.cycle_complete())
                    {
                        nState      = CONVOLVING;
                        sResponseTaker.reset_capture();
                    }
                }
                break;

                case CONVOLVING:
                {
                    if (pConvolver->idle())
                        pExecutor->submit(pConvolver);

                    if (pConvolver->completed())
                    {
                        bIRMeasured = true;
                        nState      = POSTPROCESSING;
                        pConvolver->reset();
                    }

                    dsp::fill_zero(vBuffer, to_do);
                }
                break;

                case POSTPROCESSING:
                {
                    if (pPostProcessor->idle())
                        pExecutor->submit(pPostProcessor);

                    if (pPostProcessor->completed())
                    {
                        bIRMeasured = true;
                        nState      = IDLE;
                        pPostProcessor->reset();
                    }

                    dsp::fill_zero(vBuffer, to_do);
                }
                break;

                case SAVING:
                {
                    if (pSaver->idle())
                        pExecutor->submit(pSaver);

                    if (pSaver->completed())
                    {
                        nState      = IDLE;
                        pSaver->reset();
                    }

                    dsp::fill_zero(vBuffer, to_do);
                }
                break;
            }

            sBypass.process(out, in, vBuffer, to_do);

            in         += to_do;
            out        += to_do;
            samples    -= to_do;
        }

        // Write always
//        lsp_trace("state = %d", nState);
        pStateLEDs->setValue(nState);
    }

    void profiler_mono::update_settings()
    {
        /** Wanted behaviour: controls can be used to set the plugin only in
         *  IDLE and CALIBRATION states. Otherwise, operating a control should
         *  result in the plugin being reseted. Reason: the controls alter
         *  all the measurement results.
         */

        if ((nState != CALIBRATION) && (nState != IDLE))
            bDoReset                = true; // Actual reset is done in the process() method

        bLinTrigger                 = pLinTrigger->getValue() >= 0.5f;

        // If the bLinTrigger values goes from true to false, then we know
        // the control is being reset after being operated. This way we avoid
        // resetting the plugin without a need for.
        bool bTriggerCtrlReset      = bLinTriggerPrevious && !bLinTrigger;

        if (bTriggerCtrlReset)
            bDoReset     = false;

        // ^^ Without the lines above, when update_settings() will be called again
        // after the trigger returns to false upon release, doReset will go to true,
        // thus resetting the plugin and impeding the measurement to start!

        bLinTriggerPrevious         = bLinTrigger;

        // Similarly with latency trigger:

        bLatTrigger                 = pLatTrigger->getValue() >= 0.5f;

        bool bLatCtrlReset          = bLatTriggerPrevious && !bLatTrigger;

        if (bLatCtrlReset)
            bDoReset                = false;

        bLatTriggerPrevious         = bLatTrigger;

        // Similarly with post processor trigger:

        bPostprocess                = pPostTrigger->getValue() >= 0.5f;

        bool bPostCtrlReset         = bPostprocessPrevious && !bPostprocess;

        if (bPostCtrlReset)
            bDoReset                = false;

        bPostprocessPrevious        = bPostprocess;

        bBypass                     = pBypass->getValue() >= 0.5f;
        sBypass.set_bypass(bBypass);

        fLtAmplitude                = pCalAmplitude->getValue();
        sCalOscillator.set_amplitude(fLtAmplitude);
        sSyncChirpProcessor.set_chirp_amplitude(fLtAmplitude);

        sCalOscillator.set_frequency(pCalFrequency->getValue());

        bCalibration                = pCalSwitch->getValue() >= 0.5f;

        if (bCalibration)
        {
            // Reset is done here.
            bDoReset                = false;

            pPreProcessor->reset();
            pConvolver->reset();
            pPostProcessor->reset();
            pSaver->reset();

            sLatencyDetector.reset_capture();
            sResponseTaker.reset_capture();

            nState                  = CALIBRATION;

            bResetSaver             = true;
        }

        enRtAlgo                    = get_rt_algorithm(pRTAlgoSelector->getValue());

        sLatencyDetector.set_ip_detection(pLdMaxLatency->getValue() / 1000.0f);
        sLatencyDetector.set_peak_threshold(pLdPeakThs->getValue());
        sLatencyDetector.set_abs_threshold(pLdAbsThs->getValue());
        bSkipLatencyDetection       = pLdEnableSwitch->getValue() < 0.5f;

        // Since triggers make update_settings() to be called twice (on pressure
        // and on release) these lines are needed in order to make the actual
        // duration screen reporting the correct value. If the preprocessor is
        // very fast, in fact, the value it will set for the screen wil be
        // overwritten by the release call to update settings if the duration
        // setting is overwritten here without guards.

        // Change duration setting only if the controller actually changed
        float scDurationSetting     = pDuration->getValue();

        if (scDurationSetting != fScpDurationPrevious)
        {
            sSyncChirpProcessor.set_chirp_duration(scDurationSetting);
            pActualDuration->setValue(scDurationSetting);
            fScpDurationPrevious    = scDurationSetting;
        }

        nWaitCounter                = seconds_to_samples(nSampleRate, pDuration->getValue());

        if (bLatTrigger && !bCalibration && !bLinTrigger) // Allow only if not calibrating and not measuring
        {
            // Reset is done here.
            bDoReset                = false;

            // Needs resets in case it was pressed while a previous cycle did not finish yet.
            pPreProcessor->reset();
            pConvolver->reset();
            pPostProcessor->reset();
            pSaver->reset();

            sResponseTaker.reset_capture();

            bDoLatencyOnly          = true;
            bLatencyMeasured        = false;
            sLatencyDetector.start_capture();
            pLatencyScreen->setValue(0.0f);
            nState                  = LATENCYDETECTION;

            bResetSaver             = true;
        }

        if (bLinTrigger && !bCalibration) // Allow measurement cycle to start only if not calibrating
        {
            // Reset is done here.
            bDoReset                = false;

            // Needs resets in case it was pressed while a previous cycle did not finish yet.
            pPreProcessor->reset();
            pConvolver->reset();
            pPostProcessor->reset();
            pSaver->reset();

            sResponseTaker.reset_capture();

            if (bSkipLatencyDetection && bLatencyMeasured)
            {
                nState              = PREPROCESSING;
                sResponseTaker.set_latency_samples(nLatency);
            }
            else
            {
                bLatencyMeasured    = false;
                sLatencyDetector.start_capture();
                pLatencyScreen->setValue(0.0f);
                nState              = LATENCYDETECTION;
            }

            bResetSaver             = true;
        }

        if (bPostprocess && !bCalibration && !bLinTrigger) // Allow only if not calibrating and not measuring
        {
            // Reset is done here.
            bDoReset                = false;

            pPreProcessor->reset();
            pConvolver->reset();
            pPostProcessor->reset();
            pSaver->reset();

            sLatencyDetector.reset_capture();
            sResponseTaker.reset_capture();

            bIRMeasured             = false;
            nState                  = POSTPROCESSING;

            bResetSaver             = true;
        }

        size_t saveMode             = pSaveModeSelector->getValue();

        if (saveMode != nSaveMode)
        {
            nSaveMode               = saveMode;
            bResetSaver             = true;
        }

        bFeedback                   = pFeedback->getValue() >= 0.5f;

        if (sCalOscillator.needs_update())
            sCalOscillator.update_settings();

        if (sLatencyDetector.needs_update())
            sLatencyDetector.update_settings();

        if (sResponseTaker.needs_update())
            sResponseTaker.update_settings();
    }
}
