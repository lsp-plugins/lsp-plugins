/*
 * mb_expander.h
 *
 *  Created on: 27 дек. 2019 г.
 *      Author: sadko
 */

#ifndef PLUGINS_MB_EXPANDER_H_
#define PLUGINS_MB_EXPANDER_H_

#include <metadata/plugins.h>
#include <core/util/Bypass.h>
#include <core/util/Sidechain.h>
#include <core/util/Delay.h>
#include <core/util/MeterGraph.h>
#include <core/util/MeterGraph.h>
#include <core/util/Analyzer.h>
#include <core/filters/DynamicFilters.h>
#include <core/filters/Equalizer.h>
#include <core/plugin.h>

namespace lsp
{
    class mb_expander_base: public plugin_t
    {
        protected:
            enum c_mode_t
            {
                MBEM_MONO,
                MBEM_STEREO,
                MBEM_LR,
                MBEM_MS
            };

            typedef struct channel_t
            {
                IPort          *pIn;                // Input
                IPort          *pOut;               // Output
                IPort          *pScIn;              // Sidechain
            } channel_t;

        protected:
            Analyzer        sAnalyzer;              // Analyzer
            DynamicFilters  sFilters;               // Dynamic filters for each band in 'modern' mode
            size_t          nMode;                  // Expander channel mode
            bool            bSidechain;             // External side chain
            bool            bEnvUpdate;             // Envelope filter update
            bool            bModern;                // Modern mode
            size_t          nEnvBoost;              // Envelope boost
            channel_t      *vChannels;              // Expander channels
            float           fInGain;                // Input gain
            float           fDryGain;               // Dry gain
            float           fWetGain;               // Wet gain
            float           fZoom;                  // Zoom
            uint8_t        *pData;                  // Aligned data pointer
            float          *vSc[2];                 // Sidechain signal data
            float          *vBuffer;                // Temporary buffer
            float          *vEnv;                   // Compressor envelope buffer
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

        public:
            explicit mb_expander_base(const plugin_metadata_t &metadata, bool sc, size_t mode);
            virtual ~mb_expander_base();

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
    // Different expander implementations
    class mb_expander_mono: public mb_expander_base, public mb_expander_mono_metadata
    {
        public:
            mb_expander_mono();
    };

    class mb_expander_stereo: public mb_expander_base, public mb_expander_stereo_metadata
    {
        public:
            mb_expander_stereo();
    };

    class mb_expander_lr: public mb_expander_base, public mb_expander_lr_metadata
    {
        public:
            mb_expander_lr();
    };

    class mb_expander_ms: public mb_expander_base, public mb_expander_ms_metadata
    {
        public:
            mb_expander_ms();
    };

    class sc_mb_expander_mono: public mb_expander_base, public sc_mb_expander_mono_metadata
    {
        public:
            sc_mb_expander_mono();
    };

    class sc_mb_expander_stereo: public mb_expander_base, public sc_mb_expander_stereo_metadata
    {
        public:
            sc_mb_expander_stereo();
    };

    class sc_mb_expander_lr: public mb_expander_base, public sc_mb_expander_lr_metadata
    {
        public:
            sc_mb_expander_lr();
    };

    class sc_mb_expander_ms: public mb_expander_base, public sc_mb_expander_ms_metadata
    {
        public:
            sc_mb_expander_ms();
    };
}

#endif /* PLUGINS_MB_EXPANDER_H_ */
