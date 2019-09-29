/*
 * neon-d32.cpp
 *
 *  Created on: 2 сент. 2018 г.
 *      Author: sadko
 */


#include <dsp/dsp.h>
#include <dsp/bits.h>
#include <core/types.h>

#ifdef ARCH_ARM
#include <test/test.h>
#include <dsp/arch/arm/features.h>

#define DSP_ARCH_ARM_NEON_32_IMPL

namespace neon_d32 // TODO: make constants common for all architectures
{
    //-------------------------------------------------------------------------
    // Constants definition
    #define DSP_F32VEC4(name, v)        static const float name[] __lsp_aligned16          = { v, v, v, v }
    #define DSP_U32VEC4(name, v)        static const uint32_t name[] __lsp_aligned16       = { uint32_t(v), uint32_t(v), uint32_t(v), uint32_t(v) }

    #define DSP_F32VECX4(name, a, b, c, d)  static const float name[] __lsp_aligned16      = { a, b, c, d }
    #define DSP_U32VECX4(name, a, b, c, d)  static const uint32_t name[] __lsp_aligned16   = { uint32_t(a), uint32_t(b), uint32_t(c), uint32_t(d) }

    #define DSP_F32REP4(v)              v, v, v, v
    #define DSP_U32REP4(v)              uint32_t(v), uint32_t(v), uint32_t(v), uint32_t(v)

    #define DSP_F32ARRAY(name, ...)     static const float name[] __lsp_aligned16          = { __VA_ARGS__ }
    #define DSP_U32ARRAY(name, ...)     static const uint32_t name[] __lsp_aligned16       = { __VA_ARGS__ }

    #include <dsp/common/const/const16.h>

    #undef DSP_F32ARRAY_IMPL
    #undef DSP_F32ARRAY

    #undef DSP_U32REP4
    #undef DSP_F32REP4

    #undef DSP_U32VECX4
    #undef DSP_F32VECX4

    #undef DSP_U32VEC4
    #undef DSP_F32VEC4
}

#include <dsp/arch/arm/neon-d32/copy.h>
#include <dsp/arch/arm/neon-d32/complex.h>
#include <dsp/arch/arm/neon-d32/pcomplex.h>
#include <dsp/arch/arm/neon-d32/convolution.h>

#include <dsp/arch/arm/neon-d32/graphics.h>
#include <dsp/arch/arm/neon-d32/graphics/effects.h>

#include <dsp/arch/arm/neon-d32/pmath.h>
#include <dsp/arch/arm/neon-d32/hmath.h>
#include <dsp/arch/arm/neon-d32/float.h>
#include <dsp/arch/arm/neon-d32/msmatrix.h>
#include <dsp/arch/arm/neon-d32/resampling.h>
#include <dsp/arch/arm/neon-d32/search.h>
#include <dsp/arch/arm/neon-d32/filters/static.h>
#include <dsp/arch/arm/neon-d32/filters/dynamic.h>
#include <dsp/arch/arm/neon-d32/filters/transform.h>
#include <dsp/arch/arm/neon-d32/fft.h>
#include <dsp/arch/arm/neon-d32/fastconv.h>
#include <dsp/arch/arm/neon-d32/mix.h>

#include <dsp/arch/arm/neon-d32/pmath/exp.h>
#include <dsp/arch/arm/neon-d32/pmath/log.h>
#include <dsp/arch/arm/neon-d32/pmath/pow.h>

#undef DSP_ARCH_ARM_NEON_32_IMPL

#define EXPORT2(function, export)           dsp::function = neon_d32::export; TEST_EXPORT(neon_d32::export);
#define EXPORT1(function)                   EXPORT2(function, function)

namespace neon_d32
{
    void dsp_init(const arm::cpu_features_t *f)
    {
        if ((f->hwcap & (HWCAP_ARM_NEON | HWCAP_ARM_VFPD32)) != (HWCAP_ARM_NEON | HWCAP_ARM_VFPD32))
            return;

        lsp_trace("Optimizing DSP for NEON-D32 instruction set");
        EXPORT1(copy);
        EXPORT1(move);
        EXPORT1(fill);
        EXPORT1(fill_zero);
        EXPORT1(fill_one);
        EXPORT1(fill_minus_one);

        EXPORT1(complex_mul2);
        EXPORT1(complex_mul3);
        EXPORT1(complex_div2);
        EXPORT1(complex_rdiv2);
        EXPORT1(complex_div3);
        EXPORT1(complex_mod);
        EXPORT1(complex_rcp1);
        EXPORT1(complex_rcp2);

        EXPORT1(pcomplex_r2c);
        EXPORT1(pcomplex_c2r);
        EXPORT1(pcomplex_add_r);
        EXPORT1(pcomplex_mul2);
        EXPORT1(pcomplex_mul3);
        EXPORT1(pcomplex_div2);
        EXPORT1(pcomplex_rdiv2);
        EXPORT1(pcomplex_div3);
        EXPORT1(pcomplex_mod);
        EXPORT1(pcomplex_rcp1);
        EXPORT1(pcomplex_rcp2);

        EXPORT1(convolve);

        EXPORT1(axis_apply_log1);
        EXPORT1(axis_apply_log2);
        EXPORT1(rgba32_to_bgra32);
        EXPORT1(fill_rgba);
        EXPORT1(fill_hsla);
        EXPORT1(limit_saturate1);
        EXPORT1(limit_saturate2);
        EXPORT1(limit1);
        EXPORT1(limit2);
        EXPORT1(hsla_to_rgba);
        EXPORT1(rgba_to_hsla);
        EXPORT1(rgba_to_bgra32);

        EXPORT1(eff_hsla_hue);
        EXPORT1(eff_hsla_sat);
        EXPORT1(eff_hsla_light);
        EXPORT1(eff_hsla_alpha);

        EXPORT1(abs1);
        EXPORT1(abs2);

        EXPORT1(scale2);
        EXPORT1(scale3);

        EXPORT1(add2);
        EXPORT1(sub2);
        EXPORT1(mul2);
        EXPORT1(div2);

        EXPORT1(abs_add2);
        EXPORT1(abs_sub2);
        EXPORT1(abs_mul2);
        EXPORT1(abs_div2);

        EXPORT1(scale_add3);
        EXPORT1(scale_sub3);
        EXPORT1(scale_mul3);
        EXPORT1(scale_div3);

        EXPORT1(add3);
        EXPORT1(sub3);
        EXPORT1(mul3);
        EXPORT1(div3);

        EXPORT1(abs_add3);
        EXPORT1(abs_sub3);
        EXPORT1(abs_mul3);
        EXPORT1(abs_div3);

        EXPORT1(scale_add4);
        EXPORT1(scale_sub4);
        EXPORT1(scale_mul4);
        EXPORT1(scale_div4);

        EXPORT1(exp1);
        EXPORT1(exp2);
        EXPORT1(logb1);
        EXPORT1(logb2);
        EXPORT1(loge1);
        EXPORT1(loge2);
        EXPORT1(logd1);
        EXPORT1(logd2);
        EXPORT1(powcv1);
        EXPORT1(powcv2);
        EXPORT1(powvc1);
        EXPORT1(powvc2);
        EXPORT1(powvx1);
        EXPORT1(powvx2);

        EXPORT1(h_sum);
        EXPORT1(h_abs_sum);
        EXPORT1(h_sqr_sum);

        EXPORT1(saturate);
        EXPORT1(copy_saturated);

        EXPORT1(lr_to_ms);
        EXPORT1(lr_to_mid);
        EXPORT1(lr_to_side);
        EXPORT1(ms_to_lr);
        EXPORT1(ms_to_left);
        EXPORT1(ms_to_right);

        EXPORT1(lanczos_resample_2x2);
        EXPORT1(lanczos_resample_2x3);
        EXPORT1(lanczos_resample_3x2);
        EXPORT1(lanczos_resample_3x3);
        EXPORT1(lanczos_resample_4x2);
        EXPORT1(lanczos_resample_4x3);
        EXPORT1(lanczos_resample_6x2);
        EXPORT1(lanczos_resample_6x3);
        EXPORT1(lanczos_resample_8x2);
        EXPORT1(lanczos_resample_8x3);

        EXPORT1(downsample_2x);
        EXPORT1(downsample_3x);
        EXPORT1(downsample_4x);
        EXPORT1(downsample_6x);
        EXPORT1(downsample_8x);

        EXPORT1(min);
        EXPORT1(max);
        EXPORT1(minmax);
        EXPORT1(abs_min);
        EXPORT1(abs_max);
        EXPORT1(abs_minmax);

        EXPORT1(min_index);
        EXPORT1(max_index);
        EXPORT1(minmax_index);
        EXPORT1(abs_min_index);
        EXPORT1(abs_max_index);
        EXPORT1(abs_minmax_index);

        EXPORT1(biquad_process_x1);
        EXPORT1(biquad_process_x2);
        EXPORT1(biquad_process_x4);
        EXPORT1(biquad_process_x8);

        EXPORT1(dyn_biquad_process_x1);
        EXPORT1(dyn_biquad_process_x2);
        EXPORT1(dyn_biquad_process_x4);
        EXPORT1(dyn_biquad_process_x8);

        EXPORT1(bilinear_transform_x1);
        EXPORT1(bilinear_transform_x2);
        EXPORT1(bilinear_transform_x4);
        EXPORT1(bilinear_transform_x8);

        EXPORT1(direct_fft);
        EXPORT1(reverse_fft);
        EXPORT1(packed_direct_fft);
        EXPORT1(packed_reverse_fft);

        EXPORT1(fastconv_parse);
        EXPORT1(fastconv_restore);
        EXPORT1(fastconv_apply);
        EXPORT1(fastconv_parse_apply);

        EXPORT1(mix2);
        EXPORT1(mix3);
        EXPORT1(mix4);
        EXPORT1(mix_copy2);
        EXPORT1(mix_copy3);
        EXPORT1(mix_copy4);
        EXPORT1(mix_add2);
        EXPORT1(mix_add3);
        EXPORT1(mix_add4);
    }
}

#endif /* ARCH_ARM */
