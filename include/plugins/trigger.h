/*
 * trigger.h
 *
 *  Created on: 04 мая 2016 г.
 *      Author: sadko
 */

#ifndef PLUGINS_TRIGGER_H_
#define PLUGINS_TRIGGER_H_

#ifndef LSP_NO_EXPERIMENTAL

#include <metadata/plugins.h>

#include <core/plugin.h>
#include <core/midi.h>
#include <core/ShiftBuffer.h>
#include <core/Blink.h>

#include <plugins/sampler.h>

namespace lsp
{
    class audio_trigger_kernel: public audio_trigger_kernel_metadata
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
                S_LEFT,
                S_RIGHT,
                S_MIDDLE,
                S_SIDE
            };

            enum mode_t
            {
                M_PEAK,
                M_RMS,
                M_ABS_RMS,
                M_FILTERED,
                M_SQR
            };

        protected:
            ITrigger           *pHandler;       // Trigger event handler
            ssize_t             nCounter;       // Counter for detect/release
            ssize_t             nSampleRate;    // Sample rate;
            size_t              nChannels;      // List of channels
            size_t              nState;         // Trigger state
            size_t              nReactivity;    // Reactivity (in samples)
            float               fTau;           // Tau for RMS
            float               fRmsValue;      // RMS value
            ShiftBuffer         sBuffer;        // Buffer to store samples
            Blink               sActive;        // Activity blink

            size_t              nSource;        // Trigger source
            size_t              nMode;          // Mode
            float               fDetectLevel;   // Detection level
            float               fDetectTime;    // Trigger detection time
            float               fReleaseLevel;  // Release level
            float               fReleaseTime;   // Release time
            float               fDynamics;      // Dynamics
            float               fReactivity;    // Reactivity

            IPort              *pSource;        // Source port
            IPort              *pMode;          // Mode port
            IPort              *pDetectLevel;   // Detection level port
            IPort              *pDetectTime;    // Detection time
            IPort              *pReleaseLevel;  // Release level port
            IPort              *pReleaseTime;   // Release time
            IPort              *pDynamics;      // Dynamics
            IPort              *pReactivity;    // Reactivity
            IPort              *pMeter;         // Meter
            IPort              *pActive;        // Active flag
            #ifdef LSP_DEBUG
            IPort              *pDebug;         // Debug output port
            #endif /* LSP_DEBUG */

        protected:
            inline float        get_sample(const float **data, size_t idx);
            inline float        process_sample(float sample);
            inline void         update_reactivity(size_t old_mode);

        public:
            audio_trigger_kernel();
            virtual ~audio_trigger_kernel();

        public:
            bool    init(ITrigger *handler, size_t channels);
            void    update_sample_rate(long sr);
            size_t  bind(cvector<IPort> &ports, size_t port_id);
            #ifdef LSP_DEBUG
            void    bind_debug(IPort *debug);
            #endif /* LSP_DEBUG */
            void    destroy();

            void    update_settings();
            void    process(const float **data, size_t samples);
    };

    class trigger_kernel: public sampler_kernel
    {
        protected:
            size_t              nNote;          // Trigger note
            size_t              nChannel;       // Channel

            IPort              *pChannel;       // Note port
            IPort              *pNote;          // Note port
            IPort              *pOctave;        // Octave port
            IPort              *pMidiNote;      // Output midi note #
            IPort              *pMidiIn;        // MIDI input port
            IPort              *pMidiOut;       // MIDI output port

        public:
            trigger_kernel();
            virtual ~trigger_kernel();

        public:
            virtual void trigger_on(size_t timestamp, float level);

            virtual void trigger_off(size_t timestamp, float level);

        public:
            size_t  bind(cvector<IPort> &ports, size_t port_id);
            void    bind_midi(IPort *midi_in, IPort *midi_out);
            void    update_settings();
            void    process(float **outs, const float **ins, size_t samples);
    };

    class trigger_base: public plugin_t
    {
        protected:
            typedef struct trigger_channel_t
            {
                float      *vDry;           // Dry output
                float       fPan;           // Gain
                float       fDry;           // Dry amount
                float       fWet;           // Wet amount
                Bypass      sDryBypass;     // Dry bypass
                Bypass      sMixBypass;     // Mix bypass

                IPort      *pPan;           // Pan output
                IPort      *pDry;           // Dry output
            } trigger_channel_t;

            typedef struct channel_t
            {
                float      *vIn;            // Input
                float      *vOut;           // Output
                float      *vTmpIn;         // Temporary input buffer
                float      *vTmpOut;        // Temporary output buffer
                Bypass      sBypass;        // Bypass

                IPort      *pIn;            // Input port
                IPort      *pOut;           // Output port
            } channel_t;

            typedef struct trigger_t
            {
                trigger_kernel          sKernel;            // Output kernel
                audio_trigger_kernel    sTrigger;           // Audio trigger
                float                   fOutGain;           // Output gain
                trigger_channel_t       vChannels[trigger_base_metadata::TRACKS_MAX];       // Output channels

                IPort                  *pOutGain;           // Gain port
                IPort                  *pDry;               // Dry amount port
                IPort                  *pWet;               // Wet amount port
                IPort                  *pDryBypass;         // Dry bypass port
                IPort                  *pMixBypass;         // Mix bypass port
            } trigger_t;

        protected:
            size_t              nTriggers;              // Number of triggers
            size_t              nFiles;                 // Number of files
            size_t              nChannels;              // Number of channels
            bool                bMidiPorts;             // Has MIDI port
            float               fDry;                   // Dry amount
            float               fWet;                   // Wet amount
            trigger_t          *vTriggers;              // List of triggers
            float              *pBuffer;                // Temporary buffer
            channel_t           vChannels[trigger_base_metadata::TRACKS_MAX];            // Monitoring channels

            IPort              *pMidiIn;                // MIDI input port
            IPort              *pMidiOut;               // MIDI output port
            IPort              *pBypass;                // Bypass port
            IPort              *pDry;                   // Dry output
            IPort              *pWet;                   // Wet output
            IPort              *pGain;                  // Gain output

        public:
            trigger_base(const plugin_metadata_t &metadata, size_t triggers, size_t files, size_t channels, bool midi);
            virtual ~trigger_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);
    };

    class trigger_mono: public trigger_base, public trigger_mono_metadata
    {
        public:
            trigger_mono();
            virtual ~trigger_mono();
    };

    class trigger_stereo: public trigger_base, public trigger_stereo_metadata
    {
        public:
            trigger_stereo();
            virtual ~trigger_stereo();
    };

    class trigger_mono_midi: public trigger_base, public trigger_mono_midi_metadata
    {
        public:
            trigger_mono_midi();
            virtual ~trigger_mono_midi();
    };

    class trigger_stereo_midi: public trigger_base, public trigger_stereo_midi_metadata
    {
        public:
            trigger_stereo_midi();
            virtual ~trigger_stereo_midi();
    };

}

#endif

#endif /* PLUGINS_TRIGGER_H_ */
