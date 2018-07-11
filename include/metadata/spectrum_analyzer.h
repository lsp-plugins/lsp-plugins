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
        static const size_t         WND_DFL             = windows::HANN;
        static const size_t         ENV_DFL             = envelope::PINK_NOISE;

        static const float          REACT_TIME_MIN      = 0.000;
        static const float          REACT_TIME_MAX      = 1.000;
        static const float          REACT_TIME_DFL      = 0.200;
        static const float          REACT_TIME_STEP     = 0.001;

        static const float          SELECTOR_MIN        = 0;
        static const float          SELECTOR_DFL        = 0;
        static const float          SELECTOR_MAX        = 100;
        static const float          SELECTOR_STEP       = 0.005;

        static const float          PREAMP_DFL          = 1.0;

        static const size_t         REFRESH_RATE        = 20;
    };

    #define SA_INPUT(x) IN_ ## x, OUT_ ## x, ON_ ## x, SOLO_ ## x, FREEZE_ ## x, HUE_ ## x, SHIFT_ ## x, SPECTRUM_ ## x
    #define SA_COMMON   BYPASS, TOLERANCE, WINDOW, ENVELOPE, PREAMP, REACT, CHANNEL, SELECTOR, FREQUENCY, LEVEL

    struct spectrum_analyzer_x1_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x2_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x4_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x8_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_INPUT(4),
            SA_INPUT(5),
            SA_INPUT(6),
            SA_INPUT(7),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x12_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_INPUT(4),
            SA_INPUT(5),
            SA_INPUT(6),
            SA_INPUT(7),
            SA_INPUT(8),
            SA_INPUT(9),
            SA_INPUT(10),
            SA_INPUT(11),
            SA_INPUT(12),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x16_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_INPUT(4),
            SA_INPUT(5),
            SA_INPUT(6),
            SA_INPUT(7),
            SA_INPUT(8),
            SA_INPUT(9),
            SA_INPUT(10),
            SA_INPUT(11),
            SA_INPUT(12),
            SA_INPUT(13),
            SA_INPUT(14),
            SA_INPUT(15),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x24_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_INPUT(4),
            SA_INPUT(5),
            SA_INPUT(6),
            SA_INPUT(7),
            SA_INPUT(8),
            SA_INPUT(9),
            SA_INPUT(10),
            SA_INPUT(11),
            SA_INPUT(12),
            SA_INPUT(13),
            SA_INPUT(14),
            SA_INPUT(15),
            SA_INPUT(16),
            SA_INPUT(17),
            SA_INPUT(18),
            SA_INPUT(19),
            SA_INPUT(20),
            SA_INPUT(21),
            SA_INPUT(22),
            SA_INPUT(23),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x32_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_INPUT(4),
            SA_INPUT(5),
            SA_INPUT(6),
            SA_INPUT(7),
            SA_INPUT(8),
            SA_INPUT(9),
            SA_INPUT(10),
            SA_INPUT(11),
            SA_INPUT(12),
            SA_INPUT(13),
            SA_INPUT(14),
            SA_INPUT(15),
            SA_INPUT(16),
            SA_INPUT(17),
            SA_INPUT(18),
            SA_INPUT(19),
            SA_INPUT(20),
            SA_INPUT(21),
            SA_INPUT(22),
            SA_INPUT(23),
            SA_INPUT(24),
            SA_INPUT(25),
            SA_INPUT(26),
            SA_INPUT(27),
            SA_INPUT(28),
            SA_INPUT(29),
            SA_INPUT(30),
            SA_INPUT(31),
            SA_COMMON
        };
    };

    #undef SA_INPUT
    #undef SA_COMMON
}

#endif /* METADATA_SPECTRUM_ANALYZER_H_ */
