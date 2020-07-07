/*
 * pop_destroyer.h
 *
 *  Created on: 4 июл. 2020 г.
 *      Author: sadko
 */

#ifndef PLUGINS_POP_DESTROYER_H_
#define PLUGINS_POP_DESTROYER_H_

#include <core/plugin.h>
#include <core/util/Blink.h>
#include <core/util/Bypass.h>
#include <core/util/Depopper.h>
#include <core/util/MeterGraph.h>

#include <metadata/plugins.h>

namespace lsp
{
    class pop_destroyer_base: public plugin_t, public pop_destroyer_base_metadata
    {
        protected:
            typedef struct channel_t
            {
                float              *vIn;            // Input buffer
                float              *vOut;           // Output buffer
                float              *vBuffer;        // Buffer for processing
                Bypass              sBypass;        // Bypass
                MeterGraph          sIn;            // Input metering graph
                MeterGraph          sOut;           // Output metering graph
                bool                bInVisible;     // Input signal visibility flag
                bool                bOutVisible;    // Output signal visibility flag

                IPort              *pIn;            // Input port
                IPort              *pOut;           // Output port
                IPort              *pInVisible;     // Input visibility
                IPort              *pOutVisible;    // Output visibility
                IPort              *pMeterIn;       // Input Meter
                IPort              *pMeterOut;      // Output Meter
            } channel_t;

        protected:
            size_t              nChannels;          // Number of channels
            channel_t          *vChannels;          // Array of channels
            float              *vBuffer;            // Buffer for processing
            float              *vTimePoints;        // Time points
            float               fGainIn;            // Input gain
            float               fGainOut;           // Output gain
            bool                bGainVisible;       // Gain visible
            uint8_t            *pData;              // Allocated data

            MeterGraph          sGain;              // Gain metering graph
            Blink               sActive;            // Activity indicator
            Depopper            sDepopper;          // Depopper module

            IPort              *pGainIn;            // Input gain
            IPort              *pGainOut;           // Output gain
            IPort              *pThresh;            // Threshold
            IPort              *pAttack;            // Attack time
            IPort              *pRelease;           // Release time
            IPort              *pFade;              // Fade time
            IPort              *pActive;            // Active flag
            IPort              *pBypass;            // Bypass port
            IPort              *pMeshIn;            // Input mesh
            IPort              *pMeshOut;           // Output mesh
            IPort              *pMeshGain;          // Gain mesh
            IPort              *pGainVisible;       // Gain mesh visibility
            IPort              *pGainMeter;         // Gain reduction meter

        public:
            explicit            pop_destroyer_base(size_t channels, const plugin_metadata_t &meta);
            virtual            ~pop_destroyer_base();

            virtual void        init(IWrapper *wrapper);
            virtual void        destroy();

        public:
            virtual void        update_sample_rate(long sr);
            virtual void        update_settings();
            virtual void        process(size_t samples);
            virtual bool        inline_display(ICanvas *cv, size_t width, size_t height);
    };

    class pop_destroyer_mono: public pop_destroyer_base, public pop_destroyer_mono_metadata
    {
        public:
            explicit pop_destroyer_mono();
    };

    class pop_destroyer_stereo: public pop_destroyer_base, public pop_destroyer_stereo_metadata
    {
        public:
            explicit pop_destroyer_stereo();
    };
}

#endif /* PLUGINS_POP_DESTROYER_H_ */
