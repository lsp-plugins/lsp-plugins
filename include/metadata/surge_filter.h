/*
 * surge_filter.h
 *
 *  Created on: 4 июл. 2020 г.
 *      Author: sadko
 */

#ifndef METADATA_SURGE_FILTER_H_
#define METADATA_SURGE_FILTER_H_

namespace lsp
{
    struct surge_filter_base_metadata
    {
        static const float THRESH_MIN       = GAIN_AMP_M_80_DB;
        static const float THRESH_MAX       = GAIN_AMP_M_24_DB;
        static const float THRESH_DFL       = GAIN_AMP_M_72_DB;
        static const float THRESH_STEP      = 0.01f;

        static const float ATTACK_MIN       = 0.0f;
        static const float ATTACK_MAX       = 5.0f;
        static const float ATTACK_DFL       = 0.0f;
        static const float ATTACK_STEP      = 0.01f;

        static const float RELEASE_MIN      = 0.0f;
        static const float RELEASE_MAX      = 100.0f;
        static const float RELEASE_DFL      = 10.0f;
        static const float RELEASE_STEP     = 0.01f;

        static const float FADE_MIN         = 1.0f;
        static const float FADE_MAX         = 5000.0f;
        static const float FADE_DFL         = 100.0f;
        static const float FADE_STEP        = 0.1f;

        static const size_t MESH_POINTS     = 640;
        static const float MESH_TIME        = 5.0f;
    };

    struct surge_filter_mono_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct surge_filter_stereo_metadata
    {
        static const plugin_metadata_t metadata;
    };
}


#endif /* METADATA_SURGE_FILTER_H_ */
