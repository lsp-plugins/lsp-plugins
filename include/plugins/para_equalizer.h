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
#include <core/Bypass.h>
#include <core/Filter.h>
#include <core/Equalizer.h>

namespace lsp
{
    class para_equalizer_base: public plugin_t
    {
        protected:
            enum chart_state_t
            {
                CS_UPDATE       = 1 << 0,
                CS_SYNC_AMP     = 1 << 1,
                CS_SYNC_PHASE   = 1 << 2
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
                float              *vTrAmp;         // Transfer function (amplitude)
                size_t              nSync;          // Chart state
                bool                bSolo;          // Soloing filter

                IPort              *pType;          // Filter type
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
                size_t              nLatency;       // Latency of the channel
                eq_filter_t        *vFilters;       // List of filters
                float              *vBuffer;        // Buffer for temporary data
                float              *vIn;            // Input buffer
                float              *vOut;           // Output buffer
                size_t              nSync;          // Chart state
                size_t              nFftCounter;    // FFT counter

                Bypass              sBypass;        // Bypass

                float              *vTrRe;          // Transfer function (real part)
                float              *vTrIm;          // Transfer function (imaginary part)
                float              *vTrAmp;         // Transfer function (amplitude)

                float              *vFftBuffer;     // FFT buffer
                float              *vFftAmp;        // FFT amplitude

                IPort              *pIn;            // Input port
                IPort              *pOut;           // Output port
                IPort              *pTrAmp;         // Amplitude chart
                IPort              *pFft;           // FFT chart
                IPort              *pVisible;       // Visibility flag
            } eq_channel_t;

        protected:
            size_t              nFilters;               // Number of filters
            size_t              nMode;                  // Operating mode
            equalizer_mode_t    nEqMode;                // Equalizer mode
            eq_channel_t       *vChannels;              // List of channels
            float              *vFreqs;                 // Frequency list
            float               fGainIn;                // Input gain
            float               fGainOut;               // Output gain
            bool                bListen;                // Listen mode (only for MS para_equalizer)
            size_t              nFftPeriod;             // FFT period
            fft_position_t      nFftPosition;           // FFT position
            float               fTau;                   // Reactivity
            float               fShiftGain;             // Shift gain

            float              *vSigRe;                 // Real part of signal
            float              *vSigIm;                 // Imaginary part of signal (always zero)
            float              *vFftRe;                 // Buffer for FFT transform (real part)
            float              *vFftIm;                 // Buffer for FFT transfor (imaginary part)
            float              *vFftWindow;             // FFT window
            float              *vFftEnvelope;           // FFT envelope
            uint32_t           *vIndexes;               // FFT indexes

            IPort              *pBypass;                // Bypass port
            IPort              *pGainIn;                // Input gain port
            IPort              *pGainOut;               // Output gain port
            IPort              *pFftMode;               // FFT mode
            IPort              *pReactivity;            // FFT reactivity
            IPort              *pListen;                // Listen mode (only for MS equalizer)
            IPort              *pShiftGain;             // Shift gain
            IPort              *pEqMode;                // Equalizer mode

        protected:
            void            destroy_state();
            void            update_frequencies();
            inline void     decode_filter(size_t *ftype, size_t *slope);
            void            do_fft(eq_channel_t *channel, size_t samples);
            void            init_window();
            void            init_envelope();
            inline bool     adjust_gain(size_t filter_type);
            inline equalizer_mode_t get_eq_mode();

        public:
            para_equalizer_base(const plugin_metadata_t &metadata, size_t filters, size_t mode);
            virtual ~para_equalizer_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();
            virtual void ui_activated();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);
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
