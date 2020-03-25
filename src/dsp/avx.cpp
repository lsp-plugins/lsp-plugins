/*
 * avx.cpp
 *
 *  Created on: 09 марта 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <dsp/bits.h>
#include <test/test.h>

#include <core/types.h>
#include <core/debug.h>

#include <dsp/arch/x86/features.h>

#define DSP_ARCH_X86_AVX_IMPL

#include <dsp/arch/x86/avx/xcr.h>
#include <dsp/arch/x86/avx/const.h>

#include <dsp/arch/x86/avx/copy.h>
#include <dsp/arch/x86/avx/float.h>
#include <dsp/arch/x86/avx/complex.h>
#include <dsp/arch/x86/avx/pcomplex.h>

#include <dsp/arch/x86/avx/pmath/op_kx.h>
#include <dsp/arch/x86/avx/pmath/op_vv.h>
#include <dsp/arch/x86/avx/pmath/fmop_kx.h>
#include <dsp/arch/x86/avx/pmath/fmop_vv.h>
#include <dsp/arch/x86/avx/pmath/abs_vv.h>

#include <dsp/arch/x86/avx/hmath/hsum.h>
#include <dsp/arch/x86/avx/hmath/hdotp.h>

#include <dsp/arch/x86/avx/mix.h>
#include <dsp/arch/x86/avx/search/minmax.h>

#include <dsp/arch/x86/avx/fft.h>
#include <dsp/arch/x86/avx/pfft.h>
#include <dsp/arch/x86/avx/fastconv.h>

#include <dsp/arch/x86/avx/filters/static.h>
#include <dsp/arch/x86/avx/filters/dynamic.h>
#include <dsp/arch/x86/avx/filters/transform.h>
#include <dsp/arch/x86/avx/filters/transfer.h>

#include <dsp/arch/x86/avx/msmatrix.h>
#include <dsp/arch/x86/avx/resampling.h>
#include <dsp/arch/x86/avx/convolution.h>

#undef DSP_ARCH_X86_AVX_IMPL

namespace avx
{
    using namespace x86;

    #define EXPORT2(function, export)               { dsp::function = avx::export; TEST_EXPORT(avx::export); }
    #define EXPORT1(function)                       EXPORT2(function, function)

    #define EXPORT2_X64(function, export)           IF_ARCH_X86_64(EXPORT2(function, export));
    #define SUPPORT_X64(function)                   IF_ARCH_X86_64(TEST_EXPORT(avx::function))

    #define CEXPORT2(cond, function, export)    \
    IF_ARCH_X86( \
            TEST_EXPORT(avx::export); \
            if (cond) \
                dsp::function = avx::export; \
        );

    #define CEXPORT1(cond, export)    \
    IF_ARCH_X86( \
            TEST_EXPORT(avx::export); \
            if (cond) \
                dsp::export = avx::export; \
        );

    #define CEXPORT2_X64(cond, function, export)    \
        IF_ARCH_X86_64( \
                TEST_EXPORT(avx::export); \
                if (cond) \
                    dsp::function = avx::export; \
            );

    #define CEXPORT1_X64(cond, export)    \
        IF_ARCH_X86_64( \
                TEST_EXPORT(avx::export); \
                if (cond) \
                    dsp::export = avx::export; \
            );

    void dsp_init(const cpu_features_t *f)
    {
        if (!(f->features & CPU_OPTION_AVX))
            return;

        lsp_trace("Optimizing DSP for AVX instruction set");

        TEST_EXPORT(avx::copy);

        // This routine sucks on AMD Bulldozer processor family but is pretty great on Intel
        // Not tested on AMD Processors above Bulldozer family
        bool favx   = feature_check(f, FEAT_FAST_AVX);
        bool ffma   = favx && feature_check(f, FEAT_FAST_FMA3);

        CEXPORT2_X64(favx, reverse1, reverse1);
        CEXPORT2_X64(favx, reverse2, reverse2);

        CEXPORT1(favx, limit1);
        CEXPORT1(favx, limit2);
        CEXPORT1(favx, sanitize1);
        CEXPORT1(favx, sanitize2);

        // Conditional export, depending on fast AVX implementation
        CEXPORT1(favx, add_k2);
        CEXPORT1(favx, sub_k2);
        CEXPORT1(favx, rsub_k2);
        CEXPORT1(favx, mul_k2);
        CEXPORT1(favx, div_k2);
        CEXPORT1(favx, rdiv_k2);
        CEXPORT1(favx, mod_k2);
        CEXPORT1(favx, rmod_k2);

        CEXPORT1(favx, add_k3);
        CEXPORT1(favx, sub_k3);
        CEXPORT1(favx, rsub_k3);
        CEXPORT1(favx, mul_k3);
        CEXPORT1(favx, div_k3);
        CEXPORT1(favx, rdiv_k3);
        CEXPORT1(favx, mod_k3);
        CEXPORT1(favx, rmod_k3);

        CEXPORT1(favx, add2);
        CEXPORT1(favx, sub2);
        CEXPORT1(favx, rsub2);
        CEXPORT1(favx, mul2);
        CEXPORT1(favx, div2);
        CEXPORT1(favx, rdiv2);
        CEXPORT1(favx, mod2);
        CEXPORT1(favx, rmod2);

        CEXPORT1(favx, add3);
        CEXPORT1(favx, sub3);
        CEXPORT1(favx, mul3);
        CEXPORT1(favx, div3);
        CEXPORT1(favx, mod3);

        CEXPORT1(favx, fmadd_k3);
        CEXPORT1(favx, fmsub_k3);
        CEXPORT1(favx, fmrsub_k3);
        CEXPORT1(favx, fmmul_k3);
        CEXPORT1(favx, fmdiv_k3);
        CEXPORT1(favx, fmrdiv_k3);
        CEXPORT1(favx, fmmod_k3);
        CEXPORT1(favx, fmrmod_k3);

        CEXPORT1(favx, fmadd_k4);
        CEXPORT1(favx, fmsub_k4);
        CEXPORT1(favx, fmrsub_k4);
        CEXPORT1(favx, fmmul_k4);
        CEXPORT1(favx, fmdiv_k4);
        CEXPORT1(favx, fmrdiv_k4);
        CEXPORT1(favx, fmmod_k4);
        CEXPORT1(favx, fmrmod_k4);

        CEXPORT1(favx, fmadd3);
        CEXPORT1(favx, fmsub3);
        CEXPORT1(favx, fmrsub3);
        CEXPORT1(favx, fmmul3);
        CEXPORT1(favx, fmdiv3);
        CEXPORT1(favx, fmrdiv3);
        CEXPORT1(favx, fmmod3);
        CEXPORT1(favx, fmrmod3);

        CEXPORT1(favx, fmadd4);
        CEXPORT1(favx, fmsub4);
        CEXPORT1(favx, fmrsub4);
        CEXPORT1(favx, fmmul4);
        CEXPORT1(favx, fmdiv4);
        CEXPORT1(favx, fmrdiv4);
        CEXPORT1(favx, fmmod4);
        CEXPORT1(favx, fmrmod4);

        CEXPORT2_X64(favx, abs_add2, x64_abs_add2);
        CEXPORT2_X64(favx, abs_sub2, x64_abs_sub2);
        CEXPORT2_X64(favx, abs_rsub2, x64_abs_rsub2);
        CEXPORT2_X64(favx, abs_mul2, x64_abs_mul2);
        CEXPORT2_X64(favx, abs_div2, x64_abs_div2);
        CEXPORT2_X64(favx, abs_rdiv2, x64_abs_rdiv2);

        CEXPORT2_X64(favx, abs_add3, x64_abs_add3);
        CEXPORT2_X64(favx, abs_sub3, x64_abs_sub3);
        CEXPORT2_X64(favx, abs_rsub3, x64_abs_rsub3);
        CEXPORT2_X64(favx, abs_mul3, x64_abs_mul3);
        CEXPORT2_X64(favx, abs_div3, x64_abs_div3);
        CEXPORT2_X64(favx, abs_rdiv3, x64_abs_rdiv3);

        CEXPORT2_X64(favx, abs1, x64_abs1);
        CEXPORT2_X64(favx, abs2, x64_abs2);

        CEXPORT1(favx, complex_mul2);
        CEXPORT1(favx, complex_mul3);
        CEXPORT1(favx, complex_div2);
        CEXPORT1(favx, complex_rdiv2);
        CEXPORT1(favx, complex_div3);
        CEXPORT1(favx, complex_mod);
        CEXPORT1(favx, complex_rcp1);
        CEXPORT1(favx, complex_rcp2);

        CEXPORT1(favx, pcomplex_mul2);
        CEXPORT1(favx, pcomplex_mul3);
        CEXPORT1(favx, pcomplex_div2);
        CEXPORT1(favx, pcomplex_rdiv2);
        CEXPORT1(favx, pcomplex_div3);
        CEXPORT1(favx, pcomplex_mod);
        CEXPORT1(favx, pcomplex_rcp1);
        CEXPORT1(favx, pcomplex_rcp2);

        CEXPORT1(favx, biquad_process_x1);
        CEXPORT1(favx, biquad_process_x2);
        CEXPORT1(favx, biquad_process_x4);
        EXPORT2_X64(biquad_process_x8, x64_biquad_process_x8);

        CEXPORT1(favx, dyn_biquad_process_x1);
        CEXPORT1(favx, dyn_biquad_process_x2);
        CEXPORT1(favx, dyn_biquad_process_x4);
        EXPORT2_X64(dyn_biquad_process_x8, x64_dyn_biquad_process_x8);

        CEXPORT1(favx, bilinear_transform_x1);
        CEXPORT1(favx, bilinear_transform_x2);
        CEXPORT1(favx, bilinear_transform_x4);
        CEXPORT2_X64(favx, bilinear_transform_x8, x64_bilinear_transform_x8);

        CEXPORT1(favx, h_sum);
        CEXPORT1(favx, h_sqr_sum);
        CEXPORT1(favx, h_abs_sum);

        CEXPORT1(favx, h_dotp);
        CEXPORT1(favx, h_sqr_dotp);
        CEXPORT1(favx, h_abs_dotp);

        CEXPORT1(favx, mix2);
        CEXPORT1(favx, mix_copy2);
        CEXPORT1(favx, mix_add2);
        CEXPORT1(favx, mix3);
        CEXPORT1(favx, mix_copy3);
        CEXPORT1(favx, mix_add3);
        CEXPORT1(favx, mix4);
        CEXPORT1(favx, mix_copy4);
        CEXPORT1(favx, mix_add4);

        CEXPORT1(favx, min);
        CEXPORT1(favx, max);
        CEXPORT1(favx, minmax);
        CEXPORT1(favx, abs_min);
        CEXPORT1(favx, abs_max);
        CEXPORT1(favx, abs_minmax);

        CEXPORT1(favx, lr_to_ms);
        CEXPORT1(favx, lr_to_mid);
        CEXPORT1(favx, lr_to_side);
        CEXPORT1(favx, ms_to_lr);
        CEXPORT1(favx, ms_to_left);
        CEXPORT1(favx, ms_to_right);

        CEXPORT1(favx, direct_fft);
        CEXPORT1(favx, reverse_fft);
        CEXPORT1(favx, normalize_fft2);
        CEXPORT1(favx, normalize_fft3);

        CEXPORT1(favx, packed_direct_fft);
        CEXPORT1(favx, packed_reverse_fft);

        CEXPORT1(favx, fastconv_parse);
        CEXPORT1(favx, fastconv_restore);
        CEXPORT1(favx, fastconv_apply);
        CEXPORT1(favx, fastconv_parse_apply);

        CEXPORT1(favx, filter_transfer_calc_ri);
        CEXPORT1(favx, filter_transfer_apply_ri);
        CEXPORT1(favx, filter_transfer_calc_pc);
        CEXPORT1(favx, filter_transfer_apply_pc);

        CEXPORT1(favx, lanczos_resample_2x2);
        CEXPORT1(favx, lanczos_resample_2x3);
        CEXPORT1(favx, lanczos_resample_3x2);
        CEXPORT1(favx, lanczos_resample_3x3);
        CEXPORT1(favx, lanczos_resample_4x2);
        CEXPORT1(favx, lanczos_resample_4x3);
        CEXPORT1(favx, lanczos_resample_6x2);
        CEXPORT1(favx, lanczos_resample_6x3);
        CEXPORT1(favx, lanczos_resample_8x2);
        CEXPORT1(favx, lanczos_resample_8x3);

        CEXPORT1(favx, downsample_2x);
        CEXPORT1(favx, downsample_3x);
        CEXPORT1(favx, downsample_4x);
        CEXPORT1(favx, downsample_6x);
        CEXPORT1(favx, downsample_8x);

        CEXPORT1(favx, convolve);

        // FMA3 support?
        if (f->features & CPU_OPTION_FMA3)
        {
            lsp_trace("Optimizing DSP for FMA3 instruction set");

            // Conditional export, depending on fast AVX implementation
            CEXPORT2(favx, mod2, mod2_fma3);
            CEXPORT2(favx, rmod2, rmod2_fma3);

            CEXPORT2(favx, mod3, mod3_fma3);

            CEXPORT2(favx, mod_k2, mod_k2_fma3);
            CEXPORT2(favx, rmod_k2, rmod_k2_fma3);

            CEXPORT2(favx, mod_k3, mod_k3_fma3);
            CEXPORT2(favx, rmod_k3, rmod_k3_fma3);

            CEXPORT2(favx, fmadd_k3, fmadd_k3_fma3);
            CEXPORT2(favx, fmsub_k3, fmsub_k3_fma3);
            CEXPORT2(favx, fmrsub_k3, fmrsub_k3_fma3);
            CEXPORT2(favx, fmmod_k3, fmmod_k3_fma3);
            CEXPORT2(favx, fmrmod_k3, fmrmod_k3_fma3);

            CEXPORT2(favx, fmadd_k4, fmadd_k4_fma3);
            CEXPORT2(favx, fmsub_k4, fmsub_k4_fma3);
            CEXPORT2(favx, fmrsub_k4, fmrsub_k4_fma3);
            CEXPORT2(favx, fmmod_k4, fmmod_k4_fma3);
            CEXPORT2(favx, fmrmod_k4, fmrmod_k4_fma3);

            CEXPORT2(favx, fmadd3, fmadd3_fma3);
            CEXPORT2(favx, fmsub3, fmsub3_fma3);
            CEXPORT2(favx, fmrsub3, fmrsub3_fma3);
            CEXPORT2(favx, fmmod3, fmmod3_fma3);
            CEXPORT2(favx, fmrmod3, fmrmod3_fma3);

            CEXPORT2(favx, fmadd4, fmadd4_fma3);
            CEXPORT2(favx, fmsub4, fmsub4_fma3);
            CEXPORT2(favx, fmrsub4, fmrsub4_fma3);
            CEXPORT2(favx, fmmod4, fmmod4_fma3);
            CEXPORT2(favx, fmrmod4, fmrmod4_fma3);

            CEXPORT2(favx, complex_mul2, complex_mul2_fma3);
            CEXPORT2(favx, complex_mul3, complex_mul3_fma3);
            CEXPORT2(favx, complex_div2, complex_div2_fma3);
            CEXPORT2(favx, complex_rdiv2, complex_rdiv2_fma3);
            CEXPORT2(favx, complex_div3, complex_div3_fma3);
            CEXPORT2(favx, complex_mod, complex_mod_fma3);
            CEXPORT2(favx, complex_rcp1, complex_rcp1_fma3);
            CEXPORT2(favx, complex_rcp2, complex_rcp2_fma3);

            CEXPORT2(favx, pcomplex_mul2, pcomplex_mul2_fma3);
            CEXPORT2(favx, pcomplex_mul3, pcomplex_mul3_fma3);
            CEXPORT2(favx, pcomplex_div2, pcomplex_div2_fma3);
            CEXPORT2(favx, pcomplex_rdiv2, pcomplex_rdiv2_fma3);
            CEXPORT2(favx, pcomplex_div3, pcomplex_div3_fma3);

            CEXPORT2(favx, h_sqr_sum, h_sqr_sum_fma3);
//            CEXPORT2(favx, h_dotp_sum, h_dotp_sum_fma3);

            CEXPORT2(favx, direct_fft, direct_fft_fma3);
            CEXPORT2(favx, reverse_fft, reverse_fft_fma3);
            CEXPORT2(favx, packed_direct_fft, packed_direct_fft_fma3);
            CEXPORT2(favx, packed_reverse_fft, packed_reverse_fft_fma3);

            CEXPORT2(favx, fastconv_parse, fastconv_parse_fma3);
            CEXPORT2(favx, fastconv_restore, fastconv_restore_fma3);
            CEXPORT2(favx, fastconv_apply, fastconv_apply_fma3);
            CEXPORT2(favx, fastconv_parse_apply, fastconv_parse_apply_fma3);

            CEXPORT2(favx, filter_transfer_calc_ri, filter_transfer_calc_ri_fma3);
            CEXPORT2(favx, filter_transfer_apply_ri, filter_transfer_apply_ri_fma3);
            CEXPORT2(favx, filter_transfer_calc_pc, filter_transfer_calc_pc_fma3);
            CEXPORT2(favx, filter_transfer_apply_pc, filter_transfer_apply_pc_fma3);

            CEXPORT2(favx, convolve, convolve_fma3);


            CEXPORT2(favx, biquad_process_x1, biquad_process_x1_fma3);
            CEXPORT2(favx, biquad_process_x2, biquad_process_x2_fma3);
            CEXPORT2(favx, biquad_process_x4, biquad_process_x4_fma3);
            CEXPORT2(ffma, biquad_process_x8, biquad_process_x8_fma3);

            CEXPORT2(ffma, dyn_biquad_process_x1, dyn_biquad_process_x1_fma3);
            CEXPORT2(favx, dyn_biquad_process_x2, dyn_biquad_process_x2_fma3);
            CEXPORT2(favx, dyn_biquad_process_x4, dyn_biquad_process_x4_fma3);
            CEXPORT2(ffma, dyn_biquad_process_x8, dyn_biquad_process_x8_fma3);
        }
    }

    #undef EXPORT1
    #undef EXPORT2
}

