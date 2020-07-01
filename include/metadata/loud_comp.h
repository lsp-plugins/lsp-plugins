/*
 * loud_comp.h
 *
 *  Created on: 30 июн. 2020 г.
 *      Author: sadko
 */

#ifndef METADATA_LC_VOLUME_H_
#define METADATA_LC_VOLUME_H_

namespace lsp
{
    struct loud_comp_base_metadata
    {
        static const float PHONS_MIN            = -83;
        static const float PHONS_MAX            = 7;
        static const float PHONS_DFL            = 0;
        static const float PHONS_STEP           = 0.1f;

        static const float FREQ_MIN             = SPEC_FREQ_MIN;
        static const float FREQ_MAX             = SPEC_FREQ_MAX;

        static const size_t FFT_RANK_MIN        = 8;
        static const size_t FFT_RANK_MAX        = 14;
        static const size_t FFT_RANK_IDX_DFL    = 4;

        static const size_t STD_DFL             = 1;

        static const size_t CURVE_MESH_SIZE     = 256;
    };

    struct loud_comp_mono_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct loud_comp_stereo_metadata
    {
        static const plugin_metadata_t metadata;
    };
}

#endif /* METADATA_LC_VOLUME_H_ */
