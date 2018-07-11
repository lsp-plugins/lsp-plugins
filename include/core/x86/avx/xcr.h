/*
 * xcr.h
 *
 *  Created on: 24 февр. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_X86_AVX_XCR_H_
#define CORE_X86_AVX_XCR_H_

#ifndef CORE_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* CORE_X86_AVX_IMPL */

#ifdef __AVX__
    uint64_t read_xcr(umword_t xcr_id)
    {
        uint64_t xcr;

        __asm__ __volatile__
        (
            __ASM_EMIT64("xor       %%rax, %%rax")
            __ASM_EMIT64("xor       %%rdx, %%rdx")
            __ASM_EMIT("xgetbv")
            __ASM_EMIT64("shl       $32, %%rdx")
            __ASM_EMIT64("or        %%rdx, %%rax")
            : __IF_32("=A" (xcr)) __IF_64("=a" (xcr))
            : "c" (xcr_id)
            : __IF_64("%rdx")
        );
        return xcr;
    }
#else
    uint64_t read_xcr(umword_t xcr_id)
    {
        return 0;
    }
#endif /* __AVX__ */

#endif /* CORE_X86_AVX_XCR_H_ */
