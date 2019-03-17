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
#include <core/util/SyncChirpProcessor.h>
#include <core/util/ResponseTaker.h>

namespace lsp
{
    class profiler_base: public plugin_t
    {
        protected:
            // Class to handle profiling time series generation task
            class PreProcessor: public ipc::ITask
            {
                private:
            		profiler_base *pCore;

                public:
                    explicit PreProcessor(profiler_base *base);
                    virtual ~PreProcessor();

                public:
                    virtual status_t run();
            };

            // Task to handle generation of the convolution result
            class Convolver: public ipc::ITask
            {
                private:
            		profiler_base *pCore;

                public:
                    explicit Convolver(profiler_base *base);
                    virtual ~Convolver();

                public:
                    virtual status_t run();
            };

            // Class to handle post processing of the convolution result
            class PostProcessor: public ipc::ITask
            {
                private:
            		profiler_base  *pCore;
                    ssize_t         nIROffset;
                    scp_rtcalc_t    enAlgo;

                public:
                    explicit PostProcessor(profiler_base *base);
                    virtual ~PostProcessor();

                public:
                    void set_ir_offset(ssize_t ir_offset);
                    inline ssize_t get_ir_offset() const { return nIROffset; }

                    void set_rt_algo(scp_rtcalc_t algo);

                    virtual status_t run();
            };

            // Class to handle saving of the convolution result
            class Saver: public ipc::ITask
            {
                private:
            		profiler_base  *pCore;
                    ssize_t 		nIROffset;
                    char            sFile[PATH_MAX]; // The name of file for saving

                public:
                    explicit Saver(profiler_base *base);
                    virtual ~Saver();

                public:
                    void set_file_name(const char *fname);
                    void set_ir_offset(ssize_t ir_offset);
                    inline ssize_t get_ir_offset() const { return nIROffset; }

                    bool is_file_set() const;

                    virtual status_t run();
            };

            // Object state descriptor
            enum state_t
            {
                IDLE,                 			// Realtime: doing nothing, awaiting for command
                CALIBRATION,           			// Realtime: callibrating device
                LATENCYDETECTION,          		// Realtime: detecting loopback latency
                PREPROCESSING,               	// Offline: PreProcessor task
                WAIT,                          	// Realtime: waiting for signal fall-off
                RECORDING,                    	// Realtime: recording response
                CONVOLVING,                  	// Offline: Convolver task
                POSTPROCESSING,               	// Offline: PostProcessor task
                SAVING                         	// Offline: Saver task
            };

            enum triggers_t
            {
                T_CHANGE                = 1 << 0, // Change of any following trigger below:

                T_CALIBRATION           = 1 << 1, // Calibration switch is pressed on
                T_SKIP_LATENCY_DETECT   = 1 << 2, // Latency detection switch is pressed on
                T_POSTPROCESS           = 1 << 3, // Post-process switch is pressed on
                T_POSTPROCESS_STATE     = 1 << 4, // Current post-process switch state
                T_LAT_TRIGGER           = 1 << 5, // Latency measurement trigger was pressed
                T_LAT_TRIGGER_STATE     = 1 << 6, // Latency measurement trigger state
                T_LIN_TRIGGER           = 1 << 7, // Linear measurement trigger is pressed
                T_LIN_TRIGGER_STATE     = 1 << 8, // Linear measurement trigger state
                T_FEEDBACK              = 1 << 9  // feedback break switch is pressed on
            };

            typedef struct postproc_t
            {
            	float 				fReverbTime; 			// Reverberation time [seconds]
            	size_t 				nReverbTime; 			// Reverberation time [samples]
//            	float 				fPosTime; 				// Length of the IR positive time axis [seconds]
            	float 				fCorrCoeff; 			// Energy decay correlation coefficient
            	float 				fIntgLimit; 			// IR intgration limit [seconds]
            	bool 				bRTAccuray; 			// If true, dynamic range and bacjground noise are optimal for RT accuracy.
            } posproc_t;

            typedef struct channel_t
            {
                Bypass              sBypass;
//                Oscillator          sCalOscillator;         // For calibration
                LatencyDetector     sLatencyDetector;       // For latency assessment
                ResponseTaker       sResponseTaker;         // To take response of system after Synch Chirp stimulation

                size_t              nLatency;               // Store latency value
                bool                bLatencyMeasured;       // If true, a latency measurement was performed
                bool 				bLCycleComplete; 		// If true, a latency measurement cycle was finished
                bool 				bRCycleComplete; 		// If true, a chirp response recording cycle was finished.

                postproc_t 			sPostProc;				// Holds IR postproc info.

                float              *vBuffer;                // Auxiliary processing buffer

                float 			   *vIn; 					// Input buffer binding
                float 			   *vOut; 					// Output buffer binding

                IPort              *pIn;
                IPort              *pOut;

                IPort              *pLevelMeter;            // dB Input Level Meter
                IPort              *pLatencyScreen;         // Little screen displaying latency value
                IPort              *pRTScreen;              // Little screen displaying RT value
                IPort              *pRTAccuracyLed;         // Led to show whether the RT measurement can be considered accurate
                IPort              *pILScreen;              // Little screen displaying IL (integration limit) value
                IPort              *pRScreen;               // Little screen displaying R (RT regression line correlation coefficient) value
                IPort              *pResultMesh;            // Mesh for result plot
            } channel_t;

            typedef struct response_t
            {
            	Sample		  **vResponses;
            	size_t 		   *vOffsets;
            	uint8_t 	   *pData;
            } response_t;

        protected:
            size_t              nChannels;
			channel_t          *vChannels;

			response_t 			sResponseData;

            state_t             nState;                 // Object State

            Oscillator          sCalOscillator;         // For calibration

            SyncChirpProcessor  sSyncChirpProcessor;    // To handle Synch Chirp profiling signal and related operations

            ipc::IExecutor     *pExecutor;              // Executor Service
            PreProcessor       *pPreProcessor;          // Pre Processor Task
            Convolver          *pConvolver;             // Convolver Task
            PostProcessor      *pPostProcessor;         // Post Processor Task
            Saver              *pSaver;                 // Saver Task

            size_t              nSampleRate;            // Sample Rate
            float               fLtAmplitude;           // Amplitude factor for Latency Detection chirp
            ssize_t             nWaitCounter;           // Count the samples for wait state
            bool                bDoLatencyOnly;         // If true, only latency is measured

            float               fScpDurationPrevious;   // Store Sync Chirp Duration Setting between calls to update_settings()
            bool                bIRMeasured;            // If true, an IR measurement was performed and post processed
            size_t              nSaveMode;              // Hold save mode enumeration index

            size_t              nTriggers;              // Set of triggers controlled by triggers_t

            float 			   *vTempBuffer; 			// Additional auxiliary buffer for processing
            float              *vDisplayAbscissa;       // Buffer for display. Abscissa data
            float              *vDisplayOrdinate;       // Buffer for display. Ordinate data
            uint8_t            *pData;

            IPort              *pBypass;

            IPort              *pStateLEDs;             // State LEDs

            IPort              *pCalFrequency;          // Calibration wave frequency
            IPort              *pCalAmplitude;          // Calibration wave amplitude
            IPort              *pCalSwitch;             // Calibration wave switch
            IPort              *pFeedback;              // Switch to open feedback loop

            IPort              *pLdMaxLatency;          // Latency Detector Max expected latency
            IPort              *pLdPeakThs;             // Latency Detector Peak Threshold
            IPort              *pLdAbsThs;              // Latency Detector Absolute Threshold
            IPort              *pLdEnableSwitch;        // Switch to enable LATENCYDETECTION phase in measurement (if possible)
            IPort              *pLatTrigger;            // Trigger for a latency measurement

            IPort              *pDuration;              // Profiling Sync Chirp Duration
            IPort              *pActualDuration;        // Actual Sync Chirp Duration after optimisation
            IPort              *pLinTrigger;            // Trigger for linear system measurement

            IPort              *pIROffset;              // Offset of the measured convolution result, for plot and export
            IPort              *pRTAlgoSelector;        // Selector for RT calculation algorithm
            IPort              *pPostTrigger;           // Trigger for post processing

            IPort              *pSaveModeSelector;      // Selector for Save Mode
            IPort              *pIRFileName;            // File name for IR file
            IPort              *pIRSaveCmd;             // Command to save IR file
            IPort              *pIRSaveStatus;          // IR file saving status
            IPort              *pIRSavePercent;         // IR file saving percent

        protected:
            static scp_rtcalc_t get_rt_algorithm(size_t algorithm);
            void                update_pre_processing_info();
            void                commit_state_change();
            void                reset_tasks();
            bool                update_post_processing_info();

        public:
            profiler_base(const plugin_metadata_t &metadata, size_t channels);
            virtual ~profiler_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);
    };

    class profiler_mono: public profiler_base, public profiler_mono_metadata
    {
        public:
            profiler_mono();
            virtual ~profiler_mono();
    };

    class profiler_stereo: public profiler_base, public profiler_stereo_metadata
    {
        public:
            profiler_stereo();
            virtual ~profiler_stereo();

    };
}

#endif /* PLUGINS_PROFILER_H_ */
