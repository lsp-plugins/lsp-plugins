/*
 * bt.cpp
 *
 *  Created on: 24 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

#define CASCADES            11
#define BIQUAD_X1_FLOATS    (sizeof(biquad_x1_t) / sizeof(float))
#define BIQUAD_X2_FLOATS    (sizeof(biquad_x2_t) / sizeof(float))
#define BIQUAD_X4_FLOATS    (sizeof(biquad_x4_t) / sizeof(float))
#define BIQUAD_X8_FLOATS    (sizeof(biquad_x8_t) / sizeof(float))
#define CASCADE_FLOATS      (sizeof(f_cascade_t) / sizeof(float))

namespace native
{
    void bilinear_transform_x1(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count);
    void bilinear_transform_x2(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count);
    void bilinear_transform_x4(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count);
    void bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void bilinear_transform_x1(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x2(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x4(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);
        }

        namespace avx
        {
            void x64_bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);
        }
    )
)

typedef void (* bilinear_transform_x1_t)(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count);
typedef void (* bilinear_transform_x2_t)(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count);
typedef void (* bilinear_transform_x4_t)(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count);
typedef void (* bilinear_transform_x8_t)(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);

UTEST_BEGIN("dsp.filters", bt)

    void call(const char *text, bilinear_transform_x1_t f1, bilinear_transform_x1_t f2)
    {
        if (!UTEST_SUPPORTED(f1))
            return;
        if (!UTEST_SUPPORTED(f2))
            return;

        printf("Testing %s bilinear transformation\n", text);

        FloatBuffer src(CASCADE_FLOATS * CASCADES, 64, true);
        FloatBuffer dst1(BIQUAD_X1_FLOATS * CASCADES, 64, true);
        FloatBuffer dst2(BIQUAD_X1_FLOATS * CASCADES, 64, true);

        f_cascade_t *bc = src.data<f_cascade_t>();
        for (size_t i=0; i<CASCADES; ++i)
        {
            float kt = i * 0.1;
            float kb = i * 0.05;
            bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
            bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
        }

        f1(dst1.data<biquad_x1_t>(), bc, 1.5f, CASCADES);
        f2(dst2.data<biquad_x1_t>(), bc, 1.5f, CASCADES);

        UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
        if (!dst1.equals_relative(dst2, 1e-4f))
        {
            src.dump("src");
            dst1.dump("dst1");
            dst2.dump("dst2");
            UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
        }
    }

    void call(const char *text, bilinear_transform_x2_t f1, bilinear_transform_x2_t f2)
    {
        if (!UTEST_SUPPORTED(f1))
            return;
        if (!UTEST_SUPPORTED(f2))
            return;

        printf("Testing %s bilinear transformation\n", text);

        size_t n = (CASCADES + 1) * 2;
        FloatBuffer src(CASCADE_FLOATS * n, 64, true);
        FloatBuffer dst1(BIQUAD_X2_FLOATS * (CASCADES + 1), 64, true);
        FloatBuffer dst2(BIQUAD_X2_FLOATS * (CASCADES + 1), 64, true);

        f_cascade_t *bc = src.data<f_cascade_t>();
        for (size_t i=0; i<n; ++i)
        {
            float kt = i * 0.1;
            float kb = i * 0.05;
            bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
            bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
        }

        f1(dst1.data<biquad_x2_t>(), bc, 1.5f, CASCADES + 1);
        f2(dst2.data<biquad_x2_t>(), bc, 1.5f, CASCADES + 1);

        UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
        if (!dst1.equals_relative(dst2, 1e-4f))
        {
            src.dump("src");
            dst1.dump("dst1");
            dst2.dump("dst2");
            UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
        }
    }

    void call(const char *text, bilinear_transform_x4_t f1, bilinear_transform_x4_t f2)
    {
        if (!UTEST_SUPPORTED(f1))
            return;
        if (!UTEST_SUPPORTED(f2))
            return;

        printf("Testing %s bilinear transformation\n", text);

        size_t n = (CASCADES + 3) * 4;
        FloatBuffer src(CASCADE_FLOATS * n, 64, true);
        FloatBuffer dst1(BIQUAD_X4_FLOATS * (CASCADES + 3), 64, true);
        FloatBuffer dst2(BIQUAD_X4_FLOATS * (CASCADES + 3), 64, true);

        f_cascade_t *bc = src.data<f_cascade_t>();
        for (size_t i=0; i<n; ++i)
        {
            float kt = i * 0.1;
            float kb = i * 0.05;
            bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
            bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
        }

        f1(dst1.data<biquad_x4_t>(), bc, 1.5f, CASCADES + 3);
        f2(dst2.data<biquad_x4_t>(), bc, 1.5f, CASCADES + 3);

        UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
        if (!dst1.equals_relative(dst2, 1e-4f))
        {
            src.dump("src");
            dst1.dump("dst1");
            dst2.dump("dst2");
            UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
        }
    }

    void call(const char *text, bilinear_transform_x8_t f1, bilinear_transform_x8_t f2)
    {
        if (!UTEST_SUPPORTED(f1))
            return;
        if (!UTEST_SUPPORTED(f2))
            return;

        printf("Testing %s bilinear transformation\n", text);

        size_t n = (CASCADES + 7) * 8;
        FloatBuffer src(CASCADE_FLOATS * n, 64, true);
        FloatBuffer dst1(BIQUAD_X8_FLOATS * (CASCADES + 7), 64, true);
        FloatBuffer dst2(BIQUAD_X8_FLOATS * (CASCADES + 7), 64, true);

        f_cascade_t *bc = src.data<f_cascade_t>();
        for (size_t i=0; i<n; ++i)
        {
            float kt = i * 0.1;
            float kb = i * 0.05;
            bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
            bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
        }

        f1(dst1.data<biquad_x8_t>(), bc, 1.5f, CASCADES + 7);
        f2(dst2.data<biquad_x8_t>(), bc, 1.5f, CASCADES + 7);

        UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
        if (!dst1.equals_relative(dst2, 1e-4f))
        {
            src.dump("src");
            dst1.dump("dst1");
            dst2.dump("dst2");
            UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse::bilinear_transform_x1", native::bilinear_transform_x1, sse::bilinear_transform_x1));

        IF_ARCH_X86(call("sse::bilinear_transform_x2", native::bilinear_transform_x2, sse::bilinear_transform_x2));

        IF_ARCH_X86(call("sse::bilinear_transform_x4", native::bilinear_transform_x4, sse::bilinear_transform_x4));

        IF_ARCH_X86(call("sse::bilinear_transform_x8", native::bilinear_transform_x8, sse::bilinear_transform_x8));
        IF_ARCH_X86_64(call("sse3::x64_bilinear_transform_x8", native::bilinear_transform_x8, sse3::x64_bilinear_transform_x8));
        IF_ARCH_X86_64(call("avx::x64_bilinear_transform_x8", native::bilinear_transform_x8, avx::x64_bilinear_transform_x8));
    }

UTEST_END;
