/*
 * graph_equalizer.h
 *
 *  Created on: 03 авг. 2016 г.
 *      Author: sadko
 */

#ifndef PLUGINS_GRAPH_EQUALIZER_H_
#define PLUGINS_GRAPH_EQUALIZER_H_

#include <metadata/plugins.h>
#include <core/plugin.h>
#include <core/util/Bypass.h>
#include <core/util/Analyzer.h>
#include <core/filters/Equalizer.h>

namespace lsp
{
    class graph_equalizer_base: public plugin_t
    {
        protected:
            enum eq_mode_t
            {
                EQ_MONO,
                EQ_STEREO,
                EQ_LEFT_RIGHT,
                EQ_MID_SIDE
            };

            enum chart_state_t
            {
                CS_UPDATE       = 1 << 0,
                CS_SYNC_AMP     = 1 << 1
            };

            enum fft_position_t
            {
                FFTP_NONE,
                FFTP_POST,
                FFTP_PRE
            };

            typedef struct eq_band_t
            {
                bool        bSolo;          // Solo
                size_t      nSync;          // Chart state
                float      *vTrRe;          // Transfer function (real part)
                float      *vTrIm;          // Transfer function (imaginary part)

                IPort      *pGain;          // Gain port
                IPort      *pSolo;          // Solo port
                IPort      *pMute;          // Mute port
                IPort      *pEnable;        // Enable port
                IPort      *pVisibility;    // Filter visibility
            } eq_band_t;

            typedef struct eq_channel_t
            {
                Equalizer           sEqualizer;     // Equalizer
                Bypass              sBypass;        // Bypass

                size_t              nSync;          // Chart state
                float               fInGain;        // Input gain
                float               fOutGain;       // Output gain
                eq_band_t          *vBands;         // Bands
                float              *vIn;            // Input buffer
                float              *vOut;           // Output buffer
                float              *vBuffer;        // Temporary buffer

                float              *vTrRe;          // Transfer function (real part)
                float              *vTrIm;          // Transfer function (imaginary part)

                IPort              *pIn;            // Input port
                IPort              *pOut;           // Output port
                IPort              *pInGain;        // Input gain
                IPort              *pTrAmp;         // Amplitude chart
                IPort              *pFft;           // FFT chart
                IPort              *pVisible;       // Visibility flag
                IPort              *pInMeter;       // Output level meter
                IPort              *pOutMeter;      // Output level meter
            } eq_channel_t;

        protected:
            static const float band_frequencies[];

        protected:
            inline equalizer_mode_t     get_eq_mode();

        protected:
            Analyzer            sAnalyzer;      // Analyzer
            eq_channel_t       *vChannels;      // Equalizer channels
            size_t              nBands;         // Number of bands
            size_t              nMode;          // Equalize mode
            size_t              nFftPosition;   // FFT analysis position
            size_t              nSlope;         // Slope
            bool                bListen;        // Listen
            bool                bMatched;       // Matched transorm/Bilinear transform flag
            float               fInGain;        // Input gain
            float               fZoom;          // Zoom gain
            float              *vFreqs;         // Frequency list
            uint32_t           *vIndexes;       // FFT indexes
            float_buffer_t     *pIDisplay;      // Inline display buffer

            IPort              *pEqMode;        // Equalizer mode
            IPort              *pSlope;         // Filter slope
            IPort              *pListen;        // Mid-Side listen
            IPort              *pInGain;        // Input gain
            IPort              *pOutGain;       // Output gain
            IPort              *pBypass;        // Bypass
            IPort              *pFftMode;       // FFT mode
            IPort              *pReactivity;    // FFT reactivity
            IPort              *pShiftGain;     // Shift gain
            IPort              *pZoom;          // Graph zoom
            IPort              *pBalance;       // Output balance

        public:
            graph_equalizer_base(const plugin_metadata_t &metadata, size_t bands, size_t mode);
            virtual ~graph_equalizer_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void ui_activated();

            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
    };

    class graph_equalizer_x16_mono: public graph_equalizer_base, public graph_equalizer_x16_mono_metadata
    {
        public:
            graph_equalizer_x16_mono();
    };

    class graph_equalizer_x32_mono: public graph_equalizer_base, public graph_equalizer_x32_mono_metadata
    {
        public:
            graph_equalizer_x32_mono();
    };

    class graph_equalizer_x16_stereo: public graph_equalizer_base, public graph_equalizer_x16_stereo_metadata
    {
        public:
            graph_equalizer_x16_stereo();
    };

    class graph_equalizer_x32_stereo: public graph_equalizer_base, public graph_equalizer_x32_stereo_metadata
    {
        public:
            graph_equalizer_x32_stereo();
    };

    class graph_equalizer_x16_lr: public graph_equalizer_base, public graph_equalizer_x16_lr_metadata
    {
        public:
            graph_equalizer_x16_lr();
    };

    class graph_equalizer_x32_lr: public graph_equalizer_base, public graph_equalizer_x32_lr_metadata
    {
        public:
            graph_equalizer_x32_lr();
    };

    class graph_equalizer_x16_ms: public graph_equalizer_base, public graph_equalizer_x16_ms_metadata
    {
        public:
            graph_equalizer_x16_ms();
    };

    class graph_equalizer_x32_ms: public graph_equalizer_base, public graph_equalizer_x32_ms_metadata
    {
        public:
            graph_equalizer_x32_ms();
    };

} /* namespace lsp */

#endif /* PLUGINS_GRAPH_EQUALIZER_H_ */
