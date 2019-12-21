/*
 * trigger.h
 *
 *  Created on: 04 мая 2016 г.
 *      Author: sadko
 */

#ifndef METADATA_TRIGGER_H_
#define METADATA_TRIGGER_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Trigger metadata
    struct trigger_base_metadata
    {
        static const size_t TRACKS_MAX              = 2;        // Maximum number of audio tracks
        static const size_t SAMPLE_FILES            = 8;        // Number of sample files per trigger
        static const size_t BUFFER_SIZE             = 4096;     // Size of temporary buffer

        static const float  DETECT_LEVEL_DFL        = GAIN_AMP_M_12_DB;    // Default detection level [G]

        static const float  RELEASE_LEVEL_MIN       = 0.0f;     // Minimum relative release level
        static const float  RELEASE_LEVEL_DFL       = GAIN_AMP_M_3_DB;    // Default release level [G]
        static const float  RELEASE_LEVEL_MAX       = 0.0f;     // Maximum relative release level
        static const float  RELEASE_LEVEL_STEP      = 0.0001f;  // Release level step [G]

        static const float  DETECT_TIME_MIN         = 0.0f;     // Minimum detection time [ms]
        static const float  DETECT_TIME_DFL         = 5.0f;     // Default detection time [ms]
        static const float  DETECT_TIME_MAX         = 20.0f;    // Maximum detection time [ms]
        static const float  DETECT_TIME_STEP        = 0.0025f;  // Detection time step [ms]

        static const float  RELEASE_TIME_MIN        = 0.0f;     // Minimum release time [ms]
        static const float  RELEASE_TIME_DFL        = 10.0f;    // Default release time [ms]
        static const float  RELEASE_TIME_MAX        = 100.0f;   // Maximum release time [ms]
        static const float  RELEASE_TIME_STEP       = 0.005f;   // Release time step [ms]

        static const float  DYNAMICS_MIN            = 0.0f;     // Minimum dynamics [%]
        static const float  DYNAMICS_DFL            = 10.0f;    // Default dynamics [%]
        static const float  DYNAMICS_MAX            = 100.0f;   // Maximum dynamics [%]
        static const float  DYNAMICS_STEP           = 0.05f;    // Dynamics step [%]

        static const float  REACTIVITY_MIN          = 0.000;    // Minimum reactivity [ms]
        static const float  REACTIVITY_MAX          = 250;      // Maximum reactivity [ms]
        static const float  REACTIVITY_DFL          = 20;       // Default reactivity [ms]
        static const float  REACTIVITY_STEP         = 0.01;     // Reactivity step

        static const float  HISTORY_TIME            = 5.0f;     // Amount of time to display history [s]
        static const size_t HISTORY_MESH_SIZE       = 640;      // 640 dots for history

        static const size_t MODE_DFL                = 1;        // RMS
    };

    struct trigger_midi_metadata
    {
        static const size_t CHANNEL_DFL             = 0;        // Default channel
        static const size_t NOTE_DFL                = 11;       // B
        static const size_t OCTAVE_DFL              = 2;        // 2nd octave

        static const size_t MODE_DFL                = 1;        // Trigger mode
    };

    // Trigger metadata
    struct trigger_mono_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct trigger_stereo_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct trigger_midi_mono_metadata: public trigger_midi_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct trigger_midi_stereo_metadata: public trigger_midi_metadata
    {
        static const plugin_metadata_t metadata;
    };
}

#endif /* METADATA_TRIGGER_H_ */
