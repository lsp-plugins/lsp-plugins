/*
 * scale4.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */


#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void    scale_add4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    scale_sub4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    scale_mul4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    scale_div4(float *dst, const float *src1, const float *src2, float k, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    scale_add4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_sub4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_mul4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_div4(float *dst, const float *src1, const float *src2, float k, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    scale_add4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_sub4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_mul4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_div4(float *dst, const float *src1, const float *src2, float k, size_t count);
    }
)

typedef void (* scale4_t)(float *dst, const float *src1, const float *src2, float k, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pmath", scale4, 5, 1000)

    void call(const char *label, float *dst, const float *src1, const float *src2, size_t count, scale4_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src1, src2, 0.5f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 6, 64);
        float *src1     = &dst[buf_size];
        float *src2     = &src1[buf_size];
        float *backup   = &src2[buf_size];

        for (size_t i=0; i < buf_size*3; ++i)
            dst[i]          = float(rand()) / RAND_MAX;
        dsp::copy(backup, dst, buf_size*3);

        #define CALL(...) \
            dsp::copy(dst, backup, buf_size*3); \
            call(__VA_ARGS__);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL("native:scale_add4 native", dst, src1, src2, count, native::scale_add4);
            IF_ARCH_X86(CALL("sse:scale_add4", dst, src1, src2, count, sse::scale_add4));
            IF_ARCH_ARM(CALL("neon_d32:scale_add4", dst, src1, src2, count, neon_d32::scale_add4));
            PTEST_SEPARATOR;

            CALL("native:scale_sub4 native", dst, src1, src2, count, native::scale_sub4);
            IF_ARCH_X86(CALL("sse:scale_sub4", dst, src1, src2, count, sse::scale_sub4));
            IF_ARCH_ARM(CALL("neon_d32:scale_sub4", dst, src1, src2, count, neon_d32::scale_sub4));
            PTEST_SEPARATOR;

            CALL("native:scale_mul4 native", dst, src1, src2, count, native::scale_mul4);
            IF_ARCH_X86(CALL("sse:scale_mul4", dst, src1, src2, count, sse::scale_mul4));
            IF_ARCH_ARM(CALL("neon_d32:scale_mul4", dst, src1, src2, count, neon_d32::scale_mul4));
            PTEST_SEPARATOR;

            CALL("native:scale_div4 native", dst, src1, src2, count, native::scale_div4);
            IF_ARCH_X86(CALL("sse:scale_div4", dst, src1, src2, count, sse::scale_div4));
            IF_ARCH_ARM(CALL("neon_d32:scale_div4", dst, src1, src2, count, neon_d32::scale_div4));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


