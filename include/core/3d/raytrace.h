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
        RT_AS_TRIANGLE,     // For debug purposes
        RT_AS_TETRA,        // Tetrahedron source
        RT_AS_OCTA,         // Octa source
        RT_AS_BOX,          // Simple box source
        RT_AS_ICO,          // Icosahedron source
        RT_AS_CYLINDER,     // Cylinder
        RT_AS_CONE,         // Cone
        RT_AS_OCTASPHERE,   // Omni source (octasphere)
        RT_AS_ICOSPHERE,    // Omni source (icosphere)
        RT_AS_FSPOT,        // Flat spot
        RT_AS_CSPOT,        // Cylindric spot
        RT_AS_SSPOT,        // Spherical spot
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

    typedef struct room_source_config_t
    {
        point3d_t               sPos;       // Position in 3D space
        float                   fYaw;       // Yaw angle (degrees)
        float                   fPitch;     // Pitch angle (degrees)
        float                   fRoll;      // Roll angle (degrees)
        rt_audio_source_t       enType;     // Type of source
        float                   fSize;      // Size/radius [m]
        float                   fHeight;    // Height [m]
        float                   fAngle;     // Dispersion angle [0..100] %
        float                   fCurvature; // Additional curvature [0..100] %
        float                   fAmplitude; // Initial amplitude of the signal
    } room_source_config_t;

    // Source configuration
    typedef struct rt_source_settings_t
    {
        matrix3d_t              pos;        // Position and direction of source
        rt_audio_source_t       type;       // Type of the the source
        float                   size;       // Size/radius [m]
        float                   height;     // Height [m]
        float                   angle;      // Dispersion angle [0..100] %
        float                   curvature;  // Additional curvature [0..100] %
        float                   amplitude;  // Initial amplitude of the signal
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

    typedef struct rt_capture_settings_t
    {
        matrix3d_t              pos;        // Position in 3D space
        float                   radius;     // Capture radius
        rt_audio_capture_t      type;       // Capture type
    } rt_capture_settings_t;

    /**
     * Generate raytracing source groups' mesh according to settings of the audio source
     * The function does not apply transform matrix to the output
     *
     * @param out raytracing groups
     * @param cfg source configuration
     * @return status of operation
     */
    status_t rt_gen_source_mesh(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg);

    /**
     * Generate raytracing capture mesh groups according to settings of the audio capture
     * The function does not apply transform matrix to the output
     *
     * @param out triangle mesh
     * @param cfg source configuration
     * @return status of operation
     */
    status_t rt_gen_capture_mesh(cstorage<raw_triangle_t> &out, const rt_capture_settings_t *cfg);

    /**
     * Configure capture
     * @param n number of captures generated
     * @param settings array of two structures to store capture settings
     * @param cfg capture configuration
     * @return status of operation
     */
    status_t rt_configure_capture(size_t *n, rt_capture_settings_t *settings, const room_capture_config_t *cfg);

    /**
     * Configure source settings
     * @param out source settings
     * @param in source configuration
     * @return status of operation
     */
    status_t rt_configure_source(rt_source_settings_t *out, const room_source_config_t *in);



}


#endif /* CORE_3D_RAYTRACE_H_ */
