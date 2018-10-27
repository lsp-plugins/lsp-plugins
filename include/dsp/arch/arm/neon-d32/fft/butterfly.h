/*
 * butterfly.h
 *
 *  Created on: 26 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FFT_BUTTERFLY_H_
#define DSP_ARCH_ARM_NEON_D32_FFT_BUTTERFLY_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    #define BUTTERFLY_RANK3(op1, op2) \
        __ASM_EMIT("vldm        %[XFFT_A], {q8-q11}")           /* q8   = wr1, q9 = wr2, q10 = wi1, q11 = wi2 */ \
        __ASM_EMIT("subs        %[blocks], $2") \
        __ASM_EMIT("blo         2f") \
        \
        /* 8x butterflies */ \
        /* Calculate complex c = w * b */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[dst_re], {q0-q3}")            /* q0   = ar1, q1 = br1, q2 = ar2, q3 = br2 */ \
        __ASM_EMIT("vldm        %[dst_im], {q4-q7}")            /* q4   = ai1, q5 = bi1, q6 = ai2, q7 = bi2 */ \
        __ASM_EMIT("vmul.f32    q12, q8, q1")                   /* q12  = wr1 * br1 */ \
        __ASM_EMIT("vmul.f32    q13, q9, q3")                   /* q13  = wr2 * br2 */ \
        __ASM_EMIT("vmul.f32    q14, q8, q5")                   /* q14  = wr1 * bi1 */ \
        __ASM_EMIT("vmul.f32    q15, q9, q7")                   /* q15  = wr2 * bi2 */ \
        __ASM_EMIT(op1 "        q12, q10, q5")                  /* q12  = wr1 * br1 +- wi1 * bi1 = cr1 */ \
        __ASM_EMIT(op1 "        q13, q11, q7")                  /* q13  = wr2 * br2 +- wi2 * bi2 = cr2 */ \
        __ASM_EMIT(op2 "        q14, q10, q1")                  /* q14  = wr1 * bi1 -+ wi1 * br1 = ci1 */ \
        __ASM_EMIT(op2 "        q15, q11, q3")                  /* q15  = wr1 * bi1 -+ wi2 * br2 = ci2 */ \
        /* Apply butterfly */ \
        __ASM_EMIT("vadd.f32    q0, q12")                       /* q0   = ar1 + cr1 */ \
        __ASM_EMIT("vadd.f32    q2, q13")                       /* q2   = ar2 + cr2 */ \
        __ASM_EMIT("vsub.f32    q1, q12")                       /* q1   = ar1 - cr1 */ \
        __ASM_EMIT("vsub.f32    q3, q13")                       /* q3   = ar2 - cr2 */ \
        __ASM_EMIT("vadd.f32    q4, q14")                       /* q4   = ai1 + ci1 */ \
        __ASM_EMIT("vadd.f32    q6, q15")                       /* q6   = ai2 + ci2 */ \
        __ASM_EMIT("vsub.f32    q5, q14")                       /* q5   = ai1 - ci1 */ \
        __ASM_EMIT("vsub.f32    q7, q15")                       /* q7   = ai2 - ci2 */ \
        __ASM_EMIT("vstm        %[dst_re]!, {q0-q3}") \
        __ASM_EMIT("vstm        %[dst_im]!, {q4-q7}") \
        __ASM_EMIT("subs        %[blocks], $2") \
        __ASM_EMIT("bge         1b") \
        \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[blocks], $1") \
        __ASM_EMIT("jl          4f") \
        /* 4x butterflies */ \
        __ASM_EMIT("vldm        %[dst_re], {q0-q1}")            /* q0   = ar1, q1 = br1 */ \
        __ASM_EMIT("vldm        %[dst_im], {q4-q5}")            /* q4   = ai1, q5 = bi1 */ \
        __ASM_EMIT("vmul.f32    q12, q8, q1")                   /* q12  = wr1 * br1 */ \
        __ASM_EMIT("vmul.f32    q14, q8, q5")                   /* q14  = wr1 * bi1 */ \
        __ASM_EMIT(op1 "        q12, q10, q5")                  /* q12  = wr1 * br1 +- wi1 * bi1 = cr1 */ \
        __ASM_EMIT(op2 "        q14, q10, q1")                  /* q14  = wr1 * bi1 -+ wi1 * br1 = ci1 */ \
        /* Apply butterfly */ \
        __ASM_EMIT("vadd.f32    q0, q12")                       /* q0   = ar1 + cr1 */ \
        __ASM_EMIT("vsub.f32    q1, q12")                       /* q1   = ar1 - cr1 */ \
        __ASM_EMIT("vadd.f32    q4, q14")                       /* q4   = ai1 + ci1 */ \
        __ASM_EMIT("vsub.f32    q5, q14")                       /* q5   = ai1 - ci1 */ \
        __ASM_EMIT("vstm        %[dst_re]!, {q0-q1}") \
        __ASM_EMIT("vstm        %[dst_im]!, {q4-q5}")


    void direct_butterfly_rank3(float *dst_re, float *dst_im, size_t blocks) {
        ARCH_ARM_ASM(
            BUTTERFLY_RANK3("vmla.f32", "vmls.f32")
            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [blocks] "+r" (blocks)
            : [XFFT_A] "r" (&XFFT_A[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void reverse_butterfly_rank3(float *dst_re, float *dst_im, size_t blocks) {
        ARCH_ARM_ASM(
            BUTTERFLY_RANK3("vmls.f32", "vmla.f32")
            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [blocks] "+r" (blocks)
            : [XFFT_A] "r" (&XFFT_A[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

}

#endif /* DSP_ARCH_ARM_NEON_D32_FFT_BUTTERFLY_H_ */
