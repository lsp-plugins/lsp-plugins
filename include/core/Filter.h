/*
 * filter.h
 *
 *  Created on: 28 июня 2016 г.
 *      Author: sadko
 */

#ifndef CORE_FILTER_H_
#define CORE_FILTER_H_

#include <core/types.h>

namespace lsp
{
    const size_t FILTER_BUFFER_MAX          = 0x1000;
    const size_t FILTER_RANK_MIN            = 8;
    const size_t FILTER_RANK_MAX            = 12;
    const size_t FILTER_CONVOLUTION_MAX     = (1 << FILTER_RANK_MAX);
    const size_t FILTER_POLY_MAX            = 0x20;
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
                float       ir[8];      // Impulse response
                float       delay[4];   // Delay line for processing
            } cascade_t;
            #pragma pack(pop)

            typedef struct filter_poly_t
            {
                double      vItems[(FILTER_CHAINS_MAX + 1) * 2];        // Polynom coefficients
                size_t      nItems;                                     // Number of coefficients
            } filter_poly_t;

            typedef struct filter_chain_t
            {
                cascade_t          *vItems;                             // Filter cascades
                filter_poly_t       sTop;                               // Transfer function, top part
                filter_poly_t       sBottom;                            // Transfer function, bottom part
                size_t              nItems;                             // Number of cascades
                filter_params_t     sParams;                            // Filter parameters
                uint8_t            *vData;                              // Allocated data
            } filter_chain_t;

            enum filter_mode_t
            {
                FM_BYPASS,          // Bypass filter
                FM_BILINEAR,        // Bilinear Z-transform
                FM_MATCHED          // Matched Z-transform
            };

        protected:
            filter_chain_t      sDirect;        // Filter direct chain
            size_t              nSampleRate;    // Sample rate
            filter_mode_t       nMode;          // Filter mode
            size_t              nLatency;       // Filter latency

        protected:
            static inline void poly_create(filter_poly_t *dst, size_t count);
            static inline void poly_copy(filter_poly_t *dst, const filter_poly_t *src);
            static void poly_mul(filter_poly_t *dst, const double *src);
            static double complex_transfer_calc(double *p_re, double *p_im, double x, const filter_poly_t *fp);
            static cascade_t *add_cascade(filter_chain_t *chain);

        protected:
            void freq_chart(filter_chain_t *chain, float *re, float *im, const float *f, size_t count);

            void build_filter(filter_chain_t *chain);
            void build_transfer_function(filter_chain_t *chain);

            void calc_rlc_filter(size_t type, const filter_params_t *fp, filter_chain_t *chain);
            void calc_bwc_filter(size_t type, const filter_params_t *fp, filter_chain_t *chain);
            void calc_lrx_filter(size_t type, filter_chain_t *chain);
            void bilinear_transform(filter_chain_t *chain);
            void matched_transform(filter_chain_t *chain);

        public:
            Filter();
            ~Filter();

        public:
            /**  Initialize filter
             *
             * @param sr sample rate
             * @return true on success
             */
            bool init();

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
             */
            void impulse_response(float *out, size_t length);

            /** Get frequency chart
             *
             * @param re real part of the frequency chart
             * @param im imaginary part of the frequency chart
             * @param f frequencies to calculate value
             * @param count number of dots for the chart
             */
            inline void freq_chart(float *re, float *im, const float *f, size_t count)
            {
                freq_chart(&sDirect, re, im, f, count);
            }

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
