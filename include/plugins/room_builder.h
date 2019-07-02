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
#include <data/cstorage.h>


#include <metadata/plugins.h>

namespace lsp
{
    class room_builder_base: public plugin_t
    {
        public:
            typedef struct obj_props_t
            {
                const char             *sName;      // UTF-8 object name
                bool                    bEnabled;   // Enabled flag
                point3d_t               sCenter;    // Object center
                vector3d_t              sMove;      // Object move
                float                   fYaw;       // Yaw
                float                   fPitch;     // Pitch
                float                   fRoll;      // Roll
                vector3d_t              sScale;     // Scaling of object
                float                   fHue;       // Hue color
                float                   fAbsorption[2];
                float                   lnkAbsorption;
                float                   fDispersion[2];
                float                   lnkDispersion;
                float                   fDiffusion[2];
                float                   lnkDiffusion;
                float                   fTransparency[2];
                float                   lnkTransparency;
                float                   fSndSpeed;
            } obj_props_t;

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
                IPort          *pSample;        // Convolver source sample
                IPort          *pTrack;         // Convolver source sample track
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
                IPort          *pFreqGain[room_builder_base_metadata::EQ_BANDS];    // Gain for each band of the Equalizer
            } channel_t;

            typedef struct input_t
            {
                float                  *vIn;        // Input data
                IPort                  *pIn;        // Input port
                IPort                  *pPan;       // Panning
            } input_t;

            typedef struct source_t
            {
                bool                    bEnabled;
                rt_audio_source_t       enType;
                bool                    bPhase;
                size_t                  nChannel;
                point3d_t               sPos;
                float                   fYaw;
                float                   fPitch;
                float                   fRoll;
                float                   fSize;
                float                   fHeight;
                float                   fAngle;
                float                   fCurvature;

                IPort                  *pEnabled;
                IPort                  *pType;
                IPort                  *pChannel;
                IPort                  *pPhase;
                IPort                  *pPosX;
                IPort                  *pPosY;
                IPort                  *pPosZ;
                IPort                  *pYaw;
                IPort                  *pPitch;
                IPort                  *pRoll;
                IPort                  *pSize;
                IPort                  *pHeight;
                IPort                  *pAngle;
                IPort                  *pCurvature;
            } source_t;

            typedef struct capture_t: public room_capture_config_t
            {
                bool                    bEnabled;   // Enabled flag
                ssize_t                 nRMin;      // Minimum reflection order
                ssize_t                 nRMax;      // Maximum reflection order

                Sample                  sCurrent;   // Current sample
                Sample                  sPending;   // Pending sample

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
                    size_t                  nFlags;
                    char                    sPath[PATH_MAX+1];
                    room_builder_base      *pCore;
                    Scene3D                 sScene;

                public:
                    inline SceneLoader()
                    {
                        nFlags      = 0;
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
            ssize_t                 nRenderThreads;
            status_t                enRenderStatus;
            float                   fRenderProgress;
            float                   fRenderCmd;

            input_t                 vInputs[2];
            channel_t               vChannels[2];
            convolver_t             vConvolvers[room_builder_base_metadata::CONVOLVERS];
            capture_t               vCaptures[room_builder_base_metadata::CAPTURES];
            source_t                vSources[room_builder_base_metadata::SOURCES];

            Scene3D                 sScene;
            vector3d_t              sScale;

            status_t                nSceneStatus;
            float                   fSceneProgress;
            SceneLoader             s3DLoader;

            IPort                  *pBypass;
            IPort                  *pRank;
            IPort                  *pDry;
            IPort                  *pWet;
            IPort                  *pRenderThreads;
            IPort                  *pRenderStatus;
            IPort                  *pRenderProgress;
            IPort                  *pRenderCmd;
            IPort                  *pOutGain;
            IPort                  *pPredelay;
            IPort                  *p3DFile;
            IPort                  *p3DProgress;
            IPort                  *p3DStatus;
            IPort                  *p3DOrientation;
            IPort                  *pScaleX;
            IPort                  *pScaleY;
            IPort                  *pScaleZ;

            void                   *pData;      // Allocated data
            ipc::IExecutor         *pExecutor;

        protected:
            static size_t       get_fft_rank(size_t rank);
            void                sync_offline_tasks();

        public:
            explicit room_builder_base(const plugin_metadata_t &metadata, size_t inputs);
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
            static rt_audio_source_t    decode_source_type(float value);
            static status_t             configure_capture(room_capture_settings_t *out, const room_capture_config_t *in);

            static void                 kvt_cleanup_objects(KVTStorage *kvt, size_t objects);
            static void                 read_object_properties(obj_props_t *props, const char *base, KVTStorage *kvt);
            static void                 build_object_matrix(matrix3d_t *m, const obj_props_t *props);
    };

    class room_builder_mono: public room_builder_base, public room_builder_mono_metadata
    {
        public:
            explicit room_builder_mono();
            virtual ~room_builder_mono();
    };

    class room_builder_stereo: public room_builder_base, public room_builder_stereo_metadata
    {
        public:
            explicit room_builder_stereo();
            virtual ~room_builder_stereo();
    };

}


#endif /* PLUGINS_ROOM_BUILDER_H_ */
