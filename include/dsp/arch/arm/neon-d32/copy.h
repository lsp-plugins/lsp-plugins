/*
 * copy.h
 *
 *  Created on: 3 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_COPY_H_
#define DSP_ARCH_ARM_NEON_D32_COPY_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void copy(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("cmp         %[dst], %[src]")
            __ASM_EMIT("beq         10f")
            __ASM_EMIT("subs        %[count], $0x20")
            __ASM_EMIT("blo         2f")

            /* 32x block */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[src]!, {d0-d15}")
            __ASM_EMIT("subs        %[count], $0x20")
            __ASM_EMIT("vstm        %[dst]!, {d0-d15}")
            __ASM_EMIT("bhs         1b")

            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $0x10") // + 0x20 - 0x10
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[src]!, {d0-d7}")
            __ASM_EMIT("sub         %[count], $0x10")
            __ASM_EMIT("vstm        %[dst]!, {d0-d7}")

            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $0x08") // + 0x10 - 0x8
            __ASM_EMIT("blt         5f")
            __ASM_EMIT("vld1.32     {q0-q1}, [%[src]]!")
            __ASM_EMIT("sub         %[count], $0x08")
            __ASM_EMIT("vst1.32     {q0-q1}, [%[dst]]!")

            /* 4x block */
            __ASM_EMIT("5:")
            __ASM_EMIT("adds        %[count], $0x04") // + 0x8 - 0x4
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vld1.32     {q0}, [%[src]]!")
            __ASM_EMIT("sub         %[count], $0x04")
            __ASM_EMIT("vst1.32     {q0}, [%[dst]]!")

            /* 1x blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $0x03") // + 0x4 - 0x1
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")
            __ASM_EMIT("subs        %[count], $0x01")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
            __ASM_EMIT("bge         9b")

            __ASM_EMIT("10:")

            : [src] "+r" (src), [dst] "+r"(dst),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
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
            __ASM_EMIT("subs        %[count], $32")
            __ASM_EMIT("blo         2f")

            /* 32x block */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldmdb      %[src]!, {d0-d15}")
            __ASM_EMIT("subs        %[count], $32")
            __ASM_EMIT("vstmdb      %[dst]!, {d0-d15}")
            __ASM_EMIT("bhs         1b")

            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $16") // + 0x20 - 0x10
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldmdb      %[src]!, {d0-d7}")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstmdb      %[dst]!, {d0-d7}")

            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $8") // + 0x10 - 0x8
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldmdb      %[src]!, {d0-d3}")
            __ASM_EMIT("subs        %[count], $8")
            __ASM_EMIT("vstmdb      %[dst]!, {d0-d3}")

            /* 4x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $4") // + 0x8 - 0x4
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vldmdb      %[src]!, {q0}")
            __ASM_EMIT("vstmdb      %[dst]!, {q0}")
            __ASM_EMIT("sub         %[count], $0x04")

            /* 1x blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $0x03") // + 0x4 - 0x1
            __ASM_EMIT("blt         2000f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vldmdb      %[src]!, {s0}")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vstmdb      %[dst]!, {s0}")
            __ASM_EMIT("bge         9b")
            __ASM_EMIT("b           2000f")

            // Address-increasing copy
            __ASM_EMIT("1000:")
            __ASM_EMIT("subs        %[count], $0x20")
            __ASM_EMIT("blo         2f")

            /* 32x block */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[src]!, {d0-d15}")
            __ASM_EMIT("subs        %[count], $0x20")
            __ASM_EMIT("vstm        %[dst]!, {d0-d15}")
            __ASM_EMIT("bhs         1b")

            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $0x10") // + 0x20 - 0x10
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[src]!, {d0-d7}")
            __ASM_EMIT("sub         %[count], $0x10")
            __ASM_EMIT("vstm        %[dst]!, {d0-d7}")

            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $0x08") // + 0x10 - 0x8
            __ASM_EMIT("blt         5f")
            __ASM_EMIT("vld1.32     {q0-q1}, [%[src]]!")
            __ASM_EMIT("sub         %[count], $0x08")
            __ASM_EMIT("vst1.32     {q0-q1}, [%[dst]]!")

            /* 4x block */
            __ASM_EMIT("5:")
            __ASM_EMIT("adds        %[count], $0x04") // + 0x8 - 0x4
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vld1.32     {q0}, [%[src]]!")
            __ASM_EMIT("sub         %[count], $0x04")
            __ASM_EMIT("vst1.32     {q0}, [%[dst]]!")

            /* 1x blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $0x03") // + 0x4 - 0x1
            __ASM_EMIT("blt         2000f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")
            __ASM_EMIT("subs        %[count], $0x01")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
            __ASM_EMIT("bge         9b")

            __ASM_EMIT("2000:")

            : [src] "+r" (src), [dst] "+r"(dst), [count] "+r" (count) :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

    void fill_zero(float *dst, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs        %[count], $32")
            __ASM_EMIT("veor        q0, q0")
            __ASM_EMIT("veor        q1, q1")
            __ASM_EMIT("blo         2f")

            /* 32x block */
            __ASM_EMIT("veor        q2, q2")
            __ASM_EMIT("veor        q3, q3")
            __ASM_EMIT("veor        q4, q4")
            __ASM_EMIT("veor        q5, q5")
            __ASM_EMIT("veor        q6, q6")
            __ASM_EMIT("veor        q7, q7")

            __ASM_EMIT("1:")
            __ASM_EMIT("subs        %[count], $32")
            __ASM_EMIT("vstm        %[dst]!, {q0-q7}")
            __ASM_EMIT("bhs         1b")

            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $16")
            __ASM_EMIT("blt         4f")

            __ASM_EMIT("veor        q2, q2")
            __ASM_EMIT("veor        q3, q3")
            __ASM_EMIT("sub         %[count], $16")
            __ASM_EMIT("vstm        %[dst]!, {q0-q3}")

            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         5f")
            __ASM_EMIT("sub         %[count], $0x08")
            __ASM_EMIT("vstm        %[dst]!, {q0-q1}")

            /* 4x block */
            __ASM_EMIT("5:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vst1.32     {q0}, [%[dst]]!")

            /* 1x blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
            __ASM_EMIT("bge         9b")

            __ASM_EMIT("10:")

            : [dst] "+r"(dst),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

    void fill(float *dst, float value, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vldr        s0, [%[pv]]")
            __ASM_EMIT("subs        %[count], $32")
            __ASM_EMIT("vdup.32     q0, d0[0]")
            __ASM_EMIT("vmov        q1, q0")
            __ASM_EMIT("blo         2f")

            /* 32x block */
            __ASM_EMIT("vmov        q2, q0")
            __ASM_EMIT("vmov        q3, q1")
            __ASM_EMIT("vmov        q4, q0")
            __ASM_EMIT("vmov        q5, q1")
            __ASM_EMIT("vmov        q6, q0")
            __ASM_EMIT("vmov        q7, q1")

            __ASM_EMIT("1:")
            __ASM_EMIT("subs        %[count], $32")
            __ASM_EMIT("vstm        %[dst]!, {q0-q7}")
            __ASM_EMIT("bhs         1b")

            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $16")
            __ASM_EMIT("blt         4f")

            __ASM_EMIT("vmov        q2, q0")
            __ASM_EMIT("vmov        q3, q1")
            __ASM_EMIT("sub         %[count], $16")
            __ASM_EMIT("vstm        %[dst]!, {q0-q3}")

            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         5f")
            __ASM_EMIT("sub         %[count], $0x08")
            __ASM_EMIT("vstm        %[dst]!, {q0-q1}")

            /* 4x block */
            __ASM_EMIT("5:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vst1.32     {q0}, [%[dst]]!")

            /* 1x blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
            __ASM_EMIT("bge         9b")

            __ASM_EMIT("10:")

            : [dst] "+r"(dst),
              [count] "+r" (count)
            : [pv] "r" (&value)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

    void fill_one(float *dst, size_t count)
    {
        fill(dst, 1.0f, count);
    }

    void fill_minus_one(float *dst, size_t count)
    {
        fill(dst, -1.0f, count);
    }

    void reverse1(float *dst, size_t count)
    {
        IF_ARCH_ARM(float *src);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("add         %[src], %[dst], %[count], LSL $2")
            __ASM_EMIT("lsr         %[count], $1")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("blo         2f")

            /* 16x block */
            __ASM_EMIT("1:")
            __ASM_EMIT("sub         %[src], $0x40")
            __ASM_EMIT("vldm        %[dst], {q4-q7}")
            __ASM_EMIT("vldm        %[src], {q0-q3}")
            __ASM_EMIT("vrev64.f32  q0, q0")
            __ASM_EMIT("vrev64.f32  q1, q1")
            __ASM_EMIT("vrev64.f32  q2, q2")
            __ASM_EMIT("vrev64.f32  q3, q3")
            __ASM_EMIT("vrev64.f32  q4, q4")
            __ASM_EMIT("vrev64.f32  q5, q5")
            __ASM_EMIT("vrev64.f32  q6, q6")
            __ASM_EMIT("vrev64.f32  q7, q7")
            __ASM_EMIT("vext.32     q0, q0, q0, $2")
            __ASM_EMIT("vext.32     q1, q1, q1, $2")
            __ASM_EMIT("vext.32     q2, q2, q2, $2")
            __ASM_EMIT("vext.32     q3, q3, q3, $2")
            __ASM_EMIT("vext.32     q4, q4, q4, $2")
            __ASM_EMIT("vext.32     q5, q5, q5, $2")
            __ASM_EMIT("vext.32     q6, q6, q6, $2")
            __ASM_EMIT("vext.32     q7, q7, q7, $2")
            __ASM_EMIT("vswp        q0, q3")
            __ASM_EMIT("vswp        q1, q2")
            __ASM_EMIT("vswp        q4, q7")
            __ASM_EMIT("vswp        q5, q6")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
            __ASM_EMIT("vstm        %[src], {q4-q7}")
            __ASM_EMIT("bhs         1b")

            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         4f")

            __ASM_EMIT("sub         %[src], $0x20")
            __ASM_EMIT("vldm        %[dst], {q4-q5}")
            __ASM_EMIT("vldm        %[src], {q0-q1}")
            __ASM_EMIT("vrev64.f32  q0, q0")
            __ASM_EMIT("vrev64.f32  q1, q1")
            __ASM_EMIT("vrev64.f32  q4, q4")
            __ASM_EMIT("vrev64.f32  q5, q5")
            __ASM_EMIT("vext.32     q0, q0, q0, $2")
            __ASM_EMIT("vext.32     q1, q1, q1, $2")
            __ASM_EMIT("vext.32     q4, q4, q4, $2")
            __ASM_EMIT("vext.32     q5, q5, q5, $2")
            __ASM_EMIT("vswp        q0, q1")
            __ASM_EMIT("vswp        q4, q5")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
            __ASM_EMIT("vstm        %[src], {q4-q5}")

            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("sub         %[src], $0x10")
            __ASM_EMIT("vldm        %[dst], {q4}")
            __ASM_EMIT("vldm        %[src], {q0}")
            __ASM_EMIT("vrev64.f32  q0, q0")
            __ASM_EMIT("vrev64.f32  q4, q4")
            __ASM_EMIT("vext.32     q0, q0, q0, $2")
            __ASM_EMIT("vext.32     q4, q4, q4, $2")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vstm        %[src], {q4}")
            __ASM_EMIT("vstm        %[dst]!, {q0}")

            /* 1x blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("9:")
            __ASM_EMIT("sub         %[src], $0x04")
            __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[dst]]")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d8[0]}, [%[src]]")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
            __ASM_EMIT("bge         9b")

            __ASM_EMIT("8:")

            : [dst] "+r"(dst), [src] "=&r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

    void reverse2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("add         %[dst], %[dst], %[count], LSL $2")
            __ASM_EMIT("subs        %[count], $32")
            __ASM_EMIT("blo         2f")
            /* 32x block */
            __ASM_EMIT("1:")
            __ASM_EMIT("sub         %[dst], $0x80")
            __ASM_EMIT("vldm        %[src]!, {q0-q7}")
            __ASM_EMIT("vrev64.f32  q0, q0")
            __ASM_EMIT("vrev64.f32  q1, q1")
            __ASM_EMIT("vrev64.f32  q2, q2")
            __ASM_EMIT("vrev64.f32  q3, q3")
            __ASM_EMIT("vrev64.f32  q4, q4")
            __ASM_EMIT("vrev64.f32  q5, q5")
            __ASM_EMIT("vrev64.f32  q6, q6")
            __ASM_EMIT("vrev64.f32  q7, q7")
            __ASM_EMIT("vext.32     q0, q0, q0, $2")
            __ASM_EMIT("vext.32     q1, q1, q1, $2")
            __ASM_EMIT("vext.32     q2, q2, q2, $2")
            __ASM_EMIT("vext.32     q3, q3, q3, $2")
            __ASM_EMIT("vext.32     q4, q4, q4, $2")
            __ASM_EMIT("vext.32     q5, q5, q5, $2")
            __ASM_EMIT("vext.32     q6, q6, q6, $2")
            __ASM_EMIT("vext.32     q7, q7, q7, $2")
            __ASM_EMIT("vswp        q0, q7")
            __ASM_EMIT("vswp        q1, q6")
            __ASM_EMIT("vswp        q2, q5")
            __ASM_EMIT("vswp        q3, q4")
            __ASM_EMIT("subs        %[count], $32")
            __ASM_EMIT("vstm        %[dst], {q0-q7}")
            __ASM_EMIT("bhs         1b")
            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         %[count], $16")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("sub         %[dst], $0x40")
            __ASM_EMIT("vldm        %[src]!, {q0-q3}")
            __ASM_EMIT("vrev64.f32  q0, q0")
            __ASM_EMIT("vrev64.f32  q1, q1")
            __ASM_EMIT("vrev64.f32  q2, q2")
            __ASM_EMIT("vrev64.f32  q3, q3")
            __ASM_EMIT("vext.32     q0, q0, q0, $2")
            __ASM_EMIT("vext.32     q1, q1, q1, $2")
            __ASM_EMIT("vext.32     q2, q2, q2, $2")
            __ASM_EMIT("vext.32     q3, q3, q3, $2")
            __ASM_EMIT("vswp        q0, q3")
            __ASM_EMIT("vswp        q1, q2")
            __ASM_EMIT("sub         %[count], $16")
            __ASM_EMIT("vstm        %[dst], {q0-q3}")
            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         %[count], $8")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("sub         %[dst], $0x20")
            __ASM_EMIT("vldm        %[src]!, {q0-q1}")
            __ASM_EMIT("vrev64.f32  q0, q0")
            __ASM_EMIT("vrev64.f32  q1, q1")
            __ASM_EMIT("vext.32     q0, q0, q0, $2")
            __ASM_EMIT("vext.32     q1, q1, q1, $2")
            __ASM_EMIT("vswp        q0, q1")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vstm        %[dst], {q0-q1}")
            /* 4x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("add         %[count], $4")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("sub         %[dst], $0x10")
            __ASM_EMIT("vldm        %[src]!, {q0}")
            __ASM_EMIT("vrev64.f32  q0, q0")
            __ASM_EMIT("vext.32     q0, q0, q0, $2")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vstm        %[dst], {q0}")
            /* 1x blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("sub         %[dst], $0x04")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]")
            __ASM_EMIT("bge         9b")
            /* end */
            __ASM_EMIT("10:")

            : [dst] "+r"(dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }
}


#endif /* DSP_ARCH_ARM_NEON_D32_COPY_H_ */
