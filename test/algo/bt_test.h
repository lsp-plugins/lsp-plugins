#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>
#include <core/windows.h>
#include <core/envelope.h>

#define CASCADES            11
#define BIQUAD_X1_FLOATS    (sizeof(biquad_x1_t) / sizeof(float))
#define BIQUAD_X2_FLOATS    (sizeof(biquad_x2_t) / sizeof(float))
#define BIQUAD_X4_FLOATS    (sizeof(biquad_x4_t) / sizeof(float))
#define BIQUAD_X8_FLOATS    (sizeof(biquad_x8_t) / sizeof(float))
#define CASCADE_FLOATS      (sizeof(f_cascade_t) / sizeof(float))

namespace lsp
{
    namespace native
    {
        void bilinear_transform_x1(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x2(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x4(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);
    }

    namespace sse
    {
        void bilinear_transform_x1(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x2(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x4(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);
    }

    namespace sse3
    {
        void x64_bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);
    }

#ifdef __AVX__
    namespace avx
    {
        void x64_bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);
    }
#endif /* __AVX__ */
}


namespace bt_test
{
    using namespace lsp;
    using namespace test;

//    void transpose()
//    {
//        float m[16] = {
//            1, 2, 3, 4,
//            5, 6, 7, 8,
//            9, 10, 11, 12,
//            0, 0, 0, 0
//        };
//
//        float x2, x3, x4, x5, x6, x7;
//
//        __asm__ __volatile__ (
//            __ASM_EMIT("movups      0x00(%[m]), %[x4]")
//            __ASM_EMIT("movups      0x10(%[m]), %[x3]")
//            __ASM_EMIT("movups      0x20(%[m]), %[x2]")
//
//            __ASM_EMIT("movaps      %[x3], %[x5]")              // x5 = fa2[0] fa2[1] fa2[2] fa2[3]
//            __ASM_EMIT("unpcklps    %[x2], %[x3]")              // x3 = fa2[0] fa3[0] fa2[1] fa3[1]
//            __ASM_EMIT("movaps      %[x4], %[x6]")              // x6 = fa0[0] fa0[1] fa0[2] fa0[3]
//            __ASM_EMIT("unpckhps    %[x2], %[x5]")              // x5 = fa2[2] fa3[2] fa2[3] fa3[3]
//            __ASM_EMIT("movaps      %[x6], %[x7]")              // x7 = fa0[0] fa0[1] fa0[2] fa0[3]
//            __ASM_EMIT("movaps      %[x4], %[x2]")              // x2 = fa0[0] fa0[1] fa0[2] fa0[3]
//            __ASM_EMIT("shufps      $0x4a, %[x5], %[x6]")       // x6 = fa0[2] fa0[2] fa2[2] fa3[2]
//            __ASM_EMIT("shufps      $0x40, %[x3], %[x4]")       // x4 = fa0[0] fa0[0] fa2[0] fa3[0]
//            __ASM_EMIT("shufps      $0xef, %[x5], %[x7]")       // x7 = fa0[3] fa0[3] fa2[3] fa3[3]
//            __ASM_EMIT("shufps      $0xe5, %[x3], %[x2]")       // x2 = fa0[1] fa0[1] fa2[1] fa3[1]
//
//            __ASM_EMIT("movaps      %[x4], 0x00(%[m])")
//            __ASM_EMIT("movaps      %[x2], 0x10(%[m])")
//            __ASM_EMIT("movaps      %[x6], 0x20(%[m])")
//            __ASM_EMIT("movaps      %[x7], 0x30(%[m])")
//
//            : [x2] "=&x" (x2), [x3] "=&x" (x3),
//              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7)
//            : [m] "r" (m)
//            : "memory"
//        );
//
//        for (int i=0; i<16; ++i)
//        {
//            printf("%f ", m[i]);
//            if ((i & 3) == 3)
//                printf("\n");
//        }
//    }

//    void test_avx_transpose()
//    {
//        float bc[64];
//        float bf[48];
//
//        printf("Input:\n");
//        for (int i=0; i<8; ++i)
//        {
//            float *p = &bc[i * 8];
//            p[0]    = (i+1) + 0.1;
//            p[1]    = (i+1) + 0.2;
//            p[2]    = (i+1) + 0.3;
//            p[3]    = (i+1) + 0.4;
//            p[4]    = (i+1) + 0.5;
//            p[5]    = (i+1) + 0.6;
//            p[6]    = (i+1) + 0.7;
//            p[7]    = (i+1) + 0.8;
//            printf("  %f %f %f %f %f %f %f %f\n",
//                    p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
//        }
//
//        __asm__ __volatile__
//        (
//            __ASM_EMIT("vmovups         0x00(%[bc]), %%ymm2")               // ymm2  = t0[0] t1[0] t2[0]   ?   b0[0] b1[0] b2[0]   ?
//            __ASM_EMIT("vmovups         0x20(%[bc]), %%ymm3")               // ymm3  = t0[1] t1[1] t2[1]   ?   b0[1] b1[1] b2[1]   ?
//            __ASM_EMIT("vmovups         0x40(%[bc]), %%ymm4")               // ymm4  = t0[2] t1[2] t2[2]   ?   b0[2] b1[2] b2[2]   ?
//            __ASM_EMIT("vmovups         0x60(%[bc]), %%ymm5")               // ymm5  = t0[3] t1[3] t2[3]   ?   b0[3] b1[3] b2[3]   ?
//            __ASM_EMIT("vmovups         0x80(%[bc]), %%ymm6")               // ymm6  = t0[4] t1[4] t2[4]   ?   b0[4] b1[4] b2[4]   ?
//            __ASM_EMIT("vmovups         0xa0(%[bc]), %%ymm7")               // ymm7  = t0[5] t1[5] t2[5]   ?   b0[5] b1[5] b2[5]   ?
//            __ASM_EMIT("vmovups         0xc0(%[bc]), %%ymm8")               // ymm8  = t0[6] t1[6] t2[6]   ?   b0[6] b1[6] b2[6]   ?
//            __ASM_EMIT("vmovups         0xe0(%[bc]), %%ymm9")               // ymm9  = t0[7] t1[7] t2[7]   ?   b0[7] b1[7] b2[7]   ?
//
//            // Transpose, step 1
//            __ASM_EMIT("vunpcklps       %%ymm3, %%ymm2, %%ymm10")           // ymm10 = t0[0] t0[1] t1[0] t1[1] b0[0] b0[1] b1[0] b1[1]
//            __ASM_EMIT("vunpcklps       %%ymm5, %%ymm4, %%ymm11")           // ymm11 = t0[2] t0[3] t1[2] t1[3] b0[2] b0[3] b1[2] b1[3]
//            __ASM_EMIT("vunpcklps       %%ymm7, %%ymm6, %%ymm12")           // ymm12 = t0[4] t0[5] t1[4] t1[5] b0[4] b0[5] b1[4] b1[5]
//            __ASM_EMIT("vunpcklps       %%ymm9, %%ymm8, %%ymm13")           // ymm13 = t0[6] t0[7] t1[6] t1[7] b0[6] b0[7] b1[6] b1[7]
//            __ASM_EMIT("vunpckhps       %%ymm3, %%ymm2, %%ymm2")            // ymm2  = t2[0] t2[1]   ?     ?   b2[0] b2[1]   ?     ?
//            __ASM_EMIT("vunpckhps       %%ymm5, %%ymm4, %%ymm4")            // ymm4  = t2[2] t2[3]   ?     ?   b2[2] b2[3]   ?     ?
//            __ASM_EMIT("vunpckhps       %%ymm7, %%ymm6, %%ymm6")            // ymm6  = t2[4] t2[5]   ?     ?   b2[4] b2[5]   ?     ?
//            __ASM_EMIT("vunpckhps       %%ymm9, %%ymm8, %%ymm8")            // ymm8  = t2[6] t2[7]   ?     ?   b2[6] b2[7]   ?     ?
//
//            // Transpose, step 2
//            __ASM_EMIT("vshufps         $0x44, %%ymm11, %%ymm10, %%ymm3")   // ymm3  = t0[0] t0[1] t0[2] t0[3] b0[0] b0[1] b0[2] b0[3]
//            __ASM_EMIT("vshufps         $0x44, %%ymm13, %%ymm12, %%ymm5")   // ymm5  = t0[4] t0[5] t0[6] t0[7] b0[4] b0[5] b0[6] b0[7]
//            __ASM_EMIT("vshufps         $0x44, %%ymm4,  %%ymm2,  %%ymm2")   // ymm2  = t2[0] t2[1] t2[2] t2[3] b2[0] b2[1] b2[2] b2[3]
//            __ASM_EMIT("vshufps         $0x44, %%ymm8,  %%ymm6,  %%ymm6")   // ymm6  = t2[4] t2[5] t2[6] t2[7] b2[4] b2[5] b2[6] b2[7]
//            __ASM_EMIT("vshufps         $0xee, %%ymm11, %%ymm10, %%ymm4")   // ymm4  = t1[0] t1[1] t1[2] t1[3] b1[0] b1[1] b1[2] b1[3]
//            __ASM_EMIT("vshufps         $0xee, %%ymm13, %%ymm12, %%ymm7")   // ymm7  = t1[4] t1[5] t1[6] t1[7] b1[4] b1[5] b1[6] b1[7]
//
//            // Transpose, step 3
//            __ASM_EMIT("vextractf128    $1, %%ymm3,  %%xmm8")               // xmm8  = b0[0] b0[1] b0[2] b0[3]   0     0     0     0
//            __ASM_EMIT("vextractf128    $1, %%ymm2,  %%xmm9")               // xmm9  = b2[0] b2[1] b2[2] b2[3]   0     0     0     0
//            __ASM_EMIT("vextractf128    $1, %%ymm4,  %%xmm10")              // xmm10 = b1[0] b1[1] b1[2] b1[3]   0     0     0     0  // -
//            __ASM_EMIT("vinsertf128     $1, %%xmm5,  %%ymm3, %%ymm3")       // ymm3  = t0[0] t0[1] t0[2] t0[3] t0[4] t0[5] t0[6] t0[7]
//            __ASM_EMIT("vinsertf128     $1, %%xmm6,  %%ymm2, %%ymm2")       // ymm2  = t2[0] t2[1] t2[2] t2[3] t2[4] t2[5] t2[6] t2[7]
//            __ASM_EMIT("vinsertf128     $1, %%xmm7,  %%ymm4, %%ymm4")       // ymm4  = t1[0] t1[1] t1[2] t1[3] t1[4] t1[5] t1[6] t1[7] // -
//            __ASM_EMIT("vinsertf128     $0, %%xmm8,  %%ymm5, %%ymm5")       // ymm5  = b0[0] b0[1] b0[2] b0[3] b0[4] b0[5] b0[6] b0[7]
//            __ASM_EMIT("vinsertf128     $0, %%xmm9,  %%ymm6, %%ymm6")       // ymm6  = b2[0] b2[1] b2[2] b2[3] b2[4] b2[5] b2[6] b2[7]
//            __ASM_EMIT("vinsertf128     $0, %%xmm10, %%ymm7, %%ymm7")       // ymm7  = b1[0] b1[1] b1[2] b1[3] b1[4] b1[5] b1[6] b1[7] // -
//
//            __ASM_EMIT("vmovups         %%ymm3, 0x00(%[bf])")
//            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[bf])") // -
//            __ASM_EMIT("vmovups         %%ymm2, 0x40(%[bf])")
//            __ASM_EMIT("vmovups         %%ymm5, 0x60(%[bf])")
//            __ASM_EMIT("vmovups         %%ymm7, 0x80(%[bf])") // -
//            __ASM_EMIT("vmovups         %%ymm6, 0xa0(%[bf])")
//            :
//            : [bc] "r" (bc), [bf] "r" (bf)
//            : "memory"
//        );
//
//        printf("Output:\n");
//        for (int i=0; i<6; ++i)
//        {
//            float *p = &bf[i * 8];
//            printf("  %f %f %f %f %f %f %f %f\n",
//                    p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
//        }
//    }

    void test_x1()
    {
        FBuffer src(CASCADE_FLOATS * CASCADES, true);
        FBuffer dst1(BIQUAD_X1_FLOATS * CASCADES, true);
        FBuffer dst2(BIQUAD_X1_FLOATS * CASCADES, true);

//        transpose();

        f_cascade_t *bc = src.data<f_cascade_t>();
        for (size_t i=0; i<CASCADES; ++i)
        {
            float kt = i * 0.1;
            float kb = i * 0.05;
            bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
            bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
        }

//        for (ssize_t i=0; i<CASCADES; ++i)
//        {
//            bc[i].t[0] = i; bc[i].t[1] = i + 0.1;  bc[i].t[2] = i + 0.2; bc[i].t[3] = 0;
//            bc[i].b[0] = -i; bc[i].b[1] = -i - 0.1; bc[i].b[2]  = -i - 0.2; bc[i].b[3] = 0;
//        }

        native::bilinear_transform_x1(dst1.data<biquad_x1_t>(), bc, 1.5f, CASCADES);
        sse::bilinear_transform_x1(dst2.data<biquad_x1_t>(), bc, 1.5f, CASCADES);

        for (size_t i=0; i<CASCADES; ++i)
        {
            printf("i=%d\n", int(i));
            src.dump ("  src ", i * CASCADE_FLOATS, CASCADE_FLOATS);
            dst1.dump("  dst1", i * BIQUAD_X1_FLOATS, BIQUAD_X1_FLOATS);
            dst2.dump("  dst2", i * BIQUAD_X1_FLOATS, BIQUAD_X1_FLOATS);
        }
    }

    void test_x2()
    {
        size_t n = (CASCADES + 1) * 2;
        FBuffer src(CASCADE_FLOATS * n, true);
        FBuffer dst1(BIQUAD_X2_FLOATS * (CASCADES + 1), true);
        FBuffer dst2(BIQUAD_X2_FLOATS * (CASCADES + 1), true);

//        transpose();

        f_cascade_t *bc = src.data<f_cascade_t>();
        for (size_t i=0; i<n; ++i)
        {
            float kt = i * 0.1;
            float kb = i * 0.05;
            bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
            bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
        }

        native::bilinear_transform_x2(dst1.data<biquad_x2_t>(), bc, 1.5f, CASCADES + 1);
        sse::bilinear_transform_x2(dst2.data<biquad_x2_t>(), bc, 1.5f, CASCADES + 1);

        for (size_t i=0; i<(CASCADES + 1); ++i)
        {
            printf("i=%d\n", int(i));
            src.dump ("  src ", i * CASCADE_FLOATS * 2, CASCADE_FLOATS * 2);
            dst1.dump("  dst1", i * BIQUAD_X2_FLOATS, BIQUAD_X2_FLOATS);
            dst2.dump("  dst2", i * BIQUAD_X2_FLOATS, BIQUAD_X2_FLOATS);
        }
    }

    void test_x4()
    {
        size_t n = (CASCADES + 3) * 4;
        FBuffer src(CASCADE_FLOATS * n, true);
        FBuffer dst1(BIQUAD_X4_FLOATS * (CASCADES + 3), true);
        FBuffer dst2(BIQUAD_X4_FLOATS * (CASCADES + 3), true);

//        transpose();

        f_cascade_t *bc = src.data<f_cascade_t>();
        for (size_t i=0; i<n; ++i)
        {
            float kt = i * 0.1;
            float kb = i * 0.05;
            bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
            bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
        }

        native::bilinear_transform_x4(dst1.data<biquad_x4_t>(), bc, 1.5f, CASCADES + 3);
        sse::bilinear_transform_x4(dst2.data<biquad_x4_t>(), bc, 1.5f, CASCADES + 3);

        for (size_t i=0; i<(CASCADES + 1); ++i)
        {
            printf("i=%d\n", int(i));
            src.dump ("  src ", i * CASCADE_FLOATS * 4, CASCADE_FLOATS * 4);
            dst1.dump("  dst1", i * BIQUAD_X4_FLOATS, BIQUAD_X4_FLOATS);
            dst2.dump("  dst2", i * BIQUAD_X4_FLOATS, BIQUAD_X4_FLOATS);
        }
    }

    void test_x8()
    {
        size_t n = (CASCADES + 7) * 8;
        FBuffer src(CASCADE_FLOATS * n, true);
        FBuffer dst1(BIQUAD_X8_FLOATS * (CASCADES + 7), true);
        FBuffer dst2(BIQUAD_X8_FLOATS * (CASCADES + 7), true);
        FBuffer dst3(BIQUAD_X8_FLOATS * (CASCADES + 7), true);

//        transpose();

        f_cascade_t *bc = src.data<f_cascade_t>();
        for (size_t i=0; i<n; ++i)
        {
            float kt = i * 0.1;
            float kb = i * 0.05;
            bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
            bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
        }

        native::bilinear_transform_x8(dst1.data<biquad_x8_t>(), bc, 1.5f, CASCADES + 7);
        sse::bilinear_transform_x8(dst2.data<biquad_x8_t>(), bc, 1.5f, CASCADES + 7);
#ifdef __AVX__
        avx::x64_bilinear_transform_x8(dst3.data<biquad_x8_t>(), bc, 1.5f, CASCADES + 7);
#else
        sse3::x64_bilinear_transform_x8(dst3.data<biquad_x8_t>(), bc, 1.5f, CASCADES + 7);
#endif /* __AVX__ */

        for (size_t i=0; i<(CASCADES + 1); ++i)
        {
            printf("i=%d\n", int(i));
            src.dump ("  src ", i * CASCADE_FLOATS * 8, CASCADE_FLOATS * 8);
            dst1.dump("  dst1", i * BIQUAD_X8_FLOATS, BIQUAD_X8_FLOATS);
            dst2.dump("  dst2", i * BIQUAD_X8_FLOATS, BIQUAD_X8_FLOATS);
            dst3.dump("  dst3", i * BIQUAD_X8_FLOATS, BIQUAD_X8_FLOATS);
        }
    }

    int test(int argc, const char **argv)
    {
        dsp::context_t ctx;

        dsp::init();
        dsp::start(&ctx);

//        test_avx_transpose();
//        test_x1();
//        test_x2();
//        test_x4();
        test_x8();

        dsp::finish(&ctx);

        return 0;
    }
}

#undef CASCADES
#undef BIQUAD_X1_FLOATS
#undef BIQUAD_X2_FLOATS
#undef BIQUAD_X4_FLOATS
#undef BIQUAD_X8_FLOATS
#undef CASCADE_FLOATS

