/*
 * dsp.cpp
 *
 *  Created on: 02 окт. 2015 г.
 *      Author: sadko
 */

#include <math.h>

#include <core/types.h>
#include <core/debug.h>
#include <dsp/dsp.h>

//-------------------------------------------------------------------------
// Native DSP initialization, always present
namespace native
{
    extern void dsp_init();
}

IF_ARCH_X86(
	namespace x86
	{
		extern void dsp_init();
	}
)

IF_ARCH_ARM(
	namespace arm
	{
		extern void dsp_init();
	}
)

IF_ARCH_AARCH64(
    namespace aarch64
    {
        extern void dsp_init();
    }
)

// Declare static variables
namespace dsp
{
    void    (* start)(dsp::context_t *ctx) = NULL;
    void    (* finish)(dsp::context_t *ctx) = NULL;
    info_t *(*info)() = NULL;

    void    (* copy)(float *dst, const float *src, size_t count) = NULL;
    void    (* copy_saturated)(float *dst, const float *src, size_t count) = NULL;
    void    (* saturate)(float *dst, size_t count) = NULL;
    void    (* limit_saturate1)(float *dst, size_t count) = NULL;
    void    (* limit_saturate2)(float *dst, const float *src, size_t count) = NULL;
    void    (* limit1)(float *dst, float min, float max, size_t count) = NULL;
    void    (* limit2)(float *dst, const float *src, float min, float max, size_t count) = NULL;
    void    (* sanitize1)(float *dst, size_t count) = NULL;
    void    (* sanitize2)(float *dst, const float *src, size_t count) = NULL;

    void    (* move)(float *dst, const float *src, size_t count) = NULL;
    void    (* fill)(float *dst, float value, size_t count) = NULL;
    void    (* fill_zero)(float *dst, size_t count) = NULL;
    void    (* fill_one)(float *dst, size_t count) = NULL;
    void    (* fill_minus_one)(float *dst, size_t count) = NULL;

    float   (* ipowf)(float x, int deg) = NULL;
    float   (* irootf)(float x, int deg) = NULL;

    void    (* abs1)(float *dst, size_t count) = NULL;
    void    (* abs2)(float *dst, const float *src, size_t count) = NULL;
    void    (* abs_add2)(float *dst, const float *src, size_t count) = NULL;
    void    (* abs_sub2)(float *dst, const float *src, size_t count) = NULL;
    void    (* abs_rsub2)(float *dst, const float *src, size_t count) = NULL;
    void    (* abs_mul2)(float *dst, const float *src, size_t count) = NULL;
    void    (* abs_div2)(float *dst, const float *src, size_t count) = NULL;
    void    (* abs_rdiv2)(float *dst, const float *src, size_t count) = NULL;
    void    (* abs_add3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;
    void    (* abs_sub3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;
    void    (* abs_rsub3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;
    void    (* abs_mul3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;
    void    (* abs_div3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;
    void    (* abs_rdiv3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;

    void    (* abs_normalized)(float *dst, const float *src, size_t count) = NULL;
    void    (* normalize)(float *dst, const float *src, size_t count) = NULL;
    float   (* min)(const float *src, size_t count) = NULL;
    float   (* max)(const float *src, size_t count) = NULL;
    float   (* abs_max)(const float *src, size_t count) = NULL;
    float   (* abs_min)(const float *src, size_t count) = NULL;
    size_t  (* abs_max_index)(const float *src, size_t count) = NULL;
    size_t  (* abs_min_index)(const float *src, size_t count) = NULL;
    void    (* minmax)(const float *src, size_t count, float *min, float *max) = NULL;
    void    (* abs_minmax)(const float *src, size_t count, float *min, float *max) = NULL;
    size_t  (* min_index)(const float *src, size_t count) = NULL;
    size_t  (* max_index)(const float *src, size_t count) = NULL;
    void    (* minmax_index)(const float *src, size_t count, size_t *min, size_t *max) = NULL;
    void    (* abs_minmax_index)(const float *src, size_t count, size_t *min, size_t *max) = NULL;

    void    (* add2)(float *dst, const float *src, size_t count) = NULL;
    void    (* sub2)(float *dst, const float *src, size_t count) = NULL;
    void    (* rsub2)(float *dst, const float *src, size_t count) = NULL;
    void    (* mul2)(float *dst, const float *src, size_t count) = NULL;
    void    (* div2)(float *dst, const float *src, size_t count) = NULL;
    void    (* rdiv2)(float *dst, const float *src, size_t count) = NULL;
    void    (* mod2)(float *dst, const float *src, size_t count) = NULL;
    void    (* rmod2)(float *dst, const float *src, size_t count) = NULL;

    void    (* add3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;
    void    (* sub3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;
    void    (* mul3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;
    void    (* div3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;
    void    (* mod3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;

    void    (* add_k2)(float *dst, float k, size_t count) = NULL;
    void    (* sub_k2)(float *dst, float k, size_t count) = NULL;
    void    (* rsub_k2)(float *dst, float k, size_t count) = NULL;
    void    (* mul_k2)(float *dst, float k, size_t count) = NULL;
    void    (* div_k2)(float *dst, float k, size_t count) = NULL;
    void    (* rdiv_k2)(float *dst, float k, size_t count) = NULL;
    void    (* mod_k2)(float *dst, float k, size_t count) = NULL;
    void    (* rmod_k2)(float *dst, float k, size_t count) = NULL;

    void    (* add_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* sub_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* rsub_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* mul_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* div_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* rdiv_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* mod_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* rmod_k3)(float *dst, const float *src, float k, size_t count) = NULL;

    void    (* exp1)(float *dst, size_t count) = NULL;
    void    (* exp2)(float *dst, const float *src, size_t count) = NULL;
    void    (* logb1)(float *dst, size_t count) = NULL;
    void    (* logb2)(float *dst, const float *src, size_t count) = NULL;
    void    (* loge1)(float *dst, size_t count) = NULL;
    void    (* loge2)(float *dst, const float *src, size_t count) = NULL;
    void    (* logd1)(float *dst, size_t count) = NULL;
    void    (* logd2)(float *dst, const float *src, size_t count) = NULL;

    void    (* powcv1)(float *v, float c, size_t count) = NULL;
    void    (* powcv2)(float *dst, const float *v, float c, size_t count) = NULL;
    void    (* powvc1)(float *c, float v, size_t count) = NULL;
    void    (* powvc2)(float *dst, const float *c, float v, size_t count) = NULL;
    void    (* powvx1)(float *v, const float *x, size_t count) = NULL;
    void    (* powvx2)(float *dst, const float *v, const float *x, size_t count) = NULL;

    float   (* h_sum)(const float *src, size_t count) = NULL;
    float   (* h_sqr_sum)(const float *src, size_t count) = NULL;
    float   (* h_abs_sum)(const float *src, size_t count) = NULL;
    float   (* h_dotp)(const float *a, const float *b, size_t count) = NULL;
    float   (* h_sqr_dotp)(const float *a, const float *b, size_t count) = NULL;
    float   (* h_abs_dotp)(const float *a, const float *b, size_t count) = NULL;

    void    (* fmadd_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* fmsub_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* fmrsub_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* fmmul_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* fmdiv_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* fmrdiv_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* fmmod_k3)(float *dst, const float *src, float k, size_t count) = NULL;
    void    (* fmrmod_k3)(float *dst, const float *src, float k, size_t count) = NULL;

    void    (* fmadd_k4)(float *dst, const float *src1, const float *src2, float k, size_t count) = NULL;
    void    (* fmsub_k4)(float *dst, const float *src1, const float *src2, float k, size_t count) = NULL;
    void    (* fmrsub_k4)(float *dst, const float *src1, const float *src2, float k, size_t count) = NULL;
    void    (* fmmul_k4)(float *dst, const float *src1, const float *src2, float k, size_t count) = NULL;
    void    (* fmdiv_k4)(float *dst, const float *src1, const float *src2, float k, size_t count) = NULL;
    void    (* fmrdiv_k4)(float *dst, const float *src1, const float *src2, float k, size_t count) = NULL;
    void    (* fmmod_k4)(float *dst, const float *src1, const float *src2, float k, size_t count) = NULL;
    void    (* fmrmod_k4)(float *dst, const float *src1, const float *src2, float k, size_t count) = NULL;

    void    (* fmadd3)(float *dst, const float *a, const float *b, size_t count) = NULL;
    void    (* fmsub3)(float *dst, const float *a, const float *b, size_t count) = NULL;
    void    (* fmrsub3)(float *dst, const float *a, const float *b, size_t count) = NULL;
    void    (* fmmul3)(float *dst, const float *a, const float *b, size_t count) = NULL;
    void    (* fmdiv3)(float *dst, const float *a, const float *b, size_t count) = NULL;
    void    (* fmrdiv3)(float *dst, const float *a, const float *b, size_t count) = NULL;
    void    (* fmmod3)(float *dst, const float *a, const float *b, size_t count) = NULL;
    void    (* fmrmod3)(float *dst, const float *a, const float *b, size_t count) = NULL;

    void    (* fmadd4)(float *dst, const float *a, const float *b, const float *c, size_t count) = NULL;
    void    (* fmsub4)(float *dst, const float *a, const float *b, const float *c, size_t count) = NULL;
    void    (* fmrsub4)(float *dst, const float *a, const float *b, const float *c, size_t count) = NULL;
    void    (* fmmul4)(float *dst, const float *a, const float *b, const float *c, size_t count) = NULL;
    void    (* fmdiv4)(float *dst, const float *a, const float *b, const float *c, size_t count) = NULL;
    void    (* fmrdiv4)(float *dst, const float *a, const float *b, const float *c, size_t count) = NULL;
    void    (* fmmod4)(float *dst, const float *a, const float *b, const float *c, size_t count) = NULL;
    void    (* fmrmod4)(float *dst, const float *a, const float *b, const float *c, size_t count) = NULL;

    void    (* mix2)(float *dst, const float *src, float k1, float k2, size_t count) = NULL;
    void    (* mix_copy2)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count) = NULL;
    void    (* mix_add2)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count) = NULL;
    void    (* mix3)(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count) = NULL;
    void    (* mix_copy3)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count) = NULL;
    void    (* mix_add3)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count) = NULL;
    void    (* mix4)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count) = NULL;
    void    (* mix_copy4)(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count) = NULL;
    void    (* mix_add4)(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count) = NULL;

    void    (* reverse1)(float *dst, size_t count) = NULL;
    void    (* reverse2)(float *dst, const float *src, size_t count) = NULL;

    void    (* direct_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank) = NULL;
    void    (* packed_direct_fft)(float *dst, const float *src, size_t rank) = NULL;
    void    (* reverse_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank) = NULL;
    void    (* packed_reverse_fft)(float *dst, const float *src, size_t rank) = NULL;
//        void    (* join_fft)(float *dst_re, float *dst_im, float *src_re, float *src_im, size_t rank) = NULL;
    void    (* normalize_fft3)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank) = NULL;
    void    (* normalize_fft2)(float *re, float *im, size_t rank) = NULL;
    void    (* center_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank) = NULL;
    void    (* combine_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank) = NULL;
    void    (* packed_combine_fft)(float *dst, const float *src, size_t rank) = NULL;

    // Complex-number operations
    void    (* complex_mul3)(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count) = NULL;
    void    (* complex_mul2)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count) = NULL;
    void    (* complex_div2)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count) = NULL;
    void    (* complex_rdiv2)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count) = NULL;
    void    (* complex_div3)(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count) = NULL;
    void    (* pcomplex_div2)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_rdiv2)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_div3)(float *dst, const float *t, const float *b, size_t count) = NULL;
    void    (* complex_rcp1)(float *dst_re, float *dst_im, size_t count) = NULL;
    void    (* complex_rcp2)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count) = NULL;
    void    (* pcomplex_mul3)(float *dst, const float *src1, const float *src2, size_t count) = NULL;
    void    (* pcomplex_mul2)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_rcp1)(float *dst, size_t count) = NULL;
    void    (* pcomplex_rcp2)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_fill_ri)(float *dst, float re, float im, size_t count) = NULL;
    void    (* pcomplex_r2c)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_c2r)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_add_r)(float *dst, const float *src, size_t count) = NULL;
    void    (* complex_cvt2modarg)(float *dst_mod, float *dst_arg, const float *src_re, const float *src_im, size_t count) = NULL;
    void    (* complex_cvt2reim)(float *dst_re, float *dst_im, const float *src_mod, const float *src_arg, size_t count) = NULL;
    void    (* complex_mod)(float *dst_mod, const float *src_re, const float *src_im, size_t count) = NULL;
    void    (* complex_arg)(float *dst, const float *re, const float *im, size_t count) = NULL;
    void    (* pcomplex_mod)(float *dst_mod, const float *src, size_t count) = NULL;
    void    (* pcomplex_arg)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_modarg)(float *mod, float *arg, const float *src, size_t count) = NULL;

    void    (* pcomplex_c2r_add2)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_c2r_sub2)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_c2r_rsub2)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_c2r_mul2)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_c2r_div2)(float *dst, const float *src, size_t count) = NULL;
    void    (* pcomplex_c2r_rdiv2)(float *dst, const float *src, size_t count) = NULL;

    void    (* fastconv_parse)(float *dst, const float *src, size_t rank) = NULL;
    void    (* fastconv_parse_apply)(float *dst, float *tmp, const float *c, const float *src, size_t rank) = NULL;
    void    (* fastconv_restore)(float *dst, float *tmp, size_t rank) = NULL;
    void    (* fastconv_apply)(float *dst, float *tmp, const float *c1, const float *c2, size_t rank) = NULL;

    void    (* lr_to_ms)(float *m, float *s, const float *l, const float *r, size_t count) = NULL;
    void    (* lr_to_mid)(float *m, const float *l, const float *r, size_t count) = NULL;
    void    (* lr_to_side)(float *s, const float *l, const float *r, size_t count) = NULL;
    void    (* ms_to_lr)(float *l, float *r, const float *m, const float *s, size_t count) = NULL;
    void    (* ms_to_left)(float *l, const float *m, const float *s, size_t count) = NULL;
    void    (* ms_to_right)(float *r, const float *m, const float *s, size_t count) = NULL;
    void    (* avoid_denormals)(float *dst, const float *src, size_t count) = NULL;

    void    (* biquad_process_x1)(float *dst, const float *src, size_t count, biquad_t *f) = NULL;
    void    (* biquad_process_x2)(float *dst, const float *src, size_t count, biquad_t *f) = NULL;
    void    (* biquad_process_x4)(float *dst, const float *src, size_t count, biquad_t *f) = NULL;
    void    (* biquad_process_x8)(float *dst, const float *src, size_t count, biquad_t *f) = NULL;

    void    (* dyn_biquad_process_x1)(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f) = NULL;
    void    (* dyn_biquad_process_x2)(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f) = NULL;
    void    (* dyn_biquad_process_x4)(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f) = NULL;
    void    (* dyn_biquad_process_x8)(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f) = NULL;

    void    (* filter_transfer_calc_ri)(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count) = NULL;
    void    (* filter_transfer_apply_ri)(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count) = NULL;
    void    (* filter_transfer_calc_pc)(float *dst, const f_cascade_t *c, const float *freq, size_t count) = NULL;
    void    (* filter_transfer_apply_pc)(float *dst, const f_cascade_t *c, const float *freq, size_t count) = NULL;

    void    (* bilinear_transform_x1)(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count) = NULL;
    void    (* bilinear_transform_x2)(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count) = NULL;
    void    (* bilinear_transform_x4)(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count) = NULL;
    void    (* bilinear_transform_x8)(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count) = NULL;

    void    (* matched_transform_x1)(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count) = NULL;
    void    (* matched_transform_x2)(biquad_x2_t *bf, f_cascade_t *bc, float kf, float td, size_t count) = NULL;
    void    (* matched_transform_x4)(biquad_x4_t *bf, f_cascade_t *bc, float kf, float td, size_t count) = NULL;
    void    (* matched_transform_x8)(biquad_x8_t *bf, f_cascade_t *bc, float kf, float td, size_t count) = NULL;

    void    (* axis_apply_log1)(float *x, const float *v, float zero, float norm_x, size_t count) = NULL;
    void    (* axis_apply_log2)(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count) = NULL;
    void    (* rgba32_to_bgra32)(void *dst, const void *src, size_t count) = NULL;
    void    (* abgr32_to_bgra32)(void *dst, const void *src, size_t count) = NULL;
    void    (* abgr32_to_bgrff32)(void *dst, const void *src, size_t count) = NULL;
    void    (* fill_rgba)(float *dst, float r, float g, float b, float a, size_t count) = NULL;
    void    (* fill_hsla)(float *dst, float h, float s, float l, float a, size_t count) = NULL;
    void    (* rgba_to_hsla)(float *dst, const float *src, size_t count) = NULL;
    void    (* hsla_to_rgba)(float *dst, const float *src, size_t count) = NULL;
    void    (* rgba_to_bgra32)(void *dst, const float *src, size_t count) = NULL;
    void    (* rgba32_to_bgra32_ra)(void *dst, const void *src, size_t count) = NULL;
    void    (* eff_hsla_hue)(float *dst, const float *v, const hsla_hue_eff_t *eff, size_t count) = NULL;
    void    (* eff_hsla_sat)(float *dst, const float *v, const hsla_sat_eff_t *eff, size_t count) = NULL;
    void    (* eff_hsla_light)(float *dst, const float *v, const hsla_light_eff_t *eff, size_t count) = NULL;
    void    (* eff_hsla_alpha)(float *dst, const float *v, const hsla_alpha_eff_t *eff, size_t count) = NULL;
    void    (* smooth_cubic_linear)(float *dst, float start, float stop, size_t count) = NULL;
    void    (* smooth_cubic_log)(float *dst, float start, float stop, size_t count) = NULL;

    void    (* lanczos_resample_2x2)(float *dst, const float *src, size_t count) = NULL;
    void    (* lanczos_resample_2x3)(float *dst, const float *src, size_t count) = NULL;
    void    (* lanczos_resample_3x2)(float *dst, const float *src, size_t count) = NULL;
    void    (* lanczos_resample_3x3)(float *dst, const float *src, size_t count) = NULL;
    void    (* lanczos_resample_4x2)(float *dst, const float *src, size_t count) = NULL;
    void    (* lanczos_resample_4x3)(float *dst, const float *src, size_t count) = NULL;
    void    (* lanczos_resample_6x2)(float *dst, const float *src, size_t count) = NULL;
    void    (* lanczos_resample_6x3)(float *dst, const float *src, size_t count) = NULL;
    void    (* lanczos_resample_8x2)(float *dst, const float *src, size_t count) = NULL;
    void    (* lanczos_resample_8x3)(float *dst, const float *src, size_t count) = NULL;

    void    (* downsample_2x)(float *dst, const float *src, size_t count) = NULL;
    void    (* downsample_3x)(float *dst, const float *src, size_t count) = NULL;
    void    (* downsample_4x)(float *dst, const float *src, size_t count) = NULL;
    void    (* downsample_6x)(float *dst, const float *src, size_t count) = NULL;
    void    (* downsample_8x)(float *dst, const float *src, size_t count) = NULL;

    // 3D mathematics
    void    (* init_point_xyz)(point3d_t *p, float x, float y, float z) = NULL;
    void    (* init_point)(point3d_t *p, const point3d_t *s) = NULL;
    void    (* normalize_point)(point3d_t *p) = NULL;
    void    (* scale_point1)(point3d_t *p, float r) = NULL;
    void    (* scale_point2)(point3d_t *p, const point3d_t *s, float r) = NULL;

    void    (* init_vector_dxyz)(vector3d_t *v, float dx, float dy, float dz) = NULL;
    void    (* init_vector)(vector3d_t *v, const vector3d_t *s) = NULL;
    void    (* init_vector_p2)(vector3d_t *v, const point3d_t *p1, const point3d_t *p2) = NULL;
    void    (* init_vector_pv)(vector3d_t *v, const point3d_t *pv) = NULL;
    void    (* normalize_vector)(vector3d_t *v) = NULL;
    void    (* normalize_vector2)(vector3d_t *v, const vector3d_t *src) = NULL;
    void    (* flip_vector_v1)(vector3d_t *v) = NULL;
    void    (* flip_vector_v2)(vector3d_t *v, const vector3d_t *sv) = NULL;
    void    (* scale_vector1)(vector3d_t *v, float r) = NULL;
    void    (* scale_vector2)(vector3d_t *v, const vector3d_t *s, float r) = NULL;

    void    (* init_normal3d)(vector3d_t *p, const vector3d_t *s) = NULL;
    void    (* init_normal3d_xyz)(vector3d_t *v, float x1, float y1, float z1, float x2, float y2, float z2) = NULL;
    void    (* init_normal3d_dxyz)(vector3d_t *v, float dx, float dy, float dz) = NULL;

    void    (* init_segment_xyz)(segment3d_t *s, float x0, float y0, float z0, float x1, float y1, float z1) = NULL;
    void    (* init_segment_p2)(segment3d_t *s, const point3d_t *p1, const point3d_t *p2) = NULL;
    void    (* init_segment_pv)(segment3d_t *s, const point3d_t *p) = NULL;

    void    (* init_matrix3d)(matrix3d_t *dst, const matrix3d_t *src) = NULL;
    void    (* init_matrix3d_zero)(matrix3d_t *m) = NULL;
    void    (* init_matrix3d_one)(matrix3d_t *m) = NULL;
    void    (* init_matrix3d_identity)(matrix3d_t *m) = NULL;
    void    (* init_matrix3d_translate)(matrix3d_t *m, float dx, float dy, float dz) = NULL;
    void    (* init_matrix3d_translate_p1)(matrix3d_t *m, const point3d_t *p) = NULL;
    void    (* init_matrix3d_translate_v1)(matrix3d_t *m, const vector3d_t *v) = NULL;
    void    (* init_matrix3d_scale)(matrix3d_t *m, float sx, float sy, float sz) = NULL;
    void    (* init_matrix3d_rotate_x)(matrix3d_t *m, float angle) = NULL;
    void    (* init_matrix3d_rotate_y)(matrix3d_t *m, float angle) = NULL;
    void    (* init_matrix3d_rotate_z)(matrix3d_t *m, float angle) = NULL;
    void    (* init_matrix3d_rotate_xyz)(matrix3d_t *m, float x, float y, float z, float angle) = NULL;
    void    (* init_matrix3d_frustum)(matrix3d_t *m, float left, float right, float bottom, float top, float near, float far) = NULL;
    void    (* init_matrix3d_lookat_p1v2)(matrix3d_t *m, const point3d_t *pov, const vector3d_t *fwd, const vector3d_t *up) = NULL;
    void    (* init_matrix3d_lookat_p2v1)(matrix3d_t *m, const point3d_t *pov, const point3d_t *pod, const vector3d_t *up) = NULL;
    void    (* init_matrix3d_orientation)(matrix3d_t *m, axis_orientation_t orientation) = NULL;
    void    (* calc_matrix3d_transform_p1v1)(matrix3d_t *m, const point3d_t *p, const vector3d_t *v) = NULL;
    void    (* calc_matrix3d_transform_r1)(matrix3d_t *m, const ray3d_t *r) = NULL;
    void    (* apply_matrix3d_mv2)(vector3d_t *r, const vector3d_t *v, const matrix3d_t *m) = NULL;
    void    (* apply_matrix3d_mv1)(vector3d_t *r, const matrix3d_t *m) = NULL;
    void    (* apply_matrix3d_mp2)(point3d_t *r, const point3d_t *p, const matrix3d_t *m) = NULL;
    void    (* apply_matrix3d_mp1)(point3d_t *r, const matrix3d_t *m) = NULL;
    void    (* apply_matrix3d_mm2)(matrix3d_t *r, const matrix3d_t *s, const matrix3d_t *m) = NULL;
    void    (* apply_matrix3d_mm1)(matrix3d_t *r, const matrix3d_t *m) = NULL;
    void    (* transpose_matrix3d1)(matrix3d_t *r) = NULL;
    void    (* transpose_matrix3d2)(matrix3d_t *r, const matrix3d_t *m) = NULL;

    void    (* init_ray_xyz)(ray3d_t *l, float x0, float y0, float z0, float x1, float y1, float z1) = NULL;
    void    (* init_ray_dxyz)(ray3d_t *l, float x0, float y0, float z0, float dx, float dy, float dz) = NULL;
    void    (* init_ray_pdv)(ray3d_t *l, const point3d_t *p, const vector3d_t *v) = NULL;
    void    (* init_ray_p2)(ray3d_t *l, const point3d_t *p1, const point3d_t *p2) = NULL;
    void    (* init_ray_pv)(ray3d_t *l, const point3d_t *p) = NULL;
    void    (* init_ray)(ray3d_t *l, const ray3d_t *r) = NULL;
    void    (* calc_ray_xyz)(ray3d_t *l, float x0, float y0, float z0, float x1, float y1, float z1) = NULL;
    void    (* calc_ray_dxyz)(ray3d_t *l, float x0, float y0, float z0, float dx, float dy, float dz) = NULL;
    void    (* calc_ray_pdv)(ray3d_t *l, const point3d_t *p, const vector3d_t *v) = NULL;
    void    (* calc_ray_p2)(ray3d_t *l, const point3d_t *p1, const point3d_t *p2) = NULL;
    void    (* calc_ray_pv)(ray3d_t *l, const point3d_t *p) = NULL;
    void    (* calc_ray)(ray3d_t *l, const ray3d_t *r) = NULL;

    void    (* calc_triangle3d_params)(triangle3d_t *t) = NULL;
    void    (* init_triangle3d_xyz)(triangle3d_t *t, float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2) = NULL;
    void    (* init_triangle3d_p3)(triangle3d_t *t, const point3d_t *p1, const point3d_t *p2, const point3d_t *p3) = NULL;
    void    (* init_triangle3d_pv)(triangle3d_t *t, const point3d_t *p) = NULL;
    void    (* init_triangle3d)(triangle3d_t *dst, const triangle3d_t *src) = NULL;
    void    (* calc_triangle3d_xyz)(triangle3d_t *t, float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2) = NULL;
    void    (* calc_triangle3d_p3)(triangle3d_t *t, const point3d_t *p1, const point3d_t *p2, const point3d_t *p3) = NULL;
    void    (* calc_triangle3d_pv)(triangle3d_t *t, const point3d_t *p) = NULL;
    void    (* calc_triangle3d)(triangle3d_t *dst, const triangle3d_t *src) = NULL;

    float   (* check_triplet3d_p3n)(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const vector3d_t *n) = NULL;
    float   (* check_triplet3d_pvn)(const point3d_t *pv, const vector3d_t *n) = NULL;
    float   (* check_triplet3d_v2n)(const vector3d_t *v1, const vector3d_t *v2, const vector3d_t *n) = NULL;
    float   (* check_triplet3d_vvn)(const vector3d_t *v, const vector3d_t *n) = NULL;
    float   (* check_triplet3d_vv)(const vector3d_t *v) = NULL;
    float   (* check_triplet3d_t)(const triangle3d_t *t) = NULL;
    float   (* check_triplet3d_tn)(const triangle3d_t *t, const vector3d_t *n) = NULL;

    float   (* check_point3d_on_triangle_p3p)(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const point3d_t *p) = NULL;
    float   (* check_point3d_on_triangle_pvp)(const point3d_t *pv, const point3d_t *p) = NULL;
    float   (* check_point3d_on_triangle_tp)(const triangle3d_t *t, const point3d_t *p) = NULL;

    size_t  (* longest_edge3d_p3)(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3) = NULL;
    size_t  (* longest_edge3d_pv)(const point3d_t *p) = NULL;

    float   (* calc_angle3d_v2)(const vector3d_t *v1, const vector3d_t *v2) = NULL;
    float   (* calc_angle3d_vv)(const vector3d_t *v) = NULL;

    void    (* calc_normal3d_p3)(vector3d_t *n, const point3d_t *p1, const point3d_t *p2, const point3d_t *p3) = NULL;
    void    (* calc_normal3d_pv)(vector3d_t *n, const point3d_t *pv) = NULL;
    void    (* calc_normal3d_v2)(vector3d_t *n, const vector3d_t *v1, const vector3d_t *v2) = NULL;
    void    (* calc_normal3d_vv)(vector3d_t *n, const vector3d_t *vv) = NULL;

    void    (* move_point3d_p2)(point3d_t *p, const point3d_t *p1, const point3d_t *p2, float k) = NULL;
    void    (* move_point3d_pv)(point3d_t *p, const point3d_t *pv, float k) = NULL;
    void    (* add_vector_pv1)(point3d_t *p, const vector3d_t *dv) = NULL;
    void    (* add_vector_pv2)(point3d_t *p, const point3d_t *sp, const vector3d_t *dv) = NULL;
    void    (* add_vector_pvk1)(point3d_t *p, const vector3d_t *dv, float k);
    void    (* add_vector_pvk2)(point3d_t *p, const point3d_t *sp, const vector3d_t *dv, float k);

    void    (* calc_bound_box)(bound_box3d_t *b, const point3d_t *p, size_t n) = NULL;

    float   (* calc_plane_p3)(vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2) = NULL;
    float   (* calc_plane_pv)(vector3d_t *v, const point3d_t *pv) = NULL;
    float   (* calc_plane_v1p2)(vector3d_t *v, const vector3d_t *v0, const point3d_t *p0, const point3d_t *p1) = NULL;
    float   (* orient_plane_v1p1)(vector3d_t *v, const point3d_t *sp, const vector3d_t *pl) = NULL;

    float   (* calc_oriented_plane_p3)(vector3d_t *v, const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2) = NULL;
    float   (* calc_oriented_plane_pv)(vector3d_t *v, const point3d_t *sp, const point3d_t *pv) = NULL;
    float   (* calc_rev_oriented_plane_p3)(vector3d_t *v, const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2) = NULL;
    float   (* calc_rev_oriented_plane_pv)(vector3d_t *v, const point3d_t *sp, const point3d_t *pv) = NULL;
    float   (* calc_parallel_plane_p2p2)(vector3d_t *v, const point3d_t *sp, const point3d_t *pp, const point3d_t *p0, const point3d_t *p1) = NULL;

    float   (* calc_area_p3)(const point3d_t *p0, const point3d_t *p1, const point3d_t *p2) = NULL;
    float   (* calc_area_pv)(const point3d_t *pv) = NULL;
    float   (* calc_min_distance_p3)(const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2) = NULL;
    float   (* calc_min_distance_pv)(const point3d_t *sp, const point3d_t *pv) = NULL;
    float   (* calc_avg_distance_p3)(const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2) = NULL;
    float   (* calc_distance_p2)(const point3d_t *p1, const point3d_t *p2) = NULL;
    float   (* calc_sqr_distance_p2)(const point3d_t *p1, const point3d_t *p2) = NULL;
    float   (* calc_distance_pv)(const point3d_t *pv) = NULL;
    float   (* calc_distance_v1)(const vector3d_t *v) = NULL;
    float   (* calc_sqr_distance_pv)(const point3d_t *pv) = NULL;
    void    (* calc_split_point_p2v1)(point3d_t *ip, const point3d_t *l0, const point3d_t *l1, const vector3d_t *pl) = NULL;
    void    (* calc_split_point_pvv1)(point3d_t *ip, const point3d_t *lv, const vector3d_t *pl) = NULL;

    float   (* projection_length_p2)(const point3d_t *p0, const point3d_t *p1, const point3d_t *pp) = NULL;
    float   (* projection_length_v2)(const vector3d_t *v, const vector3d_t *pv) = NULL;

    void    (* split_triangle_raw)(raw_triangle_t *out, size_t *n_out, raw_triangle_t *in, size_t *n_in, const vector3d_t *pl, const raw_triangle_t *pv) = NULL;
    void    (* cull_triangle_raw)(raw_triangle_t *in, size_t *n_in, const vector3d_t *pl, const raw_triangle_t *pv) = NULL;
    size_t  (* colocation_x2_v1p2)(const vector3d_t *v, const point3d_t *p0, const point3d_t *p1);
    size_t  (* colocation_x2_v1pv)(const vector3d_t *v, const point3d_t *pv);
    size_t  (* colocation_x3_v1p3)(const vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2) = NULL;
    size_t  (* colocation_x3_v1pv)(const vector3d_t *v, const point3d_t *pv) = NULL;
    size_t  (* colocation_x3_v3p1)(const vector3d_t *v0, const vector3d_t *v1, const vector3d_t *v2, const point3d_t *p) = NULL;
    size_t  (* colocation_x3_vvp1)(const vector3d_t *vv, const point3d_t *p) = NULL;
    void    (* unit_vector_p1p3)(vector3d_t *v, const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2) = NULL;
    void    (* unit_vector_p1pv)(vector3d_t *v, const point3d_t *sp, const point3d_t *pv) = NULL;

    void    (* vector_mul_v2)(vector3d_t *r, const vector3d_t *v1, const vector3d_t *v2) = NULL;
    void    (* vector_mul_vv)(vector3d_t *r, const vector3d_t *vv) = NULL;

    void    (* convolve)(float *dst, const float *src, const float *conv, size_t length, size_t count) = NULL;

    size_t  (* base64_enc)(void *dst, size_t *dst_left, const void *src, size_t *src_left) = NULL;
    ssize_t (* base64_dec)(void *dst, size_t *dst_left, const void *src, size_t *src_left) = NULL;
}

namespace dsp
{
    void init()
    {
        // Consider already initialized
        if (copy != NULL)
            return;

        // Information message
        lsp_trace("Initializing DSP");

        // Initialize native functions
        native::dsp_init();

        // Initialize architecture-dependent functions that utilize architecture-specific features
        IF_ARCH_X86(x86::dsp_init());
        IF_ARCH_ARM(arm::dsp_init());
        IF_ARCH_AARCH64(aarch64::dsp_init());
    }
}
