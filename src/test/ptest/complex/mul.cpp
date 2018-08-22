/*
 * mul.cpp
 *
 *  Created on: 22 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
    void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
        void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
    }

    namespace sse3
    {
        void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
        }

        namespace avx
        {
            void x64_packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
            void x64_packed_complex_mul_fma3(float *dst, const float *src1, const float *src2, size_t count);
        }
    )
)

typedef void (* complex_mul_t) (float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
typedef void (* packed_complex_mul_t) (float *dst, const float *src1, const float *src2, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.complex", mul, 5, 10000)

    void test_cplx_mul(const char *label, float *dst, const float *src1, const float *src2, size_t count, packed_complex_mul_t mul)
    {
        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            mul(dst, src1, src2, count);
        );
    }

    void test_cplx_mul(const char *label, float *dst, const float *src1, const float *src2, size_t count, complex_mul_t mul)
    {
        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            mul(dst, &dst[count], src1, &src1[count], src2, &src2[count], count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *ptr      = alloc_aligned<float>(data, buf_size *6, 64);

        float *out      = ptr;
        ptr            += buf_size*2;
        float *in1      = ptr;
        ptr            += buf_size*2;
        float *in2      = ptr;
        ptr            += buf_size*2;

        for (size_t i=0; i < (1 << (MAX_RANK + 1)); ++i)
        {
            in1[i]          = float(rand()) / RAND_MAX;
            in2[i]          = float(rand()) / RAND_MAX;
        }

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            test_cplx_mul("unpacked_native", out, in1, in2, count, native::complex_mul);
            test_cplx_mul("packed_native", out, in1, in2, count, native::packed_complex_mul);
            IF_ARCH_X86(test_cplx_mul("unpacked_sse", out, in1, in2, count, sse::complex_mul));
            IF_ARCH_X86(test_cplx_mul("packed_sse", out, in1, in2, count, sse::packed_complex_mul));
            IF_ARCH_X86(test_cplx_mul("packed_sse3", out, in1, in2, count, sse3::packed_complex_mul));
            IF_ARCH_X86_64(test_cplx_mul("x64_packed_sse3", out, in1, in2, count, sse3::x64_packed_complex_mul));
            IF_ARCH_X86_64(test_cplx_mul("x64_packed_avx", out, in1, in2, count, avx::x64_packed_complex_mul));
            IF_ARCH_X86_64(test_cplx_mul("x64_packed_fma3", out, in1, in2, count, avx::x64_packed_complex_mul_fma3));

            PTEST_SEPARATOR;
            printf("\n");
        }

        delete [] data;
    }
PTEST_END


