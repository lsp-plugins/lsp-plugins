/*
 * normalize.h
 *
 *  Created on: 18 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FFT_NORMALIZE_H_
#define DSP_ARCH_AARCH64_ASIMD_FFT_NORMALIZE_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void normalize_fft3(float *dre, float *dim, const float *re, const float *im, size_t rank)
    {
        IF_ARCH_AARCH64(
            float k = 1.0f/(1 << rank);
            size_t count = 1 << rank;
        );
        ARCH_AARCH64_ASM(
            // x8 blocks
            __ASM_EMIT("dup         v0.4s, %S[k].s[0]")             // v0   = k
            __ASM_EMIT("subs        %[count], %[count], #8")
            __ASM_EMIT("mov         v1.16b, v0.16b")
            __ASM_EMIT("b.lt        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q4, q5, [%[s_re], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[s_im], #0x00]")
            __ASM_EMIT("fmul        v4.4s, v4.4s, v0.4s")
            __ASM_EMIT("fmul        v5.4s, v5.4s, v1.4s")
            __ASM_EMIT("fmul        v6.4s, v6.4s, v0.4s")
            __ASM_EMIT("fmul        v7.4s, v7.4s, v1.4s")
            __ASM_EMIT("stp         q4, q5, [%[d_re], #0x00]")
            __ASM_EMIT("stp         q6, q7, [%[d_im], #0x00]")
            __ASM_EMIT("subs        %[count], %[count], #8")
            __ASM_EMIT("add         %[s_re], %[s_re], #0x20")
            __ASM_EMIT("add         %[s_im], %[s_im], #0x20")
            __ASM_EMIT("add         %[d_re], %[d_re], #0x20")
            __ASM_EMIT("add         %[d_im], %[d_im], #0x20")
            __ASM_EMIT("b.hs        1b")
            __ASM_EMIT("2:")
            : [s_re] "+r" (re), [s_im] "+r" (im),
              [d_re] "+r" (dre), [d_im] "+r" (dim),
              [k] "+w" (k), [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1",
              "v4", "v5", "v6", "v7"
        );
    }

    void normalize_fft2(float *re, float *im, size_t rank)
    {
        IF_ARCH_AARCH64(
            float k = 1.0f/(1 << rank);
            size_t count = 1 << rank;
        );
        ARCH_AARCH64_ASM(
            // x8 blocks
            __ASM_EMIT("dup         v0.4s, %S[k].s[0]")             // v0   = k
            __ASM_EMIT("subs        %[count], %[count], #8")
            __ASM_EMIT("mov         v1.16b, v0.16b")
            __ASM_EMIT("b.lt        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q4, q5, [%[d_re], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[d_im], #0x00]")
            __ASM_EMIT("fmul        v4.4s, v4.4s, v0.4s")
            __ASM_EMIT("fmul        v5.4s, v5.4s, v1.4s")
            __ASM_EMIT("fmul        v6.4s, v6.4s, v0.4s")
            __ASM_EMIT("fmul        v7.4s, v7.4s, v1.4s")
            __ASM_EMIT("stp         q4, q5, [%[d_re], #0x00]")
            __ASM_EMIT("stp         q6, q7, [%[d_im], #0x00]")
            __ASM_EMIT("subs        %[count], %[count], #8")
            __ASM_EMIT("add         %[d_re], %[d_re], #0x20")
            __ASM_EMIT("add         %[d_im], %[d_im], #0x20")
            __ASM_EMIT("b.hs        1b")
            __ASM_EMIT("2:")
            : [d_re] "+r" (re), [d_im] "+r" (im),
              [k] "+w" (k), [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1",
              "v4", "v5", "v6", "v7"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_FFT_NORMALIZE_H_ */
