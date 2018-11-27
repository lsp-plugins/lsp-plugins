/*
 * spectrum_analyzer.h
 *
 *  Created on: 21 янв. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_PLUGINS_SPECTRUM_ANALYZER_H_
#define CORE_PLUGINS_SPECTRUM_ANALYZER_H_

#include <core/plugin.h>
#include <core/util/Analyzer.h>

#include <metadata/plugins.h>

namespace lsp
{

    class spectrum_analyzer_base: public plugin_t
    {
        protected:
            typedef struct sa_channel_t
            {
                bool        bSolo;              // Soloing flag
                bool        bSend;              // Send to UI flag
                float       fGain;              // Makeup gain
                float       fHue;               // Hue

                // Port references
                IPort      *pIn;                // Input samples
                IPort      *pOut;               // Output samples
                IPort      *pOn;                // FFT on
                IPort      *pSolo;              // Soloing flag
                IPort      *pFreeze;            // Freeze flag
                IPort      *pHue;               // Hue of the graph color
                IPort      *pShift;             // Shift gain
                IPort      *pSpec;              // Spectrum output
            } sa_channel_t;

            enum mode_t
            {
                SA_ANALYZER,
                SA_ANALYZER_STEREO,
                SA_ANALYZER_X2,
                SA_MASTERING,
                SA_MASTERING_STEREO,
                SA_MASTERING_X2,
                SA_SPECTRALIZER,
                SA_SPECTRALIZER_STEREO,
                SA_SPECTRALIZER_X2,
            };

        protected:
            bool                create_channels(size_t channels);
            mode_t              decode_mode(size_t mode);

        protected:
            Analyzer            sAnalyzer;
            size_t              nChannels;
            sa_channel_t       *vChannels;
            float              *vFrequences;
            uint32_t           *vIndexes;
            uint8_t            *pData;

            bool                bBypass;
            size_t              nChannel;
            float               fSelector;
            float               fMinFreq;
            float               fMaxFreq;
            float               fReactivity;        // Reactivity
            float               fTau;               // Time constant (dependent on reactivity)
            float               fPreamp;            // Preamplification level
            float               fZoom;              // Zoom

            IPort              *pBypass;
            IPort              *pTolerance;
            IPort              *pWindow;
            IPort              *pEnvelope;
            IPort              *pPreamp;
            IPort              *pZoom;
            IPort              *pReactivity;
            IPort              *pChannel;
            IPort              *pSelector;
            IPort              *pFrequency;
            IPort              *pLevel;

            float_buffer_t     *pIDisplay;      // Inline display buffer

        public:
            spectrum_analyzer_base(const plugin_metadata_t &metadata);
            virtual ~spectrum_analyzer_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
    };

    class spectrum_analyzer_x1: public spectrum_analyzer_base, public spectrum_analyzer_x1_metadata
    {
        public:
            spectrum_analyzer_x1();
    };

    class spectrum_analyzer_x2: public spectrum_analyzer_base, public spectrum_analyzer_x2_metadata
    {
        public:
            spectrum_analyzer_x2();
    };

    class spectrum_analyzer_x4: public spectrum_analyzer_base, public spectrum_analyzer_x4_metadata
    {
        public:
            spectrum_analyzer_x4();
    };


    class spectrum_analyzer_x8: public spectrum_analyzer_base, public spectrum_analyzer_x8_metadata
    {
        public:
            spectrum_analyzer_x8();
    };

    class spectrum_analyzer_x12: public spectrum_analyzer_base, public spectrum_analyzer_x12_metadata
    {
        public:
            spectrum_analyzer_x12();
    };

    class spectrum_analyzer_x16: public spectrum_analyzer_base, public spectrum_analyzer_x16_metadata
    {
        public:
            spectrum_analyzer_x16();
    };

} /* namespace ddb */

#endif /* CORE_PLUGINS_SPECTRUM_ANALYZER_H_ */
