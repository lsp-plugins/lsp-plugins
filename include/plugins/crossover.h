/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 8 сент. 2020 г.
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

#ifndef PLUGINS_CROSSOVER_H_
#define PLUGINS_CROSSOVER_H_

#include <metadata/plugins.h>

#include <core/plugin.h>
#include <core/util/Bypass.h>
#include <core/util/Analyzer.h>
#include <core/util/Crossover.h>

namespace lsp
{
    class crossover_base: public plugin_t
    {
        protected:
            enum c_mode_t
            {
                XOVER_MONO,
                XOVER_STEREO,
                XOVER_LR,
                XOVER_MS
            };

            typedef struct xover_band_t
            {
                float          *vBandOut;           // Output buffer for one band
                float          *vAllOut;            // Summarized output buffer for all buffers
                float          *vTr;                // Transfer function

                float           fMakeup;            // Makeup gain

                IPort          *pOut;               // Output port
                IPort          *pSolo;              // Soloing
                IPort          *pMute;              // Muting
                IPort          *pMakeup;            // Makeup gain
                IPort          *pFreqEnd;           // Frequency range end
            } xover_band_t;

            typedef struct xover_split_t
            {
                IPort          *pSlope;             // Slope
                IPort          *pFreq;              // Split frequency
            } xover_split_t;

            typedef struct channel_t
            {
                Bypass          sBypass;            // Bypass
                Crossover       sXOver;             // Crossover module

                xover_band_t    vBands[crossover_base_metadata::BANDS_MAX];     // Crossover bands
                xover_split_t   vSplit[crossover_base_metadata::BANDS_MAX-1];   // Split bands

                float          *vIn;                // Input buffer
                float          *vOut;               // Output buffer
                float          *vBuffer;            // Common data processing buffer
                float          *vTr;                // Transfer function

                size_t          nAnInChannel;       // Analyzer channel used for input signal analysis
                size_t          nAnOutChannel;      // Analyzer channel used for output signal analysis
                bool            bInFft;             // Input signal FFT enabled
                bool            bOutFft;            // Output signal FFT enabled

                IPort          *pIn;                // Input
                IPort          *pOut;               // Output
                IPort          *pFftIn;             // Pre-processing FFT analysis data
                IPort          *pFftInSw;           // Pre-processing FFT analysis control port
                IPort          *pFftOut;            // Post-processing FFT analysis data
                IPort          *pFftOutSw;          // Post-processing FFT analysis controlport
                IPort          *pAmpGraph;          // Crossover amplitude graph
                IPort          *pInLvl;             // Input level meter
                IPort          *pOutLvl;            // Output level meter
            } channel_t;

        protected:
            Analyzer        sAnalyzer;              // Analyzer
            size_t          nMode;                  // Crossover mode
            bool            bEnvUpdate;             // Envelope filter update
            channel_t      *vChannels;              // Crossover channels
            float           fInGain;                // Input gain
            float           fOutGain;               // Output gain
            float           fZoom;                  // Zoom

            uint8_t        *pData;                  // Aligned data pointer
            float          *vTr;                    // Transfer buffer
            float          *vBFc;                   // Band filter characteristics buffer
            float          *vFreqs;                 // Analyzer FFT frequencies
            float          *vCurve;                 // Curve
            uint32_t       *vIndexes;               // Analyzer FFT indexes
            float_buffer_t *pIDisplay;              // Inline display buffer

            IPort          *pBypass;                // Bypass port
            IPort          *pInGain;                // Input gain port
            IPort          *pOutGain;               // Output gain port
            IPort          *pReactivity;            // Reactivity
            IPort          *pShiftGain;             // Shift gain port
            IPort          *pZoom;                  // Zoom port
            IPort          *pMSOut;                 // Mid/Side output

        public:
            explicit crossover_base(const plugin_metadata_t &metadata, size_t mode);
            virtual ~crossover_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void ui_activated();

            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);

            virtual void dump(IStateDumper *v) const;
    };

    class crossover_mono: public crossover_base, public crossover_mono_metadata
    {
        public:
            explicit crossover_mono();
    };

    class crossover_stereo: public crossover_base, public crossover_stereo_metadata
    {
        public:
            explicit crossover_stereo();
    };

    class crossover_lr: public crossover_base, public crossover_lr_metadata
    {
        public:
            explicit crossover_lr();
    };

    class crossover_ms: public crossover_base, public crossover_ms_metadata
    {
        public:
            explicit crossover_ms();
    };
}



#endif /* INCLUDE_PLUGINS_CROSSOVER_H_ */
