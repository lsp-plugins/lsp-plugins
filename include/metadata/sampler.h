/*
 * sampler.h
 *
 *  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#ifndef METADATA_SAMPLER_H_
#define METADATA_SAMPLER_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Sampler metadata
    struct sampler_kernel_metadata
    {
        static const float SAMPLE_LENGTH_MIN        = 0.0f;     // Minimum length (ms)
        static const float SAMPLE_LENGTH_MAX        = 64000.0f; // Maximum sample length (ms)
        static const float SAMPLE_LENGTH_DFL        = 0.0f;     // Sample length (ms)
        static const float SAMPLE_LENGTH_STEP       = 0.1f;     // Sample step (ms)

        static const float PREDELAY_MIN             = 0.0f;     // Pre-delay min (ms)
        static const float PREDELAY_MAX             = 100.0f;   // Pre-delay max (ms)
        static const float PREDELAY_DFL             = 0.0f;     // Pre-delay default (ms)
        static const float PREDELAY_STEP            = 0.1f;     // Pre-delay step (ms)

        static const float FADEOUT_MIN              = 0.0f;     // Fade-out min (ms)
        static const float FADEOUT_MAX              = 50.0f;    // Fade-out max (ms)
        static const float FADEOUT_DFL              = 10.0f;    // Fade-out default (ms)
        static const float FADEOUT_STEP             = 0.025f;   // Fade-out step (ms)

        static const size_t MESH_SIZE               = 320;      // Maximum mesh size
        static const size_t TRACKS_MAX              = 2;        // Maximum tracks per mesh/sample
        static const float ACTIVITY_LIGHTING        = 0.1f;     // Activity lighting (seconds)

        static const size_t CHANNEL_DFL             = 0;        // Default channel
        static const size_t NOTE_DFL                = 9;        // A
        static const size_t OCTAVE_DFL              = 4;        // 4th octave
    };

    struct sampler_base_metadata
    {
        static const float DRIFT_MIN                = 0.0f;     // Minimum delay
        static const float DRIFT_DFL                = 0.0f;     // Default delay
        static const float DRIFT_STEP               = 0.1f;     // Delay step
        static const float DRIFT_MAX                = 100.0f;   // Maximum delay

        static const float DYNA_MIN                 = 0.0f;     // Minimum dynamics
        static const float DYNA_DFL                 = 0.0f;     // Default dynamics
        static const float DYNA_STEP                = 0.1f;     // Dynamics step
        static const float DYNA_MAX                 = 100.0f;   // Maximum dynamics

        static const size_t PLAYBACKS_MAX           = 8192;     // Maximum number of simultaneously playing samples
        static const size_t SAMPLE_FILES            = 8;        // Number of sample files
        static const size_t BUFFER_SIZE             = 4096;     // Size of temporary buffer

        static const size_t INSTRUMENTS_MAX         = 64;       // Maximum supported instruments
    };

    // Single samplers
    struct sampler_mono_metadata: public sampler_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sampler_stereo_metadata: public sampler_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    // Multiple-instrument sampler, has always stereo in and stereo out
    struct multisampler_x12_metadata: public sampler_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct multisampler_x24_metadata: public sampler_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct multisampler_x48_metadata: public sampler_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct multisampler_x12_do_metadata: public sampler_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct multisampler_x24_do_metadata: public sampler_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct multisampler_x48_do_metadata: public sampler_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

}


#endif /* METADATA_SAMPLER_H_ */
