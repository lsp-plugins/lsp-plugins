/*
 * copy.h
 *
 *  Created on: 11 апр. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_COPY_H_
#define DSP_ARCH_AARCH64_ASIMD_COPY_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void copy(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        8f")

            /* 128x block */
            __ASM_EMIT("subs        %[count], %[count], #112")
            __ASM_EMIT("b.lt        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x040]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x060]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x040]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x060]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x080]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x0a0]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x0c0]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x0e0]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x080]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x0a0]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x0c0]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x0e0]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x100]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x120]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x140]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x160]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x100]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x120]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x140]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x160]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x180]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x1a0]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x1c0]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x1e0]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x180]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x1a0]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x1c0]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x1e0]")

            __ASM_EMIT("subs        %[count], %[count], #128")
            __ASM_EMIT("add         %[src], %[src], #0x200")
            __ASM_EMIT("add         %[dst], %[dst], #0x200")
            __ASM_EMIT("b.hs         1b")

            /* 64x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], 64") // 128 - 64
            __ASM_EMIT("b.lt        4f")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x040]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x060]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x040]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x060]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x080]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x0a0]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x0c0]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x0e0]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x080]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x0a0]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x0c0]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x0e0]")

            __ASM_EMIT("sub         %[count], %[count], #64")
            __ASM_EMIT("add         %[src], %[src], #0x100")
            __ASM_EMIT("add         %[dst], %[dst], #0x100")

            /* 32x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], 32") // 64-32
            __ASM_EMIT("b.lt        6f")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x040]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x060]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x040]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x060]")

            __ASM_EMIT("sub         %[count], %[count], #32")
            __ASM_EMIT("add         %[src], %[src], #0x080")
            __ASM_EMIT("add         %[dst], %[dst], #0x080")

            /* 16x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #16") // 32-16
            __ASM_EMIT("b.lt        8f")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")

            __ASM_EMIT("sub         %[count], %[count], #16")
            __ASM_EMIT("add         %[src], %[src], #0x040")
            __ASM_EMIT("add         %[dst], %[dst], #0x040")

            /* 8x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], %[count], #8") // 16-8
            __ASM_EMIT("b.lt        10f")
            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[src], %[src], #0x020")
            __ASM_EMIT("add         %[dst], %[dst], #0x020")

            /* 4x block */
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], %[count], #4") // 8-4
            __ASM_EMIT("b.lt        12f")
            __ASM_EMIT("ldr         q0, [%[src], #0x000]")
            __ASM_EMIT("str         q0, [%[dst], #0x000]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[src], %[src], #0x010")
            __ASM_EMIT("add         %[dst], %[dst], #0x010")

            /* 1x blocks */
            __ASM_EMIT("12:")
            __ASM_EMIT("adds        %[count], %[count], #3") // 4-1
            __ASM_EMIT("b.lt        14f")
            __ASM_EMIT("13:")
            __ASM_EMIT("ld1         {v0.s}[0], [%[src]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add         %[src], %[src], #0x004")
            __ASM_EMIT("add         %[dst], %[dst], #0x004")
            __ASM_EMIT("b.ge        13b")

            /* End of copy */
            __ASM_EMIT("14:")

            : [src] "+r" (src), [dst] "+r"(dst),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

    void move(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("cmp         %[dst], %[src]")
            __ASM_EMIT("b.eq        2000f")
            __ASM_EMIT("b.gt        1000f")

            //---------------------------------------------
            // Address-increasing copy
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        8f")

            /* 128x block */
            __ASM_EMIT("subs        %[count], %[count], #112")
            __ASM_EMIT("b.lt        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x040]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x060]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x040]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x060]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x080]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x0a0]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x0c0]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x0e0]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x080]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x0a0]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x0c0]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x0e0]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x100]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x120]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x140]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x160]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x100]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x120]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x140]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x160]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x180]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x1a0]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x1c0]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x1e0]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x180]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x1a0]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x1c0]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x1e0]")

            __ASM_EMIT("subs        %[count], %[count], #128")
            __ASM_EMIT("add         %[src], %[src], #0x200")
            __ASM_EMIT("add         %[dst], %[dst], #0x200")
            __ASM_EMIT("b.hs         1b")

            /* 64x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], 64") // 128 - 64
            __ASM_EMIT("b.lt        4f")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x040]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x060]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x040]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x060]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x080]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x0a0]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x0c0]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x0e0]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x080]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x0a0]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x0c0]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x0e0]")

            __ASM_EMIT("sub         %[count], %[count], #64")
            __ASM_EMIT("add         %[src], %[src], #0x100")
            __ASM_EMIT("add         %[dst], %[dst], #0x100")

            /* 32x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], 32") // 64-32
            __ASM_EMIT("b.lt        6f")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x040]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x060]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x040]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x060]")

            __ASM_EMIT("sub         %[count], %[count], #32")
            __ASM_EMIT("add         %[src], %[src], #0x080")
            __ASM_EMIT("add         %[dst], %[dst], #0x080")

            /* 16x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #16") // 32-16
            __ASM_EMIT("b.lt        8f")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")

            __ASM_EMIT("sub         %[count], %[count], #16")
            __ASM_EMIT("add         %[src], %[src], #0x040")
            __ASM_EMIT("add         %[dst], %[dst], #0x040")

            /* 8x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], %[count], #8") // 16-8
            __ASM_EMIT("b.lt        10f")
            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[src], %[src], #0x020")
            __ASM_EMIT("add         %[dst], %[dst], #0x020")

            /* 4x block */
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], %[count], #4") // 8-4
            __ASM_EMIT("b.lt        12f")
            __ASM_EMIT("ldr         q0, [%[src], #0x000]")
            __ASM_EMIT("str         q0, [%[dst], #0x000]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[src], %[src], #0x010")
            __ASM_EMIT("add         %[dst], %[dst], #0x010")

            /* 1x blocks */
            __ASM_EMIT("12:")
            __ASM_EMIT("adds        %[count], %[count], #3") // 4-1
            __ASM_EMIT("b.lt        2000f")
            __ASM_EMIT("13:")
            __ASM_EMIT("ld1         {v0.s}[0], [%[src]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add         %[src], %[src], #0x004")
            __ASM_EMIT("add         %[dst], %[dst], #0x004")
            __ASM_EMIT("b.ge        13b")

            /* End of move */
            __ASM_EMIT("b           2000f")

            //---------------------------------------------
            // Address-decreasing copy
            __ASM_EMIT("1000:")
            __ASM_EMIT("add         %[src], %[src], %[count], lsl #2")      // src += count
            __ASM_EMIT("add         %[dst], %[dst], %[count], lsl #2")      // dst += count
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        8f")

            /* 128x block */
            __ASM_EMIT("subs        %[count], %[count], #112")
            __ASM_EMIT("b.lt        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("sub         %[src], %[src], #0x200")
            __ASM_EMIT("sub         %[dst], %[dst], #0x200")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x180]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x1a0]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x1c0]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x1e0]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x180]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x1a0]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x1c0]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x1e0]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x100]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x120]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x140]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x160]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x100]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x120]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x140]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x160]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x080]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x0a0]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x0c0]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x0e0]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x080]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x0a0]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x0c0]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x0e0]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x040]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x060]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x040]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x060]")

            __ASM_EMIT("subs        %[count], %[count], #128")
            __ASM_EMIT("b.hs         1b")

            /* 64x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], 64") // 128 - 64
            __ASM_EMIT("b.lt        4f")

            __ASM_EMIT("sub         %[src], %[src], #0x100")
            __ASM_EMIT("sub         %[dst], %[dst], #0x100")
            __ASM_EMIT("ldp         q0, q1, [%[src], #0x080]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x0a0]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x0c0]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x0e0]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x080]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x0a0]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x0c0]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x0e0]")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x040]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x060]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x040]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x060]")
            __ASM_EMIT("sub         %[count], %[count], #64")

            /* 32x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], 32") // 64-32
            __ASM_EMIT("b.lt        6f")

            __ASM_EMIT("sub         %[src], %[src], #0x080")
            __ASM_EMIT("sub         %[dst], %[dst], #0x080")
            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x040]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x060]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x040]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x060]")
            __ASM_EMIT("sub         %[count], %[count], #32")

            /* 16x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #16") // 32-16
            __ASM_EMIT("b.lt        8f")

            __ASM_EMIT("sub         %[src], %[src], #0x040")
            __ASM_EMIT("sub         %[dst], %[dst], #0x040")
            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x020]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x020]")
            __ASM_EMIT("sub         %[count], %[count], #16")

            /* 8x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], %[count], #8") // 16-8
            __ASM_EMIT("b.lt        10f")
            __ASM_EMIT("sub         %[src], %[src], #0x020")
            __ASM_EMIT("sub         %[dst], %[dst], #0x020")
            __ASM_EMIT("ldp         q0, q1, [%[src], #0x000]")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x000]")
            __ASM_EMIT("sub         %[count], %[count], #8")

            /* 4x block */
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], %[count], #4") // 8-4
            __ASM_EMIT("b.lt        12f")
            __ASM_EMIT("sub         %[src], %[src], #0x010")
            __ASM_EMIT("sub         %[dst], %[dst], #0x010")
            __ASM_EMIT("ldr         q0, [%[src], #0x000]")
            __ASM_EMIT("str         q0, [%[dst], #0x000]")
            __ASM_EMIT("sub         %[count], %[count], #4")

            /* 1x blocks */
            __ASM_EMIT("12:")
            __ASM_EMIT("adds        %[count], %[count], #3") // 4-1
            __ASM_EMIT("b.lt        2000f")
            __ASM_EMIT("13:")
            __ASM_EMIT("sub         %[src], %[src], #0x004")
            __ASM_EMIT("sub         %[dst], %[dst], #0x004")
            __ASM_EMIT("ld1         {v0.s}[0], [%[src]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
            __ASM_EMIT("b.ge        13b")

            /* End of move */
            __ASM_EMIT("2000:")

            : [dst] "+r" (dst), [src] "=&r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_COPY_H_ */
