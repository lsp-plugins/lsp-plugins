/*
 * native.cpp
 *
 *  Created on: 04 марта 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/dsp.h>
#include <core/bits.h>
#include <core/units.h>

#include <core/native/dsp.h>
#include <core/native/fft.h>
#include <core/native/fastconv.h>
#include <core/native/filters.h>
#include <core/native/float.h>
#include <core/native/graphics.h>
#include <core/native/resampling.h>
#include <core/native/msmatrix.h>
#include <core/native/complex.h>

namespace lsp
{
    namespace native
    {
        void dsp_init()
        {
            // Generic init
            dsp::start                      = native::start;
            dsp::finish                     = native::finish;

            dsp::copy                       = native::copy;
            dsp::copy_saturated             = native::copy_saturated;
            dsp::saturate                   = native::saturate;
            dsp::move                       = native::move;
            dsp::fill                       = native::fill;
            dsp::fill_one                   = native::fill_one;
            dsp::fill_zero                  = native::fill_zero;
            dsp::fill_minus_one             = native::fill_minus_one;

            dsp::abs1                       = native::abs1;
            dsp::abs2                       = native::abs2;
            dsp::abs_add2                   = native::abs_add2;
            dsp::abs_sub2                   = native::abs_sub2;
            dsp::abs_mul2                   = native::abs_mul2;
            dsp::abs_div2                   = native::abs_div2;

            dsp::abs_normalized             = native::abs_normalized;
            dsp::normalize                  = native::normalize;

            dsp::min                        = native::min;
            dsp::max                        = native::max;
            dsp::abs_max                    = native::abs_max;
            dsp::abs_min                    = native::abs_min;
            dsp::minmax                     = native::minmax;
            dsp::abs_minmax                 = native::abs_minmax;

            dsp::min_index                  = native::min_index;
            dsp::max_index                  = native::max_index;
            dsp::abs_max_index              = native::abs_max_index;
            dsp::abs_min_index              = native::abs_min_index;

            dsp::add2                       = native::add2;
            dsp::sub2                       = native::sub2;
            dsp::mul2                       = native::mul2;
            dsp::div2                       = native::div2;
            dsp::scale2                     = native::scale2;
            dsp::add3                       = native::add3;
            dsp::sub3                       = native::sub3;
            dsp::mul3                       = native::mul3;
            dsp::div3                       = native::div3;
            dsp::scale3                     = native::scale3;

            dsp::h_sum                      = native::h_sum;
            dsp::h_sqr_sum                  = native::h_sqr_sum;
            dsp::h_abs_sum                  = native::h_abs_sum;
            dsp::scalar_mul                 = native::scalar_mul;

            dsp::scale_add3                 = native::scale_add3;
            dsp::scale_sub3                 = native::scale_sub3;
            dsp::scale_mul3                 = native::scale_mul3;
            dsp::scale_div3                 = native::scale_div3;

            dsp::mix2                       = native::mix2;
            dsp::mix_copy2                  = native::mix_copy2;
            dsp::mix_add2                   = native::mix_add2;
            dsp::mix3                       = native::mix3;
            dsp::mix_copy3                  = native::mix_copy3;
            dsp::mix_add3                   = native::mix_add3;
            dsp::mix4                       = native::mix4;
            dsp::mix_copy4                  = native::mix_copy4;
            dsp::mix_add4                   = native::mix_add4;

            dsp::reverse1                   = native::reverse1;
            dsp::reverse2                   = native::reverse2;

            dsp::direct_fft                 = native::direct_fft;
            dsp::packed_direct_fft          = native::packed_direct_fft;
            dsp::conv_direct_fft            = native::conv_direct_fft;
            dsp::reverse_fft                = native::reverse_fft;
            dsp::packed_reverse_fft         = native::packed_reverse_fft;
            dsp::normalize_fft              = native::normalize_fft;
            dsp::center_fft                 = native::center_fft;
            dsp::combine_fft                = native::combine_fft;

            dsp::fastconv_parse             = native::fastconv_parse;
            dsp::fastconv_parse_apply       = native::fastconv_parse_apply;
            dsp::fastconv_restore           = native::fastconv_restore;
            dsp::fastconv_apply             = native::fastconv_apply;

            dsp::complex_mul                = native::complex_mul;
            dsp::packed_complex_mul         = native::packed_complex_mul;
            dsp::packed_real_to_complex     = native::packed_real_to_complex;
            dsp::packed_complex_to_real     = native::packed_complex_to_real;
            dsp::packed_complex_add_to_real = native::packed_complex_add_to_real;
            dsp::complex_cvt2modarg         = native::complex_cvt2modarg;
            dsp::complex_cvt2reim           = native::complex_cvt2reim;
            dsp::complex_mod                = native::complex_mod;

            dsp::lr_to_ms                   = native::lr_to_ms;
            dsp::lr_to_mid                  = native::lr_to_mid;
            dsp::lr_to_side                 = native::lr_to_side;
            dsp::ms_to_lr                   = native::ms_to_lr;
            dsp::ms_to_left                 = native::ms_to_left;
            dsp::ms_to_right                = native::ms_to_right;
            dsp::avoid_denormals            = native::avoid_denormals;

            dsp::biquad_process_x1          = native::biquad_process_x1;
            dsp::biquad_process_x2          = native::biquad_process_x2;
            dsp::biquad_process_x4          = native::biquad_process_x4;
            dsp::biquad_process_x8          = native::biquad_process_x8;

            dsp::axis_apply_log             = native::axis_apply_log;
            dsp::rgba32_to_bgra32           = native::rgba32_to_bgra32;

            dsp::lanczos_resample_2x2       = native::lanczos_resample_2x2;
            dsp::lanczos_resample_2x3       = native::lanczos_resample_2x3;
            dsp::lanczos_resample_3x2       = native::lanczos_resample_3x2;
            dsp::lanczos_resample_3x3       = native::lanczos_resample_3x3;
            dsp::lanczos_resample_4x2       = native::lanczos_resample_4x2;
            dsp::lanczos_resample_4x3       = native::lanczos_resample_4x3;
            dsp::lanczos_resample_6x2       = native::lanczos_resample_6x2;
            dsp::lanczos_resample_6x3       = native::lanczos_resample_6x3;
            dsp::lanczos_resample_8x2       = native::lanczos_resample_8x2;
            dsp::lanczos_resample_8x3       = native::lanczos_resample_8x3;

            dsp::downsample_2x              = native::downsample_2x;
            dsp::downsample_3x              = native::downsample_3x;
            dsp::downsample_4x              = native::downsample_4x;
            dsp::downsample_6x              = native::downsample_6x;
            dsp::downsample_8x              = native::downsample_8x;
        }
    } // namespace native
} // namespace dsp

