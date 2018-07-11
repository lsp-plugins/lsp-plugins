/*
 * phase_detector.h
 *
 *  Created on: 16 окт. 2015 г.
 *      Author: sadko
 */

#ifndef METADATA_PHASE_DETECTOR_H_
#define METADATA_PHASE_DETECTOR_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Phase detector metadata
    struct phase_detector_metadata
    {
        static const plugin_metadata_t metadata;

        static const float DETECT_TIME_MIN          =   1.0f;
        static const float DETECT_TIME_MAX          =   50.0f;
        static const float DETECT_TIME_DFL          =   10.0f;
        static const float DETECT_TIME_STEP         =   0.0025f;
        static const float DETECT_TIME_RANGE_MAX    =   100.0f;
        static const float DETECT_TIME_RANGE_MIN    =   - 100.0f;

        static const size_t MESH_POINTS             =   256;

        static const float REACT_TIME_MIN           =   0.000;
        static const float REACT_TIME_MAX           =  10.000;
        static const float REACT_TIME_DFL           =   1.000;
        static const float REACT_TIME_STEP          =   0.0025;

        static const float SELECTOR_MIN             =   -100.0f;
        static const float SELECTOR_MAX             =   100.0f;
        static const float SELECTOR_DFL             =   0.0f;
        static const float SELECTOR_STEP            =   0.1f;

        static const float SAMPLES_MIN              =   - 50.0f /* DETECT_TIME_MAX [ms] */ * 0.001 /* [s/ms] */ * MAX_SAMPLE_RATE /* [ samples / s ] */;
        static const float SAMPLES_MAX              =   + 50.0f /* DETECT_TIME_MAX [ms] */ * 0.001 /* [s/ms] */ * MAX_SAMPLE_RATE /* [ samples / s ] */;
        static const float DISTANCE_MIN             =   - 50.0f /* DETECT_TIME_MAX [ms] */ * 0.001 /* [s/ms] */ * MAX_SOUND_SPEED /* [ m / s] */ * 100 /* c / m */;
        static const float DISTANCE_MAX             =   + 50.0f /* DETECT_TIME_MAX [ms] */ * 0.001 /* [s/ms] */ * MAX_SOUND_SPEED /* [ m / s] */ * 100 /* c / m */;
        static const float TIME_MIN                 =   - 50.0f /* DETECT_TIME_MAX [ms] */;
        static const float TIME_MAX                 =   + 50.0f /* DETECT_TIME_MAX [ms] */;

        enum ports
        {
            // Input Audio
            IN_A, IN_B,

            // Output Audio
            OUT_A, OUT_B,

            // Input controls
            BYPASS, RESET,
            TIME, REACTIVITY, SELECTOR,

            // Output controls/meters
            BEST_TIME, BEST_SAMPLES, BEST_DISTANCE, BEST_VALUE,
            SEL_TIME, SEL_SAMPLES, SEL_DISTANCE, SEL_VALUE,
            WORST_TIME, WORST_SAMPLES, WORST_DISTANCE, WORST_VALUE,
            FUNCTION
        };
    };
}

#endif /* METADATA_PHASE_DETECTOR_H_ */
