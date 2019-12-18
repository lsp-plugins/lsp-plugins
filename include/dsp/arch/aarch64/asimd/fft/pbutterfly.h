/*
 * pbutterfly.h
 *
 *  Created on: 18 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FFT_PBUTTERFLY_H_
#define DSP_ARCH_AARCH64_ASIMD_FFT_PBUTTERFLY_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    #define PBUTTERFLY_RANK3(op1, op2) \
        /* 8x butterflies */ \
        __ASM_EMIT("ldp         q28, q29, [%[XFFT_A], #0x00]")          /* q28  = wr1, q29 = wr2 */ \
        __ASM_EMIT("ldp         q30, q31, [%[XFFT_A], #0x20]")          /* q30  = wi1, q31 = wi2 */ \
        __ASM_EMIT("subs        %[blocks], %[blocks], #2") \
        __ASM_EMIT("b.lo        2f") \
        /* Calculate complex c = w * b */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp         q0, q4, [%[dst], #0x00]")               /* v0   = ar1, v4 = ai1 */ \
        __ASM_EMIT("ldp         q1, q5, [%[dst], #0x20]")               /* v1   = br1, v5 = bi1 */ \
        __ASM_EMIT("ldp         q2, q6, [%[dst], #0x40]")               /* v2   = ai2, v6 = ai2 */ \
        __ASM_EMIT("ldp         q3, q7, [%[dst], #0x60]")               /* v3   = bi2, v7 = bi2 */ \
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
        __ASM_EMIT("stp         q0, q4, [%[dst], #0x00]") \
        __ASM_EMIT("stp         q1, q5, [%[dst], #0x20]") \
        __ASM_EMIT("stp         q2, q6, [%[dst], #0x40]") \
        __ASM_EMIT("stp         q3, q7, [%[dst], #0x60]") \
        __ASM_EMIT("subs        %[blocks], %[blocks], #2") \
        __ASM_EMIT("add         %[dst], %[dst_re], #0x80") \
        __ASM_EMIT("b.ge        1b") \
        __ASM_EMIT("2:") \
        /* 4x butterflies */ \
        __ASM_EMIT("adds        %[blocks], %[blocks], #1") \
        __ASM_EMIT("b.lo        4f") \
        __ASM_EMIT("ldp         q0, q4, [%[dst], #0x00]")               /* v0   = ar1, v4 = ai1 */ \
        __ASM_EMIT("ldp         q1, q5, [%[dst], #0x20]")               /* v1   = br1, v5 = bi1 */ \
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
        __ASM_EMIT("stp         q0, q4, [%[dst], #0x00]") \
        __ASM_EMIT("stp         q1, q5, [%[dst], #0x20]") \
        __ASM_EMIT("4:")

    void packed_direct_butterfly_rank3(float *dst, size_t blocks)
    {
        ARCH_AARCH64_ASM(
            PBUTTERFLY_RANK3("fmla", "fmls")
            : [dst] "+r" (dst), [blocks] "+r" (blocks)
            : [XFFT_A] "r" (&XFFT_A[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v28", "v29", "v30", "v31"
        );
    }

    void packed_reverse_butterfly_rank3(float *dst, size_t blocks)
    {
        ARCH_AARCH64_ASM(
            PBUTTERFLY_RANK3("fmls", "fmla")
            : [dst] "+r" (dst), [src] "+r" (src),
              [blocks] "+r" (blocks)
            : [XFFT_A] "r" (&XFFT_A[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v28", "v29", "v30", "v31"
        );
    }

    #undef PBUTTERFLY_RANK3
}

#endif /* DSP_ARCH_AARCH64_ASIMD_FFT_PBUTTERFLY_H_ */
