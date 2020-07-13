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
        static const float THRESH_MIN       = GAIN_AMP_M_120_DB;
        static const float THRESH_MAX       = GAIN_AMP_M_24_DB;
        static const float THRESH_DFL       = GAIN_AMP_M_72_DB;
        static const float THRESH_STEP      = 0.01f;

        static const float RMS_MIN          = 1.0f;
        static const float RMS_MAX          = 100.0f;
        static const float RMS_DFL          = 10.0f;
        static const float RMS_STEP         = 0.01f;

        static const float FADEIN_MIN       = 0.0f;
        static const float FADEIN_MAX       = 1000.0f;
        static const float FADEIN_DFL       = 100.0f;
        static const float FADEIN_STEP      = 0.5f;

        static const float FADEOUT_MIN      = 0.0f;
        static const float FADEOUT_MAX      = 500.0f;
        static const float FADEOUT_DFL      = 0.0f;
        static const float FADEOUT_STEP     = 0.5f;

        static const float PAUSE_MIN        = 0.0f;
        static const float PAUSE_MAX        = 100.0f;
        static const float PAUSE_DFL        = 10.0f;
        static const float PAUSE_STEP       = 0.5f;

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
