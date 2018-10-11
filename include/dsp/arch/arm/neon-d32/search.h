/*
 * search.h
 *
 *  Created on: 9 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_SEARCH_H_
#define DSP_ARCH_ARM_NEON_D32_SEARCH_H_

namespace neon_d32
{
    #define MINMAX_SEARCH_CORE(op) \
        __ASM_EMIT("veor        q0, q0") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("blo         12f") \
        \
        __ASM_EMIT("vldm        %[src]!, {s0}") \
        __ASM_EMIT("vmov        s1, s0") \
        __ASM_EMIT("vmov        d1, d0") \
        __ASM_EMIT("vmov        q1, q0") \
        __ASM_EMIT("subs        %[count], $56") \
        __ASM_EMIT("blo         2f") \
        /* x56 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src]!, {q2-q7}") \
        __ASM_EMIT(op "         q0, q0, q2") \
        __ASM_EMIT(op "         q1, q1, q3") \
        __ASM_EMIT(op "         q0, q0, q4") \
        __ASM_EMIT(op "         q1, q1, q5") \
        __ASM_EMIT("subs        %[count], $56") \
        __ASM_EMIT("vldm        %[src]!, {q8-q15}") \
        __ASM_EMIT(op "         q0, q0, q6") \
        __ASM_EMIT(op "         q1, q1, q7") \
        __ASM_EMIT(op "         q0, q0, q8") \
        __ASM_EMIT(op "         q1, q1, q9") \
        __ASM_EMIT(op "         q0, q0, q10") \
        __ASM_EMIT(op "         q1, q1, q11") \
        __ASM_EMIT(op "         q0, q0, q12") \
        __ASM_EMIT(op "         q1, q1, q13") \
        __ASM_EMIT(op "         q0, q0, q14") \
        __ASM_EMIT(op "         q1, q1, q15") \
        __ASM_EMIT("bhs         1b") \
        /* x32 block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $24") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src]!, {q8-q15}") \
        __ASM_EMIT("sub         %[count], $32") \
        __ASM_EMIT(op "         q0, q0, q8") \
        __ASM_EMIT(op "         q1, q1, q9") \
        __ASM_EMIT(op "         q0, q0, q10") \
        __ASM_EMIT(op "         q1, q1, q11") \
        __ASM_EMIT(op "         q0, q0, q12") \
        __ASM_EMIT(op "         q1, q1, q13") \
        __ASM_EMIT(op "         q0, q0, q14") \
        __ASM_EMIT(op "         q1, q1, q15") \
        /* x16 block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $16") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[src]!, {q2-q5}") \
        __ASM_EMIT("sub         %[count], $16") \
        __ASM_EMIT(op "         q0, q0, q2") \
        __ASM_EMIT(op "         q1, q1, q3") \
        __ASM_EMIT(op "         q0, q0, q4") \
        __ASM_EMIT(op "         q1, q1, q5") \
        /* x8 block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $8") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vldm        %[src]!, {q2-q3}") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT(op "         q0, q0, q2") \
        __ASM_EMIT(op "         q1, q1, q3") \
        /* x4 block */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         10f") \
        __ASM_EMIT("vldm        %[src]!, {q2}") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT(op "         q0, q0, q2") \
        /* x1 block */ \
        __ASM_EMIT("10:") \
        __ASM_EMIT(op "         q0, q0, q1") \
        __ASM_EMIT(op "         d0, d0, d1") \
        __ASM_EMIT("vmov        s2, s1") \
        __ASM_EMIT(op "         d0, d0, d1") \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         12f") \
        __ASM_EMIT("11:") \
        __ASM_EMIT("vldm        %[src]!, {s2}") \
        __ASM_EMIT(op "         d0, d0, d1")  \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("bge         11b") \
        \
        __ASM_EMIT("vmov        %[res], s0") \
        __ASM_EMIT("12:")

    float min(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_ARM_ASM(
            MINMAX_SEARCH_CORE("vmin.f32")
            : [count] "+r" (count), [src] "+r" (src),
              [res] "+t" (res)
            :
            : "cc",
              "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
        return res;
    }

    float max(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_ARM_ASM(
            MINMAX_SEARCH_CORE("vmax.f32")
            : [count] "+r" (count), [src] "+r" (src),
              [res] "+t" (res)
            :
            : "cc",
              "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
        return res;
    }

    #undef MINMAX_SEARCH_CORE

    #define ABS_MINMAX_SEARCH_CORE(op) \
        __ASM_EMIT("veor        q0, q0") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("blo         12f") \
        \
        __ASM_EMIT("vldm        %[src]!, {s0}") \
        __ASM_EMIT("vmov        s1, s0") \
        __ASM_EMIT("vmov        d1, d0") \
        __ASM_EMIT("vabs.f32    q0, q0") \
        __ASM_EMIT("subs        %[count], $56") \
        __ASM_EMIT("vmov        q1, q0") \
        __ASM_EMIT("blo         2f") \
        /* x56 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src]!, {q2-q7}") \
        __ASM_EMIT("vabs.f32    q2, q2") \
        __ASM_EMIT("vabs.f32    q3, q3") \
        __ASM_EMIT("vabs.f32    q4, q4") \
        __ASM_EMIT("vabs.f32    q5, q5") \
        __ASM_EMIT(op "         q0, q0, q2") \
        __ASM_EMIT(op "         q1, q1, q3") \
        __ASM_EMIT(op "         q0, q0, q4") \
        __ASM_EMIT(op "         q1, q1, q5") \
        __ASM_EMIT("subs        %[count], $56") \
        __ASM_EMIT("vldm        %[src]!, {q8-q15}") \
        __ASM_EMIT("vabs.f32    q6, q6") \
        __ASM_EMIT("vabs.f32    q7, q7") \
        __ASM_EMIT("vabs.f32    q8, q8") \
        __ASM_EMIT("vabs.f32    q9, q9") \
        __ASM_EMIT("vabs.f32    q10, q10") \
        __ASM_EMIT("vabs.f32    q11, q11") \
        __ASM_EMIT("vabs.f32    q12, q12") \
        __ASM_EMIT("vabs.f32    q13, q13") \
        __ASM_EMIT("vabs.f32    q14, q14") \
        __ASM_EMIT("vabs.f32    q15, q15") \
        __ASM_EMIT(op "         q0, q0, q6") \
        __ASM_EMIT(op "         q1, q1, q7") \
        __ASM_EMIT(op "         q0, q0, q8") \
        __ASM_EMIT(op "         q1, q1, q9") \
        __ASM_EMIT(op "         q0, q0, q10") \
        __ASM_EMIT(op "         q1, q1, q11") \
        __ASM_EMIT(op "         q0, q0, q12") \
        __ASM_EMIT(op "         q1, q1, q13") \
        __ASM_EMIT(op "         q0, q0, q14") \
        __ASM_EMIT(op "         q1, q1, q15") \
        __ASM_EMIT("bhs         1b") \
        /* x32 block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $24") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src]!, {q8-q15}") \
        __ASM_EMIT("sub         %[count], $32") \
        __ASM_EMIT("vabs.f32    q8, q8") \
        __ASM_EMIT("vabs.f32    q9, q9") \
        __ASM_EMIT("vabs.f32    q10, q10") \
        __ASM_EMIT("vabs.f32    q11, q11") \
        __ASM_EMIT("vabs.f32    q12, q12") \
        __ASM_EMIT("vabs.f32    q13, q13") \
        __ASM_EMIT("vabs.f32    q14, q14") \
        __ASM_EMIT("vabs.f32    q15, q15") \
        __ASM_EMIT(op "         q0, q0, q8") \
        __ASM_EMIT(op "         q1, q1, q9") \
        __ASM_EMIT(op "         q0, q0, q10") \
        __ASM_EMIT(op "         q1, q1, q11") \
        __ASM_EMIT(op "         q0, q0, q12") \
        __ASM_EMIT(op "         q1, q1, q13") \
        __ASM_EMIT(op "         q0, q0, q14") \
        __ASM_EMIT(op "         q1, q1, q15") \
        /* x16 block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $16") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[src]!, {q2-q5}") \
        __ASM_EMIT("sub         %[count], $16") \
        __ASM_EMIT("vabs.f32    q2, q2") \
        __ASM_EMIT("vabs.f32    q3, q3") \
        __ASM_EMIT("vabs.f32    q4, q4") \
        __ASM_EMIT("vabs.f32    q5, q5") \
        __ASM_EMIT(op "         q0, q0, q2") \
        __ASM_EMIT(op "         q1, q1, q3") \
        __ASM_EMIT(op "         q0, q0, q4") \
        __ASM_EMIT(op "         q1, q1, q5") \
        /* x8 block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $8") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vldm        %[src]!, {q2-q3}") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("vabs.f32    q2, q2") \
        __ASM_EMIT("vabs.f32    q3, q3") \
        __ASM_EMIT(op "         q0, q0, q2") \
        __ASM_EMIT(op "         q1, q1, q3") \
        /* x4 block */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         10f") \
        __ASM_EMIT("vldm        %[src]!, {q2}") \
        __ASM_EMIT("vabs.f32    q2, q2") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT(op "         q0, q0, q2") \
        /* x1 block */ \
        __ASM_EMIT("10:") \
        __ASM_EMIT(op "         q0, q0, q1") \
        __ASM_EMIT(op "         d0, d0, d1") \
        __ASM_EMIT("vmov        s2, s1") \
        __ASM_EMIT(op "         d0, d0, d1") \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         12f") \
        __ASM_EMIT("11:") \
        __ASM_EMIT("vldm        %[src]!, {s2}") \
        __ASM_EMIT("vabs.f32    d1, d1")  \
        __ASM_EMIT(op "         d0, d0, d1")  \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("bge         11b") \
        \
        __ASM_EMIT("vmov        %[res], s0") \
        __ASM_EMIT("12:")

    float abs_min(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_ARM_ASM(
            ABS_MINMAX_SEARCH_CORE("vmin.f32")
            : [count] "+r" (count), [src] "+r" (src),
              [res] "+t" (res)
            :
            : "cc",
              "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
        return res;
    }

    float abs_max(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_ARM_ASM(
                ABS_MINMAX_SEARCH_CORE("vmax.f32")
            : [count] "+r" (count), [src] "+r" (src),
              [res] "+t" (res)
            :
            : "cc",
              "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
        return res;
    }

    #undef ABS_MINMAX_SEARCH_CORE

    void minmax(const float *src, size_t count, float *min, float *max)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("veor        q0, q0")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("blo         12f")

            __ASM_EMIT("vldm        %[src]!, {s0}")
            __ASM_EMIT("vmov        s1, s0")
            __ASM_EMIT("vmov        d1, d0")
            __ASM_EMIT("vmov        q1, q0")
            __ASM_EMIT("vmov        q2, q0")
            __ASM_EMIT("vmov        q3, q1")
            __ASM_EMIT("subs        %[count], $48")
            __ASM_EMIT("blo         2f")
            // x48 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[src]!, {q4-q9}")
            __ASM_EMIT("vmin.f32    q0, q0, q4")
            __ASM_EMIT("vmin.f32    q1, q1, q5")
            __ASM_EMIT("vmin.f32    q0, q0, q6")
            __ASM_EMIT("vmin.f32    q1, q1, q7")
            __ASM_EMIT("vmin.f32    q0, q0, q8")
            __ASM_EMIT("vmin.f32    q1, q1, q9")
            __ASM_EMIT("subs        %[count], $48")
            __ASM_EMIT("vldm        %[src]!, {q10-q15}")
            __ASM_EMIT("vmax.f32    q2, q2, q4")
            __ASM_EMIT("vmax.f32    q3, q3, q5")
            __ASM_EMIT("vmax.f32    q2, q2, q6")
            __ASM_EMIT("vmax.f32    q3, q3, q7")
            __ASM_EMIT("vmax.f32    q2, q2, q8")
            __ASM_EMIT("vmax.f32    q3, q3, q9")
            __ASM_EMIT("vmin.f32    q0, q0, q10")
            __ASM_EMIT("vmin.f32    q1, q1, q11")
            __ASM_EMIT("vmin.f32    q0, q0, q12")
            __ASM_EMIT("vmin.f32    q1, q1, q13")
            __ASM_EMIT("vmin.f32    q0, q0, q14")
            __ASM_EMIT("vmin.f32    q1, q1, q15")
            __ASM_EMIT("vmax.f32    q2, q2, q10")
            __ASM_EMIT("vmax.f32    q3, q3, q11")
            __ASM_EMIT("vmax.f32    q2, q2, q12")
            __ASM_EMIT("vmax.f32    q3, q3, q13")
            __ASM_EMIT("vmax.f32    q2, q2, q14")
            __ASM_EMIT("vmax.f32    q3, q3, q15")
            __ASM_EMIT("bhs         1b")
            // x32 block
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], $16")     // 48 - 32
            __ASM_EMIT("blt         4f") \
            __ASM_EMIT("vldm        %[src]!, {q8-q15}") \
            __ASM_EMIT("sub         %[count], $32") \
            __ASM_EMIT("vmin.f32    q0, q0, q8")
            __ASM_EMIT("vmin.f32    q1, q1, q9")
            __ASM_EMIT("vmin.f32    q0, q0, q10")
            __ASM_EMIT("vmin.f32    q1, q1, q11")
            __ASM_EMIT("vmin.f32    q0, q0, q12")
            __ASM_EMIT("vmin.f32    q1, q1, q13")
            __ASM_EMIT("vmin.f32    q0, q0, q14")
            __ASM_EMIT("vmin.f32    q1, q1, q15")
            __ASM_EMIT("vmax.f32    q2, q2, q8")
            __ASM_EMIT("vmax.f32    q3, q3, q9")
            __ASM_EMIT("vmax.f32    q2, q2, q10")
            __ASM_EMIT("vmax.f32    q3, q3, q11")
            __ASM_EMIT("vmax.f32    q2, q2, q12")
            __ASM_EMIT("vmax.f32    q3, q3, q13")
            __ASM_EMIT("vmax.f32    q2, q2, q14")
            __ASM_EMIT("vmax.f32    q3, q3, q15")
            // x16 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $16")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[src]!, {q8-q11}")
            __ASM_EMIT("sub         %[count], $16")
            __ASM_EMIT("vmin.f32    q0, q0, q8")
            __ASM_EMIT("vmin.f32    q1, q1, q9")
            __ASM_EMIT("vmin.f32    q0, q0, q10")
            __ASM_EMIT("vmin.f32    q1, q1, q11")
            __ASM_EMIT("vmax.f32    q2, q2, q8")
            __ASM_EMIT("vmax.f32    q3, q3, q9")
            __ASM_EMIT("vmax.f32    q2, q2, q10")
            __ASM_EMIT("vmax.f32    q3, q3, q11")
            // x8 block
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vldm        %[src]!, {q8-q9}")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vmin.f32    q0, q0, q8")
            __ASM_EMIT("vmin.f32    q1, q1, q9")
            __ASM_EMIT("vmax.f32    q2, q2, q8")
            __ASM_EMIT("vmax.f32    q3, q3, q9")
            // x4 block
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("vldm        %[src]!, {q8}")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vmin.f32    q0, q0, q8")
            __ASM_EMIT("vmax.f32    q2, q2, q8")
            // x1 block
            __ASM_EMIT("10:")
            __ASM_EMIT("vmin.f32    q0, q0, q1")
            __ASM_EMIT("vmax.f32    q2, q2, q3")
            __ASM_EMIT("vmax.f32    d4, d4, d5")
            __ASM_EMIT("vmin.f32    d0, d0, d1")
            __ASM_EMIT("vmov        s2, s1")
            __ASM_EMIT("vmov        s10, s9")
            __ASM_EMIT("vmin.f32    d0, d0, d1")
            __ASM_EMIT("vmax.f32    d4, d4, d5")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         12f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vldm        %[src]!, {s2}")
            __ASM_EMIT("vmin.f32    d0, d0, d1")
            __ASM_EMIT("vmax.f32    d4, d4, d1")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bge         11b")

            __ASM_EMIT("12:")
            __ASM_EMIT("vstm        %[min], {s0}")
            __ASM_EMIT("vstm        %[max], {s8}")
            : [count] "+r" (count), [src] "+r" (src)
            : [min] "r" (min), [max] "r" (max)
            : "cc",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void abs_minmax(const float *src, size_t count, float *min, float *max)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("veor        q0, q0")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("blo         12f")

            __ASM_EMIT("vldm        %[src]!, {s0}")
            __ASM_EMIT("vmov        s1, s0")
            __ASM_EMIT("vmov        d1, d0")
            __ASM_EMIT("vabs.f32    q0, q0")
            __ASM_EMIT("vmov        q1, q0")
            __ASM_EMIT("vmov        q2, q0")
            __ASM_EMIT("vmov        q3, q1")
            __ASM_EMIT("subs        %[count], $48")
            __ASM_EMIT("blo         2f")
            // x48 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[src]!, {q4-q9}")
            __ASM_EMIT("vabs.f32    q4, q4")
            __ASM_EMIT("vabs.f32    q5, q5")
            __ASM_EMIT("vabs.f32    q6, q6")
            __ASM_EMIT("vabs.f32    q7, q7")
            __ASM_EMIT("vabs.f32    q8, q8")
            __ASM_EMIT("vabs.f32    q9, q9")
            __ASM_EMIT("vmin.f32    q0, q0, q4")
            __ASM_EMIT("vmin.f32    q1, q1, q5")
            __ASM_EMIT("vmin.f32    q0, q0, q6")
            __ASM_EMIT("vmin.f32    q1, q1, q7")
            __ASM_EMIT("vmin.f32    q0, q0, q8")
            __ASM_EMIT("vmin.f32    q1, q1, q9")
            __ASM_EMIT("subs        %[count], $48")
            __ASM_EMIT("vldm        %[src]!, {q10-q15}")
            __ASM_EMIT("vmax.f32    q2, q2, q4")
            __ASM_EMIT("vmax.f32    q3, q3, q5")
            __ASM_EMIT("vmax.f32    q2, q2, q6")
            __ASM_EMIT("vmax.f32    q3, q3, q7")
            __ASM_EMIT("vmax.f32    q2, q2, q8")
            __ASM_EMIT("vmax.f32    q3, q3, q9")
            __ASM_EMIT("vabs.f32    q10, q10")
            __ASM_EMIT("vabs.f32    q11, q11")
            __ASM_EMIT("vabs.f32    q12, q12")
            __ASM_EMIT("vabs.f32    q13, q13")
            __ASM_EMIT("vabs.f32    q14, q14")
            __ASM_EMIT("vabs.f32    q15, q15")
            __ASM_EMIT("vmin.f32    q0, q0, q10")
            __ASM_EMIT("vmin.f32    q1, q1, q11")
            __ASM_EMIT("vmin.f32    q0, q0, q12")
            __ASM_EMIT("vmin.f32    q1, q1, q13")
            __ASM_EMIT("vmin.f32    q0, q0, q14")
            __ASM_EMIT("vmin.f32    q1, q1, q15")
            __ASM_EMIT("vmax.f32    q2, q2, q10")
            __ASM_EMIT("vmax.f32    q3, q3, q11")
            __ASM_EMIT("vmax.f32    q2, q2, q12")
            __ASM_EMIT("vmax.f32    q3, q3, q13")
            __ASM_EMIT("vmax.f32    q2, q2, q14")
            __ASM_EMIT("vmax.f32    q3, q3, q15")
            __ASM_EMIT("bhs         1b")
            // x32 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $16")     // 48 - 32
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[src]!, {q8-q15}")
            __ASM_EMIT("sub         %[count], $32")
            __ASM_EMIT("vabs.f32    q8, q8")
            __ASM_EMIT("vabs.f32    q9, q9")
            __ASM_EMIT("vabs.f32    q10, q10")
            __ASM_EMIT("vabs.f32    q11, q11")
            __ASM_EMIT("vabs.f32    q12, q12")
            __ASM_EMIT("vabs.f32    q13, q13")
            __ASM_EMIT("vabs.f32    q14, q14")
            __ASM_EMIT("vabs.f32    q15, q15")
            __ASM_EMIT("vmin.f32    q0, q0, q10")
            __ASM_EMIT("vmin.f32    q1, q1, q11")
            __ASM_EMIT("vmin.f32    q0, q0, q12")
            __ASM_EMIT("vmin.f32    q1, q1, q13")
            __ASM_EMIT("vmin.f32    q0, q0, q14")
            __ASM_EMIT("vmin.f32    q1, q1, q15")
            __ASM_EMIT("vmax.f32    q2, q2, q8")
            __ASM_EMIT("vmax.f32    q3, q3, q9")
            __ASM_EMIT("vmax.f32    q2, q2, q10")
            __ASM_EMIT("vmax.f32    q3, q3, q11")
            __ASM_EMIT("vmax.f32    q2, q2, q12")
            __ASM_EMIT("vmax.f32    q3, q3, q13")
            __ASM_EMIT("vmax.f32    q2, q2, q14")
            __ASM_EMIT("vmax.f32    q3, q3, q15")
            // x16 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $16")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[src]!, {q8-q11}")
            __ASM_EMIT("sub         %[count], $16")
            __ASM_EMIT("vabs.f32    q8, q8")
            __ASM_EMIT("vabs.f32    q9, q9")
            __ASM_EMIT("vabs.f32    q10, q10")
            __ASM_EMIT("vabs.f32    q11, q11")
            __ASM_EMIT("vmin.f32    q0, q0, q8")
            __ASM_EMIT("vmin.f32    q1, q1, q9")
            __ASM_EMIT("vmin.f32    q0, q0, q10")
            __ASM_EMIT("vmin.f32    q1, q1, q11")
            __ASM_EMIT("vmax.f32    q2, q2, q8")
            __ASM_EMIT("vmax.f32    q3, q3, q9")
            __ASM_EMIT("vmax.f32    q2, q2, q10")
            __ASM_EMIT("vmax.f32    q3, q3, q11")
            // x8 block
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vldm        %[src]!, {q8-q9}")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vabs.f32    q8, q8")
            __ASM_EMIT("vabs.f32    q9, q9")
            __ASM_EMIT("vmin.f32    q0, q0, q8")
            __ASM_EMIT("vmin.f32    q1, q1, q9")
            __ASM_EMIT("vmax.f32    q2, q2, q8")
            __ASM_EMIT("vmax.f32    q3, q3, q9")
            // x4 block
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("vldm        %[src]!, {q8}")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vabs.f32    q8, q8")
            __ASM_EMIT("vmin.f32    q0, q0, q8")
            __ASM_EMIT("vmax.f32    q2, q2, q8")
            // x1 block
            __ASM_EMIT("10:")
            __ASM_EMIT("vmin.f32    q0, q0, q1")
            __ASM_EMIT("vmax.f32    q2, q2, q3")
            __ASM_EMIT("vmax.f32    d4, d4, d5")
            __ASM_EMIT("vmin.f32    d0, d0, d1")
            __ASM_EMIT("vmov        s2, s1")
            __ASM_EMIT("vmov        s10, s9")
            __ASM_EMIT("vmin.f32    d0, d0, d1")
            __ASM_EMIT("vmax.f32    d4, d4, d5")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         12f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vldm        %[src]!, {s2}")
            __ASM_EMIT("vabs.f32    d1, d1")
            __ASM_EMIT("vmin.f32    d0, d0, d1")
            __ASM_EMIT("vmax.f32    d4, d4, d1")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bge         11b")

            __ASM_EMIT("12:")
            __ASM_EMIT("vstm        %[min], {s0}")
            __ASM_EMIT("vstm        %[max], {s8}")
            : [count] "+r" (count), [src] "+r" (src)
            : [min] "r" (min), [max] "r" (max)
            : "cc",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    IF_ARCH_ARM(
        static uint32_t indexes[] __lsp_aligned16 =
        {
            0, 1, 2, 3, 4, 5, 6, 7,
            8, 8, 8, 8, 8, 8, 8, 8,
            4, 4, 4, 4,
            1, 1, 1, 1
        };
    );

    #define IDX_COND_SEARCH(keep_op) \
        __ASM_EMIT("veor        q0, q0")                /* q0 = idx0 = 0 */ \
        __ASM_EMIT("cmp         %[count], $0") \
        __ASM_EMIT("beq         6f") \
        \
        __ASM_EMIT("vldm        %[src], {s8}") \
        __ASM_EMIT("vmov        s9, s8") \
        __ASM_EMIT("vld1.32     {q4-q5}, [%[IDXS]]!")   /* q4-q5 = { nidx0, nidx1 } */ \
        __ASM_EMIT("vmov        d5, d4")                /* q2 = val0 */ \
        __ASM_EMIT("vldm        %[IDXS], {q12-q15}")    /* q12-q15 = { incr0, incr1, incr2, incr3} */ \
        __ASM_EMIT("subs        %[count], $8") \
        __ASM_EMIT("bls         2f") \
        \
        /* 8x blocks */ \
        __ASM_EMIT("veor        q1, q1")                /* q0 = idx1 = 0 */ \
        __ASM_EMIT("vmov        q3, q2")                /* q3 = val1 */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vld1.32     {q6-q7}, [%[src]]!")    /* q6-q7 = { sample0, sample1 } */ \
        __ASM_EMIT(keep_op "    q8, q2, q6")            /* q8 = val0 <=> sample0 */ \
        __ASM_EMIT(keep_op "    q9, q3, q7") \
        __ASM_EMIT("vmvn        q10, q8")               /* q10 = !(val0 <=> sample0) */ \
        __ASM_EMIT("vmvn        q11, q9") \
        __ASM_EMIT("vand        q0, q0, q8")            /* q0 = idx0 & (val0 <=> sample0) */ \
        __ASM_EMIT("vand        q1, q1, q9") \
        __ASM_EMIT("vand        q2, q2, q8")            /* q2 = val0 & (val0 <=> sample0) */ \
        __ASM_EMIT("vand        q3, q3, q9") \
        __ASM_EMIT("vand        q6, q6, q10")           /* q6 = sample0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vand        q7, q7, q11") \
        __ASM_EMIT("vand        q8, q4, q10")           /* q8 = nidx0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vand        q9, q5, q11") \
        __ASM_EMIT("vorr        q2, q6")                /* q2 = val0 & (val0 <=> sample0) | sample0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vorr        q3, q7") \
        __ASM_EMIT("vorr        q0, q8")                /* q0 = idx0 & (val0 <=> sample0) | nidx0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vorr        q1, q9") \
        __ASM_EMIT("vadd.u32    q4, q4, q12")           /* q4 = nidx0 + 8 */ \
        __ASM_EMIT("vadd.u32    q5, q5, q13") \
        __ASM_EMIT("subs        %[count], $8")          /* count -= 8 */ \
        __ASM_EMIT("bhs         1b") \
        /* 8x post-process q6 -> q3, q4 -> q1 */ \
        __ASM_EMIT(keep_op "    q8, q2, q3")            /* q8 = val0 <=> val1 */ \
        __ASM_EMIT("vmvn        q10, q8")               /* q10 = !(val0 <=> val1) */ \
        __ASM_EMIT("vand        q0, q0, q8")            /* q0 = idx0 & (val0 <=> val1) */ \
        __ASM_EMIT("vand        q2, q2, q8")            /* q2 = val0 & (val0 <=> val1) */ \
        __ASM_EMIT("vand        q3, q3, q10")           /* q3 = val1 & !(val0 <=> val1) */ \
        __ASM_EMIT("vand        q8, q1, q10")           /* q8 = idx1 & !(val0 <=> val1) */ \
        __ASM_EMIT("vorr        q2, q3")                /* q2 = val0 & (val0 <=> val1) | val1 & !(val0 <=> val1) */ \
        __ASM_EMIT("vorr        q0, q8")                /* q0 = idx0 & (val0 <=> val1) | idx1 & !(val0 <=> val1) */ \
        /* 4x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vld1.32     {q6}, [%[src]]!")       /* q6 = sample0 */ \
        __ASM_EMIT(keep_op "    q8, q2, q6")            /* q8 = val0 <=> sample0 */ \
        __ASM_EMIT("vmvn        q10, q8")               /* q10 = !(val0 <=> sample0) */ \
        __ASM_EMIT("vand        q0, q0, q8")            /* q0 = idx0 & (val0 <=> sample0) */ \
        __ASM_EMIT("vand        q2, q2, q8")            /* q2 = val0 & (val0 <=> sample0) */ \
        __ASM_EMIT("vand        q6, q6, q10")           /* q6 = sample0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vand        q8, q4, q10")           /* q8 = nidx0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vorr        q2, q6")                /* q2 = val0 & (val0 <=> sample0) | sample0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vorr        q0, q8")                /* q0 = idx0 & (val0 <=> sample0) | nidx0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vadd.u32    q4, q4, q14")           /* q4 = nidx0 + 4 */ \
        __ASM_EMIT("sub         %[count], $4")          /* count -= 8 */ \
        __ASM_EMIT("4:") \
        /* 4x post-process, step 1 */ \
        __ASM_EMIT("vmov        q5, q4") \
        __ASM_EMIT("vmov        d8, d1") \
        __ASM_EMIT("vmov        d12, d5") \
        __ASM_EMIT(keep_op "    q8, q2, q6") \
        __ASM_EMIT("vmvn        q10, q8")  \
        __ASM_EMIT("vand        q0, q0, q8") \
        __ASM_EMIT("vand        q2, q2, q8") \
        __ASM_EMIT("vand        q6, q6, q10") \
        __ASM_EMIT("vand        q8, q4, q10") \
        __ASM_EMIT("vorr        q2, q6") \
        __ASM_EMIT("vorr        q0, q8") \
        /* 4x post-process, step 2 */ \
        __ASM_EMIT("vmov        s16, s1") \
        __ASM_EMIT("vmov        s24, s9") \
        __ASM_EMIT(keep_op "    q8, q2, q6") \
        __ASM_EMIT("vmvn        q10, q8")  \
        __ASM_EMIT("vand        q0, q0, q8") \
        __ASM_EMIT("vand        q2, q2, q8") \
        __ASM_EMIT("vand        q6, q6, q10") \
        __ASM_EMIT("vand        q8, q4, q10") \
        __ASM_EMIT("vorr        q2, q6") \
        __ASM_EMIT("vorr        q0, q8") \
        __ASM_EMIT("vmov        q4, q5") \
        /* 1x blocks */ \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("vldm        %[src]!, {s24}")        /* q6 = sample */ \
        __ASM_EMIT(keep_op "    q8, q2, q6") \
        __ASM_EMIT("vmvn        q10, q8") \
        __ASM_EMIT("vand        q0, q0, q8") \
        __ASM_EMIT("vand        q2, q2, q8") \
        __ASM_EMIT("vand        q6, q6, q10") \
        __ASM_EMIT("vand        q8, q4, q10") \
        __ASM_EMIT("vorr        q2, q6") \
        __ASM_EMIT("vorr        q0, q8") \
        __ASM_EMIT("vadd.u32    q4, q15")               /* nidx ++ */ \
        __ASM_EMIT("subs        %[count], $1")          /* count -- */ \
        __ASM_EMIT("bge         5b") \
        \
        __ASM_EMIT("6:") \
        __ASM_EMIT("vstm        %[index], {s0}")

    size_t min_index(const float *src, size_t count)
    {
        uint32_t index = 0;
        IF_ARCH_ARM(
            uint32_t *pindexes = indexes;
        );

        ARCH_ARM_ASM(
            IDX_COND_SEARCH("vcle.f32")
            : [src] "+r" (src), [count] "+r" (count),
              [IDXS] "+r" (pindexes)
            : [index] "r" (&index)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        return index;
    }

    size_t max_index(const float *src, size_t count)
    {
        uint32_t index = 0;
        IF_ARCH_ARM(
            uint32_t *pindexes = indexes;
        );

        ARCH_ARM_ASM(
            IDX_COND_SEARCH("vcge.f32")
            : [src] "+r" (src), [count] "+r" (count),
              [IDXS] "+r" (pindexes)
            : [index] "r" (&index)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        return index;
    }

    #undef IDX_COND_SEARCH

    #define IDX_ABS_COND_SEARCH(keep_op) \
        __ASM_EMIT("veor        q0, q0")                /* q0 = idx0 = 0 */ \
        __ASM_EMIT("cmp         %[count], $0") \
        __ASM_EMIT("beq         6f") \
        \
        __ASM_EMIT("vldm        %[src], {s8}") \
        __ASM_EMIT("vmov        s9, s8") \
        __ASM_EMIT("vld1.32     {q4-q5}, [%[IDXS]]!")   /* q4-q5 = { nidx0, nidx1 } */ \
        __ASM_EMIT("vmov        d5, d4")                /* q2 = val0 */ \
        __ASM_EMIT("vldm        %[IDXS], {q12-q15}")    /* q12-q15 = { incr0, incr1, incr2, incr3} */ \
        __ASM_EMIT("vabs.f32    q2, q2") \
        __ASM_EMIT("subs        %[count], $8") \
        __ASM_EMIT("bls         2f") \
        \
        /* 8x blocks */ \
        __ASM_EMIT("veor        q1, q1")                /* q0 = idx1 = 0 */ \
        __ASM_EMIT("vmov        q3, q2")                /* q3 = val1 */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vld1.32     {q6-q7}, [%[src]]!")    /* q6-q7 = { sample0, sample1 } */ \
        __ASM_EMIT("vabs.f32    q6, q6") \
        __ASM_EMIT("vabs.f32    q7, q7") \
        __ASM_EMIT(keep_op "    q8, q2, q6")            /* q8 = val0 <=> sample0 */ \
        __ASM_EMIT(keep_op "    q9, q3, q7") \
        __ASM_EMIT("vmvn        q10, q8")               /* q10 = !(val0 <=> sample0) */ \
        __ASM_EMIT("vmvn        q11, q9") \
        __ASM_EMIT("vand        q0, q0, q8")            /* q0 = idx0 & (val0 <=> sample0) */ \
        __ASM_EMIT("vand        q1, q1, q9") \
        __ASM_EMIT("vand        q2, q2, q8")            /* q2 = val0 & (val0 <=> sample0) */ \
        __ASM_EMIT("vand        q3, q3, q9") \
        __ASM_EMIT("vand        q6, q6, q10")           /* q6 = sample0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vand        q7, q7, q11") \
        __ASM_EMIT("vand        q8, q4, q10")           /* q8 = nidx0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vand        q9, q5, q11") \
        __ASM_EMIT("vorr        q2, q6")                /* q2 = val0 & (val0 <=> sample0) | sample0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vorr        q3, q7") \
        __ASM_EMIT("vorr        q0, q8")                /* q0 = idx0 & (val0 <=> sample0) | nidx0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vorr        q1, q9") \
        __ASM_EMIT("vadd.u32    q4, q4, q12")           /* q4 = nidx0 + 8 */ \
        __ASM_EMIT("vadd.u32    q5, q5, q13") \
        __ASM_EMIT("subs        %[count], $8")          /* count -= 8 */ \
        __ASM_EMIT("bhs         1b") \
        /* 8x post-process q6 -> q3, q4 -> q1 */ \
        __ASM_EMIT(keep_op "    q8, q2, q3")            /* q8 = val0 <=> val1 */ \
        __ASM_EMIT("vmvn        q10, q8")               /* q10 = !(val0 <=> val1) */ \
        __ASM_EMIT("vand        q0, q0, q8")            /* q0 = idx0 & (val0 <=> val1) */ \
        __ASM_EMIT("vand        q2, q2, q8")            /* q2 = val0 & (val0 <=> val1) */ \
        __ASM_EMIT("vand        q3, q3, q10")           /* q3 = val1 & !(val0 <=> val1) */ \
        __ASM_EMIT("vand        q8, q1, q10")           /* q8 = idx1 & !(val0 <=> val1) */ \
        __ASM_EMIT("vorr        q2, q3")                /* q2 = val0 & (val0 <=> val1) | val1 & !(val0 <=> val1) */ \
        __ASM_EMIT("vorr        q0, q8")                /* q0 = idx0 & (val0 <=> val1) | idx1 & !(val0 <=> val1) */ \
        /* 4x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vld1.32     {q6}, [%[src]]!")       /* q6 = sample0 */ \
        __ASM_EMIT("vabs.f32    q6, q6") \
        __ASM_EMIT(keep_op "    q8, q2, q6")            /* q8 = val0 <=> sample0 */ \
        __ASM_EMIT("vmvn        q10, q8")               /* q10 = !(val0 <=> sample0) */ \
        __ASM_EMIT("vand        q0, q0, q8")            /* q0 = idx0 & (val0 <=> sample0) */ \
        __ASM_EMIT("vand        q2, q2, q8")            /* q2 = val0 & (val0 <=> sample0) */ \
        __ASM_EMIT("vand        q6, q6, q10")           /* q6 = sample0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vand        q8, q4, q10")           /* q8 = nidx0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vorr        q2, q6")                /* q2 = val0 & (val0 <=> sample0) | sample0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vorr        q0, q8")                /* q0 = idx0 & (val0 <=> sample0) | nidx0 & !(val0 <=> sample0) */ \
        __ASM_EMIT("vadd.u32    q4, q4, q14")           /* q4 = nidx0 + 4 */ \
        __ASM_EMIT("sub         %[count], $4")          /* count -= 8 */ \
        __ASM_EMIT("4:") \
        /* 4x post-process, step 1 */ \
        __ASM_EMIT("vmov        q5, q4") \
        __ASM_EMIT("vmov        d8, d1") \
        __ASM_EMIT("vmov        d12, d5") \
        __ASM_EMIT(keep_op "    q8, q2, q6") \
        __ASM_EMIT("vmvn        q10, q8")  \
        __ASM_EMIT("vand        q0, q0, q8") \
        __ASM_EMIT("vand        q2, q2, q8") \
        __ASM_EMIT("vand        q6, q6, q10") \
        __ASM_EMIT("vand        q8, q4, q10") \
        __ASM_EMIT("vorr        q2, q6") \
        __ASM_EMIT("vorr        q0, q8") \
        /* 4x post-process, step 2 */ \
        __ASM_EMIT("vmov        s16, s1") \
        __ASM_EMIT("vmov        s24, s9") \
        __ASM_EMIT(keep_op "    q8, q2, q6") \
        __ASM_EMIT("vmvn        q10, q8")  \
        __ASM_EMIT("vand        q0, q0, q8") \
        __ASM_EMIT("vand        q2, q2, q8") \
        __ASM_EMIT("vand        q6, q6, q10") \
        __ASM_EMIT("vand        q8, q4, q10") \
        __ASM_EMIT("vorr        q2, q6") \
        __ASM_EMIT("vorr        q0, q8") \
        __ASM_EMIT("vmov        q4, q5") \
        /* 1x blocks */ \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("vldm        %[src]!, {s24}")        /* q6 = sample */ \
        __ASM_EMIT("vabs.f32    q6, q6") \
        __ASM_EMIT(keep_op "    q8, q2, q6") \
        __ASM_EMIT("vmvn        q10, q8") \
        __ASM_EMIT("vand        q0, q0, q8") \
        __ASM_EMIT("vand        q2, q2, q8") \
        __ASM_EMIT("vand        q6, q6, q10") \
        __ASM_EMIT("vand        q8, q4, q10") \
        __ASM_EMIT("vorr        q2, q6") \
        __ASM_EMIT("vorr        q0, q8") \
        __ASM_EMIT("vadd.u32    q4, q15")               /* nidx ++ */ \
        __ASM_EMIT("subs        %[count], $1")          /* count -- */ \
        __ASM_EMIT("bge         5b") \
        \
        __ASM_EMIT("6:") \
        __ASM_EMIT("vstm        %[index], {s0}")

    size_t abs_min_index(const float *src, size_t count)
    {
        uint32_t index = 0;
        IF_ARCH_ARM(
            uint32_t *pindexes = indexes;
        );

        ARCH_ARM_ASM(
            IDX_ABS_COND_SEARCH("vcle.f32")
            : [src] "+r" (src), [count] "+r" (count),
              [IDXS] "+r" (pindexes)
            : [index] "r" (&index)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        return index;
    }

    size_t abs_max_index(const float *src, size_t count)
    {
        uint32_t index = 0;
        IF_ARCH_ARM(
            uint32_t *pindexes = indexes;
        );

        ARCH_ARM_ASM(
            IDX_ABS_COND_SEARCH("vcge.f32")
            : [src] "+r" (src), [count] "+r" (count),
              [IDXS] "+r" (pindexes)
            : [index] "r" (&index)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        return index;
    }

    #undef IDX_ABS_COND_SEARCH
}

#endif /* DSP_ARCH_ARM_NEON_D32_SEARCH_H_ */
