/*
 * impulse_responses.h
 *
 *  Created on: 21 янв. 2015 г.
 *      Author: sadko
 */

#ifndef PLUGINS_IMPULSE_RESPONSES_H_
#define PLUGINS_IMPULSE_RESPONSES_H_

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

    class impulse_responses_base: public plugin_t
    {
        protected:
            class IRLoader;

            typedef struct reconfig_t
            {
                bool        bRender;
                size_t      nSource;
                size_t      nRank;
            } reconfig_t;

            typedef struct af_descriptor_t
            {
                AudioFile      *pCurr;
                AudioFile      *pSwap;

                Toggle          sListen;                // Listen toggle
                Sample         *pSwapSample;
                Sample         *pCurrSample;            // Rendered file sample
                float          *vThumbs[impulse_responses_base_metadata::TRACKS_MAX];           // Thumbnails
                float           fNorm;          // Norming factor
                bool            bRender;        // Flag that indicates that file needs rendering
                status_t        nStatus;
                bool            bSync;          // Synchronize file
                bool            bSwap;          // Swap samples

                float           fHeadCut;
                float           fTailCut;
                float           fFadeIn;
                float           fFadeOut;

                IRLoader       *pLoader;        // Audio file loader task

                IPort          *pFile;          // Port that contains file name
                IPort          *pHeadCut;
                IPort          *pTailCut;
                IPort          *pFadeIn;
                IPort          *pFadeOut;
                IPort          *pListen;
                IPort          *pStatus;        // Status of file loading
                IPort          *pLength;        // Length of file
                IPort          *pThumbs;        // Thumbnails of file
            } af_descriptor_t;

            typedef struct channel_t
            {
                Bypass          sBypass;
                Delay           sDelay;
                SamplePlayer    sPlayer;
                Equalizer       sEqualizer;     // Wet signal equalizer

                Convolver      *pCurr;
                Convolver      *pSwap;

                float          *vIn;
                float          *vOut;
                float          *vBuffer;
                float           fDryGain;
                float           fWetGain;
                size_t          nSource;
                size_t          nSourceReq;
                size_t          nRank;
                size_t          nRankReq;

                IPort          *pIn;
                IPort          *pOut;

                IPort          *pSource;
                IPort          *pMakeup;
                IPort          *pActivity;
                IPort          *pPredelay;

                IPort          *pWetEq;         // Wet equalization flag
                IPort          *pLowCut;        // Low-cut flag
                IPort          *pLowFreq;       // Low-cut frequency
                IPort          *pHighCut;       // High-cut flag
                IPort          *pHighFreq;      // Low-cut frequency
                IPort          *pFreqGain[impulse_responses_base_metadata::EQ_BANDS];   // Gain for each band of the Equalizer
            } channel_t;

            class IRLoader: public ipc::ITask
            {
                private:
                    impulse_responses_base     *pCore;
                    af_descriptor_t            *pDescr;

                public:
                    IRLoader(impulse_responses_base *base, af_descriptor_t *descr);
                    virtual ~IRLoader();

                public:
                    virtual status_t run();
            };

            class IRConfigurator: public ipc::ITask
            {
                private:
                    reconfig_t                  sReconfig[impulse_responses_base_metadata::TRACKS_MAX];
                    impulse_responses_base     *pCore;

                public:
                    IRConfigurator(impulse_responses_base *base);
                    virtual ~IRConfigurator();

                public:
                    virtual status_t run();

                    inline void set_render(size_t idx, bool render)     { sReconfig[idx].bRender    = render; }
                    inline void set_source(size_t idx, size_t source)   { sReconfig[idx].nSource    = source; }
                    inline void set_rank(size_t idx, size_t rank)       { sReconfig[idx].nRank      = rank; }
            };

        protected:
            status_t                load(af_descriptor_t *descr);
            status_t                reconfigure(const reconfig_t *cfg);
            static void             destroy_file(af_descriptor_t *af);
            static void             destroy_channel(channel_t *c);
            static size_t           get_fft_rank(size_t rank);

        protected:
            IRConfigurator          sConfigurator;

            size_t                  nChannels;
            channel_t              *vChannels;
            af_descriptor_t        *vFiles;
            ipc::IExecutor         *pExecutor;
            size_t                  nReconfigReq;
            size_t                  nReconfigResp;
            float                   fGain;

            IPort                  *pBypass;
            IPort                  *pRank;
            IPort                  *pDry;
            IPort                  *pWet;
            IPort                  *pOutGain;

            uint8_t                *pData;

        public:
            impulse_responses_base(const plugin_metadata_t &metadata, size_t channels);
            virtual ~impulse_responses_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void ui_activated();
            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);

    };

    class impulse_responses_mono: public impulse_responses_base, public impulse_responses_mono_metadata
    {
        public:
            impulse_responses_mono();
            virtual ~impulse_responses_mono();
    };

    class impulse_responses_stereo: public impulse_responses_base, public impulse_responses_stereo_metadata
    {
        public:
            impulse_responses_stereo();
            virtual ~impulse_responses_stereo();

    };

} /* namespace ddb */

#endif /* PLUGINS_IMPULSE_RESPONSES_H_ */
