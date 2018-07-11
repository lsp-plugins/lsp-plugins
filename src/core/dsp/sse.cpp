/*
 * sse.cpp
 *
 *  Created on: 04 марта 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>
#include <core/dsp.h>
#include <core/bits.h>

#include <core/x86/dsp.h>
#include <core/x86/sse/complex.h>
#include <core/x86/sse/addsub.h>
#include <core/x86/sse/fft.h>
#include <core/x86/sse/fastconv.h>
#include <core/x86/sse/filters.h>
#include <core/x86/sse/float.h>
#include <core/x86/sse/graphics.h>
#include <core/x86/sse/msmatrix.h>
#include <core/x86/sse/search.h>
#include <core/x86/sse/resampling.h>
#include <core/x86/sse/native.h>

namespace lsp
{
    namespace sse
    {
        static dsp::start_t     dsp_start       = NULL;
        static dsp::finish_t    dsp_finish      = NULL;

        static void start(dsp_context_t *ctx)
        {
            dsp_start(ctx);
            uint32_t    mxcsr       = read_mxcsr();
            ctx->data[ctx->top++]   = mxcsr;
            write_mxcsr(mxcsr | MXCSR_FZ | MXCSR_DAZ);
        }

        static void finish(dsp_context_t *ctx)
        {
            write_mxcsr(ctx->data[--ctx->top]);
            dsp_finish(ctx);
        }

        void dsp_init(dsp_options_t options)
        {
            if ((options & (DSP_OPTION_SSE | DSP_OPTION_SSE2)) != (DSP_OPTION_SSE | DSP_OPTION_SSE2))
                return;

            lsp_trace("Optimizing DSP for SSE instruction set");

            // Initialize MXCSR mask
            init_mxcsr_mask();

            // Save previous entry points
            dsp_start                       = dsp::start;
            dsp_finish                      = dsp::finish;
            dsp::start                      = sse::start;
            dsp::finish                     = sse::finish;

            dsp::copy                       = sse::copy;
            dsp::copy_saturated             = sse::copy_saturated;
            dsp::saturate                   = sse::saturate;
            dsp::move                       = sse::move;
            dsp::fill                       = sse::fill;
            dsp::fill_one                   = sse::fill_one;
            dsp::fill_zero                  = sse::fill_zero;
            dsp::fill_minus_one             = sse::fill_minus_one;
    //        dsp::abs                        = sse::abs;
    //        dsp::abs_normalized             = sse::abs_normalized;
            dsp::normalize                  = sse::normalize;
            dsp::min                        = sse::min;
            dsp::max                        = sse::max;
            dsp::abs_max                    = sse::abs_max;
            dsp::abs_min                    = sse::abs_min;
            dsp::minmax                     = sse::minmax;
    //        dsp::min_index                  = sse::min_index;
    //        dsp::max_index                  = sse::max_index;
            dsp::scale                      = sse::scale;
            dsp::multiply                   = sse::multiply;
            dsp::h_sum                      = sse::h_sum;
            dsp::h_sqr_sum                  = sse::h_sqr_sum;
            dsp::h_abs_sum                  = sse::h_abs_sum;
    //        dsp::scalar_mul                 = sse::scalar_mul;
    //        dsp::accumulate                 = sse::accumulate;
            dsp::add2                       = sse::add2;
            dsp::sub2                       = sse::sub2;
            dsp::add3                       = sse::add3;
            dsp::sub3                       = sse::sub3;
            dsp::add_multiplied             = sse::add_multiplied;
            dsp::sub_multiplied             = sse::sub_multiplied;
            dsp::integrate                  = sse::integrate;
            dsp::mix                        = sse::mix;
            dsp::mix_add                    = sse::mix_add;
            dsp::convolve_single            = sse::convolve_single;
            dsp::convolve                   = sse::convolve;
            dsp::reverse                    = sse::reverse;

            dsp::direct_fft                 = sse::direct_fft;
            dsp::packed_direct_fft          = sse::packed_direct_fft;
            dsp::conv_direct_fft            = sse::conv_direct_fft;
            dsp::reverse_fft                = sse::reverse_fft;
            dsp::packed_reverse_fft         = sse::packed_reverse_fft;
//            dsp::center_fft                 = sse::center_fft;
//            dsp::combine_fft                = sse::combine_fft;

            dsp::fastconv_parse             = sse::fastconv_parse;
            dsp::fastconv_parse_apply       = sse::fastconv_parse_apply;
            dsp::fastconv_restore           = sse::fastconv_restore;
            dsp::fastconv_apply             = sse::fastconv_apply;

            dsp::normalize_fft              = sse::normalize_fft;
            dsp::complex_mul                = sse::complex_mul;
            dsp::packed_complex_mul         = sse::packed_complex_mul;
            dsp::packed_real_to_complex     = sse::packed_real_to_complex;
            dsp::packed_complex_to_real     = sse::packed_complex_to_real;
            dsp::packed_complex_add_to_real = sse::packed_complex_add_to_real;
//            dsp::complex_cvt2modarg         = sse::complex_cvt2modarg;
//            dsp::complex_cvt2reim           = sse::complex_cvt2reim;
            dsp::complex_mod                = sse::complex_mod;
            dsp::lr_to_ms                   = sse::lr_to_ms;
            dsp::lr_to_mid                  = sse::lr_to_mid;
            dsp::lr_to_side                 = sse::lr_to_side;
            dsp::ms_to_lr                   = sse::ms_to_lr;
            dsp::ms_to_left                 = sse::ms_to_left;
            dsp::ms_to_right                = sse::ms_to_right;
            dsp::biquad_process             = sse::biquad_process;
            dsp::biquad_process_multi       = sse::biquad_process_multi;
            dsp::biquad_process_x1          = sse::biquad_process_x1;
            dsp::biquad_process_x2          = sse::biquad_process_x2;
            dsp::biquad_process_x4          = sse::biquad_process_x4;
            dsp::biquad_process_x8          = sse::biquad_process_x8;

            dsp::axis_apply_log             = sse::axis_apply_log;
            dsp::rgba32_to_bgra32           = sse::rgba32_to_bgra32;

            dsp::lanczos_resample_2x2       = sse::lanczos_resample_2x2;
            dsp::lanczos_resample_2x3       = sse::lanczos_resample_2x3;
            dsp::lanczos_resample_3x2       = sse::lanczos_resample_3x2;
            dsp::lanczos_resample_3x3       = sse::lanczos_resample_3x3;
            dsp::lanczos_resample_4x2       = sse::lanczos_resample_4x2;
            dsp::lanczos_resample_4x3       = sse::lanczos_resample_4x3;
            dsp::lanczos_resample_6x2       = sse::lanczos_resample_6x2;
            dsp::lanczos_resample_6x3       = sse::lanczos_resample_6x3;
            dsp::lanczos_resample_8x2       = sse::lanczos_resample_8x2;
            dsp::lanczos_resample_8x3       = sse::lanczos_resample_8x3;

            dsp::downsample_2x              = sse::downsample_2x;
            dsp::downsample_3x              = sse::downsample_3x;
            dsp::downsample_4x              = sse::downsample_4x;
            dsp::downsample_6x              = sse::downsample_6x;
            dsp::downsample_8x              = sse::downsample_8x;
        }
    }

}
