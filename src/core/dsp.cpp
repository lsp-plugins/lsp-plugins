/*
 * dsp.cpp
 *
 *  Created on: 02 окт. 2015 г.
 *      Author: sadko
 */

#include <math.h>

#include <core/types.h>
#include <core/dsp.h>
#include <core/debug.h>

#ifdef ARCH_X86
    #include <core/x86/cpuid.h>
#endif /* defined(__i386__) || defined(__x86_64__) */

namespace lsp
{
    namespace native
    {
        extern void dsp_init();
    }

    #ifdef ARCH_X86
        namespace x86
        {
            extern void dsp_init(dsp_options_t options);
        }

        namespace sse
        {
            extern void dsp_init(dsp_options_t options);
        }

        namespace sse3
        {
            extern void dsp_init(dsp_options_t options);
        }

        namespace avx
        {
            extern void dsp_init(dsp_options_t options);
        }
    #endif /* ARCH_X86 */
}

namespace lsp
{
    // Declare static variables
    namespace dsp
    {
        void    (* start)(dsp_context_t *ctx) = NULL;
        void    (* finish)(dsp_context_t *ctx) = NULL;

        void    (* copy)(float *dst, const float *src, size_t count) = NULL;
        void    (* copy_saturated)(float *dst, const float *src, size_t count) = NULL;
        void    (* saturate)(float *dst, size_t count) = NULL;
        void    (* move)(float *dst, const float *src, size_t count) = NULL;
        void    (* fill)(float *dst, float value, size_t count) = NULL;
        void    (* fill_zero)(float *dst, size_t count) = NULL;
        void    (* fill_one)(float *dst, size_t count) = NULL;
        void    (* fill_minus_one)(float *dst, size_t count) = NULL;
        void    (* abs)(float *dst, const float *src, size_t count) = NULL;
        void    (* abs_normalized)(float *dst, const float *src, size_t count) = NULL;
        void    (* normalize)(float *dst, const float *src, size_t count) = NULL;
        float   (* min)(const float *src, size_t count) = NULL;
        float   (* max)(const float *src, size_t count) = NULL;
        float   (* abs_max)(const float *src, size_t count) = NULL;
        float   (* abs_min)(const float *src, size_t count) = NULL;
        size_t  (* abs_max_index)(const float *src, size_t count) = NULL;
        size_t  (* abs_min_index)(const float *src, size_t count) = NULL;
        void    (* minmax)(const float *src, size_t count, float *min, float *max) = NULL;
        size_t  (* min_index)(const float *src, size_t count) = NULL;
        size_t  (* max_index)(const float *src, size_t count) = NULL;
        void    (* scale)(float *dst, const float *src, float k, size_t count) = NULL;
        void    (* multiply)(float *dst, const float *src1, const float *src2, size_t count) = NULL;
        float   (* h_sum)(const float *src, size_t count) = NULL;
        float   (* h_sqr_sum)(const float *src, size_t count) = NULL;
        float   (* h_abs_sum)(const float *src, size_t count) = NULL;
        float   (* scalar_mul)(const float *a, const float *b, size_t count) = NULL;
        void    (* accumulate)(float *dst, const float *src, float k, float p, size_t count) = NULL;
        void    (* add_multiplied)(float *dst, const float *src, float k, size_t count) = NULL;
        void    (* sub_multiplied)(float *dst, const float *src, float k, size_t count) = NULL;
        void    (* add)(float *dst, const float *src, size_t count) = NULL;
        void    (* sub)(float *dst, const float *src, size_t count) = NULL;
        void    (* integrate)(float *dst, const float *src, float k, size_t count) = NULL;
        void    (* mix)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count) = NULL;
        void    (* mix_add)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count) = NULL;
        float   (* convolve_single)(const float *src, const float *conv, size_t length) = NULL;
        void    (* convolve)(float *dst, const float *src, const float *conv, size_t length, size_t count) = NULL;
        void    (* reverse)(float *dst, size_t count) = NULL;
        void    (* direct_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank) = NULL;
        void    (* reverse_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank) = NULL;
//        void    (* join_fft)(float *dst_re, float *dst_im, float *src_re, float *src_im, size_t rank) = NULL;
        void    (* normalize_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank) = NULL;
        void    (* center_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank) = NULL;
        void    (* combine_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank) = NULL;
        void    (* complex_mul)(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count) = NULL;
        void    (* complex_cvt2modarg)(float *dst_mod, float *dst_arg, const float *src_re, const float *src_im, size_t count) = NULL;
        void    (* complex_cvt2reim)(float *dst_re, float *dst_im, const float *src_mod, const float *src_arg, size_t count) = NULL;
        void    (* complex_mod)(float *dst_mod, const float *src_re, const float *src_im, size_t count) = NULL;
        void    (* lr_to_ms)(float *m, float *s, const float *l, const float *r, size_t count) = NULL;
        void    (* lr_to_mid)(float *m, const float *l, const float *r, size_t count) = NULL;
        void    (* lr_to_side)(float *s, const float *l, const float *r, size_t count) = NULL;
        void    (* ms_to_lr)(float *l, float *r, const float *m, const float *s, size_t count) = NULL;
        void    (* ms_to_left)(float *l, const float *m, const float *s, size_t count) = NULL;
        void    (* ms_to_right)(float *r, const float *m, const float *s, size_t count) = NULL;
        void    (* avoid_denormals)(float *dst, const float *src, size_t count) = NULL;
        float   (* biquad_process)(float *buf, const float *ir, float sample) = NULL;
        void    (* biquad_process_multi)(float *dst, const float *src, size_t count, float *buf, const float *ir) = NULL;
        void    (* biquad_process_x1)(float *dst, const float *src, size_t count, biquad_t *f) = NULL;
        void    (* biquad_process_x2)(float *dst, const float *src, size_t count, biquad_t *f) = NULL;
        void    (* biquad_process_x4)(float *dst, const float *src, size_t count, biquad_t *f) = NULL;
        void    (* biquad_process_x8)(float *dst, const float *src, size_t count, biquad_t *f) = NULL;

        void    (* axis_apply_log)(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count) = NULL;
        void    (* rgba32_to_bgra32)(void *dst, const void *src, size_t count) = NULL;

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
    }

    namespace dsp
    {
        #ifdef ARCH_X86
        dsp_options_t do_intel_cpuid(size_t family_id, size_t max_cpuid, size_t max_ext_cpuid)
        {
            cpuid_info_t info;
            dsp_options_t options = 0;

            // FUNCTION 1
            if (max_cpuid >= 1)
            {
                cpuid(1, 0, &info);

                if (info.edx & X86_CPUID1_INTEL_EDX_FPU)
                    options     |= DSP_OPTION_FPU;
                if (info.edx & X86_CPUID1_INTEL_EDX_CMOV)
                    options     |= DSP_OPTION_CMOV;
                if (info.edx & X86_CPUID1_INTEL_EDX_MMX)
                    options     |= DSP_OPTION_MMX;
                if (info.edx & X86_CPUID1_INTEL_EDX_SSE)
                    options     |= DSP_OPTION_SSE;
                if (info.edx & X86_CPUID1_INTEL_EDX_SSE2)
                    options     |= DSP_OPTION_SSE2;
                if (info.ecx & X86_CPUID1_INTEL_ECX_SSE3)
                    options     |= DSP_OPTION_SSE3;
                if (info.ecx & X86_CPUID1_INTEL_ECX_SSSE3)
                    options     |= DSP_OPTION_SSSE3;
                if (info.ecx & X86_CPUID1_INTEL_ECX_SSE4_1)
                    options     |= DSP_OPTION_SSE4_1;
                if (info.ecx & X86_CPUID1_INTEL_ECX_SSE4_2)
                    options     |= DSP_OPTION_SSE4_2;
                if (info.ecx & X86_CPUID1_INTEL_ECX_FMA3)
                    options     |= DSP_OPTION_FMA3;
                if (info.ecx & X86_CPUID1_INTEL_ECX_AVX)
                    options     |= DSP_OPTION_AVX;
            }

            // FUNCTION 7
            if (max_cpuid >= 7)
            {
                cpuid(7, 0, &info);

                if (info.ebx & X86_CPUID7_INTEL_EBX_AVX2)
                    options     |= DSP_OPTION_AVX2;
            }

            return options;
        }

        dsp_options_t do_amd_cpuid(size_t family_id, size_t max_cpuid, size_t max_ext_cpuid)
        {
            cpuid_info_t info;

            dsp_options_t options = 0;
            if (max_cpuid < 1)
                return options;

            // FUNCTION 1
            if (max_cpuid >= 1)
            {
                cpuid(1, 0, &info);

                if (info.edx & X86_CPUID1_AMD_EDX_FPU)
                    options     |= DSP_OPTION_FPU;
                if (info.edx & X86_CPUID1_AMD_EDX_CMOV)
                    options     |= DSP_OPTION_CMOV;
                if (info.edx & X86_CPUID1_AMD_EDX_MMX)
                    options     |= DSP_OPTION_MMX;
                if (info.edx & X86_CPUID1_AMD_EDX_SSE)
                    options     |= DSP_OPTION_SSE;
                if (info.edx & X86_CPUID1_AMD_EDX_SSE2)
                    options     |= DSP_OPTION_SSE2;

                if (info.ecx & X86_CPUID1_AMD_ECX_SSE3)
                    options     |= DSP_OPTION_SSE3;
                if (info.ecx & X86_CPUID1_AMD_ECX_SSSE3)
                    options     |= DSP_OPTION_SSSE3;
                if (info.ecx & X86_CPUID1_AMD_ECX_SSE4_1)
                    options     |= DSP_OPTION_SSE4_1;
                if (info.ecx & X86_CPUID1_AMD_ECX_SSE4_2)
                    options     |= DSP_OPTION_SSE4_2;
                if (info.ecx & X86_CPUID1_AMD_ECX_FMA3)
                    options     |= DSP_OPTION_FMA3;
                if (info.ecx & X86_CPUID1_AMD_ECX_AVX)
                    options     |= DSP_OPTION_AVX;
            }

            // FUNCTION 7
            if (max_cpuid >= 7)
            {
                cpuid(7, 0, &info);

                if (info.ebx & X86_CPUID7_AMD_EBX_AVX2)
                    options     |= DSP_OPTION_AVX2;
            }

            if (max_ext_cpuid >= 0x80000001)
            {
                cpuid(0x80000001, 0, &info);

                if (info.ecx & X86_XCPUID1_AMD_ECX_FMA4)
                    options     |= DSP_OPTION_FMA4;
                if (info.ecx & X86_XCPUID1_AMD_ECX_SSE4A)
                    options     |= DSP_OPTION_SSE4A;

                if (info.edx & X86_XCPUID1_AMD_EDX_FPU)
                    options     |= DSP_OPTION_FPU;
                if (info.edx & X86_XCPUID1_AMD_EDX_CMOV)
                    options     |= DSP_OPTION_CMOV;
                if (info.edx & X86_XCPUID1_AMD_EDX_MMX)
                    options     |= DSP_OPTION_MMX;
            }

            return options;
        }
        #endif

        dsp_options_t detect_options()
        {
            dsp_options_t options = 0;

            // X86-family code
            #ifdef ARCH_X86
                // Check max CPUID
                cpuid_info_t info;
                if (!cpuid(0, 0, &info))
                    return options;

                // Detect vendor
                if ((info.ebx == X86_CPUID0_INTEL_EBX) && (info.ecx == X86_CPUID0_INTEL_ECX) && (info.edx == X86_CPUID0_INTEL_EDX))
                    options     |= DSP_OPTION_CPU_INTEL;
                else if ((info.ebx == X86_CPUID0_AMD_EBX) && (info.ecx == X86_CPUID0_AMD_ECX) && (info.edx == X86_CPUID0_AMD_EDX))
                    options     |= DSP_OPTION_CPU_AMD;

                size_t max_cpuid    = info.eax;
                if (max_cpuid <= 0)
                    return options;

                // Get model and family
                cpuid(1, 0, &info);
                size_t family_id    = (info.eax >> 8) & 0x0f;
                size_t model_id     = (info.eax >> 4) & 0x0f;

                if (family_id == 0x0f)
                    family_id   += (info.eax >> 20) & 0xff;
                if ((family_id == 0x0f) || (family_id == 0x06))
                    model_id    += (info.eax >> 12) & 0xf0;

                // Get maximum available extended CPUID
                cpuid(0x80000000, 0, &info);
                size_t max_ext_cpuid = info.eax;

                if ((options & DSP_OPTION_CPU_MASK) == DSP_OPTION_CPU_INTEL)
                    options        |= do_intel_cpuid(family_id, max_cpuid, max_ext_cpuid);
                else if ((options & DSP_OPTION_CPU_MASK) == DSP_OPTION_CPU_AMD)
                    options        |= do_amd_cpuid(family_id, max_cpuid, max_ext_cpuid);
            #endif /* ARCH_X86 */

            return options;
        }

        void init_context(dsp_context_t *ctx)
        {
            ctx->top        = 0;
        }

        void init()
        {
            // Consider already initialized
            if (copy != NULL)
                return;

            // Information message
            lsp_trace("Initializing DSP");

            dsp_options_t options   = detect_options();

            // Initialize with native functions
            native::dsp_init();

            #ifdef ARCH_X86
                x86::dsp_init(options);
                sse::dsp_init(options);
                sse3::dsp_init(options);
                avx::dsp_init(options);
            #endif /* ARCH_X86 */
        }
    }

} /* namespace lsp */
