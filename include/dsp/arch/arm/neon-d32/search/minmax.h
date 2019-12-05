/*
 * minmax.h
 *
 *  Created on: 4 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_SEARCH_MINMAX_H_
#define DSP_ARCH_ARM_NEON_D32_SEARCH_MINMAX_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    #define SEL_NONE(a, b)              a
    #define SEL_ABS(a, b)               b

    #define MINMAX_SEARCH_CORE(OP, SEL) \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("blo         12f") \
        /* x32 blocks */ \
        __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!") \
        __ASM_EMIT(SEL("", "vabs.f32    q0, q0")) \
        __ASM_EMIT("subs        %[count], $32") \
        __ASM_EMIT("vmov        q1, q0") \
        __ASM_EMIT("blt         2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src]!, {q8-q15}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT(SEL("", "vabs.f32    q10, q10")) \
        __ASM_EMIT(SEL("", "vabs.f32    q11, q11")) \
        __ASM_EMIT(SEL("", "vabs.f32    q12, q12")) \
        __ASM_EMIT(SEL("", "vabs.f32    q13, q13")) \
        __ASM_EMIT(SEL("", "vabs.f32    q14, q14")) \
        __ASM_EMIT(SEL("", "vabs.f32    q15, q15")) \
        __ASM_EMIT(OP "         q0, q0, q8") \
        __ASM_EMIT(OP "         q1, q1, q9") \
        __ASM_EMIT(OP "         q0, q0, q10") \
        __ASM_EMIT(OP "         q1, q1, q11") \
        __ASM_EMIT(OP "         q0, q0, q12") \
        __ASM_EMIT(OP "         q1, q1, q13") \
        __ASM_EMIT(OP "         q0, q0, q14") \
        __ASM_EMIT(OP "         q1, q1, q15") \
        __ASM_EMIT("subs        %[count], $32") \
        __ASM_EMIT("bhs         1b") \
        __ASM_EMIT("2:") \
        /* x16 block */ \
        __ASM_EMIT("adds        %[count], $16") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src]!, {q8-q11}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT(SEL("", "vabs.f32    q10, q10")) \
        __ASM_EMIT(SEL("", "vabs.f32    q11, q11")) \
        __ASM_EMIT(OP "         q0, q0, q8") \
        __ASM_EMIT(OP "         q1, q1, q9") \
        __ASM_EMIT(OP "         q0, q0, q10") \
        __ASM_EMIT(OP "         q1, q1, q11") \
        __ASM_EMIT("sub         %[count], $16") \
        __ASM_EMIT("4:") \
        /* x8 block */ \
        __ASM_EMIT("adds        %[count], $8") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[src]!, {q8-q9}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT(OP "         q0, q0, q8") \
        __ASM_EMIT(OP "         q1, q1, q9") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("6:") \
        __ASM_EMIT(OP "         q0, q0, q1") \
        /* x4 block */ \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vldm        %[src]!, {q8}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(OP "         q0, q0, q8") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT("8:") \
        /* x1 block */ \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[src]]!") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(OP "         q0, q0, q8")  \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("bge         9b") \
        __ASM_EMIT("10:") \
        /* end */ \
        __ASM_EMIT("vext.32     q8, q0, q0, $2") \
        __ASM_EMIT(OP "         q0, q0, q8")  \
        __ASM_EMIT("vext.32     q8, q0, q0, $1") \
        __ASM_EMIT(OP "         q0, q0, q8") \
        __ASM_EMIT("vmov        %[res], s0") \
        __ASM_EMIT("12:")

    float min(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_ARM_ASM(
            MINMAX_SEARCH_CORE("vmin.f32", SEL_NONE)
            : [count] "+r" (count), [src] "+r" (src),
              [res] "+t" (res)
            :
            : "cc",
              "q1",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
        return res;
    }

    float max(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_ARM_ASM(
            MINMAX_SEARCH_CORE("vmax.f32", SEL_NONE)
            : [count] "+r" (count), [src] "+r" (src),
              [res] "+t" (res)
            :
            : "cc",
              "q1",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
        return res;
    }

    float abs_min(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_ARM_ASM(
            MINMAX_SEARCH_CORE("vmin.f32", SEL_ABS)
            : [count] "+r" (count), [src] "+r" (src),
              [res] "+t" (res)
            :
            : "cc",
              "q1",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
        return res;
    }

    float abs_max(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_ARM_ASM(
            MINMAX_SEARCH_CORE("vmax.f32", SEL_ABS)
            : [count] "+r" (count), [src] "+r" (src),
              [res] "+t" (res)
            :
            : "cc",
              "q1",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
        return res;
    }

    #undef MINMAX_SEARCH_CORE

    #define MINMAX2_SEARCH_CORE(SEL) \
        __ASM_EMIT("veor        q0, q0") \
        __ASM_EMIT("veor        q1, q0") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("blo         12f") \
        /* x32 blocks */ \
        __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!") \
        __ASM_EMIT(SEL("", "vabs.f32    q0, q0")) \
        __ASM_EMIT("subs        %[count], $32") \
        __ASM_EMIT("vmov        q1, q0") \
        __ASM_EMIT("blt         2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src]!, {q8-q15}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT(SEL("", "vabs.f32    q10, q10")) \
        __ASM_EMIT(SEL("", "vabs.f32    q11, q11")) \
        __ASM_EMIT(SEL("", "vabs.f32    q12, q12")) \
        __ASM_EMIT(SEL("", "vabs.f32    q13, q13")) \
        __ASM_EMIT(SEL("", "vabs.f32    q14, q14")) \
        __ASM_EMIT(SEL("", "vabs.f32    q15, q15")) \
        __ASM_EMIT("vmin.f32    q0, q0, q8") \
        __ASM_EMIT("vmax.f32    q1, q1, q12") \
        __ASM_EMIT("vmin.f32    q0, q0, q9") \
        __ASM_EMIT("vmax.f32    q1, q1, q13") \
        __ASM_EMIT("vmin.f32    q0, q0, q10") \
        __ASM_EMIT("vmax.f32    q1, q1, q14") \
        __ASM_EMIT("vmin.f32    q0, q0, q11") \
        __ASM_EMIT("vmax.f32    q1, q1, q15") \
        __ASM_EMIT("vmin.f32    q0, q0, q12") \
        __ASM_EMIT("vmax.f32    q1, q1, q8") \
        __ASM_EMIT("vmin.f32    q0, q0, q13") \
        __ASM_EMIT("vmax.f32    q1, q1, q9") \
        __ASM_EMIT("vmin.f32    q0, q0, q14") \
        __ASM_EMIT("vmax.f32    q1, q1, q10") \
        __ASM_EMIT("vmin.f32    q0, q0, q15") \
        __ASM_EMIT("vmax.f32    q1, q1, q11") \
        __ASM_EMIT("subs        %[count], $32") \
        __ASM_EMIT("bhs         1b") \
        __ASM_EMIT("2:") \
        /* x16 block */ \
        __ASM_EMIT("adds        %[count], $16") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src]!, {q8-q11}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT(SEL("", "vabs.f32    q10, q10")) \
        __ASM_EMIT(SEL("", "vabs.f32    q11, q11")) \
        __ASM_EMIT("vmin.f32    q0, q0, q8") \
        __ASM_EMIT("vmax.f32    q1, q1, q10") \
        __ASM_EMIT("vmin.f32    q0, q0, q9") \
        __ASM_EMIT("vmax.f32    q1, q1, q11") \
        __ASM_EMIT("vmin.f32    q0, q0, q10") \
        __ASM_EMIT("vmax.f32    q1, q1, q8") \
        __ASM_EMIT("vmin.f32    q0, q0, q11") \
        __ASM_EMIT("vmax.f32    q1, q1, q9") \
        __ASM_EMIT("sub         %[count], $16") \
        __ASM_EMIT("4:") \
        /* x8 block */ \
        __ASM_EMIT("adds        %[count], $8") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[src]!, {q8-q9}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT("vmin.f32    q0, q0, q8") \
        __ASM_EMIT("vmax.f32    q1, q1, q9") \
        __ASM_EMIT("vmin.f32    q0, q0, q9") \
        __ASM_EMIT("vmax.f32    q1, q1, q8") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("6:") \
        /* x4 block */ \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vldm        %[src]!, {q8}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT("vmin.f32    q0, q0, q8") \
        __ASM_EMIT("vmax.f32    q1, q1, q8") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT("8:") \
        /* x1 block */ \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[src]]!") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT("vmin.f32    q0, q0, q8") \
        __ASM_EMIT("vmax.f32    q1, q1, q8") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("bge         9b") \
        __ASM_EMIT("10:") \
        /* end */ \
        __ASM_EMIT("vext.32     q8, q0, q0, $2") \
        __ASM_EMIT("vext.32     q9, q1, q1, $2") \
        __ASM_EMIT("vmin.f32    q0, q0, q8")  \
        __ASM_EMIT("vmax.f32    q1, q1, q9")  \
        __ASM_EMIT("vext.32     q8, q0, q0, $1") \
        __ASM_EMIT("vext.32     q9, q1, q1, $1") \
        __ASM_EMIT("vmin.f32    q0, q0, q8")  \
        __ASM_EMIT("vmax.f32    q1, q1, q9")  \
        __ASM_EMIT("12:") \
        __ASM_EMIT("vst1.32     {d0[0]}, [%[min]]") \
        __ASM_EMIT("vst1.32     {d2[0]}, [%[max]]")

    void minmax(const float *src, size_t count, float *min, float *max)
    {
        ARCH_ARM_ASM(
            MINMAX2_SEARCH_CORE(SEL_NONE)
            : [count] "+r" (count), [src] "+r" (src)
            : [min] "r" (min), [max] "r" (max)
            : "cc",
              "q0", "q1",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void abs_minmax(const float *src, size_t count, float *min, float *max)
    {
        ARCH_ARM_ASM(
            MINMAX2_SEARCH_CORE(SEL_ABS)
            : [count] "+r" (count), [src] "+r" (src)
            : [min] "r" (min), [max] "r" (max)
            : "cc",
              "q0", "q1",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    #undef SEL_NONE
    #undef SEL_ABS
}


#endif /* DSP_ARCH_ARM_NEON_D32_SEARCH_MINMAX_H_ */
