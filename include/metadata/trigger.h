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

    struct audio_trigger_kernel_metadata
    {
        static const size_t CHANNEL_DFL             = 0;        // Default output channel
        static const size_t NOTE_DFL                = 9;        // A
        static const size_t OCTAVE_DFL              = 4;        // 4th octave

        static const float  DETECT_LEVEL_DFL        = 0.125f;   // Default detection level [G]
        static const float  RELEASE_LEVEL_DFL       = 0.25f;    // Default release level [G]

        static const float  DETECT_TIME_MIN         = 0.0f;     // Minimum detection time [ms]
        static const float  DETECT_TIME_DFL         = 5.0f;     // Default detection time [ms]
        static const float  DETECT_TIME_MAX         = 20.0f;    // Maximum detection time [ms]
        static const float  DETECT_TIME_STEP        = 0.0025f;  // Detection time step [ms]

        static const float  RELEASE_TIME_MIN        = 0.0f;     // Minimum release time [ms]
        static const float  RELEASE_TIME_DFL        = 10.0f;    // Default release time [ms]
        static const float  RELEASE_TIME_MAX        = 100.0f;   // Maximum release time [ms]
        static const float  RELEASE_TIME_STEP       = 0.005f;   // Release time step [ms]

        static const float  REACTIVITY_MIN          = 0.000;    // Minimum reactivity
        static const float  REACTIVITY_MAX          = 0.250;    // Maximum reactivity
        static const float  REACTIVITY_DFL          = 0.050;    // Default reactivity
        static const float  REACTIVITY_STEP         = 0.0025;   // Reactivity step

    };

    struct trigger_base_metadata
    {
        static const size_t TRACKS_MAX              = 2;        // Maximum number of audio tracks
        static const size_t SAMPLE_FILES            = 8;        // Number of sample files per trigger
        static const size_t BUFFER_SIZE             = 4096;     // Size of temporary buffer
    };

    struct trigger_midi_metadata
    {
        static const size_t CHANNEL_DFL             = 0;        // Default channel
        static const size_t NOTE_DFL                = 11;       // B
        static const size_t OCTAVE_DFL              = 2;        // 2nd octave
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

    struct trigger_mono_midi_metadata: public trigger_midi_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct trigger_stereo_midi_metadata: public trigger_midi_metadata
    {
        static const plugin_metadata_t metadata;
    };
}

#endif /* METADATA_TRIGGER_H_ */
