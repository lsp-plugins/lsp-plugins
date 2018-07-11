/*
 * dyna_processor.h
 *
 *  Created on: 21 окт. 2016 г.
 *      Author: sadko
 */

#ifndef PLUGINS_DYNA_PROCESSOR_H_
#define PLUGINS_DYNA_PROCESSOR_H_

#ifdef LSP_NO_KVRDC16

#include <metadata/plugins.h>

#include <core/plugin.h>
#include <core/Bypass.h>
#include <core/Sidechain.h>
#include <core/DynamicProcessor.h>
#include <core/MeterGraph.h>

namespace lsp
{
    class dyna_processor_base: public plugin_t
    {
        protected:
            enum c_mode_t
            {
                DYNA_MONO,
                DYNA_STEREO,
                DYNA_LR,
                DYNA_MS
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
                S_MODEL     = 1 << 1,

                S_ALL       = S_MODEL
            };

            typedef struct channel_t
            {
                Bypass              sBypass;            // Bypass
                Sidechain           sSC;                // Sidechain module
                DynamicProcessor    sProc;              // Processor module
                MeterGraph          sGraph[G_TOTAL];    // Meter graph

                float              *vIn;                // Input data
                float              *vOut;               // Output data
                float              *vSc;                // Sidechain data
                float              *vEnv;               // Envelope data
                float              *vGain;              // Gain reduction data
                bool                bScActive;          // External sidechain is active
                bool                bScListen;          // Listen sidechain
                size_t              nSync;              // Synchronization flags
                size_t              nScType;            // Sidechain mode
                float               fMakeup;            // Makeup gain
                float               fFeedback;          // Feedback
                float               fDryGain;           // Dry gain
                float               fWetGain;           // Wet gain
                float               fDotIn;             // Dot input gain
                float               fDotOut;            // Dot output gain

                IPort              *pIn;                // Input port
                IPort              *pOut;               // Output port
                IPort              *pSC;                // Sidechain port

                IPort              *pGraph[G_TOTAL];    // History graphs
                IPort              *pMeter[M_TOTAL];    // Meters

                IPort              *pScType;            // Sidechain location
                IPort              *pScMode;            // Sidechain mode
                IPort              *pScListen;          // Sidechain listen
                IPort              *pScSource;          // Sidechain source
                IPort              *pScReactivity;      // Sidechain reactivity
                IPort              *pScPreamp;          // Sidechain pre-amplification

                IPort              *pDotOn[dyna_processor_base_metadata::DOTS];         // Dot enable
                IPort              *pThreshold[dyna_processor_base_metadata::DOTS];     // Threshold levels
                IPort              *pGain[dyna_processor_base_metadata::DOTS];          // Gain levels
                IPort              *pKnee[dyna_processor_base_metadata::DOTS];          // Knees levels
                IPort              *pAttackOn[dyna_processor_base_metadata::DOTS];      // Attack enable
                IPort              *pAttackLvl[dyna_processor_base_metadata::DOTS];     // Attack levels
                IPort              *pAttackTime[dyna_processor_base_metadata::RANGES];  // Attack time
                IPort              *pReleaseOn[dyna_processor_base_metadata::DOTS];     // Release enable
                IPort              *pReleaseLvl[dyna_processor_base_metadata::DOTS];    // Release levels
                IPort              *pReleaseTime[dyna_processor_base_metadata::RANGES]; // Release time
                IPort              *pLowRatio;          // Low Ratio
                IPort              *pHighRatio;         // High Ratio
                IPort              *pMakeup;            // Overall Makeup gain

                IPort              *pDryGain;           // Dry gain
                IPort              *pWetGain;           // Wet gain
                IPort              *pCurve;             // Curve graph
                IPort              *pModel;             // Curve model
            } channel_t;

        protected:
            size_t          nMode;          // Working mode
            bool            bSidechain;     // External side chain
            channel_t      *vChannels;      // Audio channels
            float          *vCurve;         // Dynamic curve
            float          *vTime;          // Time points buffer
            bool            bPause;         // Pause button
            bool            bClear;         // Clear button
            bool            bMSListen;      // Mid/Side listen
            float           fInGain;        // Input gain
            float_buffer_t *pIDisplay;      // Inline display buffer

            IPort          *pBypass;        // Bypass port
            IPort          *pInGain;        // Input gain
            IPort          *pOutGain;       // Output gain
            IPort          *pPause;         // Pause gain
            IPort          *pClear;         // Cleanup gain
            IPort          *pMSListen;      // Mid/Side listen

            uint8_t        *pData;          // Allocated data

        protected:
            float           process_feedback(channel_t *c, size_t i, size_t channels);
            void            process_non_feedback(channel_t *c, float **in, size_t samples);

        public:
            dyna_processor_base(const plugin_metadata_t &metadata, bool sc, size_t mode);
            virtual ~dyna_processor_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void ui_activated();

            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
    };

    class dyna_processor_mono: public dyna_processor_base, public dyna_processor_mono_metadata
    {
        public:
            dyna_processor_mono();
    };

    class dyna_processor_stereo: public dyna_processor_base, public dyna_processor_stereo_metadata
    {
        public:
            dyna_processor_stereo();
    };

    class dyna_processor_lr: public dyna_processor_base, public dyna_processor_lr_metadata
    {
        public:
            dyna_processor_lr();
    };

    class dyna_processor_ms: public dyna_processor_base, public dyna_processor_ms_metadata
    {
        public:
            dyna_processor_ms();
    };

    class sc_dyna_processor_mono: public dyna_processor_base, public sc_dyna_processor_mono_metadata
    {
        public:
            sc_dyna_processor_mono();
    };

    class sc_dyna_processor_stereo: public dyna_processor_base, public sc_dyna_processor_stereo_metadata
    {
        public:
            sc_dyna_processor_stereo();
    };

    class sc_dyna_processor_lr: public dyna_processor_base, public sc_dyna_processor_lr_metadata
    {
        public:
            sc_dyna_processor_lr();
    };

    class sc_dyna_processor_ms: public dyna_processor_base, public sc_dyna_processor_ms_metadata
    {
        public:
            sc_dyna_processor_ms();
    };

}

#endif

#endif /* PLUGINS_DYNA_PROCESSOR_H_ */
