/*
 * addsub.h
 *
 *  Created on: 6 мар. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_X86_SSE_ADDSUB_H_
#define CORE_X86_SSE_ADDSUB_H_

namespace lsp
{
    namespace sse
    {
        #define ADD2_CORE(OP, MV_DST, MV_SRC)   \
            __ASM_EMIT("cmp         $0x10, %[count]") \
            __ASM_EMIT("jb          2f") \
            /* 16x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT(MV_DST "     0x00(%[dst]), %%xmm0") \
            __ASM_EMIT(MV_DST "     0x10(%[dst]), %%xmm1") \
            __ASM_EMIT(MV_DST "     0x20(%[dst]), %%xmm2") \
            __ASM_EMIT(MV_DST "     0x30(%[dst]), %%xmm3") \
            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm4") \
            __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm5") \
            __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm6") \
            __ASM_EMIT(MV_SRC "     0x30(%[src]), %%xmm7") \
            __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
            __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
            __ASM_EMIT(OP "ps       %%xmm6, %%xmm2") \
            __ASM_EMIT(OP "ps       %%xmm7, %%xmm3") \
            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm1, 0x10(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm2, 0x20(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm3, 0x30(%[dst])") \
            __ASM_EMIT("sub         $0x10, %[count]") \
            __ASM_EMIT("add         $0x40, %[src]") \
            __ASM_EMIT("add         $0x40, %[dst]") \
            __ASM_EMIT("cmp         $0x10, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 8x blocks */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("test        $8, %[count]") \
            __ASM_EMIT("jz          3f") \
            __ASM_EMIT(MV_DST "     0x00(%[dst]), %%xmm0") \
            __ASM_EMIT(MV_DST "     0x10(%[dst]), %%xmm1") \
            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm4") \
            __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm5") \
            __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
            __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm1, 0x10(%[dst])") \
            __ASM_EMIT("add         $0x20, %[src]") \
            __ASM_EMIT("add         $0x20, %[dst]") \
            /* 4x blocks */ \
            __ASM_EMIT("3:") \
            __ASM_EMIT("test        $4, %[count]") \
            __ASM_EMIT("jz          4f") \
            __ASM_EMIT(MV_DST "     0x00(%[dst]), %%xmm0") \
            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm4") \
            __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("add         $0x10, %[src]") \
            __ASM_EMIT("add         $0x10, %[dst]") \
            /* 1x blocks */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("and         $3, %[count]") \
            __ASM_EMIT("jz          20000f") \
            __ASM_EMIT("5:") \
            __ASM_EMIT("movss       0x00(%[dst]), %%xmm0") \
            __ASM_EMIT("movss       0x00(%[src]), %%xmm4") \
            __ASM_EMIT(OP "ss       %%xmm4, %%xmm0") \
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("add         $0x04, %[src]") \
            __ASM_EMIT("add         $0x04, %[dst]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jnz         5b")


        void add2(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("test       $0x0f, %[dst]")
                __ASM_EMIT("jnz        10010f")
                    __ASM_EMIT("test        $0x0f, %[src]")
                    __ASM_EMIT("jnz         10001f")
                        ADD2_CORE("add", "movaps", "movaps")
                        __ASM_EMIT("jmp         20000f")
                    __ASM_EMIT("10001:")
                        ADD2_CORE("add", "movaps", "movups")
                        __ASM_EMIT("jmp         20000f")
                __ASM_EMIT("10010:")
                    __ASM_EMIT("test        $0x0f, %[src]")
                    __ASM_EMIT("jnz         10011f")
                        ADD2_CORE("add", "movups", "movaps")
                        __ASM_EMIT("jmp         20000f")
                    __ASM_EMIT("10011:")
                        ADD2_CORE("add", "movups", "movups")

                __ASM_EMIT("20000:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void sub2(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("test       $0x0f, %[dst]")
                __ASM_EMIT("jnz        10010f")
                    __ASM_EMIT("test        $0x0f, %[src]")
                    __ASM_EMIT("jnz         10001f")
                        ADD2_CORE("sub", "movaps", "movaps")
                        __ASM_EMIT("jmp         20000f")
                    __ASM_EMIT("10001:")
                        ADD2_CORE("sub", "movaps", "movups")
                        __ASM_EMIT("jmp         20000f")
                __ASM_EMIT("10010:")
                    __ASM_EMIT("test        $0x0f, %[src]")
                    __ASM_EMIT("jnz         10011f")
                        ADD2_CORE("sub", "movups", "movaps")
                        __ASM_EMIT("jmp         20000f")
                    __ASM_EMIT("10011:")
                        ADD2_CORE("sub", "movups", "movups")

                __ASM_EMIT("20000:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef ADD2_CORE

        #define ADD3_CORE(OP, MV_DST, MV_SRC1, MV_SRC2)   \
            __ASM_EMIT("cmp         $0x10, %[count]") \
            __ASM_EMIT("jb          2f") \
            /* 16x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0") \
            __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm1") \
            __ASM_EMIT(MV_SRC1 "    0x20(%[src1]), %%xmm2") \
            __ASM_EMIT(MV_SRC1 "    0x30(%[src1]), %%xmm3") \
            __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm4") \
            __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm5") \
            __ASM_EMIT(MV_SRC2 "    0x20(%[src2]), %%xmm6") \
            __ASM_EMIT(MV_SRC2 "    0x30(%[src2]), %%xmm7") \
            __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
            __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
            __ASM_EMIT(OP "ps       %%xmm6, %%xmm2") \
            __ASM_EMIT(OP "ps       %%xmm7, %%xmm3") \
            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm1, 0x10(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm2, 0x20(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm3, 0x30(%[dst])") \
            __ASM_EMIT("sub         $0x10, %[count]") \
            __ASM_EMIT("add         $0x40, %[src1]") \
            __ASM_EMIT("add         $0x40, %[src2]") \
            __ASM_EMIT("add         $0x40, %[dst]") \
            __ASM_EMIT("cmp         $0x10, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 8x blocks */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("test        $8, %[count]") \
            __ASM_EMIT("jz          3f") \
            __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0") \
            __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm1") \
            __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm4") \
            __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm5") \
            __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
            __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm1, 0x10(%[dst])") \
            __ASM_EMIT("add         $0x20, %[src1]") \
            __ASM_EMIT("add         $0x20, %[src2]") \
            __ASM_EMIT("add         $0x20, %[dst]") \
            /* 4x blocks */ \
            __ASM_EMIT("3:") \
            __ASM_EMIT("test        $4, %[count]") \
            __ASM_EMIT("jz          4f") \
            __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0") \
            __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm4") \
            __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("add         $0x10, %[src1]") \
            __ASM_EMIT("add         $0x10, %[src2]") \
            __ASM_EMIT("add         $0x10, %[dst]") \
            /* 1x blocks */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("and         $3, %[count]") \
            __ASM_EMIT("jz          20000f") \
            __ASM_EMIT("5:") \
            __ASM_EMIT("movss       0x00(%[src1]), %%xmm0") \
            __ASM_EMIT("movss       0x00(%[src2]), %%xmm4") \
            __ASM_EMIT(OP "ss       %%xmm4, %%xmm0") \
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("add         $0x04, %[src1]") \
            __ASM_EMIT("add         $0x04, %[src2]") \
            __ASM_EMIT("add         $0x04, %[dst]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jnz         5b")

        void add3(float *dst, const float *src1, const float *src2, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("test       $0x0f, %[dst]")
                __ASM_EMIT("jnz        10100f")
                    __ASM_EMIT("test        $0x0f, %[src1]")
                    __ASM_EMIT("jnz         10010f")
                        __ASM_EMIT("test        $0x0f, %[src2]")
                        __ASM_EMIT("jnz         10001f")
                            ADD3_CORE("add", "movaps", "movaps", "movaps")
                            __ASM_EMIT("jmp         20000f")
                        __ASM_EMIT("10001:")
                            ADD3_CORE("add", "movaps", "movaps", "movups")
                            __ASM_EMIT("jmp         20000f")
                    __ASM_EMIT("10010:")
                        __ASM_EMIT("test        $0x0f, %[src2]")
                        __ASM_EMIT("jnz         10011f")
                            ADD3_CORE("add", "movaps", "movups", "movaps")
                            __ASM_EMIT("jmp         20000f")
                        __ASM_EMIT("10011:")
                            ADD3_CORE("add", "movaps", "movups", "movups")
                            __ASM_EMIT("jmp         20000f")
                __ASM_EMIT("10100:")
                    __ASM_EMIT("test        $0x0f, %[src1]")
                    __ASM_EMIT("jnz         10110f")
                        __ASM_EMIT("test        $0x0f, %[src2]")
                        __ASM_EMIT("jnz         10101f")
                            ADD3_CORE("add", "movups", "movaps", "movaps")
                            __ASM_EMIT("jmp         20000f")
                        __ASM_EMIT("10101:")
                            ADD3_CORE("add", "movups", "movaps", "movups")
                            __ASM_EMIT("jmp         20000f")
                    __ASM_EMIT("10110:")
                        __ASM_EMIT("test        $0x0f, %[src2]")
                        __ASM_EMIT("jnz         10111f")
                            ADD3_CORE("add", "movups", "movups", "movaps")
                            __ASM_EMIT("jmp         20000f")
                        __ASM_EMIT("10111:")
                            ADD3_CORE("add", "movups", "movups", "movups")

                __ASM_EMIT("20000:")

                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
                :
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void sub3(float *dst, const float *src1, const float *src2, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("test       $0x0f, %[dst]")
                __ASM_EMIT("jnz        10100f")
                    __ASM_EMIT("test        $0x0f, %[src1]")
                    __ASM_EMIT("jnz         10010f")
                        __ASM_EMIT("test        $0x0f, %[src2]")
                        __ASM_EMIT("jnz         10001f")
                            ADD3_CORE("sub", "movaps", "movaps", "movaps")
                            __ASM_EMIT("jmp         20000f")
                        __ASM_EMIT("10001:")
                            ADD3_CORE("sub", "movaps", "movaps", "movups")
                            __ASM_EMIT("jmp         20000f")
                    __ASM_EMIT("10010:")
                        __ASM_EMIT("test        $0x0f, %[src2]")
                        __ASM_EMIT("jnz         10011f")
                            ADD3_CORE("sub", "movaps", "movups", "movaps")
                            __ASM_EMIT("jmp         20000f")
                        __ASM_EMIT("10011:")
                            ADD3_CORE("sub", "movaps", "movups", "movups")
                            __ASM_EMIT("jmp         20000f")
                __ASM_EMIT("10100:")
                    __ASM_EMIT("test        $0x0f, %[src1]")
                    __ASM_EMIT("jnz         10110f")
                        __ASM_EMIT("test        $0x0f, %[src2]")
                        __ASM_EMIT("jnz         10101f")
                            ADD3_CORE("sub", "movups", "movaps", "movaps")
                            __ASM_EMIT("jmp         20000f")
                        __ASM_EMIT("10101:")
                            ADD3_CORE("sub", "movups", "movaps", "movups")
                            __ASM_EMIT("jmp         20000f")
                    __ASM_EMIT("10110:")
                        __ASM_EMIT("test        $0x0f, %[src2]")
                        __ASM_EMIT("jnz         10111f")
                            ADD3_CORE("sub", "movups", "movups", "movaps")
                            __ASM_EMIT("jmp         20000f")
                        __ASM_EMIT("10111:")
                            ADD3_CORE("sub", "movups", "movups", "movups")

                __ASM_EMIT("20000:")

                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
                :
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef ADD3_CORE

    }
}

#endif /* CORE_X86_SSE_ADDSUB_H_ */
