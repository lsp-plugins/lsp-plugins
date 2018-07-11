/*
 * impulse_reverb.h
 *
 *  Created on: 13 фев. 2017 г.
 *      Author: sadko
 */

#ifndef METADATA_IMPULSE_REVERB_H_
#define METADATA_IMPULSE_REVERB_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Impulse reverb metadata
    struct impulse_reverb_base_metadata
    {
        static const float CONV_LENGTH_MIN          = 0.0f;     // Minimum convolution length (ms)
        static const float CONV_LENGTH_MAX          = 10000.0f; // Maximum convoluition length (ms)
        static const float CONV_LENGTH_DFL          = 0.0f;     // Convolution length (ms)
        static const float CONV_LENGTH_STEP         = 0.1f;     // Convolution step (ms)

        static const float PREDELAY_MIN             = 0.0f;     // Minimum pre-delay length (ms)
        static const float PREDELAY_MAX             = 100.0f;   // Maximum pre-delay length (ms)
        static const float PREDELAY_DFL             = 0.0f;     // Pre-delay length (ms)
        static const float PREDELAY_STEP            = 0.01f;    // Pre-delay step (ms)

        static const size_t MESH_SIZE               = 600;      // Maximum mesh size
        static const size_t TRACKS_MAX              = 2;        // Maximum tracks per mesh/sample

        static const size_t FILES                   = 4;        // Number of IR files
        static const size_t CONVOLVERS              = 4;        // Number of IR convolvers

        static const size_t FFT_RANK_MIN            = 9;        // Minimum FFT rank

        enum fft_rank_t
        {
            FFT_RANK_512,
            FFT_RANK_1024,
            FFT_RANK_2048,
            FFT_RANK_4096,
            FFT_RANK_8192,
            FFT_RANK_16384,
            FFT_RANK_32767,
            FFT_RANK_65536,

            FFT_RANK_DEFAULT = FFT_RANK_32767
        };
    };

    struct impulse_reverb_mono_metadata: public impulse_reverb_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct impulse_reverb_stereo_metadata: public impulse_reverb_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

}

#endif /* METADATA_IMPULSE_REVERB_H_ */
