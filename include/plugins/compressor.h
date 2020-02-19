/*
 * compressor.h
 *
 *  Created on: 16 сент. 2016 г.
 *      Author: sadko
 */

#ifndef PLUGINS_COMPRESSOR_H_
#define PLUGINS_COMPRESSOR_H_

#include <metadata/plugins.h>

#include <core/plugin.h>
#include <core/util/Bypass.h>
#include <core/util/Sidechain.h>
#include <core/util/Delay.h>
#include <core/util/MeterGraph.h>
#include <core/dynamics/Compressor.h>

namespace lsp
{
    class compressor_base: public plugin_t
    {
        protected:
            enum c_mode_t
            {
                CM_MONO,
                CM_STEREO,
                CM_LR,
                CM_MS
            };

            enum sc_source_t
            {
                SCT_FEED_FORWARD,
                SCT_FEED_BACK,
                SCT_EXTERNAL
            };

            enum sc_graph_t
            {
                G_IN,
                G_SC,
                G_ENV,
                G_GAIN,
                G_OUT,

                G_TOTAL
            };

            enum sc_meter_t
            {
                M_IN,
                M_SC,
                M_ENV,
                M_GAIN,
                M_CURVE,
                M_OUT,

                M_TOTAL
            };

            enum sync_t
            {
                S_CURVE     = 1 << 0,

                S_ALL       = S_CURVE
            };

            typedef struct channel_t
            {
                Bypass          sBypass;            // Bypass
                Sidechain       sSC;                // Sidechain module
                Compressor      sComp;              // Compression module
                Delay           sDelay;             // Lookahead delay
                MeterGraph      sGraph[G_TOTAL];    // Input meter graph

                float          *vIn;                // Input data
                float          *vOut;               // Output data
                float          *vSc;                // Sidechain data
                float          *vEnv;               // Envelope data
                float          *vGain;              // Gain reduction data
                bool            bScListen;          // Listen sidechain
                size_t          nSync;              // Synchronization flags
                size_t          nScType;            // Sidechain mode
                float           fMakeup;            // Makeup gain
                float           fFeedback;          // Feedback
                float           fDryGain;           // Dry gain
                float           fWetGain;           // Wet gain
                float           fDotIn;             // Dot input gain
                float           fDotOut;            // Dot output gain

                IPort          *pIn;                // Input port
                IPort          *pOut;               // Output port
                IPort          *pSC;                // Sidechain port

                IPort          *pGraph[G_TOTAL];    // History graphs
                IPort          *pMeter[M_TOTAL];    // Meters

                IPort          *pScType;            // Sidechain location
                IPort          *pScMode;            // Sidechain mode
                IPort          *pScLookahead;       // Sidechain lookahead
                IPort          *pScListen;          // Sidechain listen
                IPort          *pScSource;          // Sidechain source
                IPort          *pScReactivity;      // Sidechain reactivity
                IPort          *pScPreamp;          // Sidechain pre-amplification

                IPort          *pMode;              // Mode
                IPort          *pAttackLvl;         // Attack level
                IPort          *pReleaseLvl;        // Release level
                IPort          *pAttackTime;        // Attack time
                IPort          *pReleaseTime;       // Release time
                IPort          *pRatio;             // Ratio
                IPort          *pKnee;              // Knee
                IPort          *pBThresh;           // Boost threshold
                IPort          *pMakeup;            // Makeup

                IPort          *pDryGain;           // Dry gain
                IPort          *pWetGain;           // Wet gain
                IPort          *pCurve;             // Curve graph
                IPort          *pReleaseOut;        // Output release level
            } channel_t;

        protected:
            size_t          nMode;          // Compressor mode
            bool            bSidechain;     // External side chain
            channel_t      *vChannels;      // Compressor channels
            float          *vCurve;         // Compressor curve
            float          *vTime;          // Time points buffer
            bool            bPause;         // Pause button
            bool            bClear;         // Clear button
            bool            bMSListen;      // Mid/Side listen
            float           fInGain;        // Input gain
            bool            bUISync;
            float_buffer_t *pIDisplay;      // Inline display buffer

            IPort          *pBypass;        // Bypass port
            IPort          *pInGain;        // Input gain
            IPort          *pOutGain;       // Output gain
            IPort          *pPause;         // Pause gain
            IPort          *pClear;         // Cleanup gain
            IPort          *pMSListen;      // Mid/Side listen

            uint8_t        *pData;          // Compressor data

        protected:
            float           process_feedback(channel_t *c, size_t i, size_t channels);
            void            process_non_feedback(channel_t *c, float **in, size_t samples);

        public:
            explicit compressor_base(const plugin_metadata_t &metadata, bool sc, size_t mode);
            virtual ~compressor_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void ui_activated();

            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
    };

    class compressor_mono: public compressor_base, public compressor_mono_metadata
    {
        public:
            compressor_mono();
    };

    class compressor_stereo: public compressor_base, public compressor_stereo_metadata
    {
        public:
            compressor_stereo();
    };

    class compressor_lr: public compressor_base, public compressor_lr_metadata
    {
        public:
            compressor_lr();
    };

    class compressor_ms: public compressor_base, public compressor_ms_metadata
    {
        public:
            compressor_ms();
    };

    class sc_compressor_mono: public compressor_base, public sc_compressor_mono_metadata
    {
        public:
            sc_compressor_mono();
    };

    class sc_compressor_stereo: public compressor_base, public sc_compressor_stereo_metadata
    {
        public:
            sc_compressor_stereo();
    };

    class sc_compressor_lr: public compressor_base, public sc_compressor_lr_metadata
    {
        public:
            sc_compressor_lr();
    };

    class sc_compressor_ms: public compressor_base, public sc_compressor_ms_metadata
    {
        public:
            sc_compressor_ms();
    };

}

#endif /* PLUGINS_COMPRESSOR_H_ */
