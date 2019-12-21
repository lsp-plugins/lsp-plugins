/*
 * hsum.h
 *
 *  Created on: 29 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_HMATH_HSUM_H_
#define DSP_ARCH_X86_AVX_HMATH_HSUM_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    float h_sum(const float *src, size_t count)
    {
        IF_ARCH_X86(float result);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vxorps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("sub             $64, %[count]")
            __ASM_EMIT("jb              2f")
            /* x64 blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vaddps          0x00(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x20(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x40(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x60(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x80(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0xa0(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0xc0(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0xe0(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x100, %[src]")
            __ASM_EMIT("sub             $64, %[count]")
            __ASM_EMIT("jae             1b")
            /* x32 block */
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $32, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vaddps          0x00(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x20(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x40(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x60(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("sub             $32, %[count]")
            /* x16 block */
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vaddps          0x00(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x20(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("sub             $16, %[count]")
            /* x8 block */
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vaddps          0x00(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("sub             $8, %[count]")
            /* x4 block */
            __ASM_EMIT("8:")
            __ASM_EMIT("vextractf128    $0x01, %%ymm0, %%xmm1")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("jl              10f")
            __ASM_EMIT("vaddps          0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("sub             $4, %[count]")
            /* x1 block */
            __ASM_EMIT("10:")
            __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("jl              12f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vaddss          0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             11b")
            /* end */
            __ASM_EMIT("12:")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "=Yz" (result)
            :
            : "cc", "memory",
              "%xmm1"
        );

        return result;
    }

    float h_sqr_sum(const float *src, size_t count)
    {
        IF_ARCH_X86(float result);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vxorps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("sub             $48, %[count]")
            __ASM_EMIT("jb              2f")
            /* x48 blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm2")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm3")
            __ASM_EMIT("vmovups         0x40(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x60(%[src]), %%ymm5")
            __ASM_EMIT("vmovups         0x80(%[src]), %%ymm6")
            __ASM_EMIT("vmovups         0xa0(%[src]), %%ymm7")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
            __ASM_EMIT("vmulps          %%ymm4, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm5, %%ymm5, %%ymm5")
            __ASM_EMIT("vmulps          %%ymm6, %%ymm6, %%ymm6")
            __ASM_EMIT("vmulps          %%ymm7, %%ymm7, %%ymm7")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          %%ymm4, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm5, %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          %%ymm6, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm7, %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0xc0, %[src]")
            __ASM_EMIT("sub             $48, %[count]")
            __ASM_EMIT("jae             1b")
            /* x32 block */
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm2")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm3")
            __ASM_EMIT("vmovups         0x40(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x60(%[src]), %%ymm5")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
            __ASM_EMIT("vmulps          %%ymm4, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm5, %%ymm5, %%ymm5")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          %%ymm4, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm5, %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("sub             $32, %[count]")
            /* x16 block */
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm2")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm3")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("sub             $16, %[count]")
            /* x8 block */
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm2")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("sub             $8, %[count]")
            /* x4 block */
            __ASM_EMIT("8:")
            __ASM_EMIT("vextractf128    $0x01, %%ymm0, %%xmm1")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("jl              10f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%xmm2")
            __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("sub             $4, %[count]")
            /* x1 block */
            __ASM_EMIT("10:")
            __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("jl              12f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm2")
            __ASM_EMIT("vmulss          %%xmm2, %%xmm2, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             11b")
            /* end */
            __ASM_EMIT("12:")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "=Yz" (result)
            :
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return result;
    }

    float h_sqr_sum_fma3(const float *src, size_t count)
    {
        IF_ARCH_X86(float result);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vxorps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("sub             $48, %[count]")
            __ASM_EMIT("jb              2f")
            /* x48 blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm2")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm3")
            __ASM_EMIT("vmovups         0x40(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x60(%[src]), %%ymm5")
            __ASM_EMIT("vmovups         0x80(%[src]), %%ymm6")
            __ASM_EMIT("vmovups         0xa0(%[src]), %%ymm7")
            __ASM_EMIT("vfmadd231ps     %%ymm2, %%ymm2, %%ymm0")
            __ASM_EMIT("vfmadd231ps     %%ymm3, %%ymm3, %%ymm1")
            __ASM_EMIT("vfmadd231ps     %%ymm4, %%ymm4, %%ymm0")
            __ASM_EMIT("vfmadd231ps     %%ymm5, %%ymm5, %%ymm1")
            __ASM_EMIT("vfmadd231ps     %%ymm6, %%ymm6, %%ymm0")
            __ASM_EMIT("vfmadd231ps     %%ymm7, %%ymm7, %%ymm1")
            __ASM_EMIT("add             $0xc0, %[src]")
            __ASM_EMIT("sub             $48, %[count]")
            __ASM_EMIT("jae             1b")
            /* x32 block */
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm2")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm3")
            __ASM_EMIT("vmovups         0x40(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x60(%[src]), %%ymm5")
            __ASM_EMIT("vfmadd231ps     %%ymm2, %%ymm2, %%ymm0")
            __ASM_EMIT("vfmadd231ps     %%ymm3, %%ymm3, %%ymm1")
            __ASM_EMIT("vfmadd231ps     %%ymm4, %%ymm4, %%ymm0")
            __ASM_EMIT("vfmadd231ps     %%ymm5, %%ymm5, %%ymm1")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("sub             $32, %[count]")
            /* x16 block */
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm2")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm3")
            __ASM_EMIT("vfmadd231ps     %%ymm2, %%ymm2, %%ymm0")
            __ASM_EMIT("vfmadd231ps     %%ymm3, %%ymm3, %%ymm1")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("sub             $16, %[count]")
            /* x8 block */
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm2")
            __ASM_EMIT("vfmadd231ps     %%ymm2, %%ymm2, %%ymm0")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("sub             $8, %[count]")
            /* x4 block */
            __ASM_EMIT("8:")
            __ASM_EMIT("vextractf128    $0x01, %%ymm0, %%xmm1")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("jl              10f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%xmm2")
            __ASM_EMIT("vfmadd231ps     %%xmm2, %%xmm2, %%xmm0")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("sub             $4, %[count]")
            /* x1 block */
            __ASM_EMIT("10:")
            __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("jl              12f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm2")
            __ASM_EMIT("vfmadd231ss     %%xmm2, %%xmm2, %%xmm0")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             11b")
            /* end */
            __ASM_EMIT("12:")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "=Yz" (result)
            :
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return result;
    }

    float h_abs_sum(const float *src, size_t count)
    {
        IF_ARCH_X86(float result);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vxorps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vmovaps         %[SIGN], %%ymm2")
            __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovaps         %%ymm2, %%ymm3")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            /* x32 blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vandps          0x00(%[src]), %%ymm2, %%ymm4")
            __ASM_EMIT("vandps          0x20(%[src]), %%ymm3, %%ymm5")
            __ASM_EMIT("vandps          0x40(%[src]), %%ymm2, %%ymm6")
            __ASM_EMIT("vandps          0x60(%[src]), %%ymm3, %%ymm7")
            __ASM_EMIT("vaddps          %%ymm4, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm5, %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          %%ymm6, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm7, %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")
            /* x16 block */
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vandps          0x00(%[src]), %%ymm2, %%ymm4")
            __ASM_EMIT("vandps          0x20(%[src]), %%ymm3, %%ymm5")
            __ASM_EMIT("vaddps          %%ymm4, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm5, %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("sub             $16, %[count]")
            /* x8 block */
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vandps          0x00(%[src]), %%ymm2, %%ymm4")
            __ASM_EMIT("vaddps          %%ymm4, %%ymm0, %%ymm0")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("sub             $8, %[count]")
            /* x4 block */
            __ASM_EMIT("6:")
            __ASM_EMIT("vextractf128    $0x01, %%ymm0, %%xmm1")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vandps          0x00(%[src]), %%xmm2, %%xmm4")
            __ASM_EMIT("vaddps          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("sub             $4, %[count]")
            /* x1 block */
            __ASM_EMIT("8:")
            __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("jl              10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm4")
            __ASM_EMIT("vandps          %%xmm2, %%xmm4, %%xmm4")
            __ASM_EMIT("vaddss          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             9b")
            /* end */
            __ASM_EMIT("10:")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "=Yz" (result)
            : [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm1"
        );

        return result;
    }
}

#endif /* DSP_ARCH_X86_AVX_HMATH_HSUM_H_ */
