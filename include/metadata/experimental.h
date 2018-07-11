/*
 * experimental.h
 *
 *  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#ifndef METADATA_EXPERIMENTAL_H_
#define METADATA_EXPERIMENTAL_H_

//-------------------------------------------------------------------------
// Experimental plugins' metadata

namespace lsp
{
#ifndef LSP_NO_EXPERIMENTAL
    //-------------------------------------------------------------------------
    // Impulse responses metadata
    struct impulse_responses_base_metadata
    {
        static const float CONVOLUTION_TIME_MAX         = 10.0; // Max convolution time (in seconds)
        static const size_t CONVOLUTION_BUFFER_SIZE     = 4;    // The size of shift buffer relative to maximum convolution time

        static const float CONVLEN_MIN             =   0.0f;
        static const float CONVLEN_MAX             =   100.0f;
        static const float CONVLEN_DFL             =   100.0f;
        static const float CONVLEN_STEP            =   0.1f;
    };

    struct impulse_responses_metadata: public impulse_responses_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            // Input Audio
            IN,

            // Output Audio
            OUT,

            // Input controls
            BYPASS, FNAME, CHANNEL, LENGTH, DRY, WET, OUT_GAIN

            // Output controls/meters
        };
    };
#endif
}

#endif /* METADATA_EXPERIMENTAL_H_ */
