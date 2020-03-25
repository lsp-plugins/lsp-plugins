/*
 * native.cpp
 *
 *  Created on: 04 марта 2016 г.
 *      Author: sadko
 */

#include <stdlib.h>
#include <dsp/dsp.h>
#include <dsp/bits.h>
#include <core/types.h>
#include <core/units.h>
#include <test/test.h>

#define __DSP_NATIVE_IMPL

#include <dsp/arch/native/context.h>
#include <dsp/arch/native/copy.h>
#include <dsp/arch/native/complex.h>
#include <dsp/arch/native/pcomplex.h>
#include <dsp/arch/native/convolution.h>

#include <dsp/arch/native/graphics.h>
#include <dsp/arch/native/graphics/effects.h>
#include <dsp/arch/native/graphics/interpolation.h>

#include <dsp/arch/native/pmath.h>
#include <dsp/arch/native/pmath/op_kx.h>
#include <dsp/arch/native/pmath/op_vv.h>
#include <dsp/arch/native/pmath/fmop_kx.h>
#include <dsp/arch/native/pmath/fmop_vv.h>
#include <dsp/arch/native/pmath/abs_vv.h>
#include <dsp/arch/native/pmath/exp.h>
#include <dsp/arch/native/pmath/log.h>
#include <dsp/arch/native/pmath/pow.h>

#include <dsp/arch/native/hmath/hsum.h>
#include <dsp/arch/native/hmath/hdotp.h>

#include <dsp/arch/native/search.h>

#include <dsp/arch/native/filters/static.h>
#include <dsp/arch/native/filters/dynamic.h>
#include <dsp/arch/native/filters/transform.h>
#include <dsp/arch/native/filters/transfer.h>

#include <dsp/arch/native/fft.h>
#include <dsp/arch/native/fastconv.h>
#include <dsp/arch/native/float.h>
#include <dsp/arch/native/resampling.h>
#include <dsp/arch/native/msmatrix.h>
#include <dsp/arch/native/smath.h>
#include <dsp/arch/native/mix.h>
#include <dsp/arch/native/3dmath.h>

#include <dsp/arch/native/coding.h>

#undef __DSP_NATIVE_IMPL

namespace native
{
    #define EXPORT1(function)            dsp::function = native::function; TEST_EXPORT(native::function);

    void dsp_init()
    {
        // Generic init
        EXPORT1(start);
        EXPORT1(finish);
        EXPORT1(info);

        EXPORT1(copy);
        EXPORT1(copy_saturated);
        EXPORT1(saturate);
        EXPORT1(limit_saturate1);
        EXPORT1(limit_saturate2);
        EXPORT1(limit1);
        EXPORT1(limit2);
        EXPORT1(sanitize1);
        EXPORT1(sanitize2);

        EXPORT1(move);
        EXPORT1(fill);
        EXPORT1(fill_one);
        EXPORT1(fill_zero);
        EXPORT1(fill_minus_one);

        EXPORT1(ipowf);
        EXPORT1(irootf);

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

        EXPORT1(abs_normalized);
        EXPORT1(normalize);

        EXPORT1(min);
        EXPORT1(max);
        EXPORT1(abs_max);
        EXPORT1(abs_min);
        EXPORT1(minmax);
        EXPORT1(abs_minmax);

        EXPORT1(min_index);
        EXPORT1(max_index);
        EXPORT1(minmax_index);
        EXPORT1(abs_max_index);
        EXPORT1(abs_min_index);
        EXPORT1(abs_minmax_index);

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

        EXPORT1(h_sum);
        EXPORT1(h_sqr_sum);
        EXPORT1(h_abs_sum);
        EXPORT1(h_dotp);
        EXPORT1(h_sqr_dotp);
        EXPORT1(h_abs_dotp);

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

        EXPORT1(mix2);
        EXPORT1(mix_copy2);
        EXPORT1(mix_add2);
        EXPORT1(mix3);
        EXPORT1(mix_copy3);
        EXPORT1(mix_add3);
        EXPORT1(mix4);
        EXPORT1(mix_copy4);
        EXPORT1(mix_add4);

        EXPORT1(reverse1);
        EXPORT1(reverse2);

        EXPORT1(direct_fft);
        EXPORT1(packed_direct_fft);
        EXPORT1(reverse_fft);
        EXPORT1(packed_reverse_fft);
        EXPORT1(normalize_fft3);
        EXPORT1(normalize_fft2);
        EXPORT1(center_fft);
        EXPORT1(combine_fft);
        EXPORT1(packed_combine_fft);

        EXPORT1(fastconv_parse);
        EXPORT1(fastconv_parse_apply);
        EXPORT1(fastconv_restore);
        EXPORT1(fastconv_apply);

        EXPORT1(complex_mul2);
        EXPORT1(complex_mul3);
        EXPORT1(complex_div2);
        EXPORT1(complex_rdiv2);
        EXPORT1(complex_div3);
        EXPORT1(complex_rcp1);
        EXPORT1(complex_rcp2);
        EXPORT1(complex_cvt2modarg);
        EXPORT1(complex_cvt2reim);
        EXPORT1(complex_mod);
        EXPORT1(complex_arg);

        EXPORT1(pcomplex_mul2);
        EXPORT1(pcomplex_mul3);
        EXPORT1(pcomplex_div2);
        EXPORT1(pcomplex_rdiv2);
        EXPORT1(pcomplex_div3);
        EXPORT1(pcomplex_rcp1);
        EXPORT1(pcomplex_rcp2);
        EXPORT1(pcomplex_fill_ri);
        EXPORT1(pcomplex_r2c);
        EXPORT1(pcomplex_c2r);
        EXPORT1(pcomplex_add_r);
        EXPORT1(pcomplex_mod);
        EXPORT1(pcomplex_arg);
        EXPORT1(pcomplex_modarg);

        EXPORT1(pcomplex_c2r_add2);
        EXPORT1(pcomplex_c2r_sub2);
        EXPORT1(pcomplex_c2r_rsub2);
        EXPORT1(pcomplex_c2r_mul2);
        EXPORT1(pcomplex_c2r_div2);
        EXPORT1(pcomplex_c2r_rdiv2);

        EXPORT1(lr_to_ms);
        EXPORT1(lr_to_mid);
        EXPORT1(lr_to_side);
        EXPORT1(ms_to_lr);
        EXPORT1(ms_to_left);
        EXPORT1(ms_to_right);

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

        EXPORT1(bilinear_transform_x1);
        EXPORT1(bilinear_transform_x2);
        EXPORT1(bilinear_transform_x4);
        EXPORT1(bilinear_transform_x8);

        EXPORT1(matched_transform_x1);
        EXPORT1(matched_transform_x2);
        EXPORT1(matched_transform_x4);
        EXPORT1(matched_transform_x8);

        EXPORT1(axis_apply_log1);
        EXPORT1(axis_apply_log2);
        EXPORT1(rgba32_to_bgra32);
        EXPORT1(abgr32_to_bgra32);
        EXPORT1(abgr32_to_bgrff32);
        EXPORT1(rgba32_to_bgra32_ra);
        EXPORT1(fill_rgba);
        EXPORT1(fill_hsla);
        EXPORT1(rgba_to_hsla);
        EXPORT1(hsla_to_rgba);
        EXPORT1(rgba_to_bgra32);

        EXPORT1(eff_hsla_hue);
        EXPORT1(eff_hsla_sat);
        EXPORT1(eff_hsla_light);
        EXPORT1(eff_hsla_alpha);

        EXPORT1(smooth_cubic_linear);
        EXPORT1(smooth_cubic_log);

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

        // 3D math
        EXPORT1(init_point_xyz);
        EXPORT1(init_point);
        EXPORT1(normalize_point);
        EXPORT1(scale_point1);
        EXPORT1(scale_point2);

        EXPORT1(init_vector_dxyz);
        EXPORT1(init_vector);
        EXPORT1(init_vector_p2);
        EXPORT1(init_vector_pv);
        EXPORT1(normalize_vector);
        EXPORT1(normalize_vector2);
        EXPORT1(flip_vector_v1);
        EXPORT1(flip_vector_v2);
        EXPORT1(scale_vector1);
        EXPORT1(scale_vector2);

        EXPORT1(add_vector_pv1);
        EXPORT1(add_vector_pv2);
        EXPORT1(add_vector_pvk1);
        EXPORT1(add_vector_pvk2);

        EXPORT1(vector_mul_v2);
        EXPORT1(vector_mul_vv);

        EXPORT1(init_normal3d_xyz);
        EXPORT1(init_normal3d_dxyz);
        EXPORT1(init_normal3d);

        EXPORT1(init_segment_xyz);
        EXPORT1(init_segment_p2);
        EXPORT1(init_segment_pv);

        EXPORT1(init_matrix3d);
        EXPORT1(init_matrix3d_zero);
        EXPORT1(init_matrix3d_one);
        EXPORT1(init_matrix3d_identity);
        EXPORT1(init_matrix3d_translate);
        EXPORT1(init_matrix3d_translate_p1);
        EXPORT1(init_matrix3d_translate_v1);
        EXPORT1(init_matrix3d_scale);
        EXPORT1(init_matrix3d_rotate_x);
        EXPORT1(init_matrix3d_rotate_y);
        EXPORT1(init_matrix3d_rotate_z);
        EXPORT1(init_matrix3d_rotate_xyz);
        EXPORT1(init_matrix3d_frustum);
        EXPORT1(init_matrix3d_lookat_p1v2);
        EXPORT1(init_matrix3d_lookat_p2v1);
        EXPORT1(init_matrix3d_orientation);
        EXPORT1(calc_matrix3d_transform_r1);
        EXPORT1(calc_matrix3d_transform_p1v1);
        EXPORT1(apply_matrix3d_mv2);
        EXPORT1(apply_matrix3d_mv1);
        EXPORT1(apply_matrix3d_mp2);
        EXPORT1(apply_matrix3d_mp1);
        EXPORT1(apply_matrix3d_mm2);
        EXPORT1(apply_matrix3d_mm1);
        EXPORT1(transpose_matrix3d1);
        EXPORT1(transpose_matrix3d2);

        EXPORT1(init_ray_xyz);
        EXPORT1(init_ray_dxyz);
        EXPORT1(init_ray_pdv);
        EXPORT1(init_ray_p2);
        EXPORT1(init_ray_pv);
        EXPORT1(init_ray);
        EXPORT1(calc_ray_xyz);
        EXPORT1(calc_ray_dxyz);
        EXPORT1(calc_ray_pdv);
        EXPORT1(calc_ray_p2);
        EXPORT1(calc_ray_pv);
        EXPORT1(calc_ray);

        EXPORT1(calc_triangle3d_params);
        EXPORT1(init_triangle3d_xyz);
        EXPORT1(init_triangle3d_p3);
        EXPORT1(init_triangle3d_pv);
        EXPORT1(init_triangle3d);
        EXPORT1(calc_triangle3d_xyz);
        EXPORT1(calc_triangle3d_p3);
        EXPORT1(calc_triangle3d_pv);
        EXPORT1(calc_triangle3d);

        EXPORT1(check_triplet3d_p3n);
        EXPORT1(check_triplet3d_pvn);
        EXPORT1(check_triplet3d_v2n);
        EXPORT1(check_triplet3d_vvn);
        EXPORT1(check_triplet3d_vv);
        EXPORT1(check_triplet3d_t);
        EXPORT1(check_triplet3d_tn);

        EXPORT1(check_point3d_on_triangle_p3p);
        EXPORT1(check_point3d_on_triangle_pvp);
        EXPORT1(check_point3d_on_triangle_tp);

        EXPORT1(longest_edge3d_p3);
        EXPORT1(longest_edge3d_pv);

        EXPORT1(calc_angle3d_v2);
        EXPORT1(calc_angle3d_vv);

        EXPORT1(calc_normal3d_p3);
        EXPORT1(calc_normal3d_pv);
        EXPORT1(calc_normal3d_v2);
        EXPORT1(calc_normal3d_vv);

        EXPORT1(move_point3d_p2);
        EXPORT1(move_point3d_pv);

        EXPORT1(calc_bound_box);

        EXPORT1(calc_plane_p3);
        EXPORT1(calc_plane_pv);
        EXPORT1(calc_plane_v1p2);
        EXPORT1(calc_oriented_plane_p3);
        EXPORT1(calc_oriented_plane_pv);
        EXPORT1(calc_rev_oriented_plane_p3);
        EXPORT1(calc_rev_oriented_plane_pv);
        EXPORT1(calc_parallel_plane_p2p2);
        EXPORT1(orient_plane_v1p1);

        EXPORT1(calc_area_p3);
        EXPORT1(calc_area_pv);
        EXPORT1(calc_min_distance_p3);
        EXPORT1(calc_min_distance_pv);
        EXPORT1(calc_avg_distance_p3);
        EXPORT1(calc_split_point_p2v1);
        EXPORT1(calc_split_point_pvv1);
        EXPORT1(calc_distance_p2);
        EXPORT1(calc_sqr_distance_p2);
        EXPORT1(calc_distance_pv);
        EXPORT1(calc_distance_v1);
        EXPORT1(calc_sqr_distance_pv);

        EXPORT1(projection_length_p2);
        EXPORT1(projection_length_v2);

        EXPORT1(split_triangle_raw);
        EXPORT1(cull_triangle_raw);
        EXPORT1(colocation_x2_v1p2);
        EXPORT1(colocation_x2_v1pv);
        EXPORT1(colocation_x3_v1p3);
        EXPORT1(colocation_x3_v1pv);
        EXPORT1(colocation_x3_v3p1);
        EXPORT1(colocation_x3_vvp1);

        EXPORT1(unit_vector_p1p3);
        EXPORT1(unit_vector_p1pv);

        EXPORT1(convolve);

        EXPORT1(base64_enc);
        EXPORT1(base64_dec);
    }

    #undef EXPORT1
} // namespace native

