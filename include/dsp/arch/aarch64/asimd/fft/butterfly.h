/*
 * butterfly.h
 *
 *  Created on: 17 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FFT_BUTTERFLY_H_
#define DSP_ARCH_AARCH64_ASIMD_FFT_BUTTERFLY_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    #define BUTTERFLY_RANK3(op1, op2) \
        /* 8x butterflies */ \
        __ASM_EMIT("ldp         q28, q29, [%[XFFT_A], #0x00]")          /* q28  = wr1, q29 = wr2 */ \
        __ASM_EMIT("ldp         q30, q31, [%[XFFT_A], #0x20]")          /* q30  = wi1, q31 = wi2 */ \
        __ASM_EMIT("subs        %[blocks], %[blocks], #2") \
        __ASM_EMIT("b.lo        2f") \
        /* Calculate complex c = w * b */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp         q0, q1, [%[dst_re], #0x00]")            /* v0   = ar1, v1 = br1 */ \
        __ASM_EMIT("ldp         q2, q3, [%[dst_re], #0x20]")            /* v2   = ar2, v3 = br2 */ \
        __ASM_EMIT("ldp         q4, q5, [%[dst_im], #0x00]")            /* v4   = ai1, v5 = bi1 */ \
        __ASM_EMIT("ldp         q6, q7, [%[dst_im], #0x20]")            /* v6   = ai2, v7 = bi2 */ \
        /* Calc cr and ci */ \
        __ASM_EMIT("fmul        v16.4s, v28.4s, v1.4s")                 /* v16  = wr1 * br1 */ \
        __ASM_EMIT("fmul        v17.4s, v29.4s, v3.4s")                 /* v17  = wr2 * br2 */ \
        __ASM_EMIT("fmul        v18.4s, v28.4s, v5.4s")                 /* v18  = wr1 * bi1 */ \
        __ASM_EMIT("fmul        v19.4s, v29.4s, v7.4s")                 /* v19  = wr2 * bi2 */ \
        __ASM_EMIT(op1 "        v16.4s, v30.4s, v5.4s")                 /* v16  = wr1 * br1 +- wi1 * bi1 = cr1 */ \
        __ASM_EMIT(op1 "        v17.4s, v31.4s, v7.4s")                 /* v17  = wr2 * br2 +- wi2 * bi2 = cr2 */ \
        __ASM_EMIT(op2 "        v18.4s, v30.4s, v1.4s")                 /* v18  = wr1 * bi1 -+ wi1 * br1 = ci1 */ \
        __ASM_EMIT(op2 "        v19.4s, v31.4s, v3.4s")                 /* v19  = wr1 * bi1 -+ wi2 * br2 = ci2 */ \
        /* Apply butterfly */ \
        __ASM_EMIT("fsub        v1.4s, v0.4s, v16.4s")                  /* v1   = ar1 - cr1 */ \
        __ASM_EMIT("fsub        v3.4s, v2.4s, v17.4s")                  /* v3   = ar2 - cr2 */ \
        __ASM_EMIT("fsub        v5.4s, v4.4s, v18.4s")                  /* v5   = ai1 - ci1 */ \
        __ASM_EMIT("fsub        v7.4s, v6.4s, v19.4s")                  /* v7   = ai2 - ci2 */ \
        __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")                  /* v0   = ar1 + cr1 */ \
        __ASM_EMIT("fadd        v2.4s, v2.4s, v17.4s")                  /* v2   = ar2 + cr2 */ \
        __ASM_EMIT("fadd        v4.4s, v4.4s, v18.4s")                  /* v4   = ai1 + ci1 */ \
        __ASM_EMIT("fadd        v6.4s, v6.4s, v19.4s")                  /* v6   = ai2 + ci2 */ \
        __ASM_EMIT("stp         q0, q1, [%[dst_re], #0x00]") \
        __ASM_EMIT("stp         q2, q3, [%[dst_re], #0x20]") \
        __ASM_EMIT("stp         q4, q5, [%[dst_im], #0x00]") \
        __ASM_EMIT("stp         q6, q7, [%[dst_im], #0x20]") \
        __ASM_EMIT("subs        %[blocks], %[blocks], #2") \
        __ASM_EMIT("add         %[dst_re], %[dst_re], #0x40") \
        __ASM_EMIT("add         %[dst_im], %[dst_im], #0x40") \
        __ASM_EMIT("b.ge        1b") \
        __ASM_EMIT("2:") \
        /* 4x butterflies */ \
        __ASM_EMIT("adds        %[blocks], %[blocks], #1") \
        __ASM_EMIT("b.lo        4f") \
        __ASM_EMIT("ldp         q0, q1, [%[dst_re], #0x00]")            /* v0   = ar1, v1 = br1 */ \
        __ASM_EMIT("ldp         q4, q5, [%[dst_im], #0x00]")            /* v4   = ai1, v5 = bi1 */ \
        /* Calc cr and ci */ \
        __ASM_EMIT("fmul        v16.4s, v28.4s, v1.4s")                  /* v16  = wr1 * br1 */ \
        __ASM_EMIT("fmul        v18.4s, v28.4s, v5.4s")                  /* v18  = wr1 * bi1 */ \
        __ASM_EMIT(op1 "        v16.4s, v30.4s, v5.4s")                  /* v16  = wr1 * br1 +- wi1 * bi1 = cr1 */ \
        __ASM_EMIT(op2 "        v18.4s, v30.4s, v1.4s")                  /* v18  = wr1 * bi1 -+ wi1 * br1 = ci1 */ \
        /* Apply butterfly */ \
        __ASM_EMIT("fsub        v1.4s, v0.4s, v16.4s")                   /* v1   = ar1 - cr1 */ \
        __ASM_EMIT("fsub        v5.4s, v4.4s, v18.4s")                   /* v5   = ai1 - ci1 */ \
        __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")                   /* v0   = ar1 + cr1 */ \
        __ASM_EMIT("fadd        v4.4s, v4.4s, v18.4s")                   /* v4   = ai1 + ci1 */ \
        __ASM_EMIT("stp         q0, q1, [%[dst_re], #0x00]") \
        __ASM_EMIT("stp         q4, q5, [%[dst_im], #0x00]") \
        __ASM_EMIT("4:")


    void direct_butterfly_rank3(float *dst_re, float *dst_im, size_t blocks)
    {
        ARCH_AARCH64_ASM(
            BUTTERFLY_RANK3("fmla", "fmls")
            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [blocks] "+r" (blocks)
            : [XFFT_A] "r" (&XFFT_A[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v28", "v29", "v30", "v31"
        );
    }

    void reverse_butterfly_rank3(float *dst_re, float *dst_im, size_t blocks)
    {
        ARCH_AARCH64_ASM(
            BUTTERFLY_RANK3("fmls", "fmla")
            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [blocks] "+r" (blocks)
            : [XFFT_A] "r" (&XFFT_A[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v28", "v29", "v30", "v31"
        );
    }

    #undef BUTTERFLY_RANK3

#define BUTTERFLY_RANK4(op1, op2) \
    __ASM_EMIT("1:") \
        /* Initialize sub-loop */ \
        __ASM_EMIT("mov         %[pairs], #1")                          /* pairs = 1 */ \
        __ASM_EMIT("ldp         q28, q29, [%[XFFT_A], #0x00]")          /* q28  = wr1, q29 = wr2 */ \
        __ASM_EMIT("ldp         q30, q31, [%[XFFT_A], #0x20]")          /* q30  = wi1, q31 = wi2 */ \
        __ASM_EMIT("ldp         q24, q25, [%[XFFT_W], #0x00]")          /* q24  = dr,  q25 = di  */ \
        __ASM_EMIT("lsl         %[pairs], %[pairs], %[rank]")           /* pairs = 1 << rank */ \
        __ASM_EMIT("add         %[b_re], %[a_re], %[pairs], LSL #4")    /* b_re = &a_re[pairs*8] */ \
        __ASM_EMIT("add         %[b_im], %[a_im], %[pairs], LSL #4")    /* b_im = &a_im[pairs*8] */ \
        /* 8x butterflies */ \
        /* Calculate complex c = w * b */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("ldp         q0, q1, [%[a_re], #0x00]")              /* v0   = ar1, v1 = ar2 */ \
        __ASM_EMIT("ldp         q2, q3, [%[a_im], #0x00]")              /* v2   = ai1, v3 = ai2 */ \
        __ASM_EMIT("ldp         q4, q5, [%[b_re], #0x00]")              /* v4   = br1, v5 = br2 */ \
        __ASM_EMIT("ldp         q6, q7, [%[b_im], #0x00]")              /* v6   = bi1, v7 = bi2 */ \
        /* Calc cr and ci */ \
        __ASM_EMIT("fmul        v16.4s, v28.4s, v4.4s")                 /* v16  = wr1 * br1 */ \
        __ASM_EMIT("fmul        v17.4s, v29.4s, v5.4s")                 /* v17  = wr2 * br2 */ \
        __ASM_EMIT("fmul        v18.4s, v28.4s, v6.4s")                 /* v18  = wr1 * bi1 */ \
        __ASM_EMIT("fmul        v19.4s, v29.4s, v7.4s")                 /* v19  = wr2 * bi2 */ \
        __ASM_EMIT(op1 "        v16.4s, v30.4s, v6.4s")                 /* v16  = wr1 * br1 +- wi1 * bi1 = cr1 */ \
        __ASM_EMIT(op1 "        v17.4s, v31.4s, v7.4s")                 /* v17  = wr2 * br2 +- wi2 * bi2 = cr2 */ \
        __ASM_EMIT(op2 "        v18.4s, v30.4s, v4.4s")                 /* v18  = wr1 * bi1 -+ wi1 * br1 = ci1 */ \
        __ASM_EMIT(op2 "        v19.4s, v31.4s, v5.4s")                 /* v19  = wr1 * bi1 -+ wi2 * br2 = ci2 */ \
        /* Apply butterfly */ \
        __ASM_EMIT("fsub        v4.4s, v0.4s, v16.4s")                  /* v4   = ar1 - cr1 */ \
        __ASM_EMIT("fsub        v5.4s, v1.4s, v17.4s")                  /* v5   = ar2 - cr2 */ \
        __ASM_EMIT("fsub        v6.4s, v2.4s, v18.4s")                  /* v6   = ai1 - ci1 */ \
        __ASM_EMIT("fsub        v7.4s, v3.4s, v19.4s")                  /* v7   = ai2 - ci2 */ \
        __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")                  /* v0   = ar1 + cr1 */ \
        __ASM_EMIT("fadd        v1.4s, v1.4s, v17.4s")                  /* v1   = ar2 + cr2 */ \
        __ASM_EMIT("fadd        v2.4s, v2.4s, v18.4s")                  /* v2   = ai1 + ci1 */ \
        __ASM_EMIT("fadd        v3.4s, v3.4s, v19.4s")                  /* v3   = ai2 + ci2 */ \
        __ASM_EMIT("stp         q0, q1, [%[a_re], #0x00]") \
        __ASM_EMIT("stp         q2, q3, [%[a_im], #0x00]") \
        __ASM_EMIT("stp         q4, q5, [%[b_re], #0x00]") \
        __ASM_EMIT("stp         q6, q7, [%[b_im], #0x00]") \
        __ASM_EMIT("subs        %[pairs], %[pairs], #2") \
        __ASM_EMIT("add         %[a_re], %[a_re], #0x20") \
        __ASM_EMIT("add         %[a_im], %[a_im], #0x20") \
        __ASM_EMIT("add         %[b_re], %[b_re], #0x20") \
        __ASM_EMIT("add         %[b_im], %[b_im], #0x20") \
        __ASM_EMIT("b.le        4f") \
        /* Prepare next loop: rotate angle */ \
        __ASM_EMIT("fmul        v16.4s, v28.4s, v25.4s")                /* v16  = wr1 * di */ \
        __ASM_EMIT("fmul        v17.4s, v29.4s, v25.4s")                /* v17  = wr2 * di */ \
        __ASM_EMIT("fmul        v18.4s, v30.4s, v25.4s")                /* v18  = wi1 * di */ \
        __ASM_EMIT("fmul        v19.4s, v31.4s, v25.4s")                /* v19  = wi2 * di */ \
        __ASM_EMIT("fmul        v28.4s, v28.4s, v24.4s")                /* v28  = wr1 * dr */ \
        __ASM_EMIT("fmul        v29.4s, v29.4s, v24.4s")                /* v29  = wr2 * dr */ \
        __ASM_EMIT("fmul        v30.4s, v30.4s, v24.4s")                /* v30  = wi1 * dr */ \
        __ASM_EMIT("fmul        v31.4s, v31.4s, v24.4s")                /* v31  = wi2 * dr */ \
        __ASM_EMIT("fsub        v28.4s, v28.4s, v18.4s")                /* v28  = wr1*dr - wi1*di */ \
        __ASM_EMIT("fsub        v29.4s, v29.4s, v19.4s")                /* v29  = wr2*dr - wi2*di */ \
        __ASM_EMIT("fadd        v30.4s, v30.4s, v16.4s")                /* v30  = wi1*dr + wr1*di */ \
        __ASM_EMIT("fadd        v31.4s, v31.4s, v17.4s")                /* v31  = wi2*dr + wr2*di */ \
        __ASM_EMIT("b           3b") \
    __ASM_EMIT("4:") \
    __ASM_EMIT("subs        %[blocks], %[blocks], #1") \
    __ASM_EMIT("mov         %[a_re], %[b_re]") \
    __ASM_EMIT("mov         %[a_im], %[b_im]") \
    __ASM_EMIT("b.gt        1b")

    void direct_butterfly_rank4p(float *dst_re, float *dst_im, size_t rank, size_t blocks)
    {
        IF_ARCH_AARCH64(
            rank -= 3;
            const float *xfft_a = &XFFT_A[rank << 4];
            const float *xfft_dw = &XFFT_DW[rank << 3];
            float *b_re, *b_im;
            size_t pairs;
        )

        ARCH_AARCH64_ASM(
            BUTTERFLY_RANK4("fmla", "fmls")
            : [a_re] "+r" (dst_re), [a_im] "+r" (dst_im),
              [b_re] "=&r" (b_re), [b_im] "=&r" (b_im),
              [blocks] "+r" (blocks), [pairs] "=&r" (pairs)
            : [XFFT_A] "r" (xfft_a), [XFFT_W] "r" (xfft_dw),
              [rank] "r" (rank)
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v24", "v25",
              "v28", "v29", "v30", "v31"
        );
    }

    void reverse_butterfly_rank4p(float *dst_re, float *dst_im, size_t rank, size_t blocks)
    {
        IF_ARCH_AARCH64(
            rank -= 3;
            const float *xfft_a = &XFFT_A[rank << 4];
            const float *xfft_dw = &XFFT_DW[rank << 3];
            float *b_re, *b_im;
            size_t pairs;
        )

        ARCH_AARCH64_ASM(
            BUTTERFLY_RANK4("fmls", "fmla")
            : [a_re] "+r" (dst_re), [a_im] "+r" (dst_im),
              [b_re] "=&r" (b_re), [b_im] "=&r" (b_im),
              [blocks] "+r" (blocks), [pairs] "=&r" (pairs)
            : [XFFT_A] "r" (xfft_a), [XFFT_W] "r" (xfft_dw),
              [rank] "r" (rank)
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v24", "v25",
              "v28", "v29", "v30", "v31"
        );
    }

#undef BUTTERFLY_RANK4
}

#endif /* DSP_ARCH_AARCH64_ASIMD_FFT_BUTTERFLY_H_ */
