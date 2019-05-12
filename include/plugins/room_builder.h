/*
 * room_builder.h
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#ifndef PLUGINS_ROOM_BUILDER_H_
#define PLUGINS_ROOM_BUILDER_H_

#include <core/plugin.h>
#include <core/util/Bypass.h>
#include <core/util/Delay.h>
#include <core/util/Convolver.h>
#include <core/3d/Scene3D.h>
#include <core/3d/RayTrace3D.h>
#include <core/sampling/SamplePlayer.h>
#include <core/filters/Equalizer.h>

#include <metadata/plugins.h>

namespace lsp
{
    class room_builder_base: public plugin_t
    {
        protected:
            typedef struct convolver_t
            {
                Delay           sDelay;         // Delay line

                Convolver      *pCurr;          // Currently used convolver
                Convolver      *pSwap;          // Swap

                size_t          nRank;          // Last applied rank
                size_t          nRankReq;       // Rank request
                size_t          nSource;        // Source
                size_t          nFileReq;       // File request
                size_t          nTrackReq;      // Track request

                float          *vBuffer;        // Buffer for convolution
                float           fPanIn[2];      // Input panning of convolver
                float           fPanOut[2];     // Output panning of convolver

                IPort          *pMakeup;        // Makeup gain of convolver
                IPort          *pPanIn;         // Input panning of convolver
                IPort          *pPanOut;        // Output panning of convolver
                IPort          *pFile;          // Convolver source file
                IPort          *pTrack;         // Convolver source file track
                IPort          *pPredelay;      // Pre-delay
                IPort          *pMute;          // Mute button
                IPort          *pActivity;      // Activity indicator
            } convolver_t;

            typedef struct channel_t
            {
                Bypass          sBypass;
                SamplePlayer    sPlayer;
                Equalizer       sEqualizer;     // Wet signal equalizer

                float          *vOut;
                float          *vBuffer;        // Rendering buffer
                float           fDryPan[2];     // Dry panorama

                IPort          *pOut;

                IPort          *pWetEq;         // Wet equalization flag
                IPort          *pLowCut;        // Low-cut flag
                IPort          *pLowFreq;       // Low-cut frequency
                IPort          *pHighCut;       // High-cut flag
                IPort          *pHighFreq;      // Low-cut frequency
                IPort          *pFreqGain[impulse_reverb_base_metadata::EQ_BANDS];   // Gain for each band of the Equalizer
            } channel_t;

            typedef struct input_t
            {
                float                  *vIn;        // Input data
                IPort                  *pIn;        // Input port
                IPort                  *pPan;       // Panning
            } input_t;

            typedef struct capture_t: public room_capture_config_t
            {
                bool                    bEnabled;   // Enabled flag
                ssize_t                 nRMin;      // Minimum reflection order
                ssize_t                 nRMax;      // Maximum reflection order

                Sample                  sCurrent;       // Current sample
                Sample                  sPending;       // Pending sample

                IPort                  *pEnabled;
                IPort                  *pRMin;
                IPort                  *pRMax;
                IPort                  *pPosX;
                IPort                  *pPosY;
                IPort                  *pPosZ;
                IPort                  *pYaw;
                IPort                  *pPitch;
                IPort                  *pRoll;
                IPort                  *pCapsule;
                IPort                  *pConfig;
                IPort                  *pAngle;
                IPort                  *pDistance;
                IPort                  *pDirection;
                IPort                  *pSide;
            } capture_t;

        protected:
            class SceneLoader: public ipc::ITask
            {
                public:
                    room_builder_base       *pCore;
                    Scene3D                  sScene;

                public:
                    inline SceneLoader()
                    {
                        pCore       = NULL;
                    }

                    virtual ~SceneLoader();

                    void init(room_builder_base *base);
                    void destroy();

                public:
                    virtual status_t run();
            };

        protected:
            size_t                  nInputs;
            size_t                  nReconfigReq;
            size_t                  nReconfigResp;

            input_t                 vInputs[2];
            channel_t               vChannels[2];
            convolver_t             vConvolvers[room_builder_base_metadata::CONVOLVERS];
            capture_t               vCaptures[room_builder_base_metadata::CAPTURES];

            Scene3D                 sScene;
            status_t                nSceneStatus;
            float                   fSceneProgress;
            SceneLoader             s3DLoader;

            IPort                  *pBypass;
            IPort                  *pRank;
            IPort                  *pDry;
            IPort                  *pWet;
            IPort                  *pOutGain;
            IPort                  *pPredelay;
            IPort                  *p3DFile;
            IPort                  *p3DProgress;
            IPort                  *p3DStatus;

            void                   *pData;      // Allocated data
            ipc::IExecutor         *pExecutor;

        protected:
            static size_t       get_fft_rank(size_t rank);
            void                sync_offline_tasks();

        public:
            room_builder_base(const plugin_metadata_t &metadata, size_t inputs);
            virtual ~room_builder_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);

        public:
            static rt_capture_config_t  decode_config(float value);
            static rt_audio_capture_t   decode_direction(float value);
            static rt_audio_capture_t   decode_side_direction(float value);
            static status_t             configure_capture(room_capture_settings_t *out, const room_capture_config_t *in);
    };

    class room_builder_mono: public room_builder_base, public room_builder_mono_metadata
    {
        public:
            room_builder_mono();
            virtual ~room_builder_mono();
    };

    class room_builder_stereo: public room_builder_base, public room_builder_stereo_metadata
    {
        public:
            room_builder_stereo();
            virtual ~room_builder_stereo();
    };

}


#endif /* PLUGINS_ROOM_BUILDER_H_ */
