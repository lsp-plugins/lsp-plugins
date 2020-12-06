/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 3 дек. 2020 г.
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

#ifndef PLUGINS_ART_DELAY_H_
#define PLUGINS_ART_DELAY_H_

#include <metadata/plugins.h>

#include <core/plugin.h>
#include <core/util/Bypass.h>
#include <core/util/DynamicDelay.h>
#include <core/filters/Equalizer.h>

namespace lsp
{
    class art_delay_base: public plugin_t, public art_delay_base_metadata
    {
        protected:
            typedef struct art_settings_t
            {
                float               fDelay;         // Delay value
                float               fGain;          // Output gain value
                float               fFeedback;      // Feedback value
                float               fPan[2];        // Pan value
                size_t              nMaxDelay;      // Maximum possible delay
            } art_settings_t;

            struct art_delay_t;

            class DelayAllocator: public ipc::ITask
            {
                private:
                    art_delay_t    *pDelay;         // Delay pointer
                    ssize_t         nSize;          // Requested delay size

                public:
                    explicit DelayAllocator(art_delay_t *delay);
                    virtual ~DelayAllocator();

                public:
                    virtual status_t run();
            };

            typedef struct art_tempo_t
            {
                float               fTempo;         // The actual tempo
                bool                bSync;          // Sync flag

                IPort              *pTempo;         // Tempo port
                IPort              *pRatio;         // Ratio port
                IPort              *pSync;          // Sync flag
            } art_tempo_t;

            typedef struct art_delay_t
            {
                DynamicDelay       *pPDelay[2];     // Pending delay (waiting for replace)
                DynamicDelay       *pCDelay[2];     // Currently used delay for each channel
                DynamicDelay       *pGDelay[2];     // Garbage
                Equalizer           sEq[2];         // Equalizers for each channel
                Bypass              sBypass[2];     // Bypass
                DelayAllocator     *pAllocator;     // Allocator

                bool                bStereo;        // Mode: Mono/stereo
                bool                bOn;            // Delay is enabled
                bool                bSolo;          // Soloing flag
                bool                bMute;          // Muting flag
                bool                bUpdated;       // Update flag
                bool                bValidRef;      // Valid reference flag
                ssize_t             nDelayRef;      // Reference to delay

                art_settings_t      sOld;           // Old settings
                art_settings_t      sNew;           // New settings

                IPort              *pOn;            // On
                IPort              *pTempoRef;      // Tempo reference
                IPort              *pPan[2];        // Panning
                IPort              *pSolo;          // Solo flag
                IPort              *pMute;          // Mute flag
                IPort              *pDelayRef;      // Delay reference
                IPort              *pDelayMul;      // Delay reference multiplier
                IPort              *pBarFrac;       // Bar fraction
                IPort              *pBarDenom;      // Bar denominator
                IPort              *pBarMul;        // Bar multiplier
                IPort              *pFrac;          // Add fraction
                IPort              *pDenom;         // Add denominator
                IPort              *pDelay;         // Add delay
                IPort              *pEqOn;          // Equalizer on
                IPort              *pLcfOn;         // Low-cut filter on
                IPort              *pLcfFreq;       // Low-cut filter frequency
                IPort              *pHcfOn;         // High-cut filter on
                IPort              *pHcfFreq;       // High-cut filter frequency
                IPort              *pBandGain[EQ_BANDS];    // Band gain for each filter
                IPort              *pFeedOn;        // Feedback on
                IPort              *pFeedGain;      // Feedback gain
                IPort              *pGain;          // Output gain

                IPort              *pOutDelay;      // Output delay
                IPort              *pOutOfRange;    // Out of range status
                IPort              *pOutLoop;       // Dependency loop
            } art_delay_t;

        protected:
            bool                    bStereoIn;
            bool                    bMono;          // Mono switch
            size_t                  nMaxDelay;      // Maximum delay
            float                   fOldDryGain;    // Old dry gain
            float                   fNewDryGain;    // New dry gain
            float                   fOldPan[2];     // Old panning
            float                   fNewPan[2];     // New panning
            float                  *vDataBuf[2];    // Temporary data buffer
            float                  *vOutBuf[2];     // Output buffer
            float                  *vGainBuf;       // Gain control buffer
            float                  *vDelayBuf;      // Delay control buffer
            art_tempo_t            *vTempo;         // Tempo settings
            art_delay_t            *vDelays;        // Delay lines

            Bypass                  sBypass[2];     // Bypasses
            ipc::IExecutor         *pExecutor;

            IPort                  *pIn[2];         // Input ports
            IPort                  *pOut[2];        // Output ports
            IPort                  *pBypass;        // Bypass
            IPort                  *pMaxDelay;      // Maximum possible delay
            IPort                  *pPan[2];        // Panning
            IPort                  *pDryGain;       // Dry gain
            IPort                  *pWetGain;       // Wet gain
            IPort                  *pMono;          // Mono/Stereo switch
            IPort                  *pFeedback;      // Enable feedback for all delays

            uint8_t                *pData;

        protected:
            static inline float         decode_ratio(size_t v);
            static inline size_t        decode_max_delay_value(size_t v);
            bool                        check_delay_ref(art_delay_t *ad);
            void                        sync_delay(art_delay_t *ad);
            void                        process_delay(art_delay_t *ad, float **out, const float * const *in, size_t samples, size_t i, size_t count);

        public:
            explicit art_delay_base(const plugin_metadata_t &mdata, bool stereo_in);
            virtual ~art_delay_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual bool set_position(const position_t *pos);
            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);

            virtual void dump(IStateDumper *v) const;
    };

    class art_delay_mono: public art_delay_base, public art_delay_mono_metadata
    {
        public:
            explicit art_delay_mono();
    };

    class art_delay_stereo: public art_delay_base, public art_delay_stereo_metadata
    {
        public:
            explicit art_delay_stereo();
    };
}



#endif /* PLUGINS_ART_DELAY_H_ */
