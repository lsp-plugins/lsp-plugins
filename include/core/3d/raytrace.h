/*
 * raytrace.h
 *
 *  Created on: 12 мая 2019 г.
 *      Author: sadko
 */

#ifndef CORE_3D_RAYTRACE_H_
#define CORE_3D_RAYTRACE_H_

#include <data/cstorage.h>
#include <core/3d/common.h>
#include <core/status.h>

namespace lsp
{
    enum rt_audio_source_t
    {
        RT_AS_OMNI,     // Omni source (icosphere)
        RT_AS_OMNI2,    // Omni source (octasphere)
        RT_AS_ICO,      // Icosahedron source
        RT_AS_TETRA,    // Tetra source
        RT_AS_OCTA,     // Octa source
        RT_AS_BOX,      // Simple box source
        RT_AS_CYLINDER, // Cylinder
        RT_AS_CONE,     // Cone
        RT_AS_SPOT,     // Conical spot
        RT_AS_SSPOT,    // Spherical spot
        RT_AS_CSPOT,    // Cylindric spot
        RT_AS_TRIANGLE  // For debug purposes
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

    // Source configuration
    typedef struct room_source_settings_t
    {
        matrix3d_t              pos;        // Position and direction of source
        rt_audio_source_t       type;       // Type of the the source
        float                   size;       // Size/radius
        float                   height;     // Height for the CYLINDER
        float                   angle;      // Angle of the SPOT and CYLINDRIC sources (degrees)
    } room_source_settings_t;

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
        matrix3d_t              pos[2];     // Position and direction of capture
        rt_audio_capture_t      type[2];    // Type of capture
        float                   r[2];       // Capture radius
        size_t                  n;          // Number of elements
    } room_capture_settings_t;


    /**
     * Generate raytracing groups according to settings of the audio source
     * @param out raytracing groups
     * @param cfg configuration
     * @return status of operation
     */
    status_t gen_source_mesh(cstorage<rt_group_t> &out, const room_source_settings_t *cfg);

}


#endif /* CORE_3D_RAYTRACE_H_ */
