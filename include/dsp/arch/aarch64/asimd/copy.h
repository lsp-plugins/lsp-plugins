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
            __ASM_EMIT("cmp         %[dst], %[src]")
            __ASM_EMIT("b.eq        2000f")
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

#define FILL_CORE(DST) \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("mov         v2.16b, v0.16b") \
    __ASM_EMIT("mov         v3.16b, v1.16b") \
    __ASM_EMIT("mov         v4.16b, v0.16b") \
    __ASM_EMIT("mov         v5.16b, v1.16b") \
    __ASM_EMIT("mov         v6.16b, v2.16b") \
    __ASM_EMIT("mov         v7.16b, v3.16b") \
    \
    /* 128x block */ \
    __ASM_EMIT("subs        %[count], %[count], #112") \
    __ASM_EMIT("b.lt        2f") \
    __ASM_EMIT("1:") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x000]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x020]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x040]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x060]") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x080]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x0a0]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x0c0]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x0e0]") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x100]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x120]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x140]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x160]") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x180]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x1a0]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x1c0]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x1e0]") \
    \
    __ASM_EMIT("subs        %[count], %[count], #128") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x200") \
    __ASM_EMIT("b.hs         1b") \
    \
    /* 64x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], 64") /* 128 - 64 */ \
    __ASM_EMIT("b.lt        4f") \
    \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x000]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x020]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x040]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x060]") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x080]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x0a0]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x0c0]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x0e0]") \
    \
    __ASM_EMIT("sub         %[count], %[count], #64") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x100") \
    \
    /* 32x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], 32") /* 64-32 */ \
    __ASM_EMIT("b.lt        6f") \
    \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x000]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x020]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x040]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x060]") \
    \
    __ASM_EMIT("sub         %[count], %[count], #32") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x080") \
    \
    /* 16x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32-16 */ \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x000]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x020]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x040") \
    \
    /* 8x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16-8 */ \
    __ASM_EMIT("b.lt        10f") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x000]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x020") \
    \
    /* 4x block */ \
    __ASM_EMIT("10:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8-4 */ \
    __ASM_EMIT("b.lt        12f") \
    __ASM_EMIT("str         q0, [%[" DST "], #0x000]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x010") \
    \
    /* 1x blocks */ \
    __ASM_EMIT("12:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4-1 */ \
    __ASM_EMIT("b.lt        14f") \
    __ASM_EMIT("13:") \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x004") \
    __ASM_EMIT("b.ge        13b") \
    \
    /* End of copy */ \
    __ASM_EMIT("14:")

    void fill(float *dst, float k, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r        {v0.4s}, [%[k]]")
            __ASM_EMIT("mov         v1.16b, v0.16b") \
            FILL_CORE("dst")
            : [dst] "+r"(dst),
              [count] "+r" (count)
            : [k] "r" (&k)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

    void fill_zero(float *dst, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("eor         v0.16b, v0.16b, v0.16b")
            __ASM_EMIT("eor         v1.16b, v1.16b, v1.16b")
            FILL_CORE("dst")
            : [dst] "+r"(dst),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

    IF_ARCH_AARCH64(
        static const float fill_one_data[] __lsp_aligned16 =
        {
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        };

        static const float fill_minus_one_data[] __lsp_aligned16 =
        {
            -1.0f, -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f
        };
    )

    void fill_one(float *dst, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ldp         q0, q1, [%[DATA]]")
            FILL_CORE("dst")
            : [dst] "+r"(dst),
              [count] "+r" (count)
            : [DATA] "r" (fill_one_data)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

    void fill_minus_one(float *dst, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ldp         q0, q1, [%[DATA]]")
            FILL_CORE("dst")
            : [dst] "+r"(dst),
              [count] "+r" (count)
            : [DATA] "r" (fill_minus_one_data)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

#undef FILL_CORE

    void reverse1(float *dst, size_t count)
    {
        IF_ARCH_AARCH64(float *src);

        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("add             %[src], %[dst], %[count], LSL #2")
            __ASM_EMIT("lsr             %[count], %[count], #1")
            __ASM_EMIT("subs            %[count], %[count], #16")
            __ASM_EMIT("b.lt            2f")
            /* 16x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("sub             %[src], %[src], #0x40")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[src], #0x00]")
            __ASM_EMIT("ldp             q6, q7, [%[src], #0x20]")
            __ASM_EMIT("rev64           v0.4s, v0.4s")
            __ASM_EMIT("rev64           v1.4s, v1.4s")
            __ASM_EMIT("rev64           v2.4s, v2.4s")
            __ASM_EMIT("rev64           v3.4s, v3.4s")
            __ASM_EMIT("rev64           v4.4s, v4.4s")
            __ASM_EMIT("rev64           v5.4s, v5.4s")
            __ASM_EMIT("rev64           v6.4s, v6.4s")
            __ASM_EMIT("rev64           v7.4s, v7.4s")
            __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #8")
            __ASM_EMIT("ext             v1.16b, v1.16b, v1.16b, #8")
            __ASM_EMIT("ext             v2.16b, v2.16b, v2.16b, #8")
            __ASM_EMIT("ext             v3.16b, v3.16b, v3.16b, #8")
            __ASM_EMIT("ext             v4.16b, v4.16b, v4.16b, #8")
            __ASM_EMIT("ext             v5.16b, v5.16b, v5.16b, #8")
            __ASM_EMIT("ext             v6.16b, v6.16b, v6.16b, #8")
            __ASM_EMIT("ext             v7.16b, v7.16b, v7.16b, #8")
            __ASM_EMIT("stp             q7, q6, [%[dst], #0x00]")
            __ASM_EMIT("stp             q5, q4, [%[dst], #0x20]")
            __ASM_EMIT("stp             q3, q2, [%[src], #0x00]")
            __ASM_EMIT("stp             q1, q0, [%[src], #0x20]")
            __ASM_EMIT("subs            %[count], %[count], #16")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs            1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count], #8")    /* 16 - 8 */
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("sub             %[src], %[src], #0x20")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[src], #0x00]")
            __ASM_EMIT("rev64           v0.4s, v0.4s")
            __ASM_EMIT("rev64           v1.4s, v1.4s")
            __ASM_EMIT("rev64           v2.4s, v2.4s")
            __ASM_EMIT("rev64           v3.4s, v3.4s")
            __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #8")
            __ASM_EMIT("ext             v1.16b, v1.16b, v1.16b, #8")
            __ASM_EMIT("ext             v2.16b, v2.16b, v2.16b, #8")
            __ASM_EMIT("ext             v3.16b, v3.16b, v3.16b, #8")
            __ASM_EMIT("stp             q3, q2, [%[dst], #0x00]")
            __ASM_EMIT("stp             q1, q0, [%[src], #0x00]")
            __ASM_EMIT("sub             %[count], %[count], #8")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #4")    /* 8 - 4 */
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("sub             %[src], %[src], #0x10")
            __ASM_EMIT("ldr             q0, [%[dst], #0x00]")
            __ASM_EMIT("ldr             q1, [%[src], #0x00]")
            __ASM_EMIT("rev64           v0.4s, v0.4s")
            __ASM_EMIT("rev64           v1.4s, v1.4s")
            __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #8")
            __ASM_EMIT("ext             v1.16b, v1.16b, v1.16b, #8")
            __ASM_EMIT("str             q1, [%[dst], #0x00]")
            __ASM_EMIT("str             q0, [%[src], #0x00]")
            __ASM_EMIT("sub             %[count], %[count], #4")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")
            /* 1x blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], %[count], #3")    /* 4 - 1 */
            __ASM_EMIT("b.lt            8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("sub             %[src], %[src], #0x04")
            __ASM_EMIT("ld1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("ld1             {v1.s}[0], [%[src]]")
            __ASM_EMIT("st1             {v1.s}[0], [%[dst]]")
            __ASM_EMIT("st1             {v0.s}[0], [%[src]]")
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge            7b")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "=&r" (src),
              [count] "+r" (count)
            : [DATA] "r" (fill_minus_one_data)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

    void reverse2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("add             %[src], %[src], %[count], LSL #2")
            __ASM_EMIT("subs            %[count], %[count], #32")
            __ASM_EMIT("b.lt            2f")
            /* 32x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("sub             %[src], %[src], #0x80")
            __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[src], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[src], #0x60]")
            __ASM_EMIT("rev64           v0.4s, v0.4s")
            __ASM_EMIT("rev64           v1.4s, v1.4s")
            __ASM_EMIT("rev64           v2.4s, v2.4s")
            __ASM_EMIT("rev64           v3.4s, v3.4s")
            __ASM_EMIT("rev64           v4.4s, v4.4s")
            __ASM_EMIT("rev64           v5.4s, v5.4s")
            __ASM_EMIT("rev64           v6.4s, v6.4s")
            __ASM_EMIT("rev64           v7.4s, v7.4s")
            __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #8")
            __ASM_EMIT("ext             v1.16b, v1.16b, v1.16b, #8")
            __ASM_EMIT("ext             v2.16b, v2.16b, v2.16b, #8")
            __ASM_EMIT("ext             v3.16b, v3.16b, v3.16b, #8")
            __ASM_EMIT("ext             v4.16b, v4.16b, v4.16b, #8")
            __ASM_EMIT("ext             v5.16b, v5.16b, v5.16b, #8")
            __ASM_EMIT("ext             v6.16b, v6.16b, v6.16b, #8")
            __ASM_EMIT("ext             v7.16b, v7.16b, v7.16b, #8")
            __ASM_EMIT("stp             q7, q6, [%[dst], #0x00]")
            __ASM_EMIT("stp             q5, q4, [%[dst], #0x20]")
            __ASM_EMIT("stp             q3, q2, [%[dst], #0x40]")
            __ASM_EMIT("stp             q1, q0, [%[dst], #0x60]")
            __ASM_EMIT("subs            %[count], %[count], #32")
            __ASM_EMIT("add             %[dst], %[dst], #0x80")
            __ASM_EMIT("b.hs            1b")
            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count], #16")    /* 32-16 */
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("sub             %[src], %[src], #0x40")
            __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")
            __ASM_EMIT("rev64           v0.4s, v0.4s")
            __ASM_EMIT("rev64           v1.4s, v1.4s")
            __ASM_EMIT("rev64           v2.4s, v2.4s")
            __ASM_EMIT("rev64           v3.4s, v3.4s")
            __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #8")
            __ASM_EMIT("ext             v1.16b, v1.16b, v1.16b, #8")
            __ASM_EMIT("ext             v2.16b, v2.16b, v2.16b, #8")
            __ASM_EMIT("ext             v3.16b, v3.16b, v3.16b, #8")
            __ASM_EMIT("stp             q3, q2, [%[dst], #0x00]")
            __ASM_EMIT("stp             q1, q0, [%[dst], #0x20]")
            __ASM_EMIT("sub             %[count], %[count], #16")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #8")    /* 16-8 */
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("sub             %[src], %[src], #0x20")
            __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
            __ASM_EMIT("rev64           v0.4s, v0.4s")
            __ASM_EMIT("rev64           v1.4s, v1.4s")
            __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #8")
            __ASM_EMIT("ext             v1.16b, v1.16b, v1.16b, #8")
            __ASM_EMIT("stp             q1, q0, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[count], %[count], #8")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            /* 4x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], %[count], #4")    /* 8 - 4 */
            __ASM_EMIT("b.lt            8f")
            __ASM_EMIT("sub             %[src], %[src], #0x10")
            __ASM_EMIT("ldr             q0, [%[src], #0x00]")
            __ASM_EMIT("rev64           v0.4s, v0.4s")
            __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #8")
            __ASM_EMIT("str             q0, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[count], %[count], #4")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")
            /* 1x blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds            %[count], %[count], #3")    /* 4 - 1 */
            __ASM_EMIT("b.lt            10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("sub             %[src], %[src], #0x04")
            __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
            __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge            9b")
            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [DATA] "r" (fill_minus_one_data)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_COPY_H_ */
