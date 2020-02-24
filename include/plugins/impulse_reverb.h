/*
 * impulse_reverb.h
 *
 *  Created on: 13 фев. 2017 г.
 *      Author: sadko
 */

#ifndef PLUGINS_IMPULSE_REVERB_H_
#define PLUGINS_IMPULSE_REVERB_H_

#include <core/plugin.h>
#include <core/ipc/IExecutor.h>
#include <core/util/Convolver.h>
#include <core/util/Bypass.h>
#include <core/util/Delay.h>
#include <core/util/Toggle.h>
#include <core/filters/Equalizer.h>
#include <core/sampling/SamplePlayer.h>
#include <core/files/AudioFile.h>

#include <metadata/plugins.h>

namespace lsp
{

    class impulse_reverb_base: public plugin_t
    {
        protected:
            struct af_descriptor_t;

            class IRLoader: public ipc::ITask
            {
                private:
                    impulse_reverb_base     *pCore;
                    af_descriptor_t            *pDescr;

                public:
                    inline IRLoader()
                    {
                        pCore       = NULL;
                        pDescr      = NULL;
                    }

                    void init(impulse_reverb_base *base, af_descriptor_t *descr);
                    virtual ~IRLoader();

                public:
                    virtual status_t run();
            };

            typedef struct reconfig_t
            {
                bool                    bRender[impulse_reverb_base_metadata::FILES];
                size_t                  nFile[impulse_reverb_base_metadata::CONVOLVERS];
                size_t                  nTrack[impulse_reverb_base_metadata::CONVOLVERS];
                size_t                  nRank[impulse_reverb_base_metadata::CONVOLVERS];
            } reconfig_t;

            class IRConfigurator: public ipc::ITask
            {
                private:
                    reconfig_t               sReconfig;
                    impulse_reverb_base     *pCore;

                public:
                    IRConfigurator(impulse_reverb_base *base);
                    virtual ~IRConfigurator();

                public:
                    virtual status_t run();

                    inline void set_render(size_t idx, bool render)     { sReconfig.bRender[idx]    = render;   }
                    inline void set_file(size_t idx, size_t file)       { sReconfig.nFile[idx]      = file;     }
                    inline void set_track(size_t idx, size_t track)     { sReconfig.nTrack[idx]     = track;    }
                    inline void set_rank(size_t idx, size_t rank)       { sReconfig.nRank[idx]      = rank;     }
            };

            typedef struct af_descriptor_t
            {
                AudioFile      *pCurr;          // Current audio file
                AudioFile      *pSwap;          // Pointer to audio file for swapping between RT and non-RT code

                Toggle          sListen;        // Listen toggle
                Sample         *pSwapSample;
                Sample         *pCurrSample;    // Rendered file sample
                float          *vThumbs[impulse_reverb_base_metadata::TRACKS_MAX];           // Thumbnails
                float           fNorm;          // Norming factor
                bool            bRender;        // Flag that indicates that file needs rendering
                status_t        nStatus;
                bool            bSync;          // Synchronize file
                bool            bSwap;          // Swap samples

                float           fHeadCut;
                float           fTailCut;
                float           fFadeIn;
                float           fFadeOut;
                bool            bReverse;

                IRLoader        sLoader;        // Audio file loader task

                IPort          *pFile;          // Port that contains file name
                IPort          *pHeadCut;
                IPort          *pTailCut;
                IPort          *pFadeIn;
                IPort          *pFadeOut;
                IPort          *pListen;
                IPort          *pReverse;       // Reverse
                IPort          *pStatus;        // Status of file loading
                IPort          *pLength;        // Length of file
                IPort          *pThumbs;        // Thumbnails of file
            } af_descriptor_t;

            typedef struct convolver_t
            {
                Delay           sDelay;         // Delay line

                Convolver      *pCurr;          // Currently used convolver
                Convolver      *pSwap;          // Swap
//                bool            bSwap;          // Swapping flag
                size_t          nRank;          // Last applied rank
                size_t          nRankReq;       // Rank request
                size_t          nSource;        // Source
                size_t          nFileReq;       // File request
                size_t          nTrackReq;      // Track request

                float          *vBuffer;        // Buffer for convolution
                float           fPanIn[2];      // Input panning of convolver
                float           fPanOut[2];     // Output panning of convolver

                IPort          *pMakeup;        // Makeup gain of convolver
                IPort          *pPanIn;         // Input panning of convolver
                IPort          *pPanOut;        // Output panning of convolver
                IPort          *pFile;          // Convolver source file
                IPort          *pTrack;         // Convolver source file track
                IPort          *pPredelay;      // Pre-delay
                IPort          *pMute;          // Mute button
                IPort          *pActivity;      // Activity indicator
            } convolver_t;

            typedef struct channel_t
            {
                Bypass          sBypass;
                SamplePlayer    sPlayer;
                Equalizer       sEqualizer;     // Wet signal equalizer

                float          *vOut;
                float          *vBuffer;        // Rendering buffer
                float           fDryPan[2];     // Dry panorama

                IPort          *pOut;

                IPort          *pWetEq;         // Wet equalization flag
                IPort          *pLowCut;        // Low-cut flag
                IPort          *pLowFreq;       // Low-cut frequency
                IPort          *pHighCut;       // High-cut flag
                IPort          *pHighFreq;      // Low-cut frequency
                IPort          *pFreqGain[impulse_reverb_base_metadata::EQ_BANDS];   // Gain for each band of the Equalizer
            } channel_t;

            typedef struct input_t
            {
                float                  *vIn;        // Input data
                IPort                  *pIn;        // Input port
                IPort                  *pPan;       // Panning
            } input_t;

        protected:
            status_t                load(af_descriptor_t *descr);
            status_t                reconfigure(const reconfig_t *cfg);
            static void             destroy_file(af_descriptor_t *af);
            static void             destroy_channel(channel_t *c);
            static void             destroy_convolver(convolver_t *cv);
            static size_t           get_fft_rank(size_t rank);
            void                    sync_offline_tasks();

        protected:
            size_t                  nInputs;
            size_t                  nReconfigReq;
            size_t                  nReconfigResp;

            input_t                 vInputs[2];
            channel_t               vChannels[2];
            convolver_t             vConvolvers[impulse_reverb_base_metadata::CONVOLVERS];
            af_descriptor_t         vFiles[impulse_reverb_base_metadata::FILES];

            IRConfigurator          sConfigurator;

            IPort                  *pBypass;
            IPort                  *pRank;
            IPort                  *pDry;
            IPort                  *pWet;
            IPort                  *pOutGain;
            IPort                  *pPredelay;

            uint8_t                *pData;
            ipc::IExecutor         *pExecutor;

        public:
            impulse_reverb_base(const plugin_metadata_t &metadata, size_t inputs);
            virtual ~impulse_reverb_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void ui_activated();
            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);

    };

    class impulse_reverb_mono: public impulse_reverb_base, public impulse_reverb_mono_metadata
    {
        public:
            impulse_reverb_mono();
            virtual ~impulse_reverb_mono();
    };

    class impulse_reverb_stereo: public impulse_reverb_base, public impulse_reverb_stereo_metadata
    {
        public:
            impulse_reverb_stereo();
            virtual ~impulse_reverb_stereo();

    };

} /* namespace ddb */

#endif /* PLUGINS_IMPULSE_REVERB_H_ */
