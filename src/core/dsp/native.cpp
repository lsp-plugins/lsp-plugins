/*
 * native.cpp
 *
 *  Created on: 04 марта 2016 г.
 *      Author: sadko
 */

#include <core/dsp.h>
#include <core/bits.h>

#include <core/native/dsp.h>

namespace lsp
{
    namespace native
    {
        void dsp_init()
        {
            dsp::copy                       = native::copy;
            dsp::move                       = native::move;
            dsp::fill                       = native::fill;
            dsp::fill_one                   = native::fill_one;
            dsp::fill_zero                  = native::fill_zero;
            dsp::fill_minus_one             = native::fill_minus_one;
            dsp::abs                        = native::abs;
            dsp::abs_normalized             = native::abs_normalized;
            dsp::normalize                  = native::normalize;
            dsp::min                        = native::min;
            dsp::max                        = native::max;
            dsp::abs_max                    = native::abs_max;
            dsp::minmax                     = native::minmax;
            dsp::min_index                  = native::min_index;
            dsp::max_index                  = native::max_index;
            dsp::scale                      = native::scale;
            dsp::multiply                   = native::multiply;
            dsp::h_sum                      = native::h_sum;
            dsp::h_sqr_sum                  = native::h_sqr_sum;
            dsp::scalar_mul                 = native::scalar_mul;
            dsp::accumulate                 = native::accumulate;
            dsp::add_multiplied             = native::add_multiplied;
            dsp::sub_multiplied             = native::sub_multiplied;
            dsp::integrate                  = native::integrate;
            dsp::mix                        = native::mix;
            dsp::mix_add                    = native::mix_add;
            dsp::convolve                   = native::convolve;
            dsp::reverse                    = native::reverse;
            dsp::direct_fft                 = native::direct_fft;
            dsp::reverse_fft                = native::reverse_fft;
            dsp::normalize_fft              = native::normalize_fft;
            dsp::center_fft                 = native::center_fft;
            dsp::combine_fft                = native::combine_fft;
            dsp::complex_mul                = native::complex_mul;
            dsp::complex_cvt2modarg         = native::complex_cvt2modarg;
            dsp::complex_cvt2reim           = native::complex_cvt2reim;
            dsp::complex_mod                = native::complex_mod;
        }
    } // namespace native
} // namespace dsp

