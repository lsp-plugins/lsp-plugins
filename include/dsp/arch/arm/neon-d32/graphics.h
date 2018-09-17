/*
 * graphics.h
 *
 *  Created on: 17 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_GRAPHICS_H_
#define DSP_ARCH_ARM_NEON_D32_GRAPHICS_H_

namespace neon_d32
{
    void rgba32_to_bgra32(void *dst, const void *src, size_t count)
    {
        IF_ARCH_ARM(
            uint32_t mask;
            uint32_t t1, t2;
        );

        ARCH_ARM_ASM(
            // 64x blocks
            __ASM_EMIT("subs        %[count], $64")
            __ASM_EMIT("blo         2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
            __ASM_EMIT("vld4.8      {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld4.8      {q4-q5}, [%[src]]!")
            __ASM_EMIT("vld4.8      {q6-q7}, [%[src]]!")
            __ASM_EMIT("vld4.8      {q8-q9}, [%[src]]!")
            __ASM_EMIT("vld4.8      {q10-q11}, [%[src]]!")
            __ASM_EMIT("vld4.8      {q12-q13}, [%[src]]!")
            __ASM_EMIT("vld4.8      {q14-q15}, [%[src]]!")
            __ASM_EMIT("vswp        d0, d2")
            __ASM_EMIT("vswp        d4, d6")
            __ASM_EMIT("vswp        d8, d10")
            __ASM_EMIT("vswp        d12, d4")
            __ASM_EMIT("vswp        d16, d18")
            __ASM_EMIT("vswp        d20, d22")
            __ASM_EMIT("vswp        d24, d26")
            __ASM_EMIT("vswp        d28, d30")
            __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q2-q3}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q4-q5}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q6-q7}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q8-q9}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q10-q11}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q12-q13}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q14-q15}, [%[dst]]!")
            __ASM_EMIT("subs        %[count], $64")
            __ASM_EMIT("bhs         1b")

            // 32x blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $32")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
            __ASM_EMIT("vld4.8      {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld4.8      {q4-q5}, [%[src]]!")
            __ASM_EMIT("vld4.8      {q6-q7}, [%[src]]!")
            __ASM_EMIT("vswp        d0, d2")
            __ASM_EMIT("vswp        d4, d6")
            __ASM_EMIT("vswp        d8, d10")
            __ASM_EMIT("vswp        d12, d4")
            __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q2-q3}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q4-q5}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q6-q7}, [%[dst]]!")
            __ASM_EMIT("sub         %[count], $32")

            // 16x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $16")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
            __ASM_EMIT("vld4.8      {q2-q3}, [%[src]]!")
            __ASM_EMIT("vswp        d0, d2")
            __ASM_EMIT("vswp        d4, d6")
            __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
            __ASM_EMIT("vst4.8      {q2-q3}, [%[dst]]!")
            __ASM_EMIT("sub         %[count], $16")

            // 8x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
            __ASM_EMIT("vswp        d0, d2")
            __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
            __ASM_EMIT("sub         %[count], $16")

            // 4x blocks
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("vld2.8      {q0}, [%[src]]!") // d0 = RB, d1 = GA
            __ASM_EMIT("vshl.i16    d2, d0, $8")
            __ASM_EMIT("vshr.i16    d0, d0, $8")
            __ASM_EMIT("vorr        d0, d2")
            __ASM_EMIT("vst2.8      {q0}, [%[dst]]!")
            __ASM_EMIT("sub         %[count], $4")

            // 1x blocks
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         12f")
            __ASM_EMIT("mov         %[mask], $0xff")
            __ASM_EMIT("orr         %[mask], $0xff, lsl $16")           // mask = ff 00 ff 00
            __ASM_EMIT("11:")
            __ASM_EMIT("ldr         %[t1], [%[src]]!")                  // t1 = R G B A
            __ASM_EMIT("and         %[t2], %[t1], %[mask]")             // t2 = R 0 B 0
            __ASM_EMIT("and         %[t1], %[t1], %[mask], lsl $8")     // t1 = 0 G 0 A
            __ASM_EMIT("orr         %[t1], %[t1], %[t2], lsl $16")      // t1 = 0 G R A
            __ASM_EMIT("orr         %[t1], %[t1], %[t2], lsr $16")      // t1 = B G R A
            __ASM_EMIT("str         %[t1], [%[dst]]!")
            __ASM_EMIT("sub         %[count], $1")
            __ASM_EMIT("bge         11b")

            __ASM_EMIT("12:")
            : "+r" (src), "+r" (dst), "+r" (count)
              (t1) "=&r" (t1), (t2) "=&r" (t2), (mask) "=&r" (mask)
            : "m" (MASK)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}


#endif /* DSP_ARCH_ARM_NEON_D32_GRAPHICS_H_ */
