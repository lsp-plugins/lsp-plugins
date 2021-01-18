/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 18 мая 2020 г.
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

#ifndef PLUGINS_MB_GATE_H_
#define PLUGINS_MB_GATE_H_

#include <metadata/plugins.h>
#include <core/util/Bypass.h>
#include <core/util/Sidechain.h>
#include <core/util/Delay.h>
#include <core/util/MeterGraph.h>
#include <core/util/Analyzer.h>
#include <core/dynamics/Gate.h>
#include <core/filters/DynamicFilters.h>
#include <core/filters/Equalizer.h>
#include <core/plugin.h>

namespace lsp
{
    class mb_gate_base: public plugin_t
    {
        protected:
            enum c_mode_t
            {
                MBEM_MONO,
                MBEM_STEREO,
                MBEM_LR,
                MBEM_MS
            };

            enum sync_t
            {
                S_GATE_CURVE    = 1 << 0,
                S_HYST_CURVE    = 1 << 1,
                S_EQ_CURVE      = 1 << 2,

                S_DYN_CURVE     = S_GATE_CURVE | S_HYST_CURVE,
                S_ALL           = S_DYN_CURVE | S_EQ_CURVE
            };

            typedef struct gate_band_t
            {
                Sidechain       sSC;                // Sidechain module
                Equalizer       sEQ[2];             // Sidechain equalizers
                Gate            sGate;              // Gate
                Filter          sPassFilter;        // Passing filter for 'classic' mode
                Filter          sRejFilter;         // Rejection filter for 'classic' mode
                Filter          sAllFilter;         // All-pass filter for phase compensation
                Delay           sDelay;             // Delay for lookahead purpose

                float          *vTr;                // Transfer function
                float          *vVCA;               // Voltage-controlled amplification value for each band
                float           fScPreamp;          // Sidechain preamp

                float           fFreqStart;
                float           fFreqEnd;

                float           fFreqHCF;           // Cutoff frequency for low-pass filter
                float           fFreqLCF;           // Cutoff frequency for high-pass filter
                float           fMakeup;            // Makeup gain
                float           fEnvLevel;          // Envelope level
                float           fGainLevel;         // Gain adjustment level
                size_t          nLookahead;         // Lookahead amount

                bool            bEnabled;           // Enabled flag
                bool            bCustHCF;           // Custom frequency for high-cut filter
                bool            bCustLCF;           // Custom frequency for low-cut filter
                bool            bMute;              // Mute channel
                bool            bSolo;              // Solo channel
                bool            bExtSc;             // External sidechain
                size_t          nSync;              // Synchronize output data flags
                size_t          nFilterID;          // Identifier of the filter

                IPort          *pExtSc;             // External sidechain
                IPort          *pScSource;          // Sidechain source
                IPort          *pScMode;            // Sidechain mode
                IPort          *pScLook;            // Sidechain lookahead
                IPort          *pScReact;           // Sidechain reactivity
                IPort          *pScPreamp;          // Sidechain preamp
                IPort          *pScLpfOn;           // Sidechain low-pass on
                IPort          *pScHpfOn;           // Sidechain hi-pass on
                IPort          *pScLcfFreq;         // Sidechain low-cut frequency
                IPort          *pScHcfFreq;         // Sidechain hi-cut frequency
                IPort          *pScFreqChart;       // Sidechain band frequency chart

                IPort          *pEnable;            // Enable gate
                IPort          *pSolo;              // Soloing
                IPort          *pMute;              // Muting
                IPort          *pHyst;              // Hysteresis flag
                IPort          *pThresh[2];         // Threshold
                IPort          *pZone[2];           // Reduction zone
                IPort          *pAttack;            // Attack time
                IPort          *pRelease;           // Release time
                IPort          *pReduction;         // Reduction
                IPort          *pMakeup;            // Makeup gain

                IPort          *pFreqEnd;           // Frequency range end
                IPort          *pCurveGraph[2];     // Gate curve graph
                IPort          *pEnvLvl;            // Envelope level meter
                IPort          *pCurveLvl;          // Reduction curve level meter
                IPort          *pMeterGain;         // Reduction gain meter
            } gate_band_t;

            typedef struct split_t
            {
                bool            bEnabled;           // Split band is enabled
                float           fFreq;              // Split band frequency

                IPort          *pEnabled;           // Enable port
                IPort          *pFreq;              // Split frequency
            } split_t;

            typedef struct channel_t
            {
                Bypass          sBypass;            // Bypass
                Filter          sEnvBoost[2];       // Envelope boost filter
                Delay           sDelay;             // Delay for lookahead purpose

                gate_band_t     vBands[mb_gate_base_metadata::BANDS_MAX];       // Gateander bands
                split_t         vSplit[mb_gate_base_metadata::BANDS_MAX-1];     // Split bands
                gate_band_t    *vPlan[mb_gate_base_metadata::BANDS_MAX];        // Execution plan (band indexes)
                size_t          nPlanSize;              // Plan size

                float          *vIn;                // Input data buffer
                float          *vOut;               // Output data buffer
                float          *vScIn;              // Sidechain data buffer (if present)

                float          *vBuffer;            // Common data processing buffer
                float          *vScBuffer;          // Sidechain buffer
                float          *vExtScBuffer;       // External sidechain buffer
                float          *vTr;                // Transfer function
                float          *vTrMem;             // Transfer buffer (memory)
                float          *vInAnalyze;         // Input signal analysis
                float          *vOutAnalyze;        // Input signal analysis

                size_t          nAnInChannel;       // Analyzer channel used for input signal analysis
                size_t          nAnOutChannel;      // Analyzer channel used for output signal analysis
                bool            bInFft;             // Input signal FFT enabled
                bool            bOutFft;            // Output signal FFT enabled

                IPort          *pIn;                // Input
                IPort          *pOut;               // Output
                IPort          *pScIn;              // Sidechain
                IPort          *pFftIn;             // Pre-processing FFT analysis data
                IPort          *pFftInSw;           // Pre-processing FFT analysis control port
                IPort          *pFftOut;            // Post-processing FFT analysis data
                IPort          *pFftOutSw;          // Post-processing FFT analysis controlport
                IPort          *pAmpGraph;          // Gate's amplitude graph
                IPort          *pInLvl;             // Input level meter
                IPort          *pOutLvl;            // Output level meter
            } channel_t;

        protected:
            Analyzer        sAnalyzer;              // Analyzer
            DynamicFilters  sFilters;               // Dynamic filters for each band in 'modern' mode
            size_t          nMode;                  // Gate channel mode
            bool            bSidechain;             // External side chain
            bool            bEnvUpdate;             // Envelope filter update
            bool            bModern;                // Modern mode
            size_t          nEnvBoost;              // Envelope boost
            channel_t      *vChannels;              // Gate channels
            float           fInGain;                // Input gain
            float           fDryGain;               // Dry gain
            float           fWetGain;               // Wet gain
            float           fZoom;                  // Zoom
            uint8_t        *pData;                  // Aligned data pointer
            float          *vSc[2];                 // Sidechain signal data
            float          *vAnalyze[4];            // Analysis buffer
            float          *vBuffer;                // Temporary buffer
            float          *vEnv;                   // Gate envelope buffer
            float          *vTr;                    // Transfer buffer
            float          *vPFc;                   // Pass filter characteristics buffer
            float          *vRFc;                   // Reject filter characteristics buffer
            float          *vFreqs;                 // Analyzer FFT frequencies
            float          *vCurve;                 // Curve
            uint32_t       *vIndexes;               // Analyzer FFT indexes
            float_buffer_t *pIDisplay;              // Inline display buffer

            IPort          *pBypass;                // Bypass port
            IPort          *pMode;                  // Global operating mode
            IPort          *pInGain;                // Input gain port
            IPort          *pOutGain;               // Output gain port
            IPort          *pDryGain;               // Dry gain port
            IPort          *pWetGain;               // Wet gain port
            IPort          *pReactivity;            // Reactivity
            IPort          *pShiftGain;             // Shift gain port
            IPort          *pZoom;                  // Zoom port
            IPort          *pEnvBoost;              // Envelope adjust

        protected:
            static bool compare_bands_for_sort(const gate_band_t *b1, const gate_band_t *b2);

        public:
            explicit mb_gate_base(const plugin_metadata_t &metadata, bool sc, size_t mode);
            virtual ~mb_gate_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void ui_activated();

            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
    };

    //-------------------------------------------------------------------------
    // Different gate implementations
    class mb_gate_mono: public mb_gate_base, public mb_gate_mono_metadata
    {
        public:
            mb_gate_mono();
    };

    class mb_gate_stereo: public mb_gate_base, public mb_gate_stereo_metadata
    {
        public:
            mb_gate_stereo();
    };

    class mb_gate_lr: public mb_gate_base, public mb_gate_lr_metadata
    {
        public:
            mb_gate_lr();
    };

    class mb_gate_ms: public mb_gate_base, public mb_gate_ms_metadata
    {
        public:
            mb_gate_ms();
    };

    class sc_mb_gate_mono: public mb_gate_base, public sc_mb_gate_mono_metadata
    {
        public:
            sc_mb_gate_mono();
    };

    class sc_mb_gate_stereo: public mb_gate_base, public sc_mb_gate_stereo_metadata
    {
        public:
            sc_mb_gate_stereo();
    };

    class sc_mb_gate_lr: public mb_gate_base, public sc_mb_gate_lr_metadata
    {
        public:
            sc_mb_gate_lr();
    };

    class sc_mb_gate_ms: public mb_gate_base, public sc_mb_gate_ms_metadata
    {
        public:
            sc_mb_gate_ms();
    };
}


#endif /* PLUGINS_MB_GATE_H_ */
