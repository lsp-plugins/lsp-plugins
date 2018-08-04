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

#define __DSP_NATIVE_IMPL

#include <core/native/dsp.h>
#include <core/native/fft.h>
#include <core/native/search.h>
#include <core/native/fastconv.h>
#include <core/native/float.h>
#include <core/native/graphics.h>
#include <core/native/resampling.h>
#include <core/native/msmatrix.h>
#include <core/native/complex.h>
#include <core/native/3dmath.h>

namespace lsp
{

    namespace native
    {
        using namespace lsp;

        #include <core/native/smath.h>
        #include <core/native/filters/static.h>
        #include <core/native/filters/dynamic.h>
        #include <core/native/filters/transform.h>
    }
}

#undef __DSP_NATIVE_IMPL

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

            dsp::ipowf                      = native::ipowf;
            dsp::irootf                     = native::irootf;

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
            dsp::minmax_index               = native::minmax_index;
            dsp::abs_max_index              = native::abs_max_index;
            dsp::abs_min_index              = native::abs_min_index;
            dsp::abs_minmax_index           = native::abs_minmax_index;

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

            dsp::scale_add4                 = native::scale_add4;
            dsp::scale_sub4                 = native::scale_sub4;
            dsp::scale_mul4                 = native::scale_mul4;
            dsp::scale_div4                 = native::scale_div4;

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
            dsp::packed_combine_fft         = native::packed_combine_fft;

            dsp::fastconv_parse             = native::fastconv_parse;
            dsp::fastconv_parse_apply       = native::fastconv_parse_apply;
            dsp::fastconv_restore           = native::fastconv_restore;
            dsp::fastconv_apply             = native::fastconv_apply;

            dsp::complex_mul                = native::complex_mul;
            dsp::complex_rcp1               = native::complex_rcp1;
            dsp::complex_rcp2               = native::complex_rcp2;
            dsp::packed_complex_mul         = native::packed_complex_mul;
            dsp::packed_complex_rcp1        = native::packed_complex_rcp1;
            dsp::packed_complex_rcp2        = native::packed_complex_rcp2;
            dsp::packed_complex_fill        = native::packed_complex_fill;
            dsp::packed_real_to_complex     = native::packed_real_to_complex;
            dsp::packed_complex_to_real     = native::packed_complex_to_real;
            dsp::packed_complex_add_to_real = native::packed_complex_add_to_real;
            dsp::complex_cvt2modarg         = native::complex_cvt2modarg;
            dsp::complex_cvt2reim           = native::complex_cvt2reim;
            dsp::complex_mod                = native::complex_mod;
            dsp::packed_complex_mod         = native::packed_complex_mod;

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

            dsp::dyn_biquad_process_x1      = native::dyn_biquad_process_x1;
            dsp::dyn_biquad_process_x2      = native::dyn_biquad_process_x2;
            dsp::dyn_biquad_process_x4      = native::dyn_biquad_process_x4;
            dsp::dyn_biquad_process_x8      = native::dyn_biquad_process_x8;

            dsp::bilinear_transform_x1      = native::bilinear_transform_x1;
            dsp::bilinear_transform_x2      = native::bilinear_transform_x2;
            dsp::bilinear_transform_x4      = native::bilinear_transform_x4;
            dsp::bilinear_transform_x8      = native::bilinear_transform_x8;

            dsp::matched_transform_x1       = native::matched_transform_x1;
            dsp::matched_transform_x2       = native::matched_transform_x2;
            dsp::matched_transform_x4       = native::matched_transform_x4;
            dsp::matched_transform_x8       = native::matched_transform_x8;

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

            // 3D math
            dsp::init_point_xyz             = native::init_point_xyz;
            dsp::init_point                 = native::init_point;
            dsp::normalize_point            = native::normalize_point;
            dsp::scale_point1               = native::scale_point1;
            dsp::scale_point2               = native::scale_point2;

            dsp::init_vector_dxyz           = native::init_vector_dxyz;
            dsp::init_vector                = native::init_vector;
            dsp::init_vector_p2             = native::init_vector_p2;
            dsp::init_vector_pv             = native::init_vector_pv;
            dsp::normalize_vector           = native::normalize_vector;
            dsp::scale_vector1              = native::scale_vector1;
            dsp::scale_vector2              = native::scale_vector2;

            dsp::vector_mul_v2              = native::vector_mul_v2;
            dsp::vector_mul_vv              = native::vector_mul_vv;

            dsp::init_normal3d_xyz          = native::init_normal3d_xyz;
            dsp::init_normal3d_dxyz         = native::init_normal3d_dxyz;
            dsp::init_normal3d              = native::init_normal3d;

            dsp::init_segment_xyz           = native::init_segment_xyz;
            dsp::init_segment_p2            = native::init_segment_p2;
            dsp::init_segment_pv            = native::init_segment_pv;

            dsp::init_matrix3d              = native::init_matrix3d;
            dsp::init_matrix3d_zero         = native::init_matrix3d_zero;
            dsp::init_matrix3d_one          = native::init_matrix3d_one;
            dsp::init_matrix3d_identity     = native::init_matrix3d_identity;
            dsp::init_matrix3d_translate    = native::init_matrix3d_translate;
            dsp::init_matrix3d_scale        = native::init_matrix3d_scale;
            dsp::init_matrix3d_rotate_x     = native::init_matrix3d_rotate_x;
            dsp::init_matrix3d_rotate_y     = native::init_matrix3d_rotate_y;
            dsp::init_matrix3d_rotate_z     = native::init_matrix3d_rotate_z;
            dsp::init_matrix3d_rotate_xyz   = native::init_matrix3d_rotate_xyz;
            dsp::apply_matrix3d_mv2         = native::apply_matrix3d_mv2;
            dsp::apply_matrix3d_mv1         = native::apply_matrix3d_mv1;
            dsp::apply_matrix3d_mp2         = native::apply_matrix3d_mp2;
            dsp::apply_matrix3d_mp1         = native::apply_matrix3d_mp1;
            dsp::apply_matrix3d_mm2         = native::apply_matrix3d_mm2;
            dsp::apply_matrix3d_mm1         = native::apply_matrix3d_mm1;
            dsp::transpose_matrix3d1        = native::transpose_matrix3d1;
            dsp::transpose_matrix3d2        = native::transpose_matrix3d2;

            dsp::init_ray_xyz               = native::init_ray_xyz;
            dsp::init_ray_dxyz              = native::init_ray_dxyz;
            dsp::init_ray_pdv               = native::init_ray_pdv;
            dsp::init_ray_p2                = native::init_ray_p2;
            dsp::init_ray_pv                = native::init_ray_pv;
            dsp::init_ray                   = native::init_ray;
            dsp::calc_ray_xyz               = native::calc_ray_xyz;
            dsp::calc_ray_dxyz              = native::calc_ray_dxyz;
            dsp::calc_ray_pdv               = native::calc_ray_pdv;
            dsp::calc_ray_p2                = native::calc_ray_p2;
            dsp::calc_ray_pv                = native::calc_ray_pv;
            dsp::calc_ray                   = native::calc_ray;

            dsp::calc_triangle3d_params     = native::calc_triangle3d_params;
            dsp::init_triangle3d_xyz        = native::init_triangle3d_xyz;
            dsp::init_triangle3d_p3         = native::init_triangle3d_p3;
            dsp::init_triangle3d_pv         = native::init_triangle3d_pv;
            dsp::init_triangle3d            = native::init_triangle3d;
            dsp::calc_triangle3d_xyz        = native::calc_triangle3d_xyz;
            dsp::calc_triangle3d_p3         = native::calc_triangle3d_p3;
            dsp::calc_triangle3d_pv         = native::calc_triangle3d_pv;
            dsp::calc_triangle3d            = native::calc_triangle3d;

            dsp::init_intersection3d        = native::init_intersection3d;
            dsp::init_raytrace3d            = native::init_raytrace3d;
            dsp::init_raytrace3d_r          = native::init_raytrace3d_r;
            dsp::init_raytrace3d_ix         = native::init_raytrace3d_ix;

            dsp::check_triplet3d_p3n        = native::check_triplet3d_p3n;
            dsp::check_triplet3d_pvn        = native::check_triplet3d_pvn;
            dsp::check_triplet3d_v2n        = native::check_triplet3d_v2n;
            dsp::check_triplet3d_vvn        = native::check_triplet3d_vvn;
            dsp::check_triplet3d_vv         = native::check_triplet3d_vv;
            dsp::check_triplet3d_t          = native::check_triplet3d_t;
            dsp::check_triplet3d_tn         = native::check_triplet3d_tn;

//            dsp::check_point3d_location_tp  = native::check_point3d_location_tp;
//            dsp::check_point3d_location_pvp = native::check_point3d_location_pvp;
//            dsp::check_point3d_location_p3p = native::check_point3d_location_p3p;

            dsp::check_point3d_on_triangle_p3p  = native::check_point3d_on_triangle_p3p;
            dsp::check_point3d_on_triangle_pvp  = native::check_point3d_on_triangle_pvp;
            dsp::check_point3d_on_triangle_tp   = native::check_point3d_on_triangle_tp;

            dsp::check_point3d_on_edge_p2p  = native::check_point3d_on_edge_p2p;
            dsp::check_point3d_on_edge_pvp  = native::check_point3d_on_edge_pvp;

            dsp::longest_edge3d_p3          = native::longest_edge3d_p3;
            dsp::longest_edge3d_pv          = native::longest_edge3d_pv;
            dsp::find_intersection3d_rt     = native::find_intersection3d_rt;

            dsp::reflect_ray                = native::reflect_ray;

            dsp::calc_angle3d_v2            = native::calc_angle3d_v2;
            dsp::calc_angle3d_vv            = native::calc_angle3d_vv;

            dsp::calc_normal3d_p3           = native::calc_normal3d_p3;
            dsp::calc_normal3d_pv           = native::calc_normal3d_pv;
            dsp::calc_normal3d_v2           = native::calc_normal3d_v2;
            dsp::calc_normal3d_vv           = native::calc_normal3d_vv;

            dsp::move_point3d_p2            = native::move_point3d_p2;
            dsp::move_point3d_pv            = native::move_point3d_pv;

            dsp::init_octant3d_v            = native::init_octant3d_v;
            dsp::check_octant3d_rv          = native::check_octant3d_rv;

            dsp::calc_tetra3d_pv            = native::calc_tetra3d_pv;
            dsp::calc_tetra3d_pv3           = native::calc_tetra3d_pv3;
            dsp::calc_tetra3d_pvv           = native::calc_tetra3d_pvv;
        }
    } // namespace native
} // namespace dsp

