/*
 * slap_delay.h
 *
 *  Created on: 30 янв. 2017 г.
 *      Author: sadko
 */

#ifndef PLUGINS_SLAPDELAY_H_
#define PLUGINS_SLAPDELAY_H_

#include <metadata/plugins.h>

#include <core/plugin.h>
#include <core/util/Bypass.h>
#include <core/util/ShiftBuffer.h>
#include <core/filters/Equalizer.h>

namespace lsp
{
    class slap_delay_base: public plugin_t
    {
        protected:
            enum proc_mode_t
            {
                M_OFF,
                M_TIME,
                M_DISTANCE
            };

            typedef struct mono_processor_t
            {
                Equalizer   sEqualizer;

                float       fGain[2];       // Amount of gain for left and right input channels
            } mono_processor_t;

            typedef struct processor_t
            {
                mono_processor_t        vDelay[2];

                size_t                  nDelay;     // Delay
                size_t                  nNewDelay;  // New delay
                size_t                  nMode;      // Operating mode

                IPort                  *pMode;      // Operating mode port
                IPort                  *pEq;        // Equalizer
                IPort                  *pTime;      // Delay in time units
                IPort                  *pDistance;  // Delay in distance units
                IPort                  *pFrac;      // Fraction
                IPort                  *pDenom;     // Denominator
                IPort                  *pPan[2];    // Pan of left and right input channels
                IPort                  *pGain;      // Gain of the delay line
                IPort                  *pLowCut;    // Low-cut flag
                IPort                  *pLowFreq;   // Low-cut frequency
                IPort                  *pHighCut;   // High-cut flag
                IPort                  *pHighFreq;  // Low-cut frequency
                IPort                  *pSolo;      // Solo control
                IPort                  *pMute;      // Mute control
                IPort                  *pPhase;     // Phase control
                IPort                  *pFreqGain[slap_delay_base_metadata::EQ_BANDS];      // Gain for each band of the Equalizer
            } processor_t;

            typedef struct channel_t
            {
                Bypass                  sBypass;    // Bypass
                float                   fGain[2];   // Panning gain
                float                  *vRender;    // Rendering buffer
                float                  *vOut;       // Output buffer
                IPort                  *pOut;       // Output port
            } channel_t;

            typedef struct input_t
            {
                ShiftBuffer             sBuffer;    // Shift buffer of input data
                float                  *vIn;        // Input data
                IPort                  *pIn;        // Input port
                IPort                  *pPan;       // Panning
            } input_t;

        protected:
            size_t          nInputs;        // Mono/Stereo mode flag
            input_t        *vInputs;        // Inputs

            processor_t     vProcessors[slap_delay_base_metadata::MAX_PROCESSORS];    // Processors
            channel_t       vChannels[2];

            float          *vTemp;          // Temporary buffer for processing
            bool            bMono;          // Mono output flag

            IPort          *pBypass;        // Bypass
            IPort          *pTemp;          // Temperature
            IPort          *pDry;           // Dry signal amount
            IPort          *pWet;           // Wet signal amount
            IPort          *pDryMute;       // Dry mute
            IPort          *pWetMute;       // Wet mute
            IPort          *pOutGain;       // Output gain
            IPort          *pMono;          // Mono output
            IPort          *pPred;          // Pre-delay
            IPort          *pStretch;       // Time stretch
            IPort          *pTempo;         // Tempo
            IPort          *pSync;          // Sync tempo
            IPort          *pRamping;       // Ramping mode

            uint8_t        *vData;          // Allocated data

        public:
            slap_delay_base(const plugin_metadata_t &mdata, bool stereo_in);
            virtual ~slap_delay_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual bool set_position(const position_t *pos);
            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);
    };

    class slap_delay_mono: public slap_delay_base, public slap_delay_mono_metadata
    {
        public:
            slap_delay_mono();
            virtual ~slap_delay_mono();
    };

    class slap_delay_stereo: public slap_delay_base, public slap_delay_stereo_metadata
    {
        public:
            slap_delay_stereo();
            virtual ~slap_delay_stereo();
    };

} /* namespace lsp */

#endif /* PLUGINS_SLAPDELAY_H_ */
