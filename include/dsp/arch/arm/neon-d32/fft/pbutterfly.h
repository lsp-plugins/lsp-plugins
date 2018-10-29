/*
 * pbutterfly.h
 *
 *  Created on: 30 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FFT_PBUTTERFLY_H_
#define DSP_ARCH_ARM_NEON_D32_FFT_PBUTTERFLY_H_

namespace neon_d32
{
    #define PBUTTERFLY_RANK3(op1, op2) \
        __ASM_EMIT("vldm        %[XFFT_A], {q8-q11}")           /* q8   = wr1, q9 = wr2, q10 = wi1, q11 = wi2 */ \
        __ASM_EMIT("subs        %[blocks], $2") \
        __ASM_EMIT("blo         2f") \
        \
        /* 8x butterflies */ \
        /* Calculate complex c = w * b */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vld2.32     {q0-q1}, %[src]!")              /* q0   = ar1, q1 = ai1 */ \
        __ASM_EMIT("vld2.32     {q2-q3}, %[src]!")              /* q2   = br1, q3 = bi1 */ \
        __ASM_EMIT("vld2.32     {q4-q5}, %[src]!")              /* q4   = ar2, q5 = ai2 */ \
        __ASM_EMIT("vld2.32     {q6-q7}, %[src]!")              /* q6   = br2, q7 = bi2 */ \
        /* Calc cr and ci */ \
        __ASM_EMIT("vmul.f32    q12, q8, q2")                   /* q12  = wr1 * br1 */ \
        __ASM_EMIT("vmul.f32    q13, q9, q6")                   /* q13  = wr2 * br2 */ \
        __ASM_EMIT("vmul.f32    q14, q8, q3")                   /* q14  = wr1 * bi1 */ \
        __ASM_EMIT("vmul.f32    q15, q9, q7")                   /* q15  = wr2 * bi2 */ \
        __ASM_EMIT(op1 "        q12, q10, q3")                  /* q12  = wr1 * br1 +- wi1 * bi1 = cr1 */ \
        __ASM_EMIT(op1 "        q13, q11, q7")                  /* q13  = wr2 * br2 +- wi2 * bi2 = cr2 */ \
        __ASM_EMIT(op2 "        q14, q10, q2")                  /* q14  = wr1 * bi1 -+ wi1 * br1 = ci1 */ \
        __ASM_EMIT(op2 "        q15, q11, q6")                  /* q15  = wr1 * bi1 -+ wi2 * br2 = ci2 */ \
        /* Apply butterfly */ \
        __ASM_EMIT("vsub.f32    q2, q0, q12")                   /* q2   = ar1 - cr1 */ \
        __ASM_EMIT("vsub.f32    q3, q1, q14")                   /* q3   = ai1 - ci1 */ \
        __ASM_EMIT("vsub.f32    q6, q4, q13")                   /* q2   = ar2 - cr2 */ \
        __ASM_EMIT("vsub.f32    q7, q5, q15")                   /* q3   = ai2 - ci2 */ \
        __ASM_EMIT("vadd.f32    q0, q0, q12")                   /* q0   = ar1 + cr1 */ \
        __ASM_EMIT("vadd.f32    q1, q1, q14")                   /* q1   = ai1 + ci1 */ \
        __ASM_EMIT("vadd.f32    q4, q4, q13")                   /* q4   = ar2 + cr2 */ \
        __ASM_EMIT("vadd.f32    q5, q5, q15")                   /* q5   = ai2 + ci2 */ \
        __ASM_EMIT("vst2.32     {q0-q1}, [%[dst]]!") \
        __ASM_EMIT("vst2.32     {q2-q3}, [%[dst]]!") \
        __ASM_EMIT("vst2.32     {q4-q5}, [%[dst]]!") \
        __ASM_EMIT("vst2.32     {q6-q7}, [%[dst]]!") \
        __ASM_EMIT("subs        %[blocks], $2") \
        __ASM_EMIT("bge         1b") \
        \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[blocks], $1") \
        __ASM_EMIT("blo         4f") \
        /* 4x butterflies */ \
        __ASM_EMIT("vld2.32     {q0-q1}, %[src]!")              /* q0   = ar1, q1 = ai1 */ \
        __ASM_EMIT("vld2.32     {q2-q3}, %[src]!")              /* q2   = br1, q3 = bi1 */ \
        __ASM_EMIT("vmul.f32    q12, q8, q2")                   /* q12  = wr1 * br1 */ \
        __ASM_EMIT("vmul.f32    q14, q8, q3")                   /* q14  = wr1 * bi1 */ \
        __ASM_EMIT(op1 "        q12, q10, q3")                  /* q12  = wr1 * br1 +- wi1 * bi1 = cr1 */ \
        __ASM_EMIT(op2 "        q14, q10, q2")                  /* q14  = wr1 * bi1 -+ wi1 * br1 = ci1 */ \
        /* Apply butterfly */ \
        __ASM_EMIT("vsub.f32    q2, q0, q12")                   /* q2   = ar1 - cr1 */ \
        __ASM_EMIT("vsub.f32    q3, q1, q14")                   /* q3   = ai1 - ci1 */ \
        __ASM_EMIT("vadd.f32    q0, q0, q12")                   /* q0   = ar1 + cr1 */ \
        __ASM_EMIT("vadd.f32    q1, q1, q14")                   /* q1   = ai1 + ci1 */ \
        __ASM_EMIT("vst2.32     {q0-q1}, [%[dst]]!") \
        __ASM_EMIT("vst2.32     {q2-q3}, [%[dst]]!") \
        __ASM_EMIT("4:")


    void packed_direct_butterfly_rank3(float *dst, size_t blocks) {
        IF_ARCH_ARM(
            const float *src = dst;
        );

        ARCH_ARM_ASM(
            PBUTTERFLY_RANK3("vmla.f32", "vmls.f32")
            : [dst] "+r" (dst), [src] "+r" (src),
              [blocks] "+r" (blocks)
            : [XFFT_A] "r" (&XFFT_A[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void packed_reverse_butterfly_rank3(float *dst_re, float *dst_im, size_t blocks) {
        IF_ARCH_ARM(
            const float *src = dst;
        );

        ARCH_ARM_ASM(
            PBUTTERFLY_RANK3("vmls.f32", "vmla.f32")
            : [dst] "+r" (dst), [src] "+r" (src),
              [blocks] "+r" (blocks)
            : [XFFT_A] "r" (&XFFT_A[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    #undef PBUTTERFLY_RANK3
}

#endif /* DSP_ARCH_ARM_NEON_D32_FFT_PBUTTERFLY_H_ */
