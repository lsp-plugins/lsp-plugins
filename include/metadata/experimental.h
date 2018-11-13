/*
 * experimental.h
 *
 *  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#ifndef METADATA_EXPERIMENTAL_H_
#define METADATA_EXPERIMENTAL_H_

namespace lsp
{
#ifndef LSP_NO_EXPERIMENTAL
    struct test_plugin_metadata
    {
        static const float          BAND_GAIN_MIN       = GAIN_AMP_M_36_DB;
        static const float          BAND_GAIN_MAX       = GAIN_AMP_P_36_DB;
        static const float          BAND_GAIN_DFL       = GAIN_AMP_0_DB;
        static const float          BAND_GAIN_STEP      = 0.1f;

        static const float PREDELAY_MIN             = 0.0f;     // Pre-delay min (ms)
        static const float PREDELAY_MAX             = 100.0f;   // Pre-delay max (ms)
        static const float PREDELAY_DFL             = 0.0f;     // Pre-delay default (ms)
        static const float PREDELAY_STEP            = 0.1f;     // Pre-delay step (ms)

        static const float FILE_LENGTH_MIN          = 0.0f;     // Minimum convolution length (ms)
        static const float FILE_LENGTH_MAX          = 10000.0f; // Maximum convoluition length (ms)
        static const float FILE_LENGTH_DFL          = 0.0f;     // Convolution length (ms)
        static const float FILE_LENGTH_STEP         = 0.1f;     // Convolution step (ms)

        static const size_t MESH_SIZE               = 256;      // Maximum mesh size
        static const size_t TRACKS_MAX              = 2;        // Maximum tracks per mesh/sample

        static const size_t FRM_BUFFER_SIZE         = 256;

        static const plugin_metadata_t metadata;
    };
#endif
}

#endif /* METADATA_EXPERIMENTAL_H_ */
