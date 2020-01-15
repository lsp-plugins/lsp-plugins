/*
 * msmatrix.h
 *
 *  Created on: 15 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_MSMATRIX_H_
#define DSP_ARCH_AARCH64_ASIMD_MSMATRIX_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count)
    {
        ARCH_AARCH64_ASM(
            __ASM_EMIT("ldp         q24, q25, [%[HALF]]")           // v24  = 0.5, v25 = 0.5
            // x16 blocks
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lo        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[l], #0x00]")         // v0   = l
            __ASM_EMIT("ldp         q2, q3, [%[l], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[r], #0x00]")         // v4   = r
            __ASM_EMIT("ldp         q6, q7, [%[r], #0x20]")
            __ASM_EMIT("fadd        v16.4s, v0.4s, v4.4s")          // v16  = l + r
            __ASM_EMIT("fadd        v17.4s, v1.4s, v5.4s")
            __ASM_EMIT("fadd        v18.4s, v2.4s, v6.4s")
            __ASM_EMIT("fadd        v19.4s, v3.4s, v7.4s")
            __ASM_EMIT("fsub        v20.4s, v0.4s, v4.4s")          // v20  = l - r
            __ASM_EMIT("fsub        v21.4s, v1.4s, v5.4s")
            __ASM_EMIT("fsub        v22.4s, v2.4s, v6.4s")
            __ASM_EMIT("fsub        v23.4s, v3.4s, v7.4s")
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")         // v0   = (l + r)/2
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmul        v2.4s, v18.4s, v24.4s")
            __ASM_EMIT("fmul        v3.4s, v19.4s, v25.4s")
            __ASM_EMIT("fmul        v4.4s, v20.4s, v24.4s")         // v4   = (l - r)/2
            __ASM_EMIT("fmul        v5.4s, v21.4s, v25.4s")
            __ASM_EMIT("fmul        v6.4s, v22.4s, v24.4s")
            __ASM_EMIT("fmul        v7.4s, v23.4s, v25.4s")
            __ASM_EMIT("stp         q0, q1, [%[m], #0x00]")
            __ASM_EMIT("stp         q2, q3, [%[m], #0x20]")
            __ASM_EMIT("stp         q4, q5, [%[s], #0x00]")
            __ASM_EMIT("stp         q6, q7, [%[s], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[l], %[l], #0x40")
            __ASM_EMIT("add         %[r], %[r], #0x40")
            __ASM_EMIT("add         %[m], %[m], #0x40")
            __ASM_EMIT("add         %[s], %[s], #0x40")
            __ASM_EMIT("b.hs        1b")
            __ASM_EMIT("2:")
            // x8 block
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q1, [%[l], #0x00]")         // v0   = l
            __ASM_EMIT("ldp         q4, q5, [%[r], #0x00]")         // v4   = r
            __ASM_EMIT("fadd        v16.4s, v0.4s, v4.4s")          // v16  = l + r
            __ASM_EMIT("fadd        v17.4s, v1.4s, v5.4s")
            __ASM_EMIT("fsub        v20.4s, v0.4s, v4.4s")          // v20  = l - r
            __ASM_EMIT("fsub        v21.4s, v1.4s, v5.4s")
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")         // v0   = (l + r)/2
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmul        v4.4s, v20.4s, v24.4s")         // v4   = (l - r)/2
            __ASM_EMIT("fmul        v5.4s, v21.4s, v25.4s")
            __ASM_EMIT("stp         q0, q1, [%[m], #0x00]")
            __ASM_EMIT("stp         q4, q5, [%[s], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[l], %[l], #0x20")
            __ASM_EMIT("add         %[r], %[r], #0x20")
            __ASM_EMIT("add         %[m], %[m], #0x20")
            __ASM_EMIT("add         %[s], %[s], #0x20")
            __ASM_EMIT("4:")
            // x4 block
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q0, [%[l]]")                    // v0   = l
            __ASM_EMIT("ldr         q4, [%[r]]")                    // v4   = r
            __ASM_EMIT("fadd        v16.4s, v0.4s, v4.4s")          // v16  = l + r
            __ASM_EMIT("fsub        v20.4s, v0.4s, v4.4s")          // v20  = l - r
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")         // v0   = (l + r)/2
            __ASM_EMIT("fmul        v4.4s, v20.4s, v24.4s")         // v4   = (l - r)/2
            __ASM_EMIT("str         q0, [%[m]]")
            __ASM_EMIT("str         q4, [%[s]]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[l], %[l], #0x10")
            __ASM_EMIT("add         %[r], %[r], #0x10")
            __ASM_EMIT("add         %[m], %[m], #0x10")
            __ASM_EMIT("add         %[s], %[s], #0x10")
            __ASM_EMIT("6:")
            // x1 blocks
            __ASM_EMIT("adds        %[count], %[count], #1")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v0.4s}, [%[l]]")               // v0   = l
            __ASM_EMIT("ld1r        {v4.4s}, [%[r]]")               // v4   = r
            __ASM_EMIT("fadd        v16.4s, v0.4s, v4.4s")          // v16  = l + r
            __ASM_EMIT("fsub        v20.4s, v0.4s, v4.4s")          // v20  = l - r
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")         // v0   = (l + r)/2
            __ASM_EMIT("fmul        v4.4s, v20.4s, v24.4s")         // v4   = (l - r)/2
            __ASM_EMIT("st1         {v0.s}[0], [%[m]]")
            __ASM_EMIT("st1         {v4.s}[0], [%[s]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[l], %[l], #0x04")
            __ASM_EMIT("add         %[r], %[r], #0x04")
            __ASM_EMIT("add         %[m], %[m], #0x04")
            __ASM_EMIT("add         %[s], %[s], #0x04")
            __ASM_EMIT("b.ge        7b")
            __ASM_EMIT("8:")

            : [l] "+r" (l), [r] "+r" (r), [m] "+r" (m), [s] "+r" (s),
              [count] "+r" (count)
            : [HALF] "r" (&X_HALF[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23",
              "q24", "q25"
        );
    }

    void ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count)
    {
        ARCH_AARCH64_ASM(
            // x16 blocks
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lo        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[m], #0x00]")         // v0   = m
            __ASM_EMIT("ldp         q2, q3, [%[m], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[s], #0x00]")         // v4   = s
            __ASM_EMIT("ldp         q6, q7, [%[s], #0x20]")
            __ASM_EMIT("fadd        v16.4s, v0.4s, v4.4s")          // v16  = m + s
            __ASM_EMIT("fadd        v17.4s, v1.4s, v5.4s")
            __ASM_EMIT("fadd        v18.4s, v2.4s, v6.4s")
            __ASM_EMIT("fadd        v19.4s, v3.4s, v7.4s")
            __ASM_EMIT("fsub        v20.4s, v0.4s, v4.4s")          // v20  = m - s
            __ASM_EMIT("fsub        v21.4s, v1.4s, v5.4s")
            __ASM_EMIT("fsub        v22.4s, v2.4s, v6.4s")
            __ASM_EMIT("fsub        v23.4s, v3.4s, v7.4s")
            __ASM_EMIT("stp         q16, q17, [%[l], #0x00]")
            __ASM_EMIT("stp         q18, q19, [%[l], #0x20]")
            __ASM_EMIT("stp         q20, q21, [%[r], #0x00]")
            __ASM_EMIT("stp         q22, q23, [%[r], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[l], %[l], #0x40")
            __ASM_EMIT("add         %[r], %[r], #0x40")
            __ASM_EMIT("add         %[m], %[m], #0x40")
            __ASM_EMIT("add         %[s], %[s], #0x40")
            __ASM_EMIT("b.hs        1b")
            __ASM_EMIT("2:")
            // x8 block
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q1, [%[m], #0x00]")         // v0   = m
            __ASM_EMIT("ldp         q4, q5, [%[s], #0x00]")         // v4   = s
            __ASM_EMIT("fadd        v16.4s, v0.4s, v4.4s")          // v16  = m + s
            __ASM_EMIT("fadd        v17.4s, v1.4s, v5.4s")
            __ASM_EMIT("fsub        v20.4s, v0.4s, v4.4s")          // v20  = m - s
            __ASM_EMIT("fsub        v21.4s, v1.4s, v5.4s")
            __ASM_EMIT("stp         q16, q17, [%[l], #0x00]")
            __ASM_EMIT("stp         q20, q21, [%[r], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[l], %[l], #0x20")
            __ASM_EMIT("add         %[r], %[r], #0x20")
            __ASM_EMIT("add         %[m], %[m], #0x20")
            __ASM_EMIT("add         %[s], %[s], #0x20")
            __ASM_EMIT("4:")
            // x4 block
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q0, [%[m]]")                    // v0   = m
            __ASM_EMIT("ldr         q4, [%[s]]")                    // v4   = s
            __ASM_EMIT("fadd        v16.4s, v0.4s, v4.4s")          // v16  = m + s
            __ASM_EMIT("fsub        v20.4s, v0.4s, v4.4s")          // v20  = m - s
            __ASM_EMIT("str         q16, [%[l]]")
            __ASM_EMIT("str         q20, [%[r]]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[l], %[l], #0x10")
            __ASM_EMIT("add         %[r], %[r], #0x10")
            __ASM_EMIT("add         %[m], %[m], #0x10")
            __ASM_EMIT("add         %[s], %[s], #0x10")
            __ASM_EMIT("6:")
            // x1 blocks
            __ASM_EMIT("adds        %[count], %[count], #1")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v0.4s}, [%[m]]")               // v0   = m
            __ASM_EMIT("ld1r        {v4.4s}, [%[s]]")               // v4   = s
            __ASM_EMIT("fadd        v16.4s, v0.4s, v4.4s")          // v16  = m + s
            __ASM_EMIT("fsub        v20.4s, v0.4s, v4.4s")          // v20  = m - s
            __ASM_EMIT("st1         {v16.s}[0], [%[l]]")
            __ASM_EMIT("st1         {v20.s}[0], [%[r]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[l], %[l], #0x04")
            __ASM_EMIT("add         %[r], %[r], #0x04")
            __ASM_EMIT("add         %[m], %[m], #0x04")
            __ASM_EMIT("add         %[s], %[s], #0x04")
            __ASM_EMIT("b.ge        7b")
            __ASM_EMIT("8:")

            : [l] "+r" (l), [r] "+r" (r), [m] "+r" (m), [s] "+r" (s),
              [count] "+r" (count)
            : [HALF] "r" (&X_HALF[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23"
        );
    }

    #define LR_TO_PART(OP) \
            __ASM_EMIT("ldp         q24, q25, [%[HALF]]")           /* v24  = 0.5, v25 = 0.5 */ \
            /* x16 blocks */ \
            __ASM_EMIT("subs        %[count], %[count], #16") \
            __ASM_EMIT("b.lo        2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("ldp         q0, q1, [%[l], #0x00]")         /* v0   = l */ \
            __ASM_EMIT("ldp         q2, q3, [%[l], #0x20]") \
            __ASM_EMIT("ldp         q4, q5, [%[r], #0x00]")         /* v4   = r */ \
            __ASM_EMIT("ldp         q6, q7, [%[r], #0x20]") \
            __ASM_EMIT(OP "         v16.4s, v0.4s, v4.4s")          /* v16  = l op r */ \
            __ASM_EMIT(OP "         v17.4s, v1.4s, v5.4s") \
            __ASM_EMIT(OP "         v18.4s, v2.4s, v6.4s") \
            __ASM_EMIT(OP "         v19.4s, v3.4s, v7.4s") \
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")         /* v0   = (l op r)/2 */ \
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s") \
            __ASM_EMIT("fmul        v2.4s, v18.4s, v24.4s") \
            __ASM_EMIT("fmul        v3.4s, v19.4s, v25.4s") \
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]") \
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]") \
            __ASM_EMIT("subs        %[count], %[count], #16") \
            __ASM_EMIT("add         %[l], %[l], #0x40") \
            __ASM_EMIT("add         %[r], %[r], #0x40") \
            __ASM_EMIT("add         %[dst], %[dst], #0x40") \
            __ASM_EMIT("b.hs        1b") \
            __ASM_EMIT("2:") \
            /* x8 block */ \
            __ASM_EMIT("adds        %[count], %[count], #8") \
            __ASM_EMIT("b.lt        4f") \
            __ASM_EMIT("ldp         q0, q1, [%[l], #0x00]")         /* v0   = l */ \
            __ASM_EMIT("ldp         q4, q5, [%[r], #0x00]")         /* v4   = r */ \
            __ASM_EMIT(OP "         v16.4s, v0.4s, v4.4s")          /* v16  = l op r */ \
            __ASM_EMIT(OP "         v17.4s, v1.4s, v5.4s") \
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")         /* v0   = (l op r)/2 */ \
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s") \
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]") \
            __ASM_EMIT("sub         %[count], %[count], #8") \
            __ASM_EMIT("add         %[l], %[l], #0x20") \
            __ASM_EMIT("add         %[r], %[r], #0x20") \
            __ASM_EMIT("add         %[dst], %[dst], #0x20") \
            __ASM_EMIT("4:") \
            /* x4 block */ \
            __ASM_EMIT("adds        %[count], %[count], #4") \
            __ASM_EMIT("b.lt        6f") \
            __ASM_EMIT("ldr         q0, [%[l]]")                    /* v0   = l */ \
            __ASM_EMIT("ldr         q4, [%[r]]")                    /* v4   = r */ \
            __ASM_EMIT(OP "         v16.4s, v0.4s, v4.4s")          /* v16  = l op r */ \
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")         /* v0   = (l op r)/2 */ \
            __ASM_EMIT("str         q0, [%[dst]]") \
            __ASM_EMIT("sub         %[count], %[count], #4") \
            __ASM_EMIT("add         %[l], %[l], #0x10") \
            __ASM_EMIT("add         %[r], %[r], #0x10") \
            __ASM_EMIT("add         %[dst], %[dst], #0x10") \
            __ASM_EMIT("6:") \
            /* x1 blocks */ \
            __ASM_EMIT("adds        %[count], %[count], #1") \
            __ASM_EMIT("b.lt        8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("ld1r        {v0.4s}, [%[l]]")               /* v0   = l */ \
            __ASM_EMIT("ld1r        {v4.4s}, [%[r]]")               /* v4   = r */ \
            __ASM_EMIT(OP "         v16.4s, v0.4s, v4.4s")          /* v16  = l op r */ \
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")         /* v0   = (l op r)/2 */ \
            __ASM_EMIT("st1         {v0.s}[0], [%[dst]]") \
            __ASM_EMIT("subs        %[count], %[count], #1") \
            __ASM_EMIT("add         %[l], %[l], #0x04") \
            __ASM_EMIT("add         %[r], %[r], #0x04") \
            __ASM_EMIT("add         %[dst], %[dst], #0x04") \
            __ASM_EMIT("b.ge        7b") \
            __ASM_EMIT("8:")

    void lr_to_mid(float *m, const float *l, const float *r, size_t count)
    {
        ARCH_AARCH64_ASM(
            LR_TO_PART("fadd")
            : [l] "+r" (l), [r] "+r" (r), [dst] "+r" (m),
              [count] "+r" (count)
            : [HALF] "r" (&X_HALF[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q24", "q25"
        );
    }

    void lr_to_side(float *s, const float *l, const float *r, size_t count)
    {
        ARCH_AARCH64_ASM(
            LR_TO_PART("fsub")
            : [l] "+r" (l), [r] "+r" (r), [dst] "+r" (s),
              [count] "+r" (count)
            : [HALF] "r" (&X_HALF[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q24", "q25"
        );
    }

    #undef LR_TO_PART

    #define MS_TO_PART(OP) \
        /* x16 blocks */ \
        __ASM_EMIT("subs        %[count], %[count], #16") \
        __ASM_EMIT("b.lo        2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp         q0, q1, [%[m], #0x00]")         /* v0   = m */ \
        __ASM_EMIT("ldp         q2, q3, [%[m], #0x20]") \
        __ASM_EMIT("ldp         q4, q5, [%[s], #0x00]")         /* v4   = s */ \
        __ASM_EMIT("ldp         q6, q7, [%[s], #0x20]") \
        __ASM_EMIT(OP "         v16.4s, v0.4s, v4.4s")          /* v16  = m op s */ \
        __ASM_EMIT(OP "         v17.4s, v1.4s, v5.4s") \
        __ASM_EMIT(OP "         v18.4s, v2.4s, v6.4s") \
        __ASM_EMIT(OP "         v19.4s, v3.4s, v7.4s") \
        __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]") \
        __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]") \
        __ASM_EMIT("subs        %[count], %[count], #16") \
        __ASM_EMIT("add         %[m], %[m], #0x40") \
        __ASM_EMIT("add         %[s], %[s], #0x40") \
        __ASM_EMIT("add         %[dst], %[dst], #0x40") \
        __ASM_EMIT("b.hs        1b") \
        __ASM_EMIT("2:") \
        /* x8 block */ \
        __ASM_EMIT("adds        %[count], %[count], #8") \
        __ASM_EMIT("b.lt        4f") \
        __ASM_EMIT("ldp         q0, q1, [%[m], #0x00]")         /* v0   = m */ \
        __ASM_EMIT("ldp         q4, q5, [%[s], #0x00]")         /* v4   = s */ \
        __ASM_EMIT(OP "         v16.4s, v0.4s, v4.4s")          /* v16  = m op s */ \
        __ASM_EMIT(OP "         v17.4s, v1.4s, v5.4s") \
        __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]") \
        __ASM_EMIT("sub         %[count], %[count], #8") \
        __ASM_EMIT("add         %[m], %[m], #0x20") \
        __ASM_EMIT("add         %[s], %[s], #0x20") \
        __ASM_EMIT("add         %[dst], %[dst], #0x20") \
        __ASM_EMIT("4:") \
        /* x4 block */ \
        __ASM_EMIT("adds        %[count], %[count], #4") \
        __ASM_EMIT("b.lt        6f") \
        __ASM_EMIT("ldr         q0, [%[m]]")                    /* v0   = m */ \
        __ASM_EMIT("ldr         q4, [%[s]]")                    /* v4   = s */ \
        __ASM_EMIT(OP "         v16.4s, v0.4s, v4.4s")          /* v16  = m op s */ \
        __ASM_EMIT("str         q0, [%[dst]]") \
        __ASM_EMIT("sub         %[count], %[count], #4") \
        __ASM_EMIT("add         %[m], %[m], #0x10") \
        __ASM_EMIT("add         %[s], %[s], #0x10") \
        __ASM_EMIT("add         %[dst], %[dst], #0x10") \
        __ASM_EMIT("6:") \
        /* x1 blocks */ \
        __ASM_EMIT("adds        %[count], %[count], #1") \
        __ASM_EMIT("b.lt        8f") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("ld1r        {v0.4s}, [%[m]]")               /* v0   = m */ \
        __ASM_EMIT("ld1r        {v4.4s}, [%[s]]")               /* v4   = s */ \
        __ASM_EMIT(OP "         v16.4s, v0.4s, v4.4s")          /* v16  = m op s */ \
        __ASM_EMIT("st1         {v0.s}[0], [%[dst]]") \
        __ASM_EMIT("subs        %[count], %[count], #1") \
        __ASM_EMIT("add         %[m], %[m], #0x04") \
        __ASM_EMIT("add         %[s], %[s], #0x04") \
        __ASM_EMIT("add         %[dst], %[dst], #0x04") \
        __ASM_EMIT("b.ge        7b") \
        __ASM_EMIT("8:")

    void ms_to_left(float *l, const float *m, const float *s, size_t count)
    {
        ARCH_AARCH64_ASM(
            MS_TO_PART("fadd")
            : [m] "+r" (m), [s] "+r" (s), [dst] "+r" (l),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19"
        );
    }

    void ms_to_right(float *r, const float *m, const float *s, size_t count)
    {
        ARCH_AARCH64_ASM(
            MS_TO_PART("fsub")
            : [m] "+r" (m), [s] "+r" (s), [dst] "+r" (r),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19"
        );
    }

    #undef MS_TO_PART

}

#endif /* DSP_ARCH_AARCH64_ASIMD_MSMATRIX_H_ */
