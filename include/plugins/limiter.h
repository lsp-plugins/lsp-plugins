/*
 * limiter.h
 *
 *  Created on: 25 нояб. 2016 г.
 *      Author: sadko
 */

#ifndef PLUGINS_LIMITER_H_
#define PLUGINS_LIMITER_H_

#include <metadata/plugins.h>

#include <core/plugin.h>
#include <core/util/Bypass.h>
#include <core/util/Blink.h>
#include <core/util/Oversampler.h>
#include <core/util/MeterGraph.h>
#include <core/util/Dither.h>
#include <core/dynamics/Limiter.h>

namespace lsp
{
    class limiter_base: public plugin_t
    {
        protected:
            enum sc_graph_t
            {
                G_IN,
                G_OUT,
                G_SC,
                G_GAIN,

                G_TOTAL
            };

            typedef struct channel_t
            {
                Bypass          sBypass;            // Bypass
                Oversampler     sOver;              // Oversampler object for signal
                Oversampler     sScOver;            // Sidechain oversampler object for signal
                Limiter         sLimit;             // Limiter
                MeterGraph      sGraph[G_TOTAL];    // Input meter graph
                Blink           sBlink;             // Gain blink

                const float    *vIn;                // Input data
                const float    *vSc;                // Sidechain data
                float          *vOut;               // Output data

                float          *vDataBuf;           // Audio data buffer (oversampled)
                float          *vScBuf;             // Sidechain buffer
                float          *vGainBuf;           // Applying gain buffer
                float          *vOutBuf;            // Output buffer

                bool            bVisible[G_TOTAL];  // Input visibility
                bool            bOutVisible;        // Output visibility
                bool            bGainVisible;       // Gain visibility
                bool            bScVisible;         // Sidechain visibility

                IPort          *pIn;                // Input port
                IPort          *pOut;               // Output port
                IPort          *pSc;                // Sidechain port
                IPort          *pVisible[G_TOTAL];  // Input visibility

                IPort          *pGraph[G_TOTAL];    // History graphs
                IPort          *pMeter[G_TOTAL];    // Meters
            } channel_t;

        protected:
            size_t          nChannels;      // Number of channels
            bool            bSidechain;     // Sidechain presence flag
            channel_t      *vChannels;      // Audio channels
            float          *vTime;          // Time points buffer
            bool            bPause;         // Pause button
            bool            bClear;         // Clear button
            bool            bExtSc;         // External sidechain
            bool            bScListen;      // Sidechain listen
            float           fInGain;        // Input gain
            float           fOutGain;       // Output gain
            float           fPreamp;        // Sidechain pre-amplification
            float           fThresh;        // Limiter threshold
            float           fKnee;          // Limiter knee
            bool            bBoost;         // Gain boost
            size_t          nOversampling;  // Oversampling
            float           fStereoLink;    // Stereo linking
            float_buffer_t *pIDisplay;      // Inline display buffer
            bool            bUISync;        // Synchronize with UI

            Dither          sDither;        // Dither

            IPort          *pBypass;        // Bypass port
            IPort          *pInGain;        // Input gain
            IPort          *pOutGain;       // Output gain
            IPort          *pPreamp;        // Sidechain pre-amplification
            IPort          *pMode;          // Operating mode
            IPort          *pThresh;        // Limiter threshold
            IPort          *pLookahead;     // Lookahead time
            IPort          *pAttack;        // Attack time
            IPort          *pRelease;       // Release time
            IPort          *pPause;         // Pause gain
            IPort          *pClear;         // Cleanup gain
            IPort          *pExtSc;         // External sidechain
            IPort          *pScListen;      // Sidechain listen
            IPort          *pKnee;          // Limiter knee
            IPort          *pBoost;         // Gain boost
            IPort          *pOversampling;  // Oversampling
            IPort          *pDithering;     // Dithering
            IPort          *pStereoLink;    // Stereo linking

            uint8_t        *pData;          // Allocated data

        protected:
            static over_mode_t get_oversampling_mode(size_t mode);
            static bool get_filtering(size_t mode);
            static limiter_mode_t get_limiter_mode(size_t mode);
            static size_t get_dithering(size_t mode);
            void sync_latency();

        public:
            explicit limiter_base(const plugin_metadata_t &metadata, bool sc, bool stereo);
            virtual ~limiter_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void ui_activated();

            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
    };

    class limiter_mono: public limiter_base, public limiter_mono_metadata
    {
        public:
            explicit limiter_mono();
    };

    class limiter_stereo: public limiter_base, public limiter_stereo_metadata
    {
        public:
            explicit limiter_stereo();
    };

    class sc_limiter_mono: public limiter_base, public sc_limiter_mono_metadata
    {
        public:
            explicit sc_limiter_mono();
    };

    class sc_limiter_stereo: public limiter_base, public sc_limiter_stereo_metadata
    {
        public:
            explicit sc_limiter_stereo();
    };

}

#endif /* PLUGINS_LIMITER_H_ */
