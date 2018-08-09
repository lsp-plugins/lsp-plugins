/*
 * profiler.h
 *
 *  Created on: 13 Sep 2017
 *      Author: crocoduck
 */

#ifndef PLUGINS_PROFILER_H_
#define PLUGINS_PROFILER_H_

#include <core/plugin.h>
#include <metadata/plugins.h>
#include <core/util/Bypass.h>
#include <core/util/Oscillator.h>
#include <core/util/LatencyDetector.h>
#include <core/SyncChirpProcessor.h>
#include <core/ResponseTaker.h>

namespace lsp
{
    class profiler_mono: public plugin_t, public profiler_mono_metadata
    {
        protected:
            // Class to handle profiling time series generation task
            class PreProcessor: public ITask
            {
                private:
                    profiler_mono   *pCore;

                public:
                    PreProcessor(profiler_mono *base);
                    virtual ~PreProcessor();

                public:
                    virtual int run();
            };

            // Task to handle generation of the convolution result
            class Convolver: public ITask
            {
                private:
                    profiler_mono *pCore;

                public:
                    Convolver(profiler_mono *base);
                    virtual ~Convolver();

                public:
                    virtual int run();
            };

            // Class to handle post processing of the convolution result
            class PostProcessor: public ITask
            {
                private:
                    profiler_mono *pCore;

                public:
                    PostProcessor(profiler_mono *base);
                    virtual ~PostProcessor();

                public:
                    virtual int run();
            };

            // Class to handle saving of the convolution result
            class Saver: public ITask
            {
                private:
                    profiler_mono *pCore;

                public:
                    Saver(profiler_mono *base);
                    virtual ~Saver();

                public:
                    virtual int run();
            };

            // Object state descriptor
            enum state_t
            {
                IDLE,
                CALIBRATION,
                LATENCYDETECTION,
                PREPROCESSING,                          // <- Not Realtime: ITask
                WAIT,
                RECORDING,
                CONVOLVING,                             // <- Not Realtime: ITask
                POSTPROCESSING,                         // <- Not Realtime: ITask
                SAVING                                  // <- Not Realtime: ITask
            };

        protected:
            state_t             nState;                 // Object State

            IExecutor          *pExecutor;              // Executor Service
            PreProcessor       *pPreProcessor;          // Pre Processor Task
            Convolver          *pConvolver;             // Convovler Task
            PostProcessor      *pPostProcessor;         // Post Processor Task
            Saver              *pSaver;                 // Saver Task

            Bypass              sBypass;
            Oscillator          sCalOscillator;         // For calibration
            LatencyDetector     sLatencyDetector;       // For latency assessment
            SyncChirpProcessor  sSyncChirpProcessor;    // To handle Synch Chirp profiling signal and related operations
            ResponseTaker       sResponseTaker;         // To take response of system after Synch Chirp stimulation

            size_t              nSampleRate;            // Sample Rate
            float               fLtAmplitude;           // Amplitude factor for Latency Detection chirp
            scp_rtcalc_t        enRtAlgo;               // Store RT Algorithm
            ssize_t             nWaitCounter;           // Count the samples for wait state
            bool                bDoReset;               // If true, force plugin state reset
            bool                bDoLatencyOnly;         // If true, only latency is measured
            bool                bLatencyMeasured;       // If true, a latency measurement was performed
            size_t              nLatency;               // Store latency value
            float               fScpDurationPrevious;   // Store Sync Chirp Duration Setting between calls to update_settings()
            bool                bIRMeasured;            // If true, an IR measurement was performed and post processed
            bool                bFileSet;               // If true, the saving file was correctly set
            size_t              nSaveMode;              // Hold save mode enumeration index
            bool                bResetSaver;            // If true, reset save control state

            bool                bBypass;                // True if bypass switch is pressed on
            bool                bCalibration;           // True if calibration switch is pressed on
            bool                bSkipLatencyDetection;  // True if latency detection switch is pressed on
            bool                bPostprocess;           // True if postprocess switch is pressed on
            bool                bPostprocessPrevious;   // Store values of bPostprocess between calls to update_settings()
            bool                bLatTrigger;            // True if latency measurement trigger is pressed
            bool                bLatTriggerPrevious;    // Store values of bLatTrigger between calls to update_settings()
            bool                bLinTrigger;            // True if linear measurement trigger is pressed
            bool                bLinTriggerPrevious;    // Store values of bLinTrigger between calls to update_settings()
            bool                bFeedback;              // True if feedback break switch is pressed on

            float              *vBuffer;                // Auxiliary processing buffer
            float              *vDisplayAbscissa;       // Buffer for display. Abscissa data
            float              *vDisplayOrdinate;       // Buffer for display. Ordinate data
            uint8_t            *pData;

            IPort              *pIn;
            IPort              *pOut;

            IPort              *pBypass;

            IPort              *pStateLEDs;             // State LEDs

            IPort              *pCalFrequency;          // Calibration wave frequency
            IPort              *pCalAmplitude;          // Calibration wave amplitude
            IPort              *pCalSwitch;             // Calibration wave switch

            IPort              *pLdMaxLatency;          // Latency Detector Max expected latency
            IPort              *pLdPeakThs;             // Latency Detector Peak Threshold
            IPort              *pLdAbsThs;              // Latency Detector Absolute Threshold
            IPort              *pLdEnableSwitch;        // Switch to enable LATENCYDETECTION phase in measurement (if possible)

            IPort              *pDuration;              // Profiling Sync Chirp Duration
            IPort              *pActualDuration;        // Actual Sync Chirp Duration after optimisation

            IPort              *pIROffset;              // Offset of the measured convolution result, for plot and export

            IPort              *pLatencyScreen;         // Little screen displaying latency value

            IPort              *pRTAlgoSelector;        // Selector for RT calculation algorithm

            IPort              *pRTScreen;              // Little screen displaying RT value
            IPort              *pRTAccuracyLed;         // Led to show whether the RT measurement can be considered accurate

            IPort              *pILScreen;              // Little screen displaying IL (integration limit) value

            IPort              *pRScreen;               // Little screen displaying R (RT regression line correlation coefficient) value

            IPort              *pLevelMeter;            // dB Input Level Meter

            IPort              *pPostTrigger;           // Trigger for post processing

            IPort              *pSaveModeSelector;      // Selector for Save Mode

            IPort              *pIRFileName;            // File name for IR file
            IPort              *pIRSaveCmd;             // Command to save IR file
            IPort              *pIRSaveStatus;          // IR file saving status
            IPort              *pIRSavePercent;         // IR file saving percent

            IPort              *pLatTrigger;            // Trigger for a latency measurement

            IPort              *pLinTrigger;            // Trigger for linear system measurement

            IPort              *pFeedback;              // Switch to open feedback loop

            IPort              *pResultMesh;            // Mesh for result plot

        protected:
            static scp_rtcalc_t get_rt_algorithm(size_t algorithm);

        public:
            profiler_mono();
            virtual ~profiler_mono();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void process(size_t samples);
            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void destroy();
    };
}

#endif /* PLUGINS_PROFILER_H_ */
