/*
 * complex.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_X86_SSE_COMPLEX_H_
#define CORE_X86_SSE_COMPLEX_H_

namespace lsp
{
    namespace sse
    {

        static void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
        {
            if (count == 0)
                return;

            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                #ifndef __x86_64__
                __ASM_EMIT("testb $0x0f, %0")
                #else
                __ASM_EMIT("test $0x0f, %0")
                #endif
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("movss   (%2), %%xmm0")    // xmm0 = *src1_re
                __ASM_EMIT("movss   (%3), %%xmm1")    // xmm1 = *src1_im
                __ASM_EMIT("movss   (%4), %%xmm2")    // xmm2 = *src2_re
                __ASM_EMIT("movss   (%5), %%xmm3")    // xmm3 = *src2_im
                __ASM_EMIT("movaps  %%xmm0, %%xmm4")  // xmm4 = *src1_re
                __ASM_EMIT("movaps  %%xmm1, %%xmm5")  // xmm5 = *src1_im
                __ASM_EMIT("mulss   %%xmm2, %%xmm0")  // xmm0 = *src1_re * *src2_re
                __ASM_EMIT("mulss   %%xmm3, %%xmm1")  // xmm1 = *src1_im * *src2_im
                __ASM_EMIT("mulss   %%xmm5, %%xmm2")  // xmm2 = *src2_re * *src1_im
                __ASM_EMIT("mulss   %%xmm4, %%xmm3")  // xmm3 = *src2_im * *src1_re
                __ASM_EMIT("subss   %%xmm1, %%xmm0")  // xmm0 = *src1_re * *src2_re - *src1_im * *src2_im
                __ASM_EMIT("addss   %%xmm3, %%xmm2")  // xmm2 = *src2_re * *src1_im + *src2_im * *src1_re
                #ifndef __x86_64__
                __ASM_EMIT("xchg    %0, %2")
                __ASM_EMIT("xchg    %1, %3")
                __ASM_EMIT("movss   %%xmm0, (%2)")
                __ASM_EMIT("movss   %%xmm2, (%3)")
                #else
                __ASM_EMIT("movss   %%xmm0, (%0)")
                __ASM_EMIT("movss   %%xmm2, (%1)")
                #endif

                // Move pointers
                __ASM_EMIT("add     $0x4, %2")
                __ASM_EMIT("add     $0x4, %3")
                __ASM_EMIT("add     $0x4, %4")
                __ASM_EMIT("add     $0x4, %5")
                #ifndef __x86_64__
                __ASM_EMIT("xchg    %0, %2")
                __ASM_EMIT("xchg    %1, %3")
                __ASM_EMIT("add     $0x4, %2")
                __ASM_EMIT("add     $0x4, %3")
                #else
                __ASM_EMIT("add     $0x4, %0")
                __ASM_EMIT("add     $0x4, %1")
                #endif

                // Repeat loop
                __ASM_EMIT("dec %6")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                :
                  #ifndef __x86_64__
                  "+m" (dst_re), "+m" (dst_im),
                  #else
                  "+r" (dst_re), "+r" (dst_im),
                  #endif
                  "+r" (src1_re), "+r" (src1_im),
                  "+r" (src2_re), "+r" (src2_im),
                  "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
            );

            if (count >= SSE_MULTIPLE)
            {
            #ifdef __i386__
                #define cplx_mul4x(s_im, r_re1, r_im1, r_re2, r_im2) \
                        __ASM_EMIT("100:") \
                        \
                        /* Prefetch data */ \
                        __ASM_EMIT("prefetchnta 0x20(%2)") \
                        __ASM_EMIT("prefetchnta 0x20(%3)") \
                        __ASM_EMIT("prefetchnta 0x20(%4)") \
                        __ASM_EMIT("prefetchnta 0x20(%5)") \
                        /* Process data */ \
                        __ASM_EMIT(r_re1 "  (%2), %%xmm0") \
                        __ASM_EMIT(r_im1 "  (%3), %%xmm1") \
                        __ASM_EMIT(r_re2 "  (%4), %%xmm2") \
                        __ASM_EMIT(r_im2 "  (%5), %%xmm3") \
                        __ASM_EMIT("movaps  %%xmm0, %%xmm4") \
                        __ASM_EMIT("movaps  %%xmm1, %%xmm5") \
                        __ASM_EMIT("mulps   %%xmm2, %%xmm0") \
                        __ASM_EMIT("mulps   %%xmm3, %%xmm1") \
                        __ASM_EMIT("mulps   %%xmm5, %%xmm2") \
                        __ASM_EMIT("mulps   %%xmm4, %%xmm3") \
                        __ASM_EMIT("subps   %%xmm1, %%xmm0") \
                        __ASM_EMIT("addps   %%xmm3, %%xmm2") \
                        __ASM_EMIT("xchg    %0, %2") \
                        __ASM_EMIT("xchg    %1, %3") \
                        __ASM_EMIT(MOVNTPS " %%xmm0, (%2)") \
                        __ASM_EMIT(s_im "   %%xmm2, (%3)") \
                        \
                        /* Move pointers */ \
                        __ASM_EMIT("sub     $0x4, %6") \
                        __ASM_EMIT("add     $0x10, %2") \
                        __ASM_EMIT("add     $0x10, %3") \
                        __ASM_EMIT("add     $0x10, %4") \
                        __ASM_EMIT("add     $0x10, %5") \
                        __ASM_EMIT("xchg    %0, %2") \
                        __ASM_EMIT("xchg    %1, %3") \
                        __ASM_EMIT("add     $0x10, %2") \
                        __ASM_EMIT("add     $0x10, %3") \
                        \
                        /* Repeat loop */ \
                        __ASM_EMIT("test $-4, %6") \
                        __ASM_EMIT("jnz 100b")  \
                        __ASM_EMIT("jmp 32f")
            #else // __x86_64__
                register size_t offset = 0;

                #define cplx_mul4x(s_im, r_re1, r_im1, r_re2, r_im2) \
                        __ASM_EMIT("100:") \
                        \
                        /* Prefetch data */ \
                        __ASM_EMIT("prefetchnta 0x20(%2,%7)") \
                        __ASM_EMIT("prefetchnta 0x20(%3,%7)") \
                        __ASM_EMIT("prefetchnta 0x20(%4,%7)") \
                        __ASM_EMIT("prefetchnta 0x20(%5,%7)") \
                        /* Process data */ \
                        __ASM_EMIT(r_re1 "  (%2,%7), %%xmm0") \
                        __ASM_EMIT(r_im1 "  (%3,%7), %%xmm1") \
                        __ASM_EMIT(r_re2 "  (%4,%7), %%xmm2") \
                        __ASM_EMIT(r_im2 "  (%5,%7), %%xmm3") \
                        __ASM_EMIT("movaps  %%xmm0, %%xmm4") \
                        __ASM_EMIT("movaps  %%xmm1, %%xmm5") \
                        __ASM_EMIT("mulps   %%xmm2, %%xmm0") \
                        __ASM_EMIT("mulps   %%xmm3, %%xmm1") \
                        __ASM_EMIT("mulps   %%xmm5, %%xmm2") \
                        __ASM_EMIT("mulps   %%xmm4, %%xmm3") \
                        __ASM_EMIT("subps   %%xmm1, %%xmm0") \
                        __ASM_EMIT("addps   %%xmm3, %%xmm2") \
                        __ASM_EMIT(MOVNTPS " %%xmm0, (%0,%7)") \
                        __ASM_EMIT(s_im "   %%xmm2, (%1,%7)") \
                        \
                        /* Move pointers */ \
                        __ASM_EMIT("sub $0x4, %6") \
                        __ASM_EMIT("add $0x10, %7") \
                        \
                        /* Repeat loop */ \
                        __ASM_EMIT("test $-4, %6") \
                        __ASM_EMIT("jnz 100b")  \
                        __ASM_EMIT("jmp 32f")

            #endif /* __i386__ */

                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta (%0)")
                    __ASM_EMIT("prefetchnta (%1)")
                    __ASM_EMIT("prefetchnta (%2)")
                    __ASM_EMIT("prefetchnta (%3)")
                    : : "r" (src1_re), "r" (src1_im), "r" (src2_re), "r" (src2_im) :
                );

                __asm__ __volatile__
                (
                    #ifdef __x86_64__
                    __ASM_EMIT("test $0x0f, %1") // sse_aligned(dst_im)
                    #else
                    __ASM_EMIT("testb $0x0f, %1") // sse_aligned(dst_im)
                    #endif /* __x86_64__ */
                    __ASM_EMIT("jnz 16f")
                        __ASM_EMIT("test $0x0f, %2") // sse_aligned(src1_re)
                        __ASM_EMIT("jnz 8f")
                            __ASM_EMIT("test $0x0f, %3") // sse_aligned(src1_im)
                            __ASM_EMIT("jnz 4f")
                                __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                __ASM_EMIT("jnz 2f")
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 1f")
                                        cplx_mul4x(MOVNTPS, "movaps", "movaps", "movaps", "movaps")
                                    __ASM_EMIT("1:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movaps", "movaps", "movaps", "movups")
                                __ASM_EMIT("2:") // !sse_aligned(src2_re)
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 3f")
                                        cplx_mul4x(MOVNTPS, "movaps", "movaps", "movups", "movaps")
                                    __ASM_EMIT("3:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movaps", "movaps", "movups", "movups")
                            __ASM_EMIT("4:") // !sse_aligned(src1_im)
                                __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                __ASM_EMIT("jnz 6f")
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 5f")
                                        cplx_mul4x(MOVNTPS, "movaps", "movups", "movaps", "movaps")
                                    __ASM_EMIT("5:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movaps", "movups", "movaps", "movups")
                                __ASM_EMIT("6:") // !sse_aligned(src2_re)
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 7f")
                                        cplx_mul4x(MOVNTPS, "movaps", "movups", "movups", "movaps")
                                    __ASM_EMIT("7:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movaps", "movups", "movups", "movups")
                        __ASM_EMIT("8:") // !sse_aligned(src1_re)
                        __ASM_EMIT("test $0x0f, %3") // sse_aligned(src1_im)
                            __ASM_EMIT("jnz 12f")
                                __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                __ASM_EMIT("jnz 10f")
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 9f")
                                        cplx_mul4x(MOVNTPS, "movups", "movaps", "movaps", "movaps")
                                    __ASM_EMIT("9:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movups", "movaps", "movaps", "movups")
                                __ASM_EMIT("10:") // !sse_aligned(src2_re)
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 11f")
                                        cplx_mul4x(MOVNTPS, "movups", "movaps", "movups", "movaps")
                                    __ASM_EMIT("11:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movups", "movaps", "movups", "movups")
                            __ASM_EMIT("12:") // !sse_aligned(src1_im)
                                __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                __ASM_EMIT("jnz 14f")
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 13f")
                                        cplx_mul4x(MOVNTPS, "movups", "movups", "movaps", "movaps")
                                    __ASM_EMIT("13:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movups", "movups", "movaps", "movups")
                                __ASM_EMIT("14:") // !sse_aligned(src2_re)
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 15f")
                                        cplx_mul4x(MOVNTPS, "movups", "movups", "movups", "movaps")
                                    __ASM_EMIT("15:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movups", "movups", "movups", "movups")

                    __ASM_EMIT("16:") // !sse_aligned(dst_im)
                        __ASM_EMIT("test $0x0f, %2") // sse_aligned(src1_re)
                            __ASM_EMIT("jnz 24f")
                                __ASM_EMIT("test $0x0f, %3") // sse_aligned(src1_im)
                                __ASM_EMIT("jnz 20f")
                                    __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                    __ASM_EMIT("jnz 18f")
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 17f")
                                            cplx_mul4x("movups",  "movaps", "movaps", "movaps", "movaps")
                                        __ASM_EMIT("17:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movaps", "movaps", "movaps", "movups")
                                    __ASM_EMIT("18:") // !sse_aligned(src2_re)
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 19f")
                                            cplx_mul4x("movups",  "movaps", "movaps", "movups", "movaps")
                                        __ASM_EMIT("19:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movaps", "movaps", "movups", "movups")
                                __ASM_EMIT("20:") // !sse_aligned(src1_im)
                                    __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                    __ASM_EMIT("jnz 22f")
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 21f")
                                            cplx_mul4x("movups",  "movaps", "movups", "movaps", "movaps")
                                        __ASM_EMIT("21:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movaps", "movups", "movaps", "movups")
                                    __ASM_EMIT("22:") // !sse_aligned(src2_re)
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 23f")
                                            cplx_mul4x("movups",  "movaps", "movups", "movups", "movaps")
                                        __ASM_EMIT("23:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movaps", "movups", "movups", "movups")
                            __ASM_EMIT("24:") // !sse_aligned(src1_re)
                            __ASM_EMIT("test $0x0f, %3") // sse_aligned(src1_im)
                                __ASM_EMIT("jnz 28f")
                                    __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                    __ASM_EMIT("jnz 26f")
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 25f")
                                            cplx_mul4x("movups",  "movups", "movaps", "movaps", "movaps")
                                        __ASM_EMIT("25:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movups", "movaps", "movaps", "movups")
                                    __ASM_EMIT("26:") // !sse_aligned(src2_re)
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 27f")
                                            cplx_mul4x("movups",  "movups", "movaps", "movups", "movaps")
                                        __ASM_EMIT("27:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movups", "movaps", "movups", "movups")
                                __ASM_EMIT("28:") // !sse_aligned(src1_im)
                                    __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                    __ASM_EMIT("jnz 30f")
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 29f")
                                            cplx_mul4x("movups",  "movups", "movups", "movaps", "movaps")
                                        __ASM_EMIT("29:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movups", "movups", "movaps", "movups")
                                    __ASM_EMIT("30:") // !sse_aligned(src2_re)
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 31f")
                                            cplx_mul4x("movups",  "movups", "movups", "movups", "movaps")
                                        __ASM_EMIT("31:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movups", "movups", "movups", "movups")
                    __ASM_EMIT("32:")

                    :
                      #ifndef __x86_64__
                      "+m" (dst_re), "+m" (dst_im),
                      #else
                      "+r" (dst_re), "+r" (dst_im),
                      #endif
                      "+r" (src1_re), "+r" (src1_im),
                      "+r" (src2_re), "+r" (src2_im),
                      "+r" (count)
                      #ifdef __x86_64__
                      ,"+r"(offset)
                      #endif /* __x86_64__ */
                    :
                    : "cc", "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
                );

                #undef cplx_mul4x

                #ifdef __x86_64__
                    __asm__ __volatile__
                    (
                        // Update pointers
                        __ASM_EMIT("add %6, %0")
                        __ASM_EMIT("add %6, %1")
                        __ASM_EMIT("add %6, %2")
                        __ASM_EMIT("add %6, %3")
                        __ASM_EMIT("add %6, %4")
                        __ASM_EMIT("add %6, %5")

                        : "+r" (dst_re), "+r" (dst_im),
                          "+r" (src1_re), "+r" (src1_im),
                          "+r" (src2_re), "+r" (src2_im),
                          "+r" (offset)
                        :
                        : "cc",
                          "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
                    );
                #endif /* __x86_64__ */
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss   (%2), %%xmm0")
                    __ASM_EMIT("movss   (%3), %%xmm1")
                    __ASM_EMIT("movss   (%4), %%xmm2")
                    __ASM_EMIT("movss   (%5), %%xmm3")
                    __ASM_EMIT("movaps  %%xmm0, %%xmm4")
                    __ASM_EMIT("movaps  %%xmm1, %%xmm5")
                    __ASM_EMIT("mulss   %%xmm2, %%xmm0")
                    __ASM_EMIT("mulss   %%xmm3, %%xmm1")
                    __ASM_EMIT("mulss   %%xmm5, %%xmm2")
                    __ASM_EMIT("mulss   %%xmm4, %%xmm3")
                    __ASM_EMIT("subss   %%xmm1, %%xmm0")
                    __ASM_EMIT("addss   %%xmm3, %%xmm2")
                    #ifndef __x86_64__
                    __ASM_EMIT("xchg    %0, %2")
                    __ASM_EMIT("xchg    %1, %3")
                    __ASM_EMIT("movss   %%xmm0, (%2)")
                    __ASM_EMIT("movss   %%xmm2, (%3)")
                    #else
                    __ASM_EMIT("movss   %%xmm0, (%0)")
                    __ASM_EMIT("movss   %%xmm2, (%1)")
                    #endif

                    // Move pointers
                    __ASM_EMIT("add     $0x4, %2")
                    __ASM_EMIT("add     $0x4, %3")
                    __ASM_EMIT("add     $0x4, %4")
                    __ASM_EMIT("add     $0x4, %5")
                    #ifndef __x86_64__
                    __ASM_EMIT("xchg    %0, %2")
                    __ASM_EMIT("xchg    %1, %3")
                    __ASM_EMIT("add     $0x4, %2")
                    __ASM_EMIT("add     $0x4, %3")
                    #else
                    __ASM_EMIT("add     $0x4, %0")
                    __ASM_EMIT("add     $0x4, %1")
                    #endif

                    // Repeat loop
                    __ASM_EMIT("dec %6")
                    __ASM_EMIT("jnz 1b")
                    :
                      #ifndef __x86_64__
                      "+m" (dst_re), "+m" (dst_im),
                      #else
                      "+r" (dst_re), "+r" (dst_im),
                      #endif
                      "+r" (src1_re), "+r" (src1_im),
                      "+r" (src2_re), "+r" (src2_im),
                      "+r" (count)
                    :
                    : "cc", "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
                );
            }

            SFENCE;
        }

        static void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count)
        {
            if (count == 0)
                return;

            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %2")
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("movss (%0), %%xmm0")
                __ASM_EMIT("movss (%1), %%xmm1")
                __ASM_EMIT("mulss %%xmm0, %%xmm0")
                __ASM_EMIT("mulss %%xmm1, %%xmm1")
                __ASM_EMIT("addss %%xmm1, %%xmm0")
                __ASM_EMIT("sqrtss %%xmm0, %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%2)")

                // Move pointers
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("add $0x4, %1")
                __ASM_EMIT("add $0x4, %2")

                // Repeat loop
                __ASM_EMIT("dec %3")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r" (src_re), "+r"(src_im), "+r"(dst_mod), "+r" (count) :
                : "cc", "memory",
                  "%xmm0", "%xmm1"
            );

            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                size_t blocks   = regs / 4;
                regs           %= 4;

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta  0x00(%0)")
                    __ASM_EMIT("prefetchnta  0x20(%0)")
                    __ASM_EMIT("prefetchnta  0x00(%1)")
                    __ASM_EMIT("prefetchnta  0x20(%1)")
                    : : "r" (src_re), "r" (src_im)
                );

                #define cplx_mod4(l_re, l_im)   \
                { \
                    if (blocks > 0) \
                    { \
                        __asm__ __volatile__ \
                        ( \
                            __ASM_EMIT("1:") \
                            \
                            __ASM_EMIT("prefetchnta  0x40(%0)") \
                            __ASM_EMIT("prefetchnta  0x60(%0)") \
                            __ASM_EMIT("prefetchnta  0x40(%1)") \
                            __ASM_EMIT("prefetchnta  0x60(%1)") \
                            /* Process data */ \
                            __ASM_EMIT(l_re " 0x00(%0), %%xmm0") \
                            __ASM_EMIT(l_re " 0x10(%0), %%xmm1") \
                            __ASM_EMIT(l_re " 0x20(%0), %%xmm2") \
                            __ASM_EMIT(l_re " 0x30(%0), %%xmm3") \
                            __ASM_EMIT(l_im " 0x00(%1), %%xmm4") \
                            __ASM_EMIT(l_im " 0x10(%1), %%xmm5") \
                            __ASM_EMIT(l_im " 0x20(%1), %%xmm6") \
                            __ASM_EMIT(l_im " 0x30(%1), %%xmm7") \
                            \
                            __ASM_EMIT("mulps %%xmm0, %%xmm0") \
                            __ASM_EMIT("mulps %%xmm1, %%xmm1") \
                            __ASM_EMIT("mulps %%xmm2, %%xmm2") \
                            __ASM_EMIT("mulps %%xmm3, %%xmm3") \
                            __ASM_EMIT("mulps %%xmm4, %%xmm4") \
                            __ASM_EMIT("mulps %%xmm5, %%xmm5") \
                            __ASM_EMIT("mulps %%xmm6, %%xmm6") \
                            __ASM_EMIT("mulps %%xmm7, %%xmm7") \
                            \
                            __ASM_EMIT("addps %%xmm4, %%xmm0") \
                            __ASM_EMIT("addps %%xmm5, %%xmm1") \
                            __ASM_EMIT("addps %%xmm6, %%xmm2") \
                            __ASM_EMIT("addps %%xmm7, %%xmm3") \
                            \
                            __ASM_EMIT("sqrtps %%xmm0, %%xmm0") \
                            __ASM_EMIT("sqrtps %%xmm1, %%xmm1") \
                            __ASM_EMIT("sqrtps %%xmm2, %%xmm2") \
                            __ASM_EMIT("sqrtps %%xmm3, %%xmm3") \
                            \
                            __ASM_EMIT(MOVNTPS " %%xmm0, 0x00(%2)") \
                            __ASM_EMIT(MOVNTPS " %%xmm1, 0x10(%2)") \
                            __ASM_EMIT(MOVNTPS " %%xmm2, 0x20(%2)") \
                            __ASM_EMIT(MOVNTPS " %%xmm3, 0x30(%2)") \
                            \
                            /* Move pointers */ \
                            __ASM_EMIT("add $0x40, %0") \
                            __ASM_EMIT("add $0x40, %1") \
                            __ASM_EMIT("add $0x40, %2") \
                            \
                            /* Repeat loop */ \
                            __ASM_EMIT("dec %3") \
                            __ASM_EMIT("jnz 1b") \
                            __ASM_EMIT("2:") \
                            \
                            : "+r" (src_re), "+r"(src_im), "+r"(dst_mod), "+r" (blocks) : \
                            : "cc", "memory", \
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
                              "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
                        ); \
                    } \
                    \
                    if (regs > 0) \
                    { \
                        __asm__ __volatile__ \
                        ( \
                            __ASM_EMIT("1:") \
                            \
                            /* Process data */ \
                            __ASM_EMIT(l_re " (%0), %%xmm0") \
                            __ASM_EMIT(l_im " (%1), %%xmm1") \
                            __ASM_EMIT("mulps %%xmm0, %%xmm0") \
                            __ASM_EMIT("mulps %%xmm1, %%xmm1") \
                            __ASM_EMIT("addps %%xmm1, %%xmm0") \
                            __ASM_EMIT("sqrtps %%xmm0, %%xmm0") \
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%2)") \
                            \
                            /* Move pointers */ \
                            __ASM_EMIT("add $0x10, %0") \
                            __ASM_EMIT("add $0x10, %1") \
                            __ASM_EMIT("add $0x10, %2") \
                            \
                            /* Repeat loop */ \
                            __ASM_EMIT("dec %3") \
                            __ASM_EMIT("jnz 1b") \
                            __ASM_EMIT("2:") \
                            \
                            : "+r" (src_re), "+r"(src_im), "+r"(dst_mod), "+r" (regs) : \
                            : "cc", "memory", \
                              "%xmm0", "%xmm1" \
                        ); \
                    } \
                }

                if (sse_aligned(src_re))
                {
                    if (sse_aligned(src_im))
                        cplx_mod4("movaps", "movaps")
                    else
                        cplx_mod4("movaps", "movups")
                }
                else
                {
                    if (sse_aligned(src_im))
                        cplx_mod4("movups", "movaps")
                    else
                        cplx_mod4("movups", "movups")
                }

                #undef cplx_mod4
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("movss (%1), %%xmm1")
                    __ASM_EMIT("mulss %%xmm0, %%xmm0")
                    __ASM_EMIT("mulss %%xmm1, %%xmm1")
                    __ASM_EMIT("addss %%xmm1, %%xmm0")
                    __ASM_EMIT("sqrtss %%xmm0, %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%2)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")
                    __ASM_EMIT("add $0x4, %2")

                    // Repeat loop
                    __ASM_EMIT("dec %3")
                    __ASM_EMIT("jnz 1b")
                    __ASM_EMIT("2:")

                    : "+r" (src_re), "+r"(src_im), "+r"(dst_mod), "+r" (count) :
                    : "cc", "memory",
                      "%xmm0", "%xmm1"
                );
            }

            SFENCE;
        }
    }
}

#endif /* CORE_X86_SSE_COMPLEX_H_ */
