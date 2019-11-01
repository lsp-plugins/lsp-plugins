/*
 * trigger.h
 *
 *  Created on: 04 мая 2016 г.
 *      Author: sadko
 */

#ifndef PLUGINS_TRIGGER_H_
#define PLUGINS_TRIGGER_H_

#include <metadata/plugins.h>

#include <core/plugin.h>
#include <core/protocol/midi.h>
#include <core/util/MeterGraph.h>
#include <core/util/Blink.h>
#include <core/util/Sidechain.h>

#include <plugins/sampler.h>

namespace lsp
{
    class trigger_base: public plugin_t, public trigger_base_metadata
    {
        protected:
            enum state_t
            {
                T_OFF,
                T_DETECT,
                T_ON,
                T_RELEASE
            };

            enum source_t
            {
                S_MIDDLE,
                S_SIDE,
                S_LEFT,
                S_RIGHT
            };

            enum mode_t
            {
                M_PEAK,
                M_RMS,
                M_LPF,
                M_UNIFORM,
            };

            typedef struct channel_t
            {
                float      *vCtl;           // Control chain
                Bypass      sBypass;        // Bypass
                MeterGraph  sGraph;         // Metering graph
                bool        bVisible;       // Visibility flag

                IPort      *pIn;            // Input port
                IPort      *pOut;           // Output port
                IPort      *pGraph;         // Graph port
                IPort      *pMeter;         // Metering port
                IPort      *pVisible;       // Visibility port
            } channel_t;

        protected:
            // Sidechain
            Sidechain               sSidechain;             // Sidechain
            float                  *vTmp;                   // Temporary buffer

            // Instantiation parameters
            size_t                  nFiles;                 // Number of files
            size_t                  nChannels;              // Number of channels
            bool                    bMidiPorts;             // Has MIDI port

            // Processors and buffers
            sampler_kernel          sKernel;                // Output kernel
            MeterGraph              sFunction;              // Function
            MeterGraph              sVelocity;              // Trigger velocity level
            Blink                   sActive;                // Activity blink
            channel_t               vChannels[TRACKS_MAX];  // Output channels
            float                  *vTimePoints;            // Time points buffer

            // Processing variables
            ssize_t                 nCounter;               // Counter for detect/release
            size_t                  nState;                 // Trigger state
            float                   fVelocity;              // Current velocity value
            bool                    bFunctionActive;        // Function activity
            bool                    bVelocityActive;        // Velocity activity

            // Parameters
            size_t                  nNote;                  // Trigger note
            size_t                  nChannel;               // Channel
            float                   fDry;                   // Dry amount
            float                   fWet;                   // Wet amount
            bool                    bPause;                 // Pause analysis refresh
            bool                    bClear;                 // Clear analysis
            bool                    bUISync;                // Synchronize with UI

            size_t                  nDetectCounter;         // Detect counter
            size_t                  nReleaseCounter;        // Release counter
            float                   fDetectLevel;           // Detection level
            float                   fDetectTime;            // Trigger detection time
            float                   fReleaseLevel;          // Release level
            float                   fReleaseTime;           // Release time
            float                   fDynamics;              // Dynamics
            float                   fDynaTop;               // Dynamics top
            float                   fDynaBottom;            // Dynamics bottom
            float_buffer_t         *pIDisplay;              // Inline display buffer

            // Control ports
            IPort                  *pFunction;              // Trigger function
            IPort                  *pFunctionLevel;         // Function level
            IPort                  *pFunctionActive;        // Function activity
            IPort                  *pVelocity;              // Trigger velocity
            IPort                  *pVelocityLevel;         // Trigger velocity level
            IPort                  *pVelocityActive;        // Trigger velocity activity
            IPort                  *pActive;                // Trigger activity flag

            IPort                  *pMidiIn;                // MIDI input port
            IPort                  *pMidiOut;               // MIDI output port
            IPort                  *pChannel;               // Note port
            IPort                  *pNote;                  // Note port
            IPort                  *pOctave;                // Octave port
            IPort                  *pMidiNote;              // Output midi note #

            IPort                  *pBypass;                // Bypass port
            IPort                  *pDry;                   // Dry output
            IPort                  *pWet;                   // Wet output
            IPort                  *pGain;                  // Gain output
            IPort                  *pPause;                 // Pause analysis
            IPort                  *pClear;                 // Clear analysis
            IPort                  *pPreamp;                // Pre-amplification

            IPort                  *pSource;                // Source port
            IPort                  *pMode;                  // Mode port
            IPort                  *pDetectLevel;           // Detection level port
            IPort                  *pDetectTime;            // Detection time
            IPort                  *pReleaseLevel;          // Release level port
            IPort                  *pReleaseTime;           // Release time
            IPort                  *pDynamics;              // Dynamics
            IPort                  *pDynaRange1;            // Dynamics range 1
            IPort                  *pDynaRange2;            // Dynamics range 1
            IPort                  *pReactivity;            // Reactivity
            IPort                  *pReleaseValue;          // Release value

        protected:
            void                trigger_on(size_t timestamp, float level);
            void                trigger_off(size_t timestamp, float level);
            void                process_samples(const float *sc, size_t samples);
            inline void         update_counters();
            size_t              decode_mode();
            size_t              decode_source();

        public:
            explicit trigger_base(const plugin_metadata_t &metadata, size_t files, size_t channels, bool midi);
            virtual ~trigger_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void ui_activated();

            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);

    };

    class trigger_mono: public trigger_base, public trigger_mono_metadata
    {
        public:
            explicit trigger_mono();
            virtual ~trigger_mono();
    };

    class trigger_stereo: public trigger_base, public trigger_stereo_metadata
    {
        public:
            explicit trigger_stereo();
            virtual ~trigger_stereo();
    };

    class trigger_midi_mono: public trigger_base, public trigger_midi_mono_metadata
    {
        public:
            explicit trigger_midi_mono();
            virtual ~trigger_midi_mono();
    };

    class trigger_midi_stereo: public trigger_base, public trigger_midi_stereo_metadata
    {
        public:
            explicit trigger_midi_stereo();
            virtual ~trigger_midi_stereo();
    };

}

#endif /* PLUGINS_TRIGGER_H_ */
