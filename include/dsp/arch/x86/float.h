/*
 * float.h
 *
 *  Created on: 05 авг. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_FLOAT_H_
#define DSP_ARCH_X86_FLOAT_H_

#include <core/types.h>

namespace x86
{
    static uint32_t fpu_read_cr()
    {
        uint16_t cr = 0;

        ARCH_X86_ASM
        (
            __ASM_EMIT("fstcw   %[cr]")

            : [cr] "+m" (cr)
            :
            : "memory"
        );

        return cr;
    }

    static void fpu_write_cr(uint32_t value)
    {
        uint16_t cr     = value;

        ARCH_X86_ASM
        (
            __ASM_EMIT("fldcw   %[cr]")

            :
            : [cr] "m" (cr)
            :
        );
    }
}

#endif /* DSP_ARCH_X86_FLOAT_H_ */
