/*
 * oscillator.h
 *
 *  Created on: 20 Mar 2017
 *      Author: crocoduck
 */

#ifndef PLUGINS_OSCILLATOR_H_
#define PLUGINS_OSCILLATOR_H_

#include <core/plugin.h>
#include <metadata/plugins.h>
#include <core/Oscillator.h>
#include <core/Bypass.h>

namespace lsp
{
    class oscillator_mono: public plugin_t, public oscillator_mono_metadata
    {
        protected:
            Oscillator           sOsc;
            Bypass               sBypass;
            size_t               nMode;
            bool                 bMeshSync;
            bool                 bBypass;

            float               *vBuffer;
            float               *vTime;
            float               *vDisplaySamples;
            uint8_t             *pData;
            float_buffer_t      *pIDisplay;      // Inline display buffer

            IPort               *pIn;
            IPort               *pOut;
            IPort               *pBypass;
            IPort               *pFrequency;
            IPort               *pGain;
            IPort               *pDCOffset;
            IPort               *pDCRefSc;
            IPort               *pInitPhase;
            IPort               *pModeSc;
            IPort               *pOversamplerModeSc;
            IPort               *pFuncSc;
            IPort               *pSquaredSinusoidInv;
            IPort               *pParabolicInv;
            IPort               *pRectangularDutyRatio;
            IPort               *pSawtoothWidth;
            IPort               *pTrapezoidRaiseRatio;
            IPort               *pTrapezoidFallRatio;
            IPort               *pPulsePosWidthRatio;
            IPort               *pPulseNegWidthRatio;
            IPort               *pParabolicWidth;
            IPort               *pOutputMesh;

        protected:
            static fg_function_t get_function(size_t function);
            static dc_reference_t get_dc_reference(size_t reference);
            static over_mode_t get_oversampling_mode(size_t mode);

        public:
            oscillator_mono();
            virtual ~oscillator_mono();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void process(size_t samples);
            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void destroy();
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
            virtual void ui_activated();
    };
}

#endif /* PLUGINS_OSCILLATOR_H_ */
