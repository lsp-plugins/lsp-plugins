/*
 * raytrace.h
 *
 *  Created on: 12 мая 2019 г.
 *      Author: sadko
 */

#ifndef CORE_3D_RAYTRACE_H_
#define CORE_3D_RAYTRACE_H_

namespace lsp
{
    enum rt_audio_source_t
    {
        RT_AS_SPOT,
        RT_AS_SPEAKER,
        RT_AS_OMNI,
        RT_AS_TRIANGLE
    };

    enum rt_audio_capture_t
    {
        RT_AC_CARDIO,
        RT_AC_SCARDIO,
        RT_AC_HCARDIO,
        RT_AC_BIDIR,
        RT_AC_EIGHT,
        RT_AC_OMNI
    };

    enum rt_capture_config_t
    {
        RT_CC_MONO,
        RT_CC_XY,
        RT_CC_AB,
        RT_CC_ORTF,
        RT_CC_MS
    };

    // Capture configuration
    typedef struct room_capture_config_t
    {
        point3d_t               sPos;       // Position in 3D space
        float                   fYaw;       // Yaw angle (degrees)
        float                   fPitch;     // Pitch angle (degrees)
        float                   fRoll;      // Roll angle (degrees)
        float                   fCapsule;   // Capsule size
        rt_capture_config_t     sConfig;    // Capture configuration
        float                   fAngle;     // Capture angle between microphones
        float                   fDistance;  // Capture distance between AB microphones
        rt_audio_capture_t      enDirection;// Capture microphone direction
        rt_audio_capture_t      enSide;     // Side microphone direction
    } room_capture_config_t;

    typedef struct room_capture_settings_t
    {
        ray3d_t                 pos[2];     // Position and direction of capture
        rt_audio_capture_t      type[2];    // Type of capture
        float                   r[2];       // Capture radius
        size_t                  n;          // Number of elements
    } room_capture_settings_t;
}


#endif /* CORE_3D_RAYTRACE_H_ */
