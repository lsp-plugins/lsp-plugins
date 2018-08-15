/*
 * filter.h
 *
 *  Created on: 28 июня 2016 г.
 *      Author: sadko
 */

#ifndef CORE_FILTERS_FILTER_H_
#define CORE_FILTERS_FILTER_H_

#include <dsp/dsp.h>
#include <core/filters/common.h>
#include <core/filters/FilterBank.h>

namespace lsp
{
    class Filter
    {
        protected:
            #pragma pack(push, 16)
            typedef struct cascade_t
            {
                double      t[4];       // Top part of polynom (zeros)
                double      b[4];       // Bottom part of polynom (poles)
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
                FF_OWN_BANK     = 1 << 0,       // Filter has it's own filter bank
                FF_REBUILD      = 1 << 1,       // Need to rebuild filter
                FF_CLEAR        = 1 << 2        // Need to clear filter memory
            };

        protected:
            FilterBank         *pBank;          // External bank of filters
            filter_params_t     sParams;        // Filter parameters
            size_t              nSampleRate;    // Sample rate
            filter_mode_t       nMode;          // Filter mode
            size_t              nItems;         // Number of cascades
            cascade_t          *vItems;         // Filter cascades
            uint8_t            *vData;          // Allocated data
            size_t              nFlags;         // Filter flags
            size_t              nLatency;       // Filter latency

        protected:

            void complex_transfer_calc(float *re, float *im, double f);
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

            /** Get frequency chart
             *
             * @param c complex transfer function results
             * @param f frequencies to calculate value
             * @param count number of dots for the chart
             */
            void freq_chart(float *c, const float *f, size_t count);

            /** Mark filter to be cleared
             *
             */
            inline void clear()             { nFlags     |= FF_CLEAR;       }

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


#endif /* CORE_FILTERS_FILTER_H_ */
