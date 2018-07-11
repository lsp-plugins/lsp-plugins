/*
 * avx256.cpp
 *
 *  Created on: 09 марта 2016 г.
 *      Author: sadko
 */

#ifdef __AVX__

#include <core/types.h>
#include <core/debug.h>
#include <core/dsp.h>
#include <core/bits.h>

#include <core/dsp.h>
#include <core/x86/avx256/const.h>
#include <core/x86/avx256/vector.h>

#endif /* __AVX__ */

namespace lsp
{
    namespace avx256
    {
        void dsp_init()
        {
            #ifdef __AVX__
            lsp_trace("Optimizing DSP for AVX instruction set");

//            dsp::copy                       = sse::copy;
//            dsp::move                       = sse::move;
//            dsp::fill                       = sse::fill;
//            dsp::fill_one                   = sse::fill_one;
//            dsp::fill_zero                  = sse::fill_zero;
//            dsp::fill_minus_one             = sse::fill_minus_one;
//            dsp::abs                        = sse::abs;
    //        dsp::abs_normalized             = sse::abs_normalized;
//            dsp::normalize                  = sse::normalize;
//            dsp::min                        = sse::min;
//            dsp::max                        = sse::max;
//            dsp::minmax                     = sse::minmax;
    //        dsp::min_index                  = sse::min_index;
    //        dsp::max_index                  = sse::max_index;
//            dsp::scale                      = sse::scale;
            dsp::multiply                   = avx256::multiply;
//            dsp::h_sum                      = sse::h_sum;
//            dsp::h_sqr_sum                  = sse::h_sqr_sum;
    //        dsp::scalar_mul                 = sse::scalar_mul;
    //        dsp::accumulate                 = sse::accumulate;
//            dsp::add_multiplied             = sse::add_multiplied;
//            dsp::sub_multiplied             = sse::sub_multiplied;
//            dsp::integrate                  = sse::integrate;
//            dsp::mix                        = sse::mix;
//            dsp::mix_add                    = sse::mix_add;
//            dsp::convolve                   = sse::convolve;
//            dsp::reverse                    = sse::reverse;
//            dsp::direct_fft                 = sse::direct_fft;
//            dsp::reverse_fft                = sse::reverse_fft;
//            dsp::center_fft                 = sse::center_fft;
//            dsp::combine_fft                = sse::combine_fft;
//            dsp::normalize_fft              = sse::normalize_fft;
//            dsp::complex_mul                = sse::complex_mul;
//            dsp::complex_cvt2modarg         = sse::complex_cvt2modarg;
//            dsp::complex_cvt2reim           = sse::complex_cvt2reim;
//            dsp::complex_mod                = sse::complex_mod;
            #endif /* __AVX__ */
        }
    }
}

