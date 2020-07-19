/*
 * loud_comp.h
 *
 *  Created on: 29 июн. 2020 г.
 *      Author: sadko
 */

#ifndef PLUGINS_LOUD_COMP_H_
#define PLUGINS_LOUD_COMP_H_

#include <core/plugin.h>
#include <core/util/Bypass.h>
#include <core/util/Delay.h>
#include <core/util/Blink.h>
#include <core/util/Oscillator.h>
#include <core/util/SpectralProcessor.h>

#include <metadata/plugins.h>

namespace lsp
{
    class loud_comp_base: public plugin_t, public loud_comp_base_metadata
    {
        protected:
            typedef struct channel_t
            {
                float              *vIn;        // Input buffer
                float              *vOut;       // Output buffer
                float              *vDry;       // Dry signal
                float              *vBuffer;    // Temporary buffer
                float               fInLevel;   // Input level
                float               fOutLevel;  // Output level
                bool                bHClip;     // Hard-clip

                Bypass              sBypass;    // Bypass
                Delay               sDelay;     // Delay (for bypass)
                SpectralProcessor   sProc;      // Spectral processor
                Blink               sClipInd;   // Clip blink

                IPort              *pIn;        // Input port
                IPort              *pOut;       // Output port
                IPort              *pMeterIn;   // Input meter
                IPort              *pMeterOut;  // Output meter
                IPort              *pHClipInd;  // Hard clipping indicator
            } channel_t;

        protected:
            size_t              nChannels;      // Number of channels
            size_t              nMode;          // Current curve mode
            size_t              nRank;          // Current FFT rank
            float               fGain;          // Input gain
            float               fVolume;        // Volume
            bool                bBypass;        // Bypass
            bool                bRelative;      // Display relative curve instead of absolute
            bool                bReference;     // Reference generator
            bool                bHClipOn;       // Enable hard-clipping
            float               fHClipLvl;      // Hard-clip threshold
            channel_t          *vChannels[2];   // Audio channels
            float              *vTmpBuf;        // Temporary buffer for interpolating curve characteristics
            float              *vFreqApply;     // Frequency response applied to the output signal
            float              *vFreqMesh;      // List of frequencies for the mesh
            float              *vAmpMesh;       // List of amplitudes for the mesh
            bool                bSyncMesh;      // Synchronize mesh response with UI
            float_buffer_t     *pIDisplay;      // Inline display buffer

            Oscillator          sOsc;           // Oscillator for reference sound

            uint8_t            *pData;          // Allocation data

            IPort              *pBypass;        // Bypass
            IPort              *pGain;          // Input gain
            IPort              *pMode;          // Curve mode selector
            IPort              *pRank;          // FFT rank selector
            IPort              *pVolume;        // Output volume
            IPort              *pMesh;          // Output mesh response
            IPort              *pRelative;      // Relative mesh display
            IPort              *pReference;     // Enable reference sine generator
            IPort              *pHClipOn;       // Enable Hard clip
            IPort              *pHClipRange;    // Hard clipping range
            IPort              *pHClipReset;    // Hard clipping reset

        protected:
            void                update_response_curve();
            void                process_spectrum(channel_t *c, float *buf);

            static void         process_callback(void *object, void *subject, float *buf, size_t rank);

        public:
            explicit loud_comp_base(const plugin_metadata_t &mdata, size_t channels);
            virtual ~loud_comp_base();

            virtual void        init(IWrapper *wrapper);
            virtual void        destroy();

        public:
            virtual void        ui_activated();
            virtual void        update_sample_rate(long sr);
            virtual void        update_settings();
            virtual void        process(size_t samples);
            virtual bool        inline_display(ICanvas *cv, size_t width, size_t height);
            virtual void        dump(IStateDumper *v) const;
    };

    class loud_comp_mono: public loud_comp_base, public loud_comp_mono_metadata
    {
        public:
            explicit loud_comp_mono();
    };

    class loud_comp_stereo: public loud_comp_base, public loud_comp_stereo_metadata
    {
        public:
            explicit loud_comp_stereo();
    };
}

#endif /* PLUGINS_LOUD_COMP_H_ */
