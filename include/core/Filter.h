/*
 * filter.h
 *
 *  Created on: 28 июня 2016 г.
 *      Author: sadko
 */

#ifndef CORE_FILTER_H_
#define CORE_FILTER_H_

#include <core/types.h>
#include <core/dsp.h>
#include <core/FilterBank.h>

namespace lsp
{
    const size_t FILTER_BUFFER_MAX          = 0x1000;
    const size_t FILTER_RANK_MIN            = 8;
    const size_t FILTER_RANK_MAX            = 12;
    const size_t FILTER_CONVOLUTION_MAX     = (1 << FILTER_RANK_MAX);
    const size_t FILTER_CHAINS_MAX          = 0x20;

    enum filter_type_t
    {
        // Disabled filter
        FLT_NONE,

        // RLC filters
        FLT_BT_RLC_LOPASS,
        FLT_MT_RLC_LOPASS,
        FLT_BT_RLC_HIPASS,
        FLT_MT_RLC_HIPASS,
        FLT_BT_RLC_LOSHELF,
        FLT_MT_RLC_LOSHELF,
        FLT_BT_RLC_HISHELF,
        FLT_MT_RLC_HISHELF,
        FLT_BT_RLC_BELL,
        FLT_MT_RLC_BELL,
        FLT_BT_RLC_RESONANCE,
        FLT_MT_RLC_RESONANCE,
        FLT_BT_RLC_NOTCH,
        FLT_MT_RLC_NOTCH,
        FLT_BT_RLC_LADDERPASS,
        FLT_MT_RLC_LADDERPASS,
        FLT_BT_RLC_LADDERREJ,
        FLT_MT_RLC_LADDERREJ,
        FLT_BT_RLC_BANDPASS,
        FLT_MT_RLC_BANDPASS,

        // Butterworth-Chebyshev filters
        FLT_BT_BWC_LOPASS,
        FLT_MT_BWC_LOPASS,
        FLT_BT_BWC_HIPASS,
        FLT_MT_BWC_HIPASS,
        FLT_BT_BWC_LOSHELF,
        FLT_MT_BWC_LOSHELF,
        FLT_BT_BWC_HISHELF,
        FLT_MT_BWC_HISHELF,
        FLT_BT_BWC_BELL,
        FLT_MT_BWC_BELL,
        FLT_BT_BWC_LADDERPASS,
        FLT_MT_BWC_LADDERPASS,
        FLT_BT_BWC_LADDERREJ,
        FLT_MT_BWC_LADDERREJ,
        FLT_BT_BWC_BANDPASS,
        FLT_MT_BWC_BANDPASS,

        // Linkwitz–Riley filters
        FLT_BT_LRX_LOPASS,
        FLT_MT_LRX_LOPASS,
        FLT_BT_LRX_HIPASS,
        FLT_MT_LRX_HIPASS,
        FLT_BT_LRX_LOSHELF,
        FLT_MT_LRX_LOSHELF,
        FLT_BT_LRX_HISHELF,
        FLT_MT_LRX_HISHELF,
        FLT_BT_LRX_BELL,
        FLT_MT_LRX_BELL,
        FLT_BT_LRX_LADDERPASS,
        FLT_MT_LRX_LADDERPASS,
        FLT_BT_LRX_LADDERREJ,
        FLT_MT_LRX_LADDERREJ,
        FLT_BT_LRX_BANDPASS,
        FLT_MT_LRX_BANDPASS
    };

    struct filter_t;

    typedef void (* filter_function_t)(filter_t *f, const float *in, float *out, float *pass, size_t samples);
    typedef void (* filter_chart_t)(filter_t *f, float *re, float *im, size_t count);
    typedef void (* filter_update_t)(filter_t *f);

    typedef struct filter_params_t
    {
        size_t      nType;      // Filter class
        float       fFreq;      // Frequency
        float       fFreq2;     // Second frequency (for bandpass filter)
        float       fGain;      // Gain
        size_t      nSlope;     // Filter slope
        float       fQuality;   // Quality factor
    } filter_params_t;

    class Filter
    {
        protected:
            #pragma pack(push, 16)
            typedef struct cascade_t
            {
                double      t[4];       // Top part of polynom (zeros)
                double      b[4];       // Bottom part of polynom (poles)
//                float       ir[8];      // Impulse response
            } cascade_t;
            #pragma pack(pop)

            enum filter_mode_t
            {
                FM_BYPASS,          // Bypass filter
                FM_BILINEAR,        // Bilinear Z-transform
                FM_MATCHED          // Matched Z-transform
            };

            enum filter_flags_t
            {
                FF_REBUILD,
                FF_CLEAR
            };

        protected:
            filter_params_t     sParams;        // Filter parameters
            FilterBank          sBank;          // Internal bank of filters
            size_t              nSampleRate;    // Sample rate
            filter_mode_t       nMode;          // Filter mode
            size_t              nLatency;       // Filter latency
            size_t              nItems;         // Number of cascades
            cascade_t          *vItems;         // Filter cascades
//            biquad_t           *vFilters;       // Chain of filters
            FilterBank         *pBank;          // External bank of filters
            uint8_t            *vData;          // Allocated data
            size_t              nFlags;         // Filter flags

        protected:

            void complex_transfer_calc(float *re, float *im, double f);
//            void optimize_structure();
            cascade_t *add_cascade();

            void calc_rlc_filter(size_t type, const filter_params_t *fp);
            void calc_bwc_filter(size_t type, const filter_params_t *fp);
            void calc_lrx_filter(size_t type, const filter_params_t *fp);
            float bilinear_relative(float f1, float f2);
            void bilinear_transform();
            void matched_transform();

        public:
            Filter();
            ~Filter();

        public:
            /**  Initialize filter
             *
             * @param fb filter bank to use
             * @return true on success
             */
            bool init(FilterBank *fb);

            /** Destroy filter data
             *
             */
            void destroy();

            /** Update filter parameters
             *
             * @param params filter parameters
             */
            void update(size_t sr, const filter_params_t *params);

            /** Get current filter parameters
             *
             * @param params pointer to filter parameters to store
             */
            void get_params(filter_params_t *params);

            /** Process signal
             *
             * @param out output signal
             * @param in input signal
             * @param samples number of samples to process
             */
            void process(float *out, const float *in, size_t samples);

            /** Get the impulse response of the filter
             *
             * @param out output buffer to store the impulse response
             * @param length length of the impulse response
             * @return true if impulse response can be taken, false if need to take it from bank
             */
            bool impulse_response(float *out, size_t length);

            /** Get frequency chart
             *
             * @param re real part of the frequency chart
             * @param im imaginary part of the frequency chart
             * @param f frequencies to calculate value
             * @param count number of dots for the chart
             */
            void freq_chart(float *re, float *im, const float *f, size_t count);

            /** Rebuild filter
             * Forces the filter to rebuild into bank of filters
             */
            void rebuild();

            /** Get filter latency
             *
             * @return filter latency in samples
             */
            inline size_t latency() const   { return nLatency;  };

            /** Check if the filter is bypassed
             *
             * @return true if the filter is bypassed
             */
            inline bool inactive() const    { return nMode == FM_BYPASS; }

            /** Check if the filter is active
             *
             * @return true if the filter is active
             */
            inline bool active() const      { return nMode != FM_BYPASS; }
    };
}


#endif /* CORE_FILTER_H_ */
