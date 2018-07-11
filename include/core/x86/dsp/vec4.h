/*
 * vec4.h
 *
 *  Created on: 15 июля 2016 г.
 *      Author: sadko
 */

#ifndef CORE_X86_DSP_VEC4_H_
#define CORE_X86_DSP_VEC4_H_

namespace lsp
{
    namespace sse
    {
        static float vec4_scalar_mul(const float *a, const float *b)
        {
            float result;

            __asm__ __volatile__
            (
                // Calculate multiplication
                __ASM_EMIT("movaps (%[a]), %%xmm0") // xmm0 = a0 a1 a2 a3
                __ASM_EMIT("movaps (%[b]), %%xmm1") // xmm1 = b0 b1 b2 b3
                __ASM_EMIT("mulps %%xmm1, %%xmm0") // xmm0 = a0*b0 a1*b1 a2*b2 a3*b3

                // Calculate horizontal sum
                __ASM_EMIT("movhlps %%xmm0, %%xmm1")
                __ASM_EMIT("addps %%xmm1, %%xmm0")
                __ASM_EMIT("movaps %%xmm0, %%xmm1")
                __ASM_EMIT("shufps $0x55, %%xmm0, %%xmm0")
                __ASM_EMIT("addps %%xmm1, %%xmm0")

                : "=Yz" (result)
                : [a] "r" (a), [b] "r" (b)
                : "memory", "%xmm1"
            );

            return result;
        }

        static float vec4_push(float *v, float value)
        {
            __asm__ __volatile__
            (
                // Read vector
                __ASM_EMIT("movaps %%xmm0, %%xmm2")     // xmm2 = V ? ? ?
                __ASM_EMIT("movaps (%[v]), %%xmm0")      // xmm0 = v0 v1 v2 v3
                __ASM_EMIT("movaps %%xmm0, %%xmm1")     // xmm1 = v0 v1 v2 v3

                // Update vector
                __ASM_EMIT("shufps $0xf0, %%xmm0, %%xmm2") // xmm2 = V V v3 v3
                __ASM_EMIT("shufps $0x29, %%xmm2, %%xmm1") // xmm1 = v1 v2 v3 V
                __ASM_EMIT("shufps $0x00, %%xmm0, %%xmm0") // xmm0 = v0 v0 v0 v0

                // Store vector
                __ASM_EMIT("movaps %%xmm1, (%[v])")

                : "=Yz" (value)
                : [v] "r" (v)
                : "memory", "%xmm1", "%xmm2"
            );

            return value;
        }

        static float vec4_unshift(float *v, float value)
        {
            __asm__ __volatile__
            (
                // Read vector
                __ASM_EMIT("movaps %%xmm0, %%xmm2")     // xmm2 = V ? ? ?
                __ASM_EMIT("movaps (%[v]), %%xmm0")      // xmm0 = v0 v1 v2 v3
                __ASM_EMIT("movaps %%xmm0, %%xmm1")     // xmm1 = v0 v1 v2 v3

                // Update vector
                __ASM_EMIT("shufps $0x00, %%xmm0, %%xmm2") // xmm2 = V V v0 v0
                __ASM_EMIT("shufps $0x9c, %%xmm1, %%xmm2") // xmm2 = V v0 v1 v2
                __ASM_EMIT("shufps $0xff, %%xmm0, %%xmm0") // xmm0 = v3 v3 v3 v3

                // Store vector
                __ASM_EMIT("movaps %%xmm2, (%[v])")

                : "=Yz" (value)
                : [v] "r" (v)
                : "memory", "%xmm1", "%xmm2"
            );

            return value;
        }

        static void vec4_zero(float *v)
        {
            __asm__ __volatile__
            (
                // Store empty vector
                __ASM_EMIT("xorps %%xmm0, %%xmm0")
                __ASM_EMIT("movaps %%xmm0, (%[v])")

                : : [v] "r" (v)
                : "memory", "%xmm0"
            );
        }
    }
}

#endif /* CORE_X86_DSP_VEC4_H_ */
