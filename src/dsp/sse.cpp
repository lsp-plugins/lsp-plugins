/*
 * sse.cpp
 *
 *  Created on: 04 марта 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <dsp/bits.h>
#include <core/types.h>
#include <core/debug.h>
#include <test/test.h>

#include <dsp/arch/x86/features.h>

#define DSP_ARCH_X86_SSE_IMPL

namespace sse // TODO: make constants common for all architectures
{
    #include <dsp/arch/x86/sse/const.h>

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

#include <dsp/arch/x86/sse/mxcsr.h>
#include <dsp/arch/x86/sse/copy.h>
#include <dsp/arch/x86/sse/pmath.h>
#include <dsp/arch/x86/sse/hsum.h>
#include <dsp/arch/x86/sse/mix.h>
#include <dsp/arch/x86/sse/abs.h>
#include <dsp/arch/x86/sse/smath.h>

#include <dsp/arch/x86/sse/fft.h>
#include <dsp/arch/x86/sse/fastconv.h>
#include <dsp/arch/x86/sse/graphics.h>
#include <dsp/arch/x86/sse/msmatrix.h>
#include <dsp/arch/x86/sse/search.h>
#include <dsp/arch/x86/sse/resampling.h>

#include <dsp/arch/x86/sse/complex.h>
#include <dsp/arch/x86/sse/pcomplex.h>

#include <dsp/arch/x86/sse/convolution.h>

#include <dsp/arch/x86/sse/filters/static.h>
#include <dsp/arch/x86/sse/filters/dynamic.h>
#include <dsp/arch/x86/sse/filters/transform.h>

#include <dsp/arch/x86/sse/3dmath.h>

#undef DSP_ARCH_X86_SSE_IMPL

namespace sse
{
    using namespace x86;

    static dsp::start_t     dsp_start       = NULL;
    static dsp::finish_t    dsp_finish      = NULL;

    void start(dsp::context_t *ctx)
    {
        dsp_start(ctx);
        uint32_t    mxcsr       = read_mxcsr();
        ctx->data[ctx->top++]   = mxcsr;
        write_mxcsr(mxcsr | MXCSR_ALL_MASK | MXCSR_FZ | MXCSR_DAZ);
    }

    void finish(dsp::context_t *ctx)
    {
        write_mxcsr(ctx->data[--ctx->top]);
        dsp_finish(ctx);
    }

    #define EXPORT2(function, export)           dsp::function = sse::export; TEST_EXPORT(sse::export);
    #define EXPORT1(function)                   EXPORT2(function, function);

    void dsp_init(const cpu_features_t *f)
    {
        if (((f->features) & (CPU_OPTION_SSE | CPU_OPTION_SSE2)) != (CPU_OPTION_SSE | CPU_OPTION_SSE2))
            return;

        lsp_trace("Optimizing DSP for SSE instruction set");

        // Initialize MXCSR mask
        if (f->features & CPU_OPTION_FXSAVE)
            init_mxcsr_mask();
        else
            mxcsr_mask  = MXCSR_DEFAULT;

        // Save previous entry points
        dsp_start                       = dsp::start;
        dsp_finish                      = dsp::finish;

        // Export routines
        EXPORT1(start);
        EXPORT1(finish);

        if (!feature_check(f, FEAT_FAST_MOVS))
        {
            EXPORT1(copy);
        }
        else
        {
            TEST_EXPORT(copy);
        }
        TEST_EXPORT(copy_movntps);

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
        EXPORT1(abs_mul2);
        EXPORT1(abs_div2);

        EXPORT1(abs_add3);
        EXPORT1(abs_sub3);
        EXPORT1(abs_mul3);
        EXPORT1(abs_div3);

        EXPORT1(min);
        EXPORT1(max);
        EXPORT1(abs_max);
        EXPORT1(abs_min);
        EXPORT1(minmax);
        EXPORT1(abs_minmax);

        EXPORT1(add2);
        EXPORT1(sub2);
        EXPORT1(mul2);
        EXPORT1(div2);
        EXPORT1(scale2);

        EXPORT1(add3);
        EXPORT1(sub3);
        EXPORT1(mul3);
        EXPORT1(div3);
        EXPORT1(scale3);

        EXPORT1(h_sum);
        EXPORT1(h_sqr_sum);
        EXPORT1(h_abs_sum);
//            EXPORT1(scalar_mul);

        EXPORT1(scale_add3);
        EXPORT1(scale_sub3);
        EXPORT1(scale_mul3);
        EXPORT1(scale_div3);

        EXPORT1(scale_add4);
        EXPORT1(scale_sub4);
        EXPORT1(scale_mul4);
        EXPORT1(scale_div4);

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
//            EXPORT1(center_fft);
//            EXPORT1(combine_fft);

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
        EXPORT1(complex_mod);

        EXPORT1(pcomplex_mul2);
        EXPORT1(pcomplex_mul3);
        EXPORT1(pcomplex_div2);
        EXPORT1(pcomplex_rdiv2);
        EXPORT1(pcomplex_div3);
        EXPORT1(pcomplex_rcp1);
        EXPORT1(pcomplex_rcp2);
        EXPORT1(pcomplex_r2c);
        EXPORT1(pcomplex_c2r);
        EXPORT1(pcomplex_add_r);
        EXPORT1(pcomplex_mod);
//            EXPORT1(complex_cvt2modarg);
//            EXPORT1(complex_cvt2reim);

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

        EXPORT1(bilinear_transform_x1);
        EXPORT1(bilinear_transform_x2);
        EXPORT1(bilinear_transform_x4);
        EXPORT1(bilinear_transform_x8);

        EXPORT1(axis_apply_log1);
        EXPORT1(axis_apply_log2);
        EXPORT1(rgba32_to_bgra32);
        EXPORT1(fill_rgba);
        EXPORT1(fill_hsla);

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

        // 3D Math
        EXPORT1(init_point_xyz);
        EXPORT1(init_point);
        EXPORT1(normalize_point);
        EXPORT1(scale_point1);
        EXPORT1(scale_point2);

        EXPORT1(init_vector_dxyz);
        EXPORT1(init_vector);
        EXPORT1(normalize_vector);
        EXPORT1(scale_vector1);
        EXPORT1(scale_vector2);

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

        EXPORT1(init_segment_xyz);
        EXPORT1(init_segment_p2);
        EXPORT1(init_segment_pv);

        EXPORT1(init_matrix3d);
        EXPORT1(init_matrix3d_zero);
        EXPORT1(init_matrix3d_one);
        EXPORT1(init_matrix3d_identity);
        EXPORT1(init_matrix3d_translate);
        EXPORT1(init_matrix3d_scale);
        EXPORT1(init_matrix3d_rotate_x);
        EXPORT1(init_matrix3d_rotate_y);
        EXPORT1(init_matrix3d_rotate_z);
        EXPORT1(init_matrix3d_rotate_xyz);
        EXPORT1(apply_matrix3d_mv2);
        EXPORT1(apply_matrix3d_mv1);
        EXPORT1(apply_matrix3d_mp2);
        EXPORT1(apply_matrix3d_mp1);
        EXPORT1(apply_matrix3d_mm2);
        EXPORT1(apply_matrix3d_mm1);
        EXPORT1(transpose_matrix3d1);
        EXPORT1(transpose_matrix3d2);

//            EXPORT1(check_point3d_location_tp);
//            EXPORT1(check_point3d_location_pvp);
//            EXPORT1(check_point3d_location_p3p);

        EXPORT1(check_point3d_on_triangle_p3p);
        EXPORT1(check_point3d_on_triangle_pvp);
        EXPORT1(check_point3d_on_triangle_tp);

        EXPORT1(check_point3d_on_edge_p2p);
        EXPORT1(check_point3d_on_edge_pvp);

        EXPORT1(longest_edge3d_p3);
        EXPORT1(longest_edge3d_pv);

        EXPORT1(check_triplet3d_p3n);
        EXPORT1(check_triplet3d_pvn);
        EXPORT1(check_triplet3d_v2n);
        EXPORT1(check_triplet3d_vvn);
        EXPORT1(check_triplet3d_vv);
        EXPORT1(check_triplet3d_t);
        EXPORT1(check_triplet3d_tn);

        EXPORT1(find_intersection3d_rt);
//            EXPORT1(reflect_ray);

        EXPORT1(calc_angle3d_v2);
        EXPORT1(calc_angle3d_vv);

        EXPORT1(calc_normal3d_p3);
        EXPORT1(calc_normal3d_pv);
        EXPORT1(calc_normal3d_v2);
        EXPORT1(calc_normal3d_vv);

        EXPORT1(move_point3d_p2);
        EXPORT1(move_point3d_pv);

        EXPORT1(check_octant3d_rv);

        EXPORT1(convolve);
    }

    #undef EXPORT1
    #undef EXPORT2
}
