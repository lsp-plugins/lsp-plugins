/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 4 нояб. 2016 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PLUGINS_EXPANDER_H_
#define PLUGINS_EXPANDER_H_

#include <metadata/plugins.h>

#include <core/plugin.h>
#include <core/util/Bypass.h>
#include <core/util/Sidechain.h>
#include <core/util/MeterGraph.h>
#include <core/util/Delay.h>
#include <core/dynamics/Expander.h>

namespace lsp
{
    class expander_base: public plugin_t
    {
        protected:
            enum c_mode_t
            {
                EM_MONO,
                EM_STEREO,
                EM_LR,
                EM_MS
            };

            enum sc_source_t
            {
                SCT_INTERNAL,
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
                Equalizer       sSCEq;              // Sidechain equalizer
                Expander        sExp;               // Expansion module
                Delay           sDelay;             // Lookahead delay
                Delay           sCompDelay;         // Compensation delay
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
                IPort          *pScHpfMode;         // Sidechain high-pass filter mode
                IPort          *pScHpfFreq;         // Sidechain high-pass filter frequency
                IPort          *pScLpfMode;         // Sidechain low-pass filter mode
                IPort          *pScLpfFreq;         // Sidechain low-pass filter frequency

                IPort          *pMode;              // Mode
                IPort          *pAttackLvl;         // Attack level
                IPort          *pReleaseLvl;        // Release level
                IPort          *pAttackTime;        // Attack time
                IPort          *pReleaseTime;       // Release time
                IPort          *pRatio;             // Ratio
                IPort          *pKnee;              // Knee
                IPort          *pMakeup;            // Makeup

                IPort          *pDryGain;           // Dry gain
                IPort          *pWetGain;           // Wet gain
                IPort          *pCurve;             // Curve graph
                IPort          *pReleaseOut;        // Output release level
            } channel_t;

        protected:
            size_t          nMode;          // Expander mode
            bool            bSidechain;     // External side chain
            channel_t      *vChannels;      // Expander channels
            float          *vCurve;         // Expander curve
            float          *vTime;          // Time points buffer
            bool            bPause;         // Pause button
            bool            bClear;         // Clear button
            bool            bMSListen;      // Mid/Side listen
            float           fInGain;        // Input gain
            bool            bUISync;        // UI Sync
            float_buffer_t *pIDisplay;      // Inline display buffer

            IPort          *pBypass;        // Bypass port
            IPort          *pInGain;        // Input gain
            IPort          *pOutGain;       // Output gain
            IPort          *pPause;         // Pause gain
            IPort          *pClear;         // Cleanup gain
            IPort          *pMSListen;      // Mid/Side listen

            uint8_t        *pData;          // Expander data

        public:
            expander_base(const plugin_metadata_t &metadata, bool sc, size_t mode);
            virtual ~expander_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void ui_activated();

            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
    };

    class expander_mono: public expander_base, public expander_mono_metadata
    {
        public:
            expander_mono();
    };

    class expander_stereo: public expander_base, public expander_stereo_metadata
    {
        public:
            expander_stereo();
    };

    class expander_lr: public expander_base, public expander_lr_metadata
    {
        public:
            expander_lr();
    };

    class expander_ms: public expander_base, public expander_ms_metadata
    {
        public:
            expander_ms();
    };

    class sc_expander_mono: public expander_base, public sc_expander_mono_metadata
    {
        public:
            sc_expander_mono();
    };

    class sc_expander_stereo: public expander_base, public sc_expander_stereo_metadata
    {
        public:
            sc_expander_stereo();
    };

    class sc_expander_lr: public expander_base, public sc_expander_lr_metadata
    {
        public:
            sc_expander_lr();
    };

    class sc_expander_ms: public expander_base, public sc_expander_ms_metadata
    {
        public:
            sc_expander_ms();
    };

}

#endif /* PLUGINS_EXPANDER_H_ */
