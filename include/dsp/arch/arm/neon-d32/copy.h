/*
 * copy.h
 *
 *  Created on: 3 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_COPY_H_
#define DSP_ARCH_ARM_NEON_D32_COPY_H_

namespace neon_d32
{
    void copy(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("cmp         %[dst], %[src]")
            __ASM_EMIT("beq         2000f")
            __ASM_EMIT("subs        %[count], $0x40")
            __ASM_EMIT("blo         2f")

            /* 64x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldmia.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vldmia.32   %[src]!, {q2-q3}")
            __ASM_EMIT("vldmia.32   %[src]!, {q4-q5}")
            __ASM_EMIT("vldmia.32   %[src]!, {q6-q7}")
            __ASM_EMIT("vldmia.32   %[src]!, {q8-q9}")
            __ASM_EMIT("vldmia.32   %[src]!, {q10-q11}")
            __ASM_EMIT("vldmia.32   %[src]!, {q12-q13}")
            __ASM_EMIT("vldmia.32   %[src]!, {q14-q15}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q2-q3}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q4-q5}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q6-q7}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q8-q9}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q10-q11}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q12-q13}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q14-q15}")
            __ASM_EMIT("subs        %[count], $0x40")
            __ASM_EMIT("bhs         1b")

            /* 32x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $0x20") // +0x40 - 0x20
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldmia.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vldmia.32   %[src]!, {q2-q3}")
            __ASM_EMIT("vldmia.32   %[src]!, {q4-q5}")
            __ASM_EMIT("vldmia.32   %[src]!, {q6-q7}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q2-q3}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q4-q5}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q6-q7}")
            __ASM_EMIT("sub         %[count], $0x20")

            /* 16x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $0x10") // + 0x20 - 0x10
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldmia.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vldmia.32   %[src]!, {q2-q3}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q2-q3}")
            __ASM_EMIT("sub         %[count], $0x10")

            /* 8x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $0x08") // + 0x10 - 0x8
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vldmia.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("sub         %[count], $0x08")

            /* 4x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $0x04") // + 0x8 - 0x4
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("vldmia.32   %[src]!, {q0}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q0}")
            __ASM_EMIT("sub         %[count], $0x04")

            /* 1x blocks */
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], $0x03") // + 0x4 - 0x1
            __ASM_EMIT("blt         2000f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vldmia.32   %[src]!, {s0}")
            __ASM_EMIT("vstmia.32   %[dst]!, {s0}")
            __ASM_EMIT("subs        %[count], $0x01")
            __ASM_EMIT("bge         11b")


            __ASM_EMIT("2000:")

            : [src] "+r" (src), [dst] "+r"(dst), [count] "+r" (count) :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void copy_new(float *dst, const float *src, size_t count)
    {
        const float *src2;
        float *dst2;
        size_t incr;

        ARCH_ARM_ASM
        (
            __ASM_EMIT("cmp         %[dst1], %[src1]")
            __ASM_EMIT("beq         2000f")
            __ASM_EMIT("mov         %[incr], $0x40")
            __ASM_EMIT("add         %[src2], %[src1], $0x20")
            __ASM_EMIT("add         %[dst2], %[dst1], $0x20")
            __ASM_EMIT("subs        %[count], $0x40")
            __ASM_EMIT("blo         2f")

            /* 64x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32     {q0-q1}, [%[src1]], %[incr]")
            __ASM_EMIT("vld1.32     {q2-q3}, [%[src2]], %[incr]")
            __ASM_EMIT("vld1.32     {q4-q5}, [%[src1]], %[incr]")
            __ASM_EMIT("vld1.32     {q6-q7}, [%[src2]], %[incr]")
            __ASM_EMIT("vld1.32     {q8-q9}, [%[src1]], %[incr]")
            __ASM_EMIT("vld1.32     {q10-q11}, [%[src2]], %[incr]")
            __ASM_EMIT("vld1.32     {q12-q13}, [%[src1]], %[incr]")
            __ASM_EMIT("vld1.32     {q14-q15}, [%[src2]], %[incr]")
            __ASM_EMIT("vst1.32     {q0-q1}, [%[dst1]], %[incr]")
            __ASM_EMIT("vst1.32     {q2-q3}, [%[dst2]], %[incr]")
            __ASM_EMIT("vst1.32     {q4-q5}, [%[dst1]], %[incr]")
            __ASM_EMIT("vst1.32     {q6-q7}, [%[dst2]], %[incr]")
            __ASM_EMIT("vst1.32     {q8-q9}, [%[dst1]], %[incr]")
            __ASM_EMIT("vst1.32     {q10-q11}, [%[dst2]], %[incr]")
            __ASM_EMIT("vst1.32     {q12-q13}, [%[dst1]], %[incr]")
            __ASM_EMIT("vst1.32     {q14-q15}, [%[dst2]], %[incr]")
            __ASM_EMIT("subs        %[count], $0x40")
            __ASM_EMIT("bhs         1b")

            /* 32x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $0x20") // +0x40 - 0x20
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vld1.32     {q0-q1}, [%[src1]], %[incr]")
            __ASM_EMIT("vld1.32     {q2-q3}, [%[src2]], %[incr]")
            __ASM_EMIT("vld1.32     {q4-q5}, [%[src1]], %[incr]")
            __ASM_EMIT("vld1.32     {q6-q7}, [%[src2]], %[incr]")
            __ASM_EMIT("vst1.32     {q0-q1}, [%[dst1]], %[incr]")
            __ASM_EMIT("vst1.32     {q2-q3}, [%[dst2]], %[incr]")
            __ASM_EMIT("vst1.32     {q4-q5}, [%[dst1]], %[incr]")
            __ASM_EMIT("vst1.32     {q6-q7}, [%[dst2]], %[incr]")
            __ASM_EMIT("sub         %[count], $0x20")

            /* 16x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $0x10") // + 0x20 - 0x10
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vld1.32     {q0-q1}, [%[src1]], %[incr]")
            __ASM_EMIT("vld1.32     {q2-q3}, [%[src2]], %[incr]")
            __ASM_EMIT("vst1.32     {q0-q1}, [%[dst1]], %[incr]")
            __ASM_EMIT("vst1.32     {q2-q3}, [%[dst2]], %[incr]")
            __ASM_EMIT("sub         %[count], $0x10")

            /* 8x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $0x08") // + 0x10 - 0x8
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vld1.32     {q0-q1}, [%[src1]]!")
            __ASM_EMIT("vst1.32     {q0-q1}, [%[dst1]]!")
            __ASM_EMIT("sub         %[count], $0x08")

            /* 4x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $0x04") // + 0x8 - 0x4
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("vld1.32     {q0}, [%[src1]]!")
            __ASM_EMIT("vst1.32     {q0}, [%[dst1]]!")
            __ASM_EMIT("sub         %[count], $0x04")

            /* 1x blocks */
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], $0x03") // + 0x4 - 0x1
            __ASM_EMIT("blt         2000f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vldmia.32   %[src1]!, {s0}")
            __ASM_EMIT("vstmia.32   %[dst1]!, {s0}")
            __ASM_EMIT("subs        %[count], $0x01")
            __ASM_EMIT("bge         11b")


            __ASM_EMIT("2000:")

            : [src1] "+r" (src), [dst1] "+r"(dst),
              [src2] "=&r" (src2), [dst2] "=&r"(dst2),
              [incr] "=&r" (incr),
              [count] "+r" (count) :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void move(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("cmp         %[dst], %[src]")
            __ASM_EMIT("beq         2000f")
            __ASM_EMIT("blo         1000f")

            // Address-decreasing copy
            __ASM_EMIT("add         %[src], %[src], %[count], lsl $2")
            __ASM_EMIT("add         %[dst], %[dst], %[count], lsl $2")
            __ASM_EMIT("subs        %[count], $0x40")
            __ASM_EMIT("blo         2f")

            /* 64x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldmdb.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vldmdb.32   %[src]!, {q2-q3}")
            __ASM_EMIT("vldmdb.32   %[src]!, {q4-q5}")
            __ASM_EMIT("vldmdb.32   %[src]!, {q6-q7}")
            __ASM_EMIT("vldmdb.32   %[src]!, {q8-q9}")
            __ASM_EMIT("vldmdb.32   %[src]!, {q10-q11}")
            __ASM_EMIT("vldmdb.32   %[src]!, {q12-q13}")
            __ASM_EMIT("vldmdb.32   %[src]!, {q14-q15}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q2-q3}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q4-q5}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q6-q7}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q8-q9}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q10-q11}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q12-q13}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q14-q15}")
            __ASM_EMIT("subs        %[count], $0x40")
            __ASM_EMIT("bhs         1b")

            /* 32x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $0x20") // +0x40 - 0x20
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldmdb.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vldmdb.32   %[src]!, {q2-q3}")
            __ASM_EMIT("vldmdb.32   %[src]!, {q4-q5}")
            __ASM_EMIT("vldmdb.32   %[src]!, {q6-q7}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q2-q3}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q4-q5}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q6-q7}")
            __ASM_EMIT("sub         %[count], $0x20")

            /* 16x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $0x10") // + 0x20 - 0x10
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldmdb.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vldmdb.32   %[src]!, {q2-q3}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q2-q3}")
            __ASM_EMIT("sub         %[count], $0x10")

            /* 8x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $0x08") // + 0x10 - 0x8
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vldmdb.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("sub         %[count], $0x08")

            /* 4x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $0x04") // + 0x8 - 0x4
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("vldmdb.32   %[src]!, {q0}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {q0}")
            __ASM_EMIT("sub         %[count], $0x04")

            /* 1x blocks */
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], $0x03") // + 0x4 - 0x1
            __ASM_EMIT("blt         2000f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vldmdb.32   %[src]!, {s0}")
            __ASM_EMIT("vstmdb.32   %[dst]!, {s0}")
            __ASM_EMIT("subs        %[count], $0x01")
            __ASM_EMIT("bge         11b")
            __ASM_EMIT("b           2000f")

            // Address-increasing copy
            __ASM_EMIT("1000:")
            __ASM_EMIT("subs        %[count], $0x40")
            __ASM_EMIT("blo         2f")

            /* 64x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldmia.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vldmia.32   %[src]!, {q2-q3}")
            __ASM_EMIT("vldmia.32   %[src]!, {q4-q5}")
            __ASM_EMIT("vldmia.32   %[src]!, {q6-q7}")
            __ASM_EMIT("vldmia.32   %[src]!, {q8-q9}")
            __ASM_EMIT("vldmia.32   %[src]!, {q10-q11}")
            __ASM_EMIT("vldmia.32   %[src]!, {q12-q13}")
            __ASM_EMIT("vldmia.32   %[src]!, {q14-q15}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q2-q3}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q4-q5}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q6-q7}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q8-q9}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q10-q11}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q12-q13}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q14-q15}")
            __ASM_EMIT("subs        %[count], $0x40")
            __ASM_EMIT("bhs         1b")

            /* 32x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $0x20") // +0x40 - 0x20
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldmia.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vldmia.32   %[src]!, {q2-q3}")
            __ASM_EMIT("vldmia.32   %[src]!, {q4-q5}")
            __ASM_EMIT("vldmia.32   %[src]!, {q6-q7}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q2-q3}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q4-q5}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q6-q7}")
            __ASM_EMIT("sub         %[count], $0x20")

            /* 16x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $0x10") // + 0x20 - 0x10
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldmia.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vldmia.32   %[src]!, {q2-q3}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q2-q3}")
            __ASM_EMIT("sub         %[count], $0x10")

            /* 8x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $0x08") // + 0x10 - 0x8
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vldmia.32   %[src]!, {q0-q1}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q0-q1}")
            __ASM_EMIT("sub         %[count], $0x08")

            /* 4x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $0x04") // + 0x8 - 0x4
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("vldmia.32   %[src]!, {q0}")
            __ASM_EMIT("vstmia.32   %[dst]!, {q0}")
            __ASM_EMIT("sub         %[count], $0x04")

            /* 1x blocks */
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], $0x03") // + 0x4 - 0x1
            __ASM_EMIT("blt         2000f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vldmia.32   %[src]!, {s0}")
            __ASM_EMIT("vstmia.32   %[dst]!, {s0}")
            __ASM_EMIT("subs        %[count], $0x01")
            __ASM_EMIT("bge         11b")


            __ASM_EMIT("2000:")

            : [src] "+r" (src), [dst] "+r"(dst), [count] "+r" (count) :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

}


#endif /* DSP_ARCH_ARM_NEON_D32_COPY_H_ */
