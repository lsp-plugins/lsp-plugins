/*
 * minmax.cpp
 *
 *  Created on: 08 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    float   min(const float *src, size_t count);
    float   max(const float *src, size_t count);
    void    minmax(const float *src, size_t count, float *min, float *max);

    float   abs_min(const float *src, size_t count);
    float   abs_max(const float *src, size_t count);
    void    abs_minmax(const float *src, size_t count, float *min, float *max);
}

IF_ARCH_X86(
    namespace sse
    {
        float   min(const float *src, size_t count);
        float   max(const float *src, size_t count);
        void    minmax(const float *src, size_t count, float *min, float *max);

        float   abs_min(const float *src, size_t count);
        float   abs_max(const float *src, size_t count);
        void    abs_minmax(const float *src, size_t count, float *min, float *max);
    }

    namespace avx
    {
        float   min(const float *src, size_t count);
        float   max(const float *src, size_t count);
        void    minmax(const float *src, size_t count, float *min, float *max);

        float   abs_min(const float *src, size_t count);
        float   abs_max(const float *src, size_t count);
        void    abs_minmax(const float *src, size_t count, float *min, float *max);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        float   min(const float *src, size_t count);
        float   max(const float *src, size_t count);
        void    minmax(const float *src, size_t count, float *min, float *max);

        float   abs_min(const float *src, size_t count);
        float   abs_max(const float *src, size_t count);
        void    abs_minmax(const float *src, size_t count, float *min, float *max);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        float   min(const float *src, size_t count);
        float   max(const float *src, size_t count);
        void    minmax(const float *src, size_t count, float *min, float *max);

        float   abs_min(const float *src, size_t count);
        float   abs_max(const float *src, size_t count);
        void    abs_minmax(const float *src, size_t count, float *min, float *max);
    }
)

typedef float (* search1_t) (const float *src, size_t count);
typedef void  (* search2_t) (const float *src, size_t count, float *min, float *max);

//-----------------------------------------------------------------------------
// Performance test for minimum and maximum searching
PTEST_BEGIN("dsp.search", minmax, 5, 1000)

    void call(const char *label, const float *in, size_t count, search1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(in, count);
        );
    }

    void call(const char *label, const float *in, size_t count, search2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        float min, max;
        PTEST_LOOP(buf,
            func(in, count, &min, &max);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;

        float *in       = alloc_aligned<float>(data, buf_size, 64);
        for (size_t i=0; i < (1 << MAX_RANK); ++i)
            in[i]          = float(rand()) / RAND_MAX;

        #define CALL(func) \
            call(#func, in, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            //--------------
            CALL(native::min);
            IF_ARCH_X86(CALL(sse::min));
            IF_ARCH_X86(CALL(avx::min));
            IF_ARCH_ARM(CALL(neon_d32::min));
            IF_ARCH_AARCH64(CALL(asimd::min));

            CALL(native::abs_min);
            IF_ARCH_X86(CALL(sse::abs_min));
            IF_ARCH_X86(CALL(avx::abs_min));
            IF_ARCH_ARM(CALL(neon_d32::abs_min));
            IF_ARCH_AARCH64(CALL(asimd::abs_min));
            PTEST_SEPARATOR;

            //--------------
            CALL(native::max);
            IF_ARCH_X86(CALL(sse::max));
            IF_ARCH_X86(CALL(avx::max));
            IF_ARCH_ARM(CALL(neon_d32::max));
            IF_ARCH_AARCH64(CALL(asimd::max));

            CALL(native::abs_max);
            IF_ARCH_X86(CALL(sse::abs_max));
            IF_ARCH_X86(CALL(avx::abs_max));
            IF_ARCH_ARM(CALL(neon_d32::abs_max));
            IF_ARCH_AARCH64(CALL(asimd::abs_max));
            PTEST_SEPARATOR;

            //--------------
            CALL(native::minmax);
            IF_ARCH_X86(CALL(sse::minmax));
            IF_ARCH_X86(CALL(avx::minmax));
            IF_ARCH_ARM(CALL(neon_d32::minmax));
            IF_ARCH_AARCH64(CALL(asimd::minmax));

            CALL(native::abs_minmax);
            IF_ARCH_X86(CALL(sse::abs_minmax));
            IF_ARCH_X86(CALL(avx::abs_minmax));
            IF_ARCH_ARM(CALL(neon_d32::abs_minmax));
            IF_ARCH_AARCH64(CALL(asimd::abs_minmax));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }

PTEST_END


