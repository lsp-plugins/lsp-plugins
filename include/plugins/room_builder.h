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

            enum prop_sync_t
            {
                PS_NAME                 = 1 << 0,
                PS_POS_X                = 1 << 1,
                PS_POS_Y                = 1 << 2,
                PS_POS_Z                = 1 << 3,
                PS_YAW                  = 1 << 4,
                PS_PITCH                = 1 << 5,
                PS_ROLL                 = 1 << 6,
                PS_SIZE_X               = 1 << 7,
                PS_SIZE_Y               = 1 << 8,
                PS_SIZE_Z               = 1 << 9,
                PS_HUE                  = 1 << 10,

                PS_ABSORPTION_0         = 1 << 11,
                PS_ABSORPTION_1         = 1 << 12,
                PS_DISPERSION_0         = 1 << 13,
                PS_DISPERSION_1         = 1 << 14,
                PS_DISSIPATION_0        = 1 << 15,
                PS_DISSIPATION_1        = 1 << 16,
                PS_TRANSPARENCY_0       = 1 << 17,
                PS_TRANSPARENCY_1       = 1 << 18,
                PS_SOUND_SPEED          = 1 << 19,

                PS_SYNC_ALL             = ((1 << 20) - 1)
            };

            enum osc_sync_t
            {
                OSC_OBJECT_COUNT        = 1 << 0,
                OSC_OBJECTS             = 1 << 1,

                OSC_SYNC_ALL            = ((1 << 2) - 1)
            };

            typedef struct obj_props_t
            {
                char                   *sName;      // UTF-8 object name
                point3d_t               sPos;       // Object relative position
                float                   fYaw;       // Yaw
                float                   fPitch;     // Pitch
                float                   fRoll;      // Roll
                float                   fSizeX;     // Size of object (X)
                float                   fSizeY;     // Size of object (Y)
                float                   fSizeZ;     // Size of object (Z)
                float                   fHue;       // Hue color
                float                   fAbsorption[2];
                float                   fDispersion[2];
                float                   fDissipation[2];
                float                   fTransparency[2];
                float                   fSndSpeed;

                size_t                  nSync;      // Sync flags
            } obj_props_t;

        protected:
            class SceneLoader: public ipc::ITask
            {
                public:
                    char                    sPath[PATH_MAX];
                    room_builder_base      *pCore;
                    Scene3D                 sScene;
                    cstorage<obj_props_t>   vProps;

                protected:
                    void drop_props();

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
            source_t                vSources[room_builder_base_metadata::SOURCES];

            Scene3D                 sScene;
            cstorage<obj_props_t>   vObjectProps;
            size_t                  nOscSync;

            status_t                nSceneStatus;
            float                   fSceneProgress;
            SceneLoader             s3DLoader;

            IPort                  *pOscIn;
            IPort                  *pOscOut;
            IPort                  *pBypass;
            IPort                  *pRank;
            IPort                  *pDry;
            IPort                  *pWet;
            IPort                  *pOutGain;
            IPort                  *pPredelay;
            IPort                  *p3DFile;
            IPort                  *p3DProgress;
            IPort                  *p3DStatus;
            IPort                  *p3DOrientation;

            void                   *pData;      // Allocated data
            ipc::IExecutor         *pExecutor;

        protected:
            static size_t       get_fft_rank(size_t rank);
            void                sync_offline_tasks();
            void                perform_osc_transmit();
            void                perform_osc_receive();

        public:
            room_builder_base(const plugin_metadata_t &metadata, size_t inputs);
            virtual ~room_builder_base();

        public:
            virtual void init(IWrapper *wrapper);
            virtual void destroy();

            virtual void update_settings();
            virtual void update_sample_rate(long sr);

            virtual void process(size_t samples);

            virtual void ui_activated();

        public:
            static rt_capture_config_t  decode_config(float value);
            static rt_audio_capture_t   decode_direction(float value);
            static rt_audio_capture_t   decode_side_direction(float value);
            static rt_audio_source_t    decode_source_type(float value);
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
