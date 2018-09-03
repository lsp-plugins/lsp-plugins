/*
 * vector.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_VECTOR_H_
#define DSP_ARCH_X86_AVX_VECTOR_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    #if 0
    static void multiply(float *dst, const float *src1, const float *src2, size_t count)
    {
        if (count == 0)
            return;

        ARCH_X86_ASM
        (
            __ASM_EMIT("1:")

            // Check conditions
            __ASM_EMIT("test $0x01f, %2")
            __ASM_EMIT("jz 2f")

            // Process data
            __ASM_EMIT("vmovss (%[src1]), %%xmm0")
            __ASM_EMIT("vmulss (%[src2]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss %%xmm0, (%[dst])")

            // Move pointers
            __ASM_EMIT("add $0x4, %[src1]")
            __ASM_EMIT("add $0x4, %[src2]")
            __ASM_EMIT("add $0x4, %[dst]")

            // Repeat loop
            __ASM_EMIT("dec %[count]")
            __ASM_EMIT("jnz 1b")
            __ASM_EMIT("2:")

            : [src1] "+r" (src1), [src2] "+r"(src2), [dst]"+r"(dst), [count]"+r" (count) :
            : "cc", "memory",
              "%xmm0", "%xmm1"
        );

        size_t regs     = count / AVX_MULTIPLE;
        count          %= AVX_MULTIPLE;

        if (regs > 0)
        {
            #ifdef __i386__
            size_t blocks   = regs / 4;
            regs           %= 4;
            #else
            size_t blocks   = regs / 8;
            regs           %= 8;
            #endif /* __i386__ */

            // Prefetch data for iteration
            ARCH_X86_ASM
            (
                __ASM_EMIT      ("prefetchnta  0x000(%[src1])")
                __ASM_EMIT      ("prefetchnta  0x040(%[src1])")
                __ASM_EMIT64    ("prefetchnta  0x080(%[src1])")
                __ASM_EMIT64    ("prefetchnta  0x0c0(%[src1])")
                __ASM_EMIT      ("prefetchnta  0x000(%[src2])")
                __ASM_EMIT      ("prefetchnta  0x040(%[src2])")
                __ASM_EMIT64    ("prefetchnta  0x080(%[src2])")
                __ASM_EMIT64    ("prefetchnta  0x0c0(%[src2])")
                : : [src1] "r" (src1), [src2] "r" (src2)
            );

            #define avx_multiply(l_s1, l_s2)   \
            { \
                if (blocks > 0) \
                { \
                    ARCH_X86_ASM \
                    ( \
                        __ASM_EMIT("1:") \
                        \
                        __ASM_EMIT      ("prefetchnta  0x100(%[src1])") \
                        __ASM_EMIT      ("prefetchnta  0x140(%[src1])") \
                        __ASM_EMIT64    ("prefetchnta  0x180(%[src1])") \
                        __ASM_EMIT64    ("prefetchnta  0x1c0(%[src1])") \
                        __ASM_EMIT      ("prefetchnta  0x100(%[src2])") \
                        __ASM_EMIT      ("prefetchnta  0x140(%[src2])") \
                        __ASM_EMIT64    ("prefetchnta  0x180(%[src2])") \
                        __ASM_EMIT64    ("prefetchnta  0x1c0(%[src2])") \
                        /* Process data */ \
                        __ASM_EMIT      (l_s1 " 0x000(%[src1]), %%ymm0") \
                        __ASM_EMIT      (l_s1 " 0x020(%[src1]), %%ymm2") \
                        __ASM_EMIT      (l_s1 " 0x040(%[src1]), %%ymm4") \
                        __ASM_EMIT      (l_s1 " 0x060(%[src1]), %%ymm6") \
                        __ASM_EMIT64    (l_s1 " 0x080(%[src1]), %%ymm8") \
                        __ASM_EMIT64    (l_s1 " 0x0a0(%[src1]), %%ymm10") \
                        __ASM_EMIT64    (l_s1 " 0x0c0(%[src1]), %%ymm12") \
                        __ASM_EMIT64    (l_s1 " 0x0e0(%[src1]), %%ymm14") \
                        \
                        __ASM_EMIT      ("vmulps 0x000(%[src2]), %%ymm0, %%ymm0") \
                        __ASM_EMIT      ("vmulps 0x020(%[src2]), %%ymm2, %%ymm2") \
                        __ASM_EMIT      ("vmulps 0x040(%[src2]), %%ymm4, %%ymm4") \
                        __ASM_EMIT      ("vmulps 0x060(%[src2]), %%ymm6, %%ymm6") \
                        __ASM_EMIT64    ("vmulps 0x080(%[src2]), %%ymm8, %%ymm8") \
                        __ASM_EMIT64    ("vmulps 0x0a0(%[src2]), %%ymm10, %%ymm10") \
                        __ASM_EMIT64    ("vmulps 0x0c0(%[src2]), %%ymm12, %%ymm12") \
                        __ASM_EMIT64    ("vmulps 0x0e0(%[src2]), %%ymm14, %%ymm14") \
                        \
                        __ASM_EMIT      (VMOVNTPS " %%ymm0,  0x000(%[dst])") \
                        __ASM_EMIT      (VMOVNTPS " %%ymm2,  0x020(%[dst])") \
                        __ASM_EMIT      (VMOVNTPS " %%ymm4,  0x040(%[dst])") \
                        __ASM_EMIT      (VMOVNTPS " %%ymm6,  0x060(%[dst])") \
                        __ASM_EMIT64    (VMOVNTPS " %%ymm8,  0x080(%[dst])") \
                        __ASM_EMIT64    (VMOVNTPS " %%ymm10, 0x0a0(%[dst])") \
                        __ASM_EMIT64    (VMOVNTPS " %%ymm12, 0x0c0(%[dst])") \
                        __ASM_EMIT64    (VMOVNTPS " %%ymm14, 0x0e0(%[dst])") \
                        \
                        /* Move pointers */ \
                        __ASM_EMIT32    ("add $0x080, %[src1]") \
                        __ASM_EMIT32    ("add $0x080, %[src2]") \
                        __ASM_EMIT32    ("add $0x080, %[dst]") \
                        __ASM_EMIT64    ("add $0x100, %[src1]") \
                        __ASM_EMIT64    ("add $0x100, %[src2]") \
                        __ASM_EMIT64    ("add $0x100, %[dst]") \
                        \
                        /* Repeat loop */ \
                        __ASM_EMIT("dec %[count]") \
                        __ASM_EMIT("jnz 1b") \
                        __ASM_EMIT("2:") \
                        \
                        : [src1] "+r" (src1), [src2] "+r"(src2), [dst] "+r"(dst), [count] "+r" (blocks) : \
                        : "cc", "memory", \
                          __IF_64( \
                            "%xmm8", "%xmm9", "%xmm10", "%xmm11", \
                            "%xmm12", "%xmm13", "%xmm14", "%xmm15", \
                          ) \
                          "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
                          "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
                    ); \
                } \
                \
                if (regs > 0) \
                { \
                    ARCH_X86_ASM \
                    ( \
                        __ASM_EMIT("1:") \
                        \
                        /* Process data */ \
                        __ASM_EMIT      (l_s1 " (%[src1]), %%ymm0") \
                        __ASM_EMIT      ("vmulps (%[src2]), %%ymm0, %%ymm0") \
                        __ASM_EMIT      (VMOVNTPS " %%ymm0, (%[dst])") \
                        \
                        /* Move pointers */ \
                        __ASM_EMIT      ("add $0x20, %[src1]") \
                        __ASM_EMIT      ("add $0x20, %[src2]") \
                        __ASM_EMIT      ("add $0x20, %[dst]") \
                        \
                        /* Repeat loop */ \
                        __ASM_EMIT      ("dec %[count]") \
                        __ASM_EMIT      ("jnz 1b") \
                        __ASM_EMIT      ("2:") \
                        \
                        : [src1] "+r" (src1), [src2] "+r"(src2), [dst] "+r"(dst), [count] "+r" (regs) : \
                        : "cc", "memory", \
                          "%xmm0", "%xmm1" \
                    ); \
                } \
            }

            if (avx_aligned(src1))
            {
                if (avx_aligned(src2))
                    avx_multiply("vmovaps", "vmovaps")
                else
                    avx_multiply("vmovaps", "vmovups")
            }
            else
            {
                if (avx_aligned(src2))
                    avx_multiply("vmovups", "vmovaps")
                else
                    avx_multiply("vmovups", "vmovups")
            }

            #undef avx_multiply
        }

        if (count > 0)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("1:")

                // Process data
                __ASM_EMIT("vmovss (%[src1]), %%xmm0")
                __ASM_EMIT("vmulss (%[src2]), %%xmm0, %%xmm0")
                __ASM_EMIT("vmovss %%xmm0, (%[dst])")

                // Move pointers
                __ASM_EMIT("add $0x4, %[src1]")
                __ASM_EMIT("add $0x4, %[src2]")
                __ASM_EMIT("add $0x4, %[dst]")

                // Repeat loop
                __ASM_EMIT("dec %[count]")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : [src1] "+r" (src1), [src2] "+r"(src2), [dst]"+r"(dst), [count]"+r" (count) :
                : "cc", "memory",
                  "%xmm0", "%xmm1"
            );
        }

        VZEROUPPER;
    }
    }
    #endif

    #if 0
    void add_multiplied(float *dst, const float *src, float k, size_t count)
    {
        if (count == 0)
            return;

        ARCH_X86_ASM
        (
            __ASM_EMIT("vshufps $0x00, %[k], %[k], %%xmm4")
            : : [k] "x" (k)
            : "%xmm4"
        );

        // Align destination
        ARCH_X86_ASM
        (
            __ASM_EMIT("1:")

            // Check conditions
            __ASM_EMIT("test            $0x0f, %[dst]")
            __ASM_EMIT("jz              2f")

            __ASM_EMIT("vmulss          (%[src]), %%xmm4, %%xmm0")
            __ASM_EMIT("vaddss          (%[dst]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, (%[dst])")
            __ASM_EMIT("add             $0x4, %[src]")
            __ASM_EMIT("add             $0x4, %[dst]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jnz             1b")

            // 4 operations per time
            __ASM_EMIT("2:")
            __ASM_EMIT("test            $0x1f, %[dst]")
            __ASM_EMIT("jz              3f")
            __ASM_EMIT("cmp             $4, %[count]")
            __ASM_EMIT("jb              3f")

            __ASM_EMIT("vmovups         (%[src]), %%xmm0")
            __ASM_EMIT("vmulps          %%xmm0, %%xmm4, %%xmm0")
            __ASM_EMIT("vaddps          (%[dst]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovaps         %%xmm0, (%[dst])")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")

            __ASM_EMIT("3:")

            : [src] "+r" (src), [dst] "+r" (dst), [count] "+r" (count) :
            : "cc", "memory",
              "%xmm0"
        );

        #define ADD_MULT_CORE(ld_m)   \
            ARCH_X86_ASM \
            ( \
                __ASM_EMIT("100:") \
                __ASM_EMIT("vinsertf128     $1, %%xmm4, %%ymm4, %%ymm4") \
                \
                __ASM_EMIT("cmp             $32, %[count]") \
                __ASM_EMIT("jb 200f") \
                \
                __ASM_EMIT("vmovaps         %%ymm4, %%ymm5") \
                __ASM_EMIT("vmovaps         %%ymm4, %%ymm6") \
                __ASM_EMIT("vmovaps         %%ymm5, %%ymm7") \
                \
                /* Block 4x8 loop */ \
                __ASM_EMIT("10:") \
                \
                __ASM_EMIT(ld_m "           0x00(%[src]), %%ymm0") \
                __ASM_EMIT(ld_m "           0x20(%[src]), %%ymm1") \
                __ASM_EMIT(ld_m "           0x40(%[src]), %%ymm2") \
                __ASM_EMIT(ld_m "           0x60(%[src]), %%ymm3") \
                \
                __ASM_EMIT("vmulps          %%ymm0, %%ymm4, %%ymm0") \
                __ASM_EMIT("vmulps          %%ymm1, %%ymm5, %%ymm1") \
                __ASM_EMIT("vmulps          %%ymm2, %%ymm6, %%ymm2") \
                __ASM_EMIT("vmulps          %%ymm3, %%ymm7, %%ymm3") \
                \
                __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm0, %%ymm0") \
                __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm1, %%ymm1") \
                __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm2, %%ymm2") \
                __ASM_EMIT("vaddps          0x60(%[dst]), %%ymm3, %%ymm3") \
                \
                __ASM_EMIT("vmovaps         %%ymm0, 0x00(%[dst])") \
                __ASM_EMIT("vmovaps         %%ymm1, 0x20(%[dst])") \
                __ASM_EMIT("vmovaps         %%ymm2, 0x40(%[dst])") \
                __ASM_EMIT("vmovaps         %%ymm3, 0x60(%[dst])") \
                \
                __ASM_EMIT("sub $32,        %[count]") \
                __ASM_EMIT("add $0x80,      %[src]") \
                __ASM_EMIT("add $0x80,      %[dst]") \
                __ASM_EMIT("cmp $32,        %[count]") \
                __ASM_EMIT("jae             10b") \
                \
                /* Block 1x8 loop */  \
                __ASM_EMIT("200:") \
                __ASM_EMIT("cmp             $8, %[count]") \
                __ASM_EMIT("jb              300f") \
                \
                __ASM_EMIT("20:") \
                __ASM_EMIT(ld_m "           (%[src]), %%ymm0") \
                __ASM_EMIT("vmulps          %%ymm0, %%ymm4, %%ymm0") \
                __ASM_EMIT("vaddps          (%[dst]), %%ymm0, %%ymm0") \
                __ASM_EMIT("vmovaps         %%ymm0, (%[dst])") \
                __ASM_EMIT("sub             $8, %[count]") \
                __ASM_EMIT("add             $0x20, %[src]") \
                __ASM_EMIT("add             $0x20, %[dst]") \
                __ASM_EMIT("cmp             $8, %[count]") \
                __ASM_EMIT("jae             20b") \
                \
                /* Block 1x4 */ \
                __ASM_EMIT("300:") \
                __ASM_EMIT("cmp             $4, %[count]") \
                __ASM_EMIT("jb              400f") \
                __ASM_EMIT(ld_m "           (%[src]), %%xmm0") \
                __ASM_EMIT("vmulps          %%xmm0, %%xmm4, %%xmm0") \
                __ASM_EMIT("vaddps          (%[dst]), %%xmm0, %%xmm0") \
                __ASM_EMIT("vmovaps         %%xmm0, (%[dst])") \
                __ASM_EMIT("sub             $4, %[count]") \
                __ASM_EMIT("add             $0x10, %[src]") \
                __ASM_EMIT("add             $0x10, %[dst]") \
                \
                /* Blocks 1x1 */ \
                __ASM_EMIT("400:") \
                __ASM_EMIT("test            %[count], %[count]") \
                __ASM_EMIT("jz              500f") \
                \
                __ASM_EMIT("30:") \
                __ASM_EMIT("vmulss          (%[src]), %%xmm4, %%xmm0") \
                __ASM_EMIT("vaddss          (%[dst]), %%xmm0, %%xmm0") \
                __ASM_EMIT("vmovss          %%xmm0, (%[dst])") \
                __ASM_EMIT("add             $0x4, %[src]") \
                __ASM_EMIT("add             $0x4, %[dst]") \
                __ASM_EMIT("dec             %[count]") \
                __ASM_EMIT("jnz             30b") \
                \
                /* End */ \
                __ASM_EMIT("500:") \
                \
                : [src] "+r" (src), [dst] "+r" (dst), [count] "+r" (count) : \
                : "cc", "memory", \
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
            );

        if (avx_aligned(src))
        {
            ADD_MULT_CORE("vmovaps");
        }
        else
        {
            ADD_MULT_CORE("vmovups");
        }

        VZEROUPPER;

        #undef ADD_MULT_CORE
    }
    #endif

    void add_multiplied(float *dst, const float *src, float k, size_t count)
    {
        if (count == 0)
            return;

        ARCH_X86_ASM
        (
            __ASM_EMIT("vshufps $0x00, %[k], %[k], %%xmm7")
            : : [k] "x" (k)
            : "%xmm7"
        );

        // Align destination
        ARCH_X86_ASM
        (
            __ASM_EMIT("1:")

            // Check conditions
            __ASM_EMIT("test            $0x0f, %[dst]")
            __ASM_EMIT("jz              2f")

            __ASM_EMIT("vmovss          (%[src]), %%xmm0")
            __ASM_EMIT("vfmadd213ss     (%[dst]), %%xmm7, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, (%[dst])")
            __ASM_EMIT("add             $0x4, %[src]")
            __ASM_EMIT("add             $0x4, %[dst]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jnz             1b")

            __ASM_EMIT("2:")

            : [src] "+r" (src), [dst] "+r" (dst), [count] "+r" (count) :
            : "cc", "memory",
              "%xmm0", "%xmm1"
        );
        /*
        -- non-fma:
        __ASM_EMIT("vmulps          %%ymm0, %%ymm6, %%ymm0") \
        __ASM_EMIT("vmulps          %%ymm1, %%ymm7, %%ymm1") \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm6, %%ymm2") \
        __ASM_EMIT("vmulps          %%ymm3, %%ymm7, %%ymm3") \
        __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm0, %%ymm0") \
        __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm1, %%ymm1") \
        __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm2, %%ymm2") \
        __ASM_EMIT("vaddps          0x60(%[dst]), %%ymm3, %%ymm3") \
        */

        #define ADD_MULT_CORE(ld_m)   \
            ARCH_X86_ASM \
            ( \
                __ASM_EMIT("100:") \
                \
                __ASM_EMIT("cmp             $32, %[count]") \
                __ASM_EMIT("jb 200f") \
                \
                __ASM_EMIT("vinsertf128     $1, %%xmm7, %%ymm7, %%ymm7") \
                __ASM_EMIT("vmovaps         %%ymm7, %%ymm6") \
                \
                /* Block 4x8 loop */ \
                __ASM_EMIT("10:") \
                \
                __ASM_EMIT(ld_m "           0x00(%[src]), %%xmm0") \
                __ASM_EMIT(ld_m "           0x10(%[src]), %%xmm1") \
                __ASM_EMIT(ld_m "           0x20(%[src]), %%xmm2") \
                __ASM_EMIT(ld_m "           0x30(%[src]), %%xmm3") \
                __ASM_EMIT("vinsertf128     $1, %%xmm2, %%ymm0, %%ymm0") \
                __ASM_EMIT("vinsertf128     $1, %%xmm3, %%ymm1, %%ymm1") \
                __ASM_EMIT(ld_m "           0x40(%[src]), %%xmm2") \
                __ASM_EMIT(ld_m "           0x50(%[src]), %%xmm3") \
                __ASM_EMIT(ld_m "           0x60(%[src]), %%xmm4") \
                __ASM_EMIT(ld_m "           0x70(%[src]), %%xmm5") \
                __ASM_EMIT("vinsertf128     $1, %%xmm4, %%ymm2, %%ymm2") \
                __ASM_EMIT("vinsertf128     $1, %%xmm5, %%ymm3, %%ymm3") \
                \
                __ASM_EMIT("vfmadd213ps     0x00(%[dst]), %%ymm6, %%ymm0") \
                __ASM_EMIT("vfmadd213ps     0x20(%[dst]), %%ymm7, %%ymm1") \
                __ASM_EMIT("vfmadd213ps     0x40(%[dst]), %%ymm6, %%ymm2") \
                __ASM_EMIT("vfmadd213ps     0x60(%[dst]), %%ymm7, %%ymm3") \
                \
                __ASM_EMIT("vmovaps         %%xmm0, 0x00(%[dst])") \
                __ASM_EMIT("vmovaps         %%xmm1, 0x10(%[dst])") \
                __ASM_EMIT("vextractf128    $1, %%ymm0, 0x20(%[dst])") \
                __ASM_EMIT("vextractf128    $1, %%ymm1, 0x30(%[dst])") \
                __ASM_EMIT("vmovaps         %%xmm2, 0x40(%[dst])") \
                __ASM_EMIT("vmovaps         %%xmm3, 0x50(%[dst])") \
                __ASM_EMIT("vextractf128    $1, %%ymm2, 0x60(%[dst])") \
                __ASM_EMIT("vextractf128    $1, %%ymm3, 0x70(%[dst])") \
                \
                __ASM_EMIT("sub             $32, %[count]") \
                __ASM_EMIT("add             $0x80, %[src]") \
                __ASM_EMIT("add             $0x80, %[dst]") \
                __ASM_EMIT("cmp             $32, %[count]") \
                __ASM_EMIT("jae             10b") \
                \
                /* Block 1x4 */ \
                __ASM_EMIT("200:") \
                __ASM_EMIT("cmp             $4, %[count]") \
                __ASM_EMIT("jb              300f") \
                __ASM_EMIT(ld_m "           (%[src]), %%xmm0") \
                __ASM_EMIT("vfmadd213ps     (%[dst]), %%xmm4, %%xmm0") \
                __ASM_EMIT("vmovaps         %%xmm0, (%[dst])") \
                __ASM_EMIT("sub             $4, %[count]") \
                __ASM_EMIT("add             $0x10, %[src]") \
                __ASM_EMIT("add             $0x10, %[dst]") \
                \
                /* Blocks 1x1 */ \
                __ASM_EMIT("300:") \
                __ASM_EMIT("test            %[count], %[count]") \
                __ASM_EMIT("jz              400f") \
                \
                __ASM_EMIT("30:") \
                __ASM_EMIT("vmovss          (%[src]), %%xmm0") \
                __ASM_EMIT("vfmadd213ss     (%[dst]), %%xmm4, %%xmm0") \
                __ASM_EMIT("vmovss          %%xmm0, (%[dst])") \
                __ASM_EMIT("add             $0x4, %[src]") \
                __ASM_EMIT("add             $0x4, %[dst]") \
                __ASM_EMIT("dec             %[count]") \
                __ASM_EMIT("jnz             30b") \
                \
                /* End */ \
                __ASM_EMIT("400:") \
                \
                : [src] "+r" (src), [dst] "+r" (dst), [count] "+r" (count) : \
                : "cc", "memory", \
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
            );

        if (ptrdiff_t(src)&0x0f)
        {
            ADD_MULT_CORE("vmovups");
        }
        else
        {
            ADD_MULT_CORE("vmovaps");
        }

        VZEROUPPER;

        #undef ADD_MULT_CORE
    }
}

#endif /* DSP_ARCH_X86_AVX_VECTOR_H_ */
