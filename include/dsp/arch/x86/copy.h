/*
 * copy.h
 *
 *  Created on: 4 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_COPY_H_
#define DSP_ARCH_X86_COPY_H_

namespace x86
{
    void copy(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t tmp);

        ARCH_X86_64_ASM
        (
            __ASM_EMIT("mov     %[count], %[tmp]")
            __ASM_EMIT("shr     $1, %[count]")
            __ASM_EMIT("jz      1f")
            __ASM_EMIT("rep     movsq")
            __ASM_EMIT("1:")
            __ASM_EMIT("test    $1, %[tmp]")
            __ASM_EMIT("jz      2f")
            __ASM_EMIT("movsd   ")
            __ASM_EMIT("2:")
            : [dst] "+D" (dst), [src] "+S" (src),
              [tmp] "=&r" (tmp), [count] "+c" (count)
            :
            : "cc", "memory"
        );

        ARCH_I386_ASM
        (
            __ASM_EMIT("rep     movsl")
            : [dst] "+D" (dst), [src] "+S" (src),
              [count] "+c" (count)
            :
            : "cc", "memory"
        );
    }
}

#endif /* DSP_ARCH_X86_COPY_H_ */
