/*
 * spectrum_analyzer.h
 *
 *  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#ifndef METADATA_SPECTRUM_ANALYZER_H_
#define METADATA_SPECTRUM_ANALYZER_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Spectrum analyzer
    struct spectrum_analyzer_base_metadata
    {
        static const float          FREQ_MIN            = SPEC_FREQ_MIN;
        static const float          FREQ_DFL            = 1000.0f;
        static const float          FREQ_MAX            = SPEC_FREQ_MAX;

        static const size_t         PORTS_PER_INPUT     = 6;
        static const size_t         RANK_MIN            = 10;
        static const size_t         RANK_DFL            = 12;
        static const size_t         RANK_MAX            = 14;
        static const size_t         MESH_POINTS         = 640;

        static const float          THRESH_HI_DB        = 0.0f;
        static const float          THRESH_LO_DB        = -48.0f;
        static const float          SPECTRALIZER_BOOST  = 16.0f;
//        static const size_t         MMESH_POINTS        = 40;
        static const size_t         MMESH_STEP          = 16;
        static const size_t         WND_DFL             = windows::HANN;
        static const size_t         ENV_DFL             = envelope::PINK_NOISE;
        static const size_t         FB_ROWS             = 360;
        static const float          FB_TIME             = 8.0f;

        static const float          REACT_TIME_MIN      = 0.000;
        static const float          REACT_TIME_MAX      = 1.000;
        static const float          REACT_TIME_DFL      = 0.200;
        static const float          REACT_TIME_STEP     = 0.001;

        static const float          SELECTOR_MIN        = 0.01;
        static const float          SELECTOR_DFL        = 0;
        static const float          SELECTOR_MAX        = 100;
        static const float          SELECTOR_STEP       = 0.005;

        static const float          ZOOM_MIN            = GAIN_AMP_M_36_DB;
        static const float          ZOOM_MAX            = GAIN_AMP_0_DB;
        static const float          ZOOM_DFL            = GAIN_AMP_0_DB;
        static const float          ZOOM_STEP           = 0.025f;

        static const float          PREAMP_DFL          = 1.0;

        static const size_t         REFRESH_RATE        = 20;
    };

    struct spectrum_analyzer_x1_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct spectrum_analyzer_x2_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct spectrum_analyzer_x4_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct spectrum_analyzer_x8_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct spectrum_analyzer_x12_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct spectrum_analyzer_x16_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct spectrum_analyzer_x24_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct spectrum_analyzer_x32_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };
}

#endif /* METADATA_SPECTRUM_ANALYZER_H_ */
