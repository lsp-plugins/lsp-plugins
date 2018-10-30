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
        /* Calc cr and ci */ \
        __ASM_EMIT("vldm        %[dst], {q0-q7}")               /* q0   = ar1, q1 = ai1, q2 = br1, q3 = bi1, q4 = ar2, q5 = ai2, q6 = br2, q7 = bi2 */\
        __ASM_EMIT("vmul.f32    q12, q8, q2")                   /* q12  = wr1 * br1 */ \
        __ASM_EMIT("vmul.f32    q14, q8, q3")                   /* q14  = wr1 * bi1 */ \
        __ASM_EMIT("vmul.f32    q13, q9, q6")                   /* q13  = wr2 * br2 */ \
        __ASM_EMIT("vmul.f32    q15, q9, q7")                   /* q15  = wr2 * bi2 */ \
        __ASM_EMIT(op1 "        q12, q10, q3")                  /* q12  = wr1 * br1 +- wi1 * bi1 = cr1 */ \
        __ASM_EMIT(op1 "        q13, q11, q7")                  /* q13  = wr2 * br2 +- wi2 * bi2 = cr2 */ \
        __ASM_EMIT(op2 "        q14, q10, q2")                  /* q14  = wr1 * bi1 -+ wi1 * br1 = ci1 */ \
        __ASM_EMIT(op2 "        q15, q11, q6")                  /* q15  = wr1 * bi1 -+ wi2 * br2 = ci2 */ \
        /* Apply butterfly */ \
        __ASM_EMIT("vsub.f32    q2, q0, q12")                   /* q2   = ar1 - cr1 */ \
        __ASM_EMIT("vsub.f32    q3, q1, q14")                   /* q3   = ai1 - ci1 */ \
        __ASM_EMIT("vsub.f32    q6, q4, q13")                   /* q6   = ar2 - cr2 */ \
        __ASM_EMIT("vsub.f32    q7, q5, q15")                   /* q7   = ai2 - ci2 */ \
        __ASM_EMIT("vadd.f32    q0, q0, q12")                   /* q0   = ar1 + cr1 */ \
        __ASM_EMIT("vadd.f32    q1, q1, q14")                   /* q1   = ai1 + ci1 */ \
        __ASM_EMIT("vadd.f32    q4, q4, q13")                   /* q4   = ar2 + cr2 */ \
        __ASM_EMIT("vadd.f32    q5, q5, q15")                   /* q5   = ai2 + ci2 */ \
        __ASM_EMIT("vstm        %[dst]!, {q0-q7}")               \
        __ASM_EMIT("subs        %[blocks], $2") \
        __ASM_EMIT("bge         1b") \
        \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[blocks], $1") \
        __ASM_EMIT("blo         4f") \
        /* 4x butterflies */ \
        __ASM_EMIT("vldm        %[dst], {q0-q3}")               /* q0   = ar1, q1 = ai1, q2 = br1, q3 = bi1 */ \
        __ASM_EMIT("vmul.f32    q12, q8, q2")                   /* q12  = wr1 * br1 */ \
        __ASM_EMIT("vmul.f32    q14, q8, q3")                   /* q14  = wr1 * bi1 */ \
        __ASM_EMIT(op1 "        q12, q10, q3")                  /* q12  = wr1 * br1 +- wi1 * bi1 = cr1 */ \
        __ASM_EMIT(op2 "        q14, q10, q2")                  /* q14  = wr1 * bi1 -+ wi1 * br1 = ci1 */ \
        /* Apply butterfly */ \
        __ASM_EMIT("vsub.f32    q2, q0, q12")                   /* q2   = ar1 - cr1 */ \
        __ASM_EMIT("vsub.f32    q3, q1, q14")                   /* q3   = ai1 - ci1 */ \
        __ASM_EMIT("vadd.f32    q0, q0, q12")                   /* q0   = ar1 + cr1 */ \
        __ASM_EMIT("vadd.f32    q1, q1, q14")                   /* q1   = ai1 + ci1 */ \
        __ASM_EMIT("vstm        %[dst], {q0-q3}") \
        __ASM_EMIT("4:")

    void packed_direct_butterfly_rank3(float *dst, size_t blocks) {
        ARCH_ARM_ASM(
            PBUTTERFLY_RANK3("vmla.f32", "vmls.f32")
            : [dst] "+r" (dst), [blocks] "+r" (blocks)
            : [XFFT_A] "r" (&XFFT_A[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void packed_reverse_butterfly_rank3(float *dst, size_t blocks) {
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

    #define PBUTTERFLY_RANK4(op1, op2) \
        __ASM_EMIT("1:") \
            /* Initialize sub-loop */ \
            __ASM_EMIT("mov         %[pairs], $1")                  /* pairs = 1 */ \
            __ASM_EMIT("vldm        %[XFFT_A], {q8-q11}")           /* q8   = wr1, q9 = wr2, q10 = wi1, q11 = wi2 */ \
            __ASM_EMIT("lsl         %[pairs], %[pairs], %[rank]")   /* pairs = 1 << rank */ \
            __ASM_EMIT("add         %[b], %[a], %[pairs], LSL $5")  /* b = &a[pairs*8] */ \
            /* 8x butterflies */ \
            /* Calculate complex c = w * b */ \
            __ASM_EMIT("3:") \
            /* Calc cr and ci */ \
            __ASM_EMIT("vldm        %[b], {q4-q7}")                 /* q4   = br1, q5 = bi1, q6 = br2, q7 = bi2 */ \
            __ASM_EMIT("vldm        %[a], {q0-q3}")                 /* q0   = ar1, q1 = ai1, q2 = ar2, q3 = ai2 */ \
            __ASM_EMIT("vmul.f32    q12, q8, q4")                   /* q12  = wr1 * br1 */ \
            __ASM_EMIT("vmul.f32    q14, q8, q5")                   /* q14  = wr1 * bi1 */ \
            __ASM_EMIT("vmul.f32    q13, q9, q6")                   /* q13  = wr2 * br2 */ \
            __ASM_EMIT("vmul.f32    q15, q9, q7")                   /* q15  = wr2 * bi2 */ \
            __ASM_EMIT(op1 "        q12, q10, q5")                  /* q12  = wr1 * br1 +- wi1 * bi1 = cr1 */ \
            __ASM_EMIT(op1 "        q13, q11, q7")                  /* q13  = wr2 * br2 +- wi2 * bi2 = cr2 */ \
            __ASM_EMIT(op2 "        q14, q10, q4")                  /* q14  = wr1 * bi1 -+ wi1 * br1 = ci1 */ \
            __ASM_EMIT(op2 "        q15, q11, q6")                  /* q15  = wr1 * bi1 -+ wi2 * br2 = ci2 */ \
            /* Apply butterfly */ \
            __ASM_EMIT("vsub.f32    q4, q0, q12")                   /* q4   = ar1 - cr1 */ \
            __ASM_EMIT("vsub.f32    q5, q1, q14")                   /* q5   = ai1 - ci1 */ \
            __ASM_EMIT("vsub.f32    q6, q2, q13")                   /* q6   = ar2 - cr2 */ \
            __ASM_EMIT("vsub.f32    q7, q3, q15")                   /* q7   = ai2 - ci2 */ \
            __ASM_EMIT("vadd.f32    q0, q0, q12")                   /* q0   = ar1 + cr1 */ \
            __ASM_EMIT("vadd.f32    q1, q1, q14")                   /* q1   = ai1 + ci1 */ \
            __ASM_EMIT("vadd.f32    q2, q2, q13")                   /* q2   = ar2 + cr2 */ \
            __ASM_EMIT("vadd.f32    q3, q3, q15")                   /* q3   = ai2 + ci2 */ \
            __ASM_EMIT("vstm        %[b]!, {q4-q7}") \
            __ASM_EMIT("subs        %[pairs], $2") \
            __ASM_EMIT("vstm        %[a]!, {q0-q3}") \
            __ASM_EMIT("beq         4f") \
            /* Prepare next loop: rotate angle */ \
            __ASM_EMIT("vld1.32     {q0-q1}, [%[XFFT_W]]")          /* q0   = dr, q1 = di */ \
            __ASM_EMIT("vmul.f32    q12, q8, q1")                   /* q12  = wr1 * di */ \
            __ASM_EMIT("vmul.f32    q13, q9, q1")                   /* q13  = wr2 * di */ \
            __ASM_EMIT("vmul.f32    q14, q10, q1")                  /* q14  = wi1 * di */ \
            __ASM_EMIT("vmul.f32    q15, q11, q1")                  /* q15  = wi2 * di */ \
            __ASM_EMIT("vmul.f32    q8, q8, q0")                    /* q8   = wr1 * dr */ \
            __ASM_EMIT("vmul.f32    q9, q9, q0")                    /* q9   = wr2 * dr */ \
            __ASM_EMIT("vmul.f32    q10, q10, q0")                  /* q10  = wi1 * dr */ \
            __ASM_EMIT("vmul.f32    q11, q11, q0")                  /* q11  = wi2 * dr */ \
            __ASM_EMIT("vsub.f32    q8, q8, q14")                   /* q8   = wr1*dr - wi1*di */ \
            __ASM_EMIT("vsub.f32    q9, q9, q15")                   /* q9   = wr2*dr - wi2*di */ \
            __ASM_EMIT("vadd.f32    q10, q10, q12")                 /* q10  = wi1*dr + wr1*di */ \
            __ASM_EMIT("vadd.f32    q11, q11, q13")                 /* q11  = wi2*dr + wr2*di */ \
            __ASM_EMIT("b           3b") \
        __ASM_EMIT("4:") \
        __ASM_EMIT("subs        %[blocks], $1") \
        __ASM_EMIT("mov         %[a], %[b]") \
        __ASM_EMIT("bne         1b")

    void packed_direct_butterfly_rank4p(float *dst, size_t rank, size_t blocks) {
        IF_ARCH_ARM(
            rank -= 3;
            const float *xfft_a = &XFFT_A[rank << 4];
            const float *xfft_dw = &XFFT_DW[rank << 3];
            float *b;
            size_t pairs;
        )

        ARCH_ARM_ASM(
            PBUTTERFLY_RANK4("vmla.f32", "vmls.f32")
            : [a] "+r" (dst), [b] "=&r" (b),
              [blocks] "+r" (blocks), [pairs] "=&r" (pairs)
            : [XFFT_A] "r" (xfft_a), [XFFT_W] "r" (xfft_dw),
              [rank] "r" (rank)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void packed_reverse_butterfly_rank4p(float *dst, size_t rank, size_t blocks) {
        IF_ARCH_ARM(
            rank -= 3;
            const float *xfft_a = &XFFT_A[rank << 4];
            const float *xfft_dw = &XFFT_DW[rank << 3];
            float *b;
            size_t pairs;
        )

        ARCH_ARM_ASM(
            PBUTTERFLY_RANK4("vmls.f32", "vmla.f32")
            : [a] "+r" (dst), [b] "=&r" (b),
              [blocks] "+r" (blocks), [pairs] "=&r" (pairs)
            : [XFFT_A] "r" (xfft_a), [XFFT_W] "r" (xfft_dw),
              [rank] "r" (rank)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FFT_PBUTTERFLY_H_ */
