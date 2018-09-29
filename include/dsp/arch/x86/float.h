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
    static const uint32_t fpu_p_nan_i[] __lsp_aligned16 = { FLOAT_SAT_P_NAN_I };
    static const uint32_t fpu_n_nan_i[] __lsp_aligned16 = { FLOAT_SAT_N_NAN_I };
    static const uint32_t fpu_p_inf_i[] __lsp_aligned16 = { FLOAT_SAT_P_INF_I };
    static const uint32_t fpu_n_inf_i[] __lsp_aligned16 = { FLOAT_SAT_N_INF_I };

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

    void copy_saturated(float *dst, const float *src, size_t count)
    {
        register uint32_t tmp;

        #define STORE_VALUE_REPEAT(c)           \
            __ASM_EMIT("mov %[" #c "], %[tmp]")  \
            __ASM_EMIT("stosl")   \
            __ASM_EMIT("dec %[count]")  \
            __ASM_EMIT("jnz 1b")

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("cld")
            __ASM_EMIT("test %[count], %[count]")
            __ASM_EMIT("jz 100f")

            // Perform main loop
            __ASM_EMIT("1:")

            // Load value
            __ASM_EMIT("lodsl")

            // Test for sign
            __ASM_EMIT("test %[tmp], %[tmp]")
            __ASM_EMIT("js 50f")

            // Positive part
            // Test for +inf, +nan
            __ASM_EMIT("cmp $0x7F800000, %[tmp]")
            __ASM_EMIT("je 10f")
            __ASM_EMIT("ja 20f")

            // Store non-updated value and repeat cycle
            __ASM_EMIT("stosl")
            __ASM_EMIT("dec %[count]")
            __ASM_EMIT("jnz 1b")
            __ASM_EMIT("jmp 100f")

            // Negative part
            // Test for -inf, -nan
            __ASM_EMIT("50:")
            __ASM_EMIT("cmp $0xFF800000, %[tmp]")
            __ASM_EMIT("je 60f")
            __ASM_EMIT("ja 70f")

            // Store non-updated value and repeat cycle
            __ASM_EMIT("stosl")
            __ASM_EMIT("dec %[count]")
            __ASM_EMIT("jnz 1b")
            __ASM_EMIT("jmp 100f")

            // Replacement code
            __ASM_EMIT("10:")
                STORE_VALUE_REPEAT(X_P_INF)
                __ASM_EMIT("jmp 100f")
            __ASM_EMIT("20:")
                STORE_VALUE_REPEAT(X_P_NAN)
                __ASM_EMIT("jmp 100f")
            __ASM_EMIT("60:")
                STORE_VALUE_REPEAT(X_N_INF)
                __ASM_EMIT("jmp 100f")
            __ASM_EMIT("70:")
                STORE_VALUE_REPEAT(X_N_NAN)

            // End of cycle
            __ASM_EMIT("100:")

            : [dst] "+D" (dst), [src] "+S" (src), [tmp] "=&a" (tmp), [count] "+r" (count)
            :
                [X_P_NAN] "i" (uint32_t(FLOAT_SAT_P_NAN_I)),
                [X_N_NAN] "i" (uint32_t(FLOAT_SAT_N_NAN_I)),
                [X_P_INF] "i" (uint32_t(FLOAT_SAT_P_INF_I)),
                [X_N_INF] "i" (uint32_t(FLOAT_SAT_N_INF_I))
            : "memory", "cc"
        );

        #undef STORE_VALUE_REPEAT
    }

    void saturate(float *dst, size_t count)
    {
        register uint32_t tmp;

        #define STORE_VALUE_REPEAT(c)     \
            __ASM_EMIT("movl %[" #c "], -0x4(%[dst])")   \
            __ASM_EMIT("dec %[count]") \
            __ASM_EMIT("jnz 1b")

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("cld")
            __ASM_EMIT("test %[count], %[count]")
            __ASM_EMIT("jz 100f")

            // Perform main loop
            __ASM_EMIT("1:")

            // Load value
            __ASM_EMIT("lodsl")

            // Test for sign
            __ASM_EMIT("test %[tmp], %[tmp]")
            __ASM_EMIT("js 50f")

            // Positive part
            // Test for +inf, +nan
            __ASM_EMIT("cmp $0x7F800000, %[tmp]")
            __ASM_EMIT("je 10f")
            __ASM_EMIT("ja 20f")

            // Repeat cycle
            __ASM_EMIT("dec %[count]")
            __ASM_EMIT("jnz 1b")
            __ASM_EMIT("jmp 100f")

            // Negative part
            // Test for -inf, -nan
            __ASM_EMIT("50:")
            __ASM_EMIT("cmp $0xFF800000, %[tmp]")
            __ASM_EMIT("je 60f")
            __ASM_EMIT("ja 70f")

            // Repeat cycle
            __ASM_EMIT("dec %[count]")
            __ASM_EMIT("jnz 1b")
            __ASM_EMIT("jmp 100f")

            // Replacement code
            __ASM_EMIT("10:")
                STORE_VALUE_REPEAT(X_P_INF)
                __ASM_EMIT("jmp 100f")
            __ASM_EMIT("20:")
                STORE_VALUE_REPEAT(X_P_NAN)
                __ASM_EMIT("jmp 100f")
            __ASM_EMIT("60:")
                STORE_VALUE_REPEAT(X_N_INF)
                __ASM_EMIT("jmp 100f")
            __ASM_EMIT("70:")
                STORE_VALUE_REPEAT(X_N_NAN)

            // End of cycle
            __ASM_EMIT("100:")

            : [dst] "+S" (dst), [tmp] "=&a" (tmp), [count] "+r" (count)
            :
                [X_P_NAN] "i" (uint32_t(FLOAT_SAT_P_NAN_I)),
                [X_N_NAN] "i" (uint32_t(FLOAT_SAT_N_NAN_I)),
                [X_P_INF] "i" (uint32_t(FLOAT_SAT_P_INF_I)),
                [X_N_INF] "i" (uint32_t(FLOAT_SAT_N_INF_I))
            : "memory", "cc"
        );

        #undef STORE_VALUE_REPEAT
    }

    void copy_saturated_cmov(float *dst, const float *src, size_t count)
    {
        register uint32_t tmp;

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("cld")
            __ASM_EMIT("test %[count], %[count]")
            __ASM_EMIT("jz 100f")

            // Perform main loop
            __ASM_EMIT("1:")

            // Load value
            __ASM_EMIT("lodsl")

            // Test for sign
            __ASM_EMIT("test %[tmp], %[tmp]")
            __ASM_EMIT("js 50f")

            // Positive part
            // Test for +inf, +nan
            __ASM_EMIT("cmp $0x7F800000, %[tmp]")
            __ASM_EMIT("cmove %[X_P_INF], %[tmp]")
            __ASM_EMIT("cmova %[X_P_NAN], %[tmp]")

            // Store updated value and repeat cycle
            __ASM_EMIT("stosl")
            __ASM_EMIT("dec %[count]")
            __ASM_EMIT("jnz 1b")
            __ASM_EMIT("jmp 100f")

            // Negative part
            // Test for -inf, -nan
            __ASM_EMIT("50:")
            __ASM_EMIT("cmp $0xFF800000, %[tmp]")
            __ASM_EMIT("cmove %[X_N_INF], %[tmp]")
            __ASM_EMIT("cmova %[X_N_NAN], %[tmp]")

            // Store updated value and repeat cycle
            __ASM_EMIT("stosl")
            __ASM_EMIT("dec %[count]")
            __ASM_EMIT("jnz 1b")

            // End of cycle
            __ASM_EMIT("100:")

            : [dst] "+D" (dst), [src] "+S" (src), [tmp] "=&a" (tmp), [count] "+r" (count)
            :
                [X_P_NAN] "m" (fpu_p_nan_i),
                [X_N_NAN] "m" (fpu_n_nan_i),
                [X_P_INF] "m" (fpu_p_inf_i),
                [X_N_INF] "m" (fpu_n_inf_i)
            : "memory", "cc"
        );
    }

    void saturate_cmov(float *dst, size_t count)
    {
        register uint32_t tmp;

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("cld")
            __ASM_EMIT("test %[count], %[count]")
            __ASM_EMIT("jz 100f")

            // Perform main loop
            __ASM_EMIT("1:")

            // Load value
            __ASM_EMIT("lodsl")

            // Test for sign
            __ASM_EMIT("test %[tmp], %[tmp]")
            __ASM_EMIT("js 10f")

            // Positive part
            // Test for +inf, +nan
            __ASM_EMIT("cmp $0x7F800000, %[tmp]")
            __ASM_EMIT("jae 50f")
            __ASM_EMIT("dec %[count]")
            __ASM_EMIT("jnz 1b")
            __ASM_EMIT("jmp 100f")

            // Negative part
            // Test for -inf, -nan
            __ASM_EMIT("10:")
            __ASM_EMIT("cmp $0xFF800000, %[tmp]")
            __ASM_EMIT("jae 60f")
            __ASM_EMIT("dec %[count]")
            __ASM_EMIT("jnz 1b")
            __ASM_EMIT("jmp 100f")

            // Update value
            __ASM_EMIT("50:")
            __ASM_EMIT("cmove %[X_P_INF], %[tmp]")
            __ASM_EMIT("cmova %[X_P_NAN], %[tmp]")
            __ASM_EMIT("mov %[tmp], -4(%[dst])")
            __ASM_EMIT("dec %[count]")
            __ASM_EMIT("jnz 1b")
            __ASM_EMIT("jmp 100f")

            // Update value
            __ASM_EMIT("60:")
            __ASM_EMIT("cmove %[X_N_INF], %[tmp]")
            __ASM_EMIT("cmova %[X_N_NAN], %[tmp]")
            __ASM_EMIT("mov %[tmp], -4(%[dst])")
            __ASM_EMIT("dec %[count]")
            __ASM_EMIT("jnz 1b")

            // End of cycle
            __ASM_EMIT("100:")

            : [dst] "+S" (dst), [tmp] "=&a" (tmp), [count] "+r" (count)
            :
                [X_P_NAN] "m" (fpu_p_nan_i),
                [X_N_NAN] "m" (fpu_n_nan_i),
                [X_P_INF] "m" (fpu_p_inf_i),
                [X_N_INF] "m" (fpu_n_inf_i)
            : "memory", "cc"
        );

        #undef STORE_VALUE_REPEAT
    }
}

#endif /* DSP_ARCH_X86_FLOAT_H_ */
