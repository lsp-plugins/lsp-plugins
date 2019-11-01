/*
 * sampler.h
 *
 *  Created on: 21 янв. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_PLUGINS_SAMPLER_H_
#define CORE_PLUGINS_SAMPLER_H_

#include <metadata/plugins.h>

#include <core/plugin.h>
#include <core/protocol/midi.h>

#include <core/ITrigger.h>
#include <core/util/Bypass.h>
#include <core/util/Toggle.h>
#include <core/util/Blink.h>
#include <core/util/Randomizer.h>
#include <core/files/AudioFile.h>
#include <core/sampling/SamplePlayer.h>


namespace lsp
{
    class sampler_kernel: public sampler_kernel_metadata, public ITrigger
    {
        protected:
            struct afile_t;

            class AFLoader: public ipc::ITask
            {
                private:
                    sampler_kernel         *pCore;
                    afile_t                *pFile;

                public:
                    AFLoader(sampler_kernel *base, afile_t *descr);
                    virtual ~AFLoader();

                public:
                    virtual status_t run();
            };

        protected:
            struct afsample_t
            {
                AudioFile          *pFile;                  // File
                float               fNorm;                  // Normalizing factor
                Sample             *pSample;                // Sample
                float              *vThumbs[TRACKS_MAX];    // List of thumbnails
            };

            enum afindex_t
            {
                AFI_CURR,
                AFI_NEW,
                AFI_OLD,
                AFI_TOTAL
            };

            struct afile_t
            {
                size_t              nID;                    // ID of sample
                AFLoader           *pLoader;                // Audio file loader task

                bool                bDirty;                 // Dirty flag
                float               fVelocity;              // Velocity
                float               fHeadCut;               // Head cut (ms)
                float               fTailCut;               // Tail cut (ms)
                float               fFadeIn;                // Fade In (ms)
                float               fFadeOut;               // Fade Out (ms)
                float               fPreDelay;              // Pre-delay
                Toggle              sListen;                // Listen toggle
                float               fMakeup;                // Makeup gain
                float               fGains[TRACKS_MAX];     // List of gain values
                float               fLength;                // Length in milliseconds
                status_t            nStatus;                // Loading status
                Blink               sNoteOn;                // Note on led
                bool                bOn;                    // On flag

                IPort              *pFile;                  // Audio file port
                IPort              *pHeadCut;               // Head cut
                IPort              *pTailCut;               // Tail cut
                IPort              *pFadeIn;                // Fade in length
                IPort              *pFadeOut;               // Fade out length
                IPort              *pMakeup;                // Makup gain
                IPort              *pVelocity;              // Velocity range top
                IPort              *pPreDelay;              // Pre-delay
                IPort              *pListen;                // Listen trigger
                IPort              *pGains[TRACKS_MAX];     // List of gain ports
                IPort              *pLength;                // Length of the file
                IPort              *pStatus;                // Status of the file
                IPort              *pMesh;                  // Dump of the file data
                IPort              *pNoteOn;                // Note on flag
                IPort              *pOn;                    // Sample on flag
                IPort              *pActive;                // Sample activity flag

                afsample_t         *vData[AFI_TOTAL];       // Currently used audio file
            };

        protected:
            ipc::IExecutor     *pExecutor;                  // Executor service
            afile_t            *vFiles;                     // List of audio files
            afile_t           **vActive;                    // List of active audio files
            SamplePlayer        vChannels[TRACKS_MAX];      // List of channels
            Bypass              vBypass[TRACKS_MAX];        // List of bypasses
            Blink               sActivity;                  // Note on led for instrument

            size_t              nFiles;                     // Number of files
            size_t              nActive;                    // Number of active files
            size_t              nChannels;                  // Number of audio channels (mono/stereo)
            float              *vBuffer;                    // Buffer
            bool                bBypass;                    // Bypass flag
            bool                bReorder;                   // Reorder flag
            float               fFadeout;                   // Fadeout in milliseconds
            float               fDynamics;                  // Dynamics
            float               fDrift;                     // Time drifting
            size_t              nSampleRate;                // Sample rate
            Toggle              sListen;                    // Listen toggle

            Randomizer          sRandom;                    // Randomizer

            IPort              *pDynamics;                  // Dynamics port
            IPort              *pDrift;                     // Time drifting port
            IPort              *pActivity;                  // Activity port
            IPort              *pListen;                    // Listen trigger
            uint8_t            *pData;                      // Pointer to aligned data

        protected:
            void        destroy_state();
            void        destroy_afsample(afsample_t *af);
            int         load_file(afile_t *file);
            void        copy_asample(afsample_t *dst, const afsample_t *src);
            void        clear_asample(afsample_t *dst);
            void        render_sample(afile_t *af);
            void        reorder_samples();
            void        process_listen_events();
            void        output_parameters(size_t samples);
            void        process_file_load_requests();
            void        play_sample(const afile_t *af, float gain, size_t delay);
            void        cancel_sample(const afile_t *af, size_t fadeout, size_t delay);

        public:
            explicit sampler_kernel();
            virtual ~sampler_kernel();

        public:
            virtual void trigger_on(size_t timestamp, float level);
            virtual void trigger_off(size_t timestamp, float level);
            virtual void trigger_stop(size_t timestamp);

        public:
            void    set_fadeout(float length);

        public:
            bool    init(ipc::IExecutor *executor, size_t files, size_t channels);
            size_t  bind(cvector<IPort> &ports, size_t port_id, bool dynamics);
            void    bind_activity(IPort *activity);
            void    destroy();

            void    update_settings();
            void    update_sample_rate(long sr);

            /** Process the sampler kernel
             *
             * @param outs list of outputs (should be not the sampe as ins)
             * @param ins list of inputs, elements may be NULL
             * @param samples number of samples to process
             */
            void    process(float **outs, const float **ins, size_t samples);
    };

    class sampler_base: public plugin_t
    {
        protected:
            static const size_t BITMASK_MAX        = ((sampler_base_metadata::INSTRUMENTS_MAX + 31) >> 5);

        protected:
            enum dm_mode_t
            {
                DM_APPLY_GAIN   = 1 << 0,
                DM_APPLY_PAN    = 1 << 1
            };

            typedef struct sampler_channel_t
            {
                float      *vDry;           // Dry output
                float       fPan;           // Gain
                Bypass      sBypass;        // Bypass
                Bypass      sDryBypass;     // Dry channel bypass

                IPort      *pDry;           // Dry port
                IPort      *pPan;           // Gain output
            } sampler_channel_t;

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

            typedef struct sampler_t
            {
                sampler_kernel      sSampler;           // Sampler
                float               fGain;              // Overall gain
                size_t              nNote;              // Trigger note
                size_t              nChannel;           // Channel
                size_t              nMuteGroup;         // Mute group
                bool                bMuting;            // Muting flag
                bool                bNoteOff;           // Handle note-off event

                sampler_channel_t   vChannels[sampler_kernel_metadata::TRACKS_MAX];       // Sampler output channels
                IPort              *pGain;              // Gain output port
                IPort              *pBypass;            // Bypass port
                IPort              *pDryBypass;         // Dry bypass port
                IPort              *pChannel;           // Note port
                IPort              *pNote;              // Note port
                IPort              *pOctave;            // Octave port
                IPort              *pMuteGroup;         // Mute group
                IPort              *pMuting;            // Muting
                IPort              *pMidiNote;          // Output midi note #
                IPort              *pNoteOff;           // Note off switch
            } sampler_t;

        protected:
            size_t              nChannels;          // Number of channels per output
            size_t              nSamplers;          // Number of samplers
            size_t              nFiles;             // Number of files per sampler
            size_t              nDOMode;            // Mode of direct output
            bool                bDryPorts;          // Dry ports allocated as temporary buffers
            sampler_t          *vSamplers;          // Lisf of samplers

            channel_t           vChannels[sampler_kernel_metadata::TRACKS_MAX];            // Temporary buffers for processing
            Toggle              sMute;              // Mute request
            float              *pBuffer;            // Buffer data used by vChannels
            float               fDry;               // Dry amount
            float               fWet;               // Wet amount

            IPort              *pMidiIn;            // MIDI input port
            IPort              *pMidiOut;           // MIDI output port

            IPort              *pBypass;            // Bypass port
            IPort              *pMute;              // Mute request port
            IPort              *pMuting;            // MIDI muting
            IPort              *pNoteOff;           // Note-off event handling
            IPort              *pFadeout;           // Note-off fadeout
            IPort              *pDry;               // Dry amount port
            IPort              *pWet;               // Wet amount port
            IPort              *pGain;              // Output gain port
            IPort              *pDOGain;            // Direct output gain flag
            IPort              *pDOPan;             // Direct output panning flag

        protected:
            void        process_trigger_events();

        public:
            explicit sampler_base(const plugin_metadata_t &metadata, size_t samplers, size_t channels, size_t files, bool dry_ports);
            virtual ~sampler_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);
    };

    class sampler_mono: public sampler_base, public sampler_mono_metadata
    {
        public:
            sampler_mono();
    };

    class sampler_stereo: public sampler_base, public sampler_stereo_metadata
    {
        public:
            sampler_stereo();
    };

    class multisampler_x12: public sampler_base, public multisampler_x12_metadata
    {
        public:
            multisampler_x12();
    };

    class multisampler_x24: public sampler_base, public multisampler_x24_metadata
    {
        public:
            multisampler_x24();
    };

    class multisampler_x48: public sampler_base, public multisampler_x48_metadata
    {
        public:
            multisampler_x48();
    };

    class multisampler_x12_do: public sampler_base, public multisampler_x12_do_metadata
    {
        public:
            multisampler_x12_do();
    };

    class multisampler_x24_do: public sampler_base, public multisampler_x24_do_metadata
    {
        public:
            multisampler_x24_do();
    };

    class multisampler_x48_do: public sampler_base, public multisampler_x48_do_metadata
    {
        public:
            multisampler_x48_do();
    };

} /* namespace ddb */

#endif /* CORE_PLUGINS_SAMPLER_H_ */
