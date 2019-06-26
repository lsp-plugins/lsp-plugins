/*
 * graphics.h
 *
 *  Created on: 27 авг. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_GRAPHICS_H_
#define DSP_ARCH_X86_GRAPHICS_H_

namespace x86
{
#ifdef ARCH_I386
    // Limited number of registers
    void rgba32_to_bgra32(void *dst, const void *src, size_t count)
    {
        uint32_t t1, t2;

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test    %[count], %[count]")
            __ASM_EMIT("jz      2f")

            // Loop multiple of 4
            __ASM_EMIT("1:")
            __ASM_EMIT("mov     (%[src]), %[t1]")       // t1 = *src = RGBA
            __ASM_EMIT("mov     %[t1], %[t2]")          // t1 = *src = RGBA
            __ASM_EMIT("and     $0x00ff00ff, %[t1]")    // t1 = R0B0
            __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
            __ASM_EMIT("ror     $16, %[t1]")            // t1 = B0R0
            __ASM_EMIT("add     $0x4, %[src]")          // src ++
            __ASM_EMIT("or      %[t2], %[t1]")          // t1 = BGRA
            __ASM_EMIT("mov     %[t1], (%[dst])")       // *dst = BGRA
            __ASM_EMIT("add     $0x4, %[dst]")          // dst ++
            __ASM_EMIT("dec     %[count]")              // count --
            __ASM_EMIT("jnz     1b")                    // count > 0 ?

            // Loop not multiple of 4
            __ASM_EMIT("2:")

            : [count] "+r" (count),
              [dst] "+r"(dst),
              [src] "+r"(src),
              [t1] "=&r" (t1),
              [t2] "=&r" (t2)
            :
            : "cc", "memory"
        );
    }
#endif /* ARCH_I386 */

#ifdef ARCH_X86_64
    void rgba32_to_bgra32(void *dst, const void *src, size_t count)
    {
        uint32_t t1, t2, t3;

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("sub     $4, %[count]")
            __ASM_EMIT("jb      2f")

            // Loop multiple of 4
            __ASM_EMIT("1:")
            __ASM_EMIT("mov     0x00(%[src]), %[t1]")   // t1 = src[0] = RGBA
            __ASM_EMIT("mov     %[t1], %[t2]")          // t1 = src[0] = RGBA
            __ASM_EMIT("and     $0x00ff00ff, %[t1]")    // t1 = R0B0
            __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
            __ASM_EMIT("ror     $16, %[t1]")            // t1 = B0R0
            __ASM_EMIT("mov     0x04(%[src]), %[t3]")   // t3 = src[1] = RGBA
            __ASM_EMIT("or      %[t2], %[t1]")          // t1 = BGRA
            __ASM_EMIT("mov     %[t1], 0x00(%[dst])")   // dst[0] = BGRA

            __ASM_EMIT("mov     %[t3], %[t2]")          // t2 = RGBA
            __ASM_EMIT("and     $0x00ff00ff, %[t3]")    // t3 = R0B0
            __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
            __ASM_EMIT("ror     $16, %[t3]")            // t3 = B0R0
            __ASM_EMIT("mov     0x08(%[src]), %[t1]")   // t1 = src[2] = RGBA
            __ASM_EMIT("or      %[t2], %[t3]")          // t3 = BGRA
            __ASM_EMIT("mov     %[t3], 0x04(%[dst])")   // dst[1] = BGRA

            __ASM_EMIT("mov     %[t1], %[t2]")          // t2 = RGBA
            __ASM_EMIT("and     $0x00ff00ff, %[t1]")    // t1 = R0B0
            __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
            __ASM_EMIT("ror     $16, %[t1]")            // t1 = B0R0
            __ASM_EMIT("mov     0x0c(%[src]), %[t3]")   // t3 = src[3] = RGBA
            __ASM_EMIT("or      %[t2], %[t1]")          // t1 = BGRA
            __ASM_EMIT("mov     %[t1], 0x08(%[dst])")   // dst[2] = BGRA

            __ASM_EMIT("mov     %[t3], %[t2]")          // t2 = RGBA
            __ASM_EMIT("and     $0x00ff00ff, %[t3]")    // t3 = R0B0
            __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
            __ASM_EMIT("ror     $16, %[t3]")            // t3 = B0R0
            __ASM_EMIT("add     $0x10, %[src]")         // src += 16
            __ASM_EMIT("or      %[t2], %[t3]")          // t3 = BGRA
            __ASM_EMIT("mov     %[t3], 0x0c(%[dst])")   // dst[3] = BGRA

            __ASM_EMIT("add     $0x10, %[dst]")         // dst += 16
            __ASM_EMIT("sub     $4, %[count]")          // count -= 4
            __ASM_EMIT("jae     1b")

            // Loop not multiple of 4
            __ASM_EMIT("2:")
            __ASM_EMIT("add     $3, %[count]")
            __ASM_EMIT("jl      4f")

            // Complete tail
            __ASM_EMIT("3:")
            __ASM_EMIT("mov     (%[src]), %[t1]")       // t1 = *src = RGBA
            __ASM_EMIT("mov     %[t1], %[t2]")          // t2 = t1 RGBA
            __ASM_EMIT("add     $4, %[src]")            // src++
            __ASM_EMIT("and     $0x00ff00ff, %[t1]")    // t1 = R0B0
            __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
            __ASM_EMIT("ror     $16, %[t1]")            // t1 = B0R0
            __ASM_EMIT("or      %[t2], %[t1]")          // t1 = BGRA
            __ASM_EMIT("mov     %[t1], (%[dst])")       // *dst = BGRA
            __ASM_EMIT("add     $4, %[dst]")            // dst++
            __ASM_EMIT("dec     %[count]")              // count--
            __ASM_EMIT("jge     3b")

            __ASM_EMIT("4:")

            : [count] "+r" (count),
              [dst] "+r"(dst),
              [src] "+r"(src),
              [t1] "=&r" (t1),
              [t2] "=&r" (t2),
              [t3] "=&r" (t3)
            :
            : "cc", "memory"
        );
    }
    #endif /* ARCH_X86_64 */

    // Limited number of registers
    void abgr32_to_bgra32(void *dst, const void *src, size_t count)
    {
        uint32_t tmp;

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test    %[count], %[count]")
            __ASM_EMIT("jz      2f")

            // Loop multiple of 4
            __ASM_EMIT("1:")
            __ASM_EMIT("lodsl") // eax = *src = RGBA
            __ASM_EMIT("ror     $8, %%eax")             // eax = ARGB
            __ASM_EMIT("stosl") // eax = *src = RGBA
            __ASM_EMIT("loop    1b")                    // count > 0 ?

            // Loop not multiple of 4
            __ASM_EMIT("2:")

            : [count] "+c" (count),
              [dst] "+D"(dst),
              [src] "+S"(src),
              [tmp] "=a"(tmp)
            :
            : "cc", "memory"
        );
    }
}

#endif /* DSP_ARCH_X86_GRAPHICS_H_ */
