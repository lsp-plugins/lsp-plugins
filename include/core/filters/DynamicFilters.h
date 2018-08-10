/*
 * DynamicFilters.h
 *
 *  Created on: 1 февр. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_FILTERS_DYNAMICFILTERS_H_
#define CORE_FILTERS_DYNAMICFILTERS_H_

#include <dsp/dsp.h>
#include <core/filters/common.h>
#include <core/status.h>

namespace lsp
{
    /** This class implements set of dynamic filters grouped
     * into one object for resource economy purpose.
     *
     */
    class DynamicFilters
    {
        protected:
            typedef struct filter_t
            {
                filter_params_t     sParams;                // Filter parameters
                bool                bActive;                // Filter activity
            } filter_t;

            union biquad_bank_t
            {
                void               *ptr;
                biquad_x1_t        *x1;
                biquad_x2_t        *x2;
                biquad_x4_t        *x4;
                biquad_x8_t        *x8;
            };

            static const f_cascade_t    sNormal;

        protected:
            filter_t           *vFilters;           // Array of filters
            f_cascade_t        *vCascades;          // Analog filter cascade bank
            float              *vMemory;            // Filter memory
            biquad_bank_t       vBiquads;           // Biquad bank
            size_t              nFilters;           // Number of filters
            size_t              nSampleRate;        // Sample rate
            void               *pData;              // Aligned pointer data
            bool                bClearMem;          // Clear memory

        protected:
            size_t              quantify(size_t c, size_t nc);
            size_t              build_filter_bank(f_cascade_t *dst, const filter_params_t *fp, size_t cj, const float *sfg, size_t samples);
            size_t              build_lrx_ladder_filter_bank(f_cascade_t *dst, const filter_params_t *fp, size_t cj, const float *sfg, size_t samples, size_t ftype);
            size_t              build_lrx_shelf_filter_bank(f_cascade_t *dst, const filter_params_t *fp, size_t cj, const float *sfg, size_t samples, size_t ftype);

            size_t              precalc_lrx_ladder_filter_bank(f_cascade_t *dst, const filter_params_t *fp, size_t cj, const float *sfg, size_t samples);
            void                calc_lrx_ladder_filter_bank(f_cascade_t *dst, const filter_params_t *fp, size_t cj, size_t samples, size_t ftype, size_t nc);

            void                complex_transfer_calc(float *re, float *im, double f, size_t nc);

        public:
            DynamicFilters();
            ~DynamicFilters();

        public:
            /** Initialize the dynamic filters set
             *
             * @param filters
             * @return
             */
            status_t init(size_t filters);

            /** Set sample rate
             *
             * @param sr sample rate
             */
            void set_sample_rate(size_t sr);

            /** Destroy the dynamic filters set
             *
             */
            void destroy();

            /** Check that filter is active
             *
             * @param id ID of filter
             * @return true if filter is active
             */
            inline bool filter_active(size_t id) const { return (id < nFilters) ? vFilters[id].bActive : false; };

            /** Check that filter is inactive
             *
             * @param id ID of filter
             * @return true if filter is inactive
             */
            inline bool filter_inactive(size_t id) const { return (id < nFilters) ? !vFilters[id].bActive : true; };

            /** Set activity of the specific filter
             *
             * @param id filter identifier
             * @param active activity of the specific filter
             * @return true on success
             */
            inline bool set_filter_active(size_t id, bool active)
            {
                if (id >= nFilters)
                    return false;
                vFilters[id].bActive        = true;
                return true;
            }

            /** Update filter parameters
             * @param id ID of the filter
             * @param params  filter parameters
             * @return true on success
             */
            bool set_params(size_t id, const filter_params_t *params);

            /** Get filter parameters
             * @param id ID of the filter
             * @param params  filter parameters
             * @return true on success
             */
            bool get_params(size_t id, filter_params_t *params);

            /** Process signal with filter varying by it's gain parameter
             *
             * @param id filer identifier
             * @param out output signal
             * @param in input signal
             * @param gain the gain level of the filter
             * @param samples number of samples to process
             */
            void process(size_t id, float *out, const float *in, const float *gain, size_t samples);

            /** Get frequency chart of the specific filter
             *
             * @param id ID of the filter
             * @param re real part of the frequency chart
             * @param im imaginary part of the frequency chart
             * @param f frequencies to calculate value
             * @param count number of dots for the chart
             */
            bool freq_chart(size_t id, float *re, float *im, const float *f, float gain, size_t count);

            /** Get frequency chart of the specific filter
             *
             * @param id ID of the filter
             * @param dst array of complex numbers to store data
             * @param f frequencies to calculate value
             * @param count number of dots for the chart
             */
            bool freq_chart(size_t id, float *dst, const float *f, float gain, size_t count);
    };
} /* namespace lsp */

#endif /* CORE_FILTERS_DYNAMICFILTERS_H_ */
