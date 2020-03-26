/*
 * asimd.cpp
 *
 *  Created on: 11 апр. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/types.h>

#ifdef ARCH_AARCH64

#include <test/test.h>
#include <dsp/arch/aarch64/features.h>

#define DSP_ARCH_AARCH64_ASIMD_IMPL

namespace asimd // TODO: make constants common for all architectures
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

#include <dsp/arch/aarch64/asimd/copy.h>
#include <dsp/arch/aarch64/asimd/float.h>

#include <dsp/arch/aarch64/asimd/pmath/op_kx.h>
#include <dsp/arch/aarch64/asimd/pmath/op_vv.h>
#include <dsp/arch/aarch64/asimd/pmath/fmop_kx.h>
#include <dsp/arch/aarch64/asimd/pmath/fmop_vv.h>
#include <dsp/arch/aarch64/asimd/pmath/abs_vv.h>
#include <dsp/arch/aarch64/asimd/pmath/log.h>
#include <dsp/arch/aarch64/asimd/pmath/exp.h>
#include <dsp/arch/aarch64/asimd/pmath/pow.h>

#include <dsp/arch/aarch64/asimd/hmath/hsum.h>
#include <dsp/arch/aarch64/asimd/hmath/hdotp.h>

#include <dsp/arch/aarch64/asimd/mix.h>
#include <dsp/arch/aarch64/asimd/msmatrix.h>
#include <dsp/arch/aarch64/asimd/search/minmax.h>
#include <dsp/arch/aarch64/asimd/search/iminmax.h>
#include <dsp/arch/aarch64/asimd/resampling.h>
#include <dsp/arch/aarch64/asimd/convolution.h>

#include <dsp/arch/aarch64/asimd/complex.h>
#include <dsp/arch/aarch64/asimd/pcomplex.h>

#include <dsp/arch/aarch64/asimd/fft.h>
#include <dsp/arch/aarch64/asimd/pfft.h>
#include <dsp/arch/aarch64/asimd/fastconv.h>

#include <dsp/arch/aarch64/asimd/filters/static.h>
#include <dsp/arch/aarch64/asimd/filters/dynamic.h>
#include <dsp/arch/aarch64/asimd/filters/transfer.h>
#include <dsp/arch/aarch64/asimd/filters/transform.h>


#define EXPORT2(function, export)           dsp::function = asimd::export; TEST_EXPORT(asimd::export);
#define EXPORT1(function)                   EXPORT2(function, function)

#undef DSP_ARCH_AARCH64_ASIMD_IMPL

namespace asimd
{
    void dsp_init(const aarch64::cpu_features_t *f)
    {
        if ((f->hwcap & (HWCAP_AARCH64_ASIMD)) != (HWCAP_AARCH64_ASIMD))
            return;

        lsp_trace("Optimizing DSP for ASIMD instruction set");
        EXPORT1(copy);
        EXPORT1(move);
        EXPORT1(fill);
        EXPORT1(fill_zero);
        EXPORT1(fill_one);
        EXPORT1(fill_minus_one);
        EXPORT1(reverse1);
        EXPORT1(reverse2);

        EXPORT1(saturate);
        EXPORT1(copy_saturated);
        EXPORT1(limit_saturate1);
        EXPORT1(limit_saturate2);
        EXPORT1(limit1);
        EXPORT1(limit2);
        EXPORT1(sanitize1);
        EXPORT1(sanitize2);

        EXPORT1(add_k2);
        EXPORT1(sub_k2);
        EXPORT1(rsub_k2);
        EXPORT1(mul_k2);
        EXPORT1(div_k2);
        EXPORT1(rdiv_k2);
        EXPORT1(mod_k2);
        EXPORT1(rmod_k2);

        EXPORT1(add_k3);
        EXPORT1(sub_k3);
        EXPORT1(rsub_k3);
        EXPORT1(mul_k3);
        EXPORT1(div_k3);
        EXPORT1(rdiv_k3);
        EXPORT1(mod_k3);
        EXPORT1(rmod_k3);

        EXPORT1(add2);
        EXPORT1(sub2);
        EXPORT1(rsub2);
        EXPORT1(mul2);
        EXPORT1(div2);
        EXPORT1(rdiv2);
        EXPORT1(mod2);
        EXPORT1(rmod2);

        EXPORT1(add3);
        EXPORT1(sub3);
        EXPORT1(mul3);
        EXPORT1(div3);
        EXPORT1(mod3);

        EXPORT1(fmadd_k3);
        EXPORT1(fmsub_k3);
        EXPORT1(fmrsub_k3);
        EXPORT1(fmmul_k3);
        EXPORT1(fmdiv_k3);
        EXPORT1(fmrdiv_k3);
        EXPORT1(fmmod_k3);
        EXPORT1(fmrmod_k3);

        EXPORT1(fmadd_k4);
        EXPORT1(fmsub_k4);
        EXPORT1(fmrsub_k4);
        EXPORT1(fmmul_k4);
        EXPORT1(fmdiv_k4);
        EXPORT1(fmrdiv_k4);
        EXPORT1(fmmod_k4);
        EXPORT1(fmrmod_k4);

        EXPORT1(fmadd3);
        EXPORT1(fmsub3);
        EXPORT1(fmrsub3);
        EXPORT1(fmmul3);
        EXPORT1(fmdiv3);
        EXPORT1(fmrdiv3);
        EXPORT1(fmmod3);
        EXPORT1(fmrmod3);

        EXPORT1(fmadd4);
        EXPORT1(fmsub4);
        EXPORT1(fmrsub4);
        EXPORT1(fmmul4);
        EXPORT1(fmdiv4);
        EXPORT1(fmrdiv4);
        EXPORT1(fmmod4);
        EXPORT1(fmrmod4);

        EXPORT1(abs1);
        EXPORT1(abs2);

        EXPORT1(abs_add2);
        EXPORT1(abs_sub2);
        EXPORT1(abs_rsub2);
        EXPORT1(abs_mul2);
        EXPORT1(abs_div2);
        EXPORT1(abs_rdiv2);

        EXPORT1(abs_add3);
        EXPORT1(abs_sub3);
        EXPORT1(abs_rsub3);
        EXPORT1(abs_mul3);
        EXPORT1(abs_div3);
        EXPORT1(abs_rdiv3);

        EXPORT1(h_sum);
        EXPORT1(h_sqr_sum);
        EXPORT1(h_abs_sum);

        EXPORT1(h_dotp);
        EXPORT1(h_abs_dotp);
        EXPORT1(h_sqr_dotp);

        EXPORT1(logb1);
        EXPORT1(logb2);
        EXPORT1(loge1);
        EXPORT1(loge2);
        EXPORT1(logd1);
        EXPORT1(logd2);

        EXPORT1(exp1);
        EXPORT1(exp2);

        EXPORT1(powcv1);
        EXPORT1(powcv2);
        EXPORT1(powvc1);
        EXPORT1(powvc2);
        EXPORT1(powvx1);
        EXPORT1(powvx2);

        EXPORT1(mix2);
        EXPORT1(mix3);
        EXPORT1(mix4);
        EXPORT1(mix_copy2);
        EXPORT1(mix_copy3);
        EXPORT1(mix_copy4);
        EXPORT1(mix_add2);
        EXPORT1(mix_add3);
        EXPORT1(mix_add4);

        EXPORT1(lr_to_ms);
        EXPORT1(lr_to_mid);
        EXPORT1(lr_to_side);
        EXPORT1(ms_to_lr);
        EXPORT1(ms_to_left);
        EXPORT1(ms_to_right);

        EXPORT1(min)
        EXPORT1(max)
        EXPORT1(minmax)
        EXPORT1(abs_min)
        EXPORT1(abs_max)
        EXPORT1(abs_minmax)

        EXPORT1(min_index)
        EXPORT1(max_index)
        EXPORT1(minmax_index)
        EXPORT1(abs_min_index)
        EXPORT1(abs_max_index)
        EXPORT1(abs_minmax_index)

        EXPORT1(complex_mul2);
        EXPORT1(complex_mul3);
        EXPORT1(complex_div2);
        EXPORT1(complex_rdiv2);
        EXPORT1(complex_div3);
        EXPORT1(complex_mod);
        EXPORT1(complex_rcp1);
        EXPORT1(complex_rcp2);

        EXPORT1(pcomplex_mul2);
        EXPORT1(pcomplex_mul3);
        EXPORT1(pcomplex_div2);
        EXPORT1(pcomplex_rdiv2);
        EXPORT1(pcomplex_div3);
        EXPORT1(pcomplex_mod);
        EXPORT1(pcomplex_rcp1);
        EXPORT1(pcomplex_rcp2);
        EXPORT1(pcomplex_r2c);
        EXPORT1(pcomplex_c2r);
        EXPORT1(pcomplex_add_r);

        EXPORT1(direct_fft);
        EXPORT1(reverse_fft);
        EXPORT1(normalize_fft2);
        EXPORT1(normalize_fft3);

        EXPORT1(packed_direct_fft);
        EXPORT1(packed_reverse_fft);

        EXPORT1(fastconv_parse);
        EXPORT1(fastconv_restore);
        EXPORT1(fastconv_apply);
        EXPORT1(fastconv_parse_apply);

        EXPORT1(biquad_process_x1);
        EXPORT1(biquad_process_x2);
        EXPORT1(biquad_process_x4);
        EXPORT1(biquad_process_x8);

        EXPORT1(dyn_biquad_process_x1);
        EXPORT1(dyn_biquad_process_x2);
        EXPORT1(dyn_biquad_process_x4);
        EXPORT1(dyn_biquad_process_x8);

        EXPORT1(filter_transfer_calc_ri);
        EXPORT1(filter_transfer_apply_ri);
        EXPORT1(filter_transfer_calc_pc);
        EXPORT1(filter_transfer_apply_pc);

        EXPORT1(dyn_biquad_process_x1);
        EXPORT1(dyn_biquad_process_x2);
        EXPORT1(dyn_biquad_process_x4);
        EXPORT1(dyn_biquad_process_x8);

        EXPORT1(bilinear_transform_x1);
        EXPORT1(bilinear_transform_x2);
        EXPORT1(bilinear_transform_x4);
        EXPORT1(bilinear_transform_x8);

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

        EXPORT1(convolve);
    }
}

#endif /* ARCH_AARCH64 */

