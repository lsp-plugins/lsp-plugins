/*
 * para_equalizer.h
 *
 *  Created on: 30 июня 2016 г.
 *      Author: sadko
 */

#ifndef PLUGINS_PARA_EQUALIZER_H_
#define PLUGINS_PARA_EQUALIZER_H_

#include <metadata/plugins.h>
#include <core/plugin.h>
#include <core/util/Bypass.h>
#include <core/util/Analyzer.h>
#include <core/filters/Filter.h>
#include <core/filters/Equalizer.h>

namespace lsp
{
    class para_equalizer_base: public plugin_t
    {
        protected:
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

            enum eq_mode_t
            {
                EQ_MONO,
                EQ_STEREO,
                EQ_LEFT_RIGHT,
                EQ_MID_SIDE
            };

            typedef struct eq_filter_t
            {
                float              *vTrRe;          // Transfer function (real part)
                float              *vTrIm;          // Transfer function (imaginary part)
                size_t              nSync;          // Chart state
                bool                bSolo;          // Soloing filter

                IPort              *pType;          // Filter type
                IPort              *pMode;          // Filter mode
                IPort              *pFreq;          // Filter frequency
                IPort              *pSlope;         // Filter slope
                IPort              *pSolo;          // Solo port
                IPort              *pMute;          // Mute port
                IPort              *pGain;          // Filter gain
                IPort              *pQuality;       // Quality factor
                IPort              *pActivity;      // Filter activity flag
                IPort              *pTrAmp;         // Amplitude chart
            } eq_filter_t;

            typedef struct eq_channel_t
            {
                Equalizer           sEqualizer;     // Equalizer
                Bypass              sBypass;        // Bypass

                size_t              nLatency;       // Latency of the channel
                float               fInGain;        // Input gain
                float               fOutGain;       // Output gain
                float               fPitch;         // Frequency shift
                eq_filter_t        *vFilters;       // List of filters
                float              *vBuffer;        // Buffer for temporary data
                float              *vIn;            // Input buffer
                float              *vOut;           // Output buffer
                size_t              nSync;          // Chart state

                float              *vTrRe;          // Transfer function (real part)
                float              *vTrIm;          // Transfer function (imaginary part)

                IPort              *pIn;            // Input port
                IPort              *pOut;           // Output port
                IPort              *pInGain;        // Input gain
                IPort              *pTrAmp;         // Amplitude chart
                IPort              *pPitch;         // Frequency shift
                IPort              *pFft;           // FFT chart
                IPort              *pVisible;       // Visibility flag
                IPort              *pInMeter;       // Output level meter
                IPort              *pOutMeter;      // Output level meter
            } eq_channel_t;

        protected:
            Analyzer            sAnalyzer;              // Analyzer
            size_t              nFilters;               // Number of filters
            size_t              nMode;                  // Operating mode
            eq_channel_t       *vChannels;              // List of channels
            float              *vFreqs;                 // Frequency list
            uint32_t           *vIndexes;               // FFT indexes
            float               fGainIn;                // Input gain
            float               fZoom;                  // Zoom gain
            bool                bListen;                // Listen mode (only for MS para_equalizer)
            fft_position_t      nFftPosition;           // FFT position
            float_buffer_t     *pIDisplay;              // Inline display buffer

            IPort              *pBypass;                // Bypass port
            IPort              *pGainIn;                // Input gain port
            IPort              *pGainOut;               // Output gain port
            IPort              *pFftMode;               // FFT mode
            IPort              *pReactivity;            // FFT reactivity
            IPort              *pListen;                // Listen mode (only for MS equalizer)
            IPort              *pShiftGain;             // Shift gain
            IPort              *pZoom;                  // Graph zoom
            IPort              *pEqMode;                // Equalizer mode
            IPort              *pBalance;               // Output balance

        protected:
            void            destroy_state();
            inline void     decode_filter(size_t *ftype, size_t *slope, size_t mode);
            inline bool     adjust_gain(size_t filter_type);
            inline equalizer_mode_t get_eq_mode();

        public:
            explicit para_equalizer_base(const plugin_metadata_t &metadata, size_t filters, size_t mode);
            virtual ~para_equalizer_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();
            virtual void ui_activated();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
    };

    class para_equalizer_x16_mono: public para_equalizer_base, public para_equalizer_x16_mono_metadata
    {
        public:
            para_equalizer_x16_mono();
    };

    class para_equalizer_x32_mono: public para_equalizer_base, public para_equalizer_x32_mono_metadata
    {
        public:
            para_equalizer_x32_mono();
    };

    class para_equalizer_x16_stereo: public para_equalizer_base, public para_equalizer_x16_stereo_metadata
    {
        public:
            para_equalizer_x16_stereo();
    };

    class para_equalizer_x32_stereo: public para_equalizer_base, public para_equalizer_x32_stereo_metadata
    {
        public:
            para_equalizer_x32_stereo();
    };

    class para_equalizer_x16_lr: public para_equalizer_base, public para_equalizer_x16_lr_metadata
    {
        public:
            para_equalizer_x16_lr();
    };

    class para_equalizer_x32_lr: public para_equalizer_base, public para_equalizer_x32_lr_metadata
    {
        public:
            para_equalizer_x32_lr();
    };

    class para_equalizer_x16_ms: public para_equalizer_base, public para_equalizer_x16_ms_metadata
    {
        public:
            para_equalizer_x16_ms();
    };

    class para_equalizer_x32_ms: public para_equalizer_base, public para_equalizer_x32_ms_metadata
    {
        public:
            para_equalizer_x32_ms();
    };

} /* namespace lsp */

#endif /* PLUGINS_PARA_EQUALIZER_H_ */
