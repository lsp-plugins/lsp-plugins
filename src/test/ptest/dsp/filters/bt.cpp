/*
 * bt.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define PERF_BUF_SIZE   0x200

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

IF_ARCH_ARM(
    namespace neon_d32
    {
        void bilinear_transform_x1(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x2(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x4(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);
    }
)

typedef void (* bilinear_transform_x1_t)(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count);
typedef void (* bilinear_transform_x2_t)(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count);
typedef void (* bilinear_transform_x4_t)(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count);
typedef void (* bilinear_transform_x8_t)(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);

static const f_cascade_t test_c =
{
    1, 2, 1, 0,
    1, -2, 1, 0
};

//-----------------------------------------------------------------------------
// Performance test for bilinear transform
PTEST_BEGIN("dsp.filters", bt, 10, 10000)

    void call(const char * label, size_t count, bilinear_transform_x1_t func)
    {
        printf("Testing %s bilinear transform on buffer size %d ...\n", label, int(count));

        void *p1 = NULL, *p2 = NULL;
        biquad_x1_t *dst = alloc_aligned<biquad_x1_t>(p1, count, 64);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count, 64);

        for (size_t i=0; i<count; ++i)
            src[i]  = test_c;

        PTEST_LOOP(label,
            func(dst, src, 1.0f, count);
            func(dst, src, 1.0f, count);
            func(dst, src, 1.0f, count);
            func(dst, src, 1.0f, count);
            func(dst, src, 1.0f, count);
            func(dst, src, 1.0f, count);
            func(dst, src, 1.0f, count);
            func(dst, src, 1.0f, count);
        );

        free_aligned(p1);
        free_aligned(p2);
    }

    void call(const char * label, size_t count, bilinear_transform_x2_t func)
    {
        printf("Testing %s bilinear transform on buffer size %d ...\n", label, int(count));

        count++;
        void *p1 = NULL, *p2 = NULL;
        biquad_x2_t *dst = alloc_aligned<biquad_x2_t>(p1, count, 32);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count*2, 32);

        for (size_t i=0; i<count*2; ++i)
            src[i]  = test_c;

        PTEST_LOOP(label,
            func(dst, src, 1.0f, count);
            func(dst, src, 1.0f, count);
            func(dst, src, 1.0f, count);
            func(dst, src, 1.0f, count);
        );

        free_aligned(p1);
        free_aligned(p2);
    }

    void call(const char * label, size_t count, bilinear_transform_x4_t func)
    {
        printf("Testing %s bilinear transform on buffer size %d ...\n", label, int(count));

        count += 3;
        void *p1 = NULL, *p2 = NULL;
        biquad_x4_t *dst = alloc_aligned<biquad_x4_t>(p1, count, 32);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count*4, 32);

        for (size_t i=0; i<count*4; ++i)
            src[i]  = test_c;

        PTEST_LOOP(label,
            func(dst, src, 1.0f, count);
            func(dst, src, 1.0f, count);
        );

        free_aligned(p1);
        free_aligned(p2);
    }

    void call(const char * label, size_t count, bilinear_transform_x8_t func)
    {
        printf("Testing %s bilinear transform on buffer size %d ...\n", label, int(count));

        count += 7;
        void *p1 = NULL, *p2 = NULL;
        biquad_x8_t *dst = alloc_aligned<biquad_x8_t>(p1, count, 32);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count*8, 32);

        for (size_t i=0; i<count*8; ++i)
            src[i]  = test_c;

        PTEST_LOOP(label,
            func(dst, src, 1.0f, count);
        );

        free_aligned(p1);
        free_aligned(p2);
    }

    PTEST_MAIN
    {
        call("native::bilinear_transform_x1 x8", PERF_BUF_SIZE, native::bilinear_transform_x1);
        IF_ARCH_X86(call("sse::bilinear_transform_x1 x8", PERF_BUF_SIZE, sse::bilinear_transform_x1));
        IF_ARCH_ARM(call("neon_d32::bilinear_transform_x1 x8", PERF_BUF_SIZE, neon_d32::bilinear_transform_x1));
        PTEST_SEPARATOR;

        call("native::bilinear_transform_x2 x4", PERF_BUF_SIZE, native::bilinear_transform_x2);
        IF_ARCH_X86(call("sse::bilinear_transform_x2 x4", PERF_BUF_SIZE, sse::bilinear_transform_x2));
        IF_ARCH_ARM(call("neon_d32::bilinear_transform_x2 x4", PERF_BUF_SIZE, neon_d32::bilinear_transform_x2));
        PTEST_SEPARATOR;

        call("native::bilinear_transform_x4 x2", PERF_BUF_SIZE, native::bilinear_transform_x4);
        IF_ARCH_X86(call("sse::bilinear_transform_x4 x2", PERF_BUF_SIZE, sse::bilinear_transform_x4));
        IF_ARCH_ARM(call("neon_d32::bilinear_transform_x4 x2", PERF_BUF_SIZE, neon_d32::bilinear_transform_x4));
        PTEST_SEPARATOR;

        call("native::bilinear_transform_x8 x1", PERF_BUF_SIZE, native::bilinear_transform_x8);
        IF_ARCH_X86(call("sse::bilinear_transform_x8 x1", PERF_BUF_SIZE, sse::bilinear_transform_x8));
        IF_ARCH_X86_64(call("sse3::x64_bilinear_transform_x8 x1", PERF_BUF_SIZE, sse3::x64_bilinear_transform_x8));
        IF_ARCH_X86_64(call("avx::x64_bilinear_transform_x8 x1", PERF_BUF_SIZE, avx::x64_bilinear_transform_x8));
        IF_ARCH_ARM(call("neon_d32::bilinear_transform_x8 x1", PERF_BUF_SIZE, neon_d32::bilinear_transform_x8));
        PTEST_SEPARATOR;
    }

PTEST_END





