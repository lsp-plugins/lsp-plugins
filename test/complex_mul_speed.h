#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>

#include <core/types.h>
#include <core/dsp.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace native
    {
        void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
        void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
    }

    namespace sse
    {
        void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
        void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
    }

    namespace sse3
    {
        void x64_packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
    }
}

namespace complex_mul_speed_test
{
    using namespace lsp;

    typedef void (* complex_mul_t) (float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
    typedef void (* complex_packet_mul_t) (float *dst, const float *src1, const float *src2, size_t count);

    void test_cplx_mul(float *dst, float *src1, float *src2, size_t rank, complex_mul_t mul, const char *label)
    {
        printf("Testing %s of %d elements (rank = %d)...\n", label, 1 << rank, int(rank));
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do iterations
            for (size_t i=0; i<10000; ++i)
                mul(dst, &dst[1<<rank], src1, &src1[1<<rank], src2, &src2[1<<rank], 1<<rank);

            // Calculate statistics
            iterations     += 10000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_cplx_mul(float *dst, float *src1, float *src2, size_t rank, complex_packet_mul_t mul, const char *label)
    {
        printf("Testing %s of %d elements (rank = %d)...\n", label, 1 << rank, int(rank));
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<10000; ++i)
                mul(dst, src1, src2, 1<<rank);

            // Calculate statistics
            iterations     += 10000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        size_t buf_size = 1 << MAX_RANK;

        uint8_t *data   = new uint8_t[buf_size*6*sizeof(float) + DEFAULT_ALIGN];
        float *ptr      = reinterpret_cast<float *>(ALIGN_PTR(data, DEFAULT_ALIGN));

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
            printf("\n");
//            test_cplx_mul(out, in1, in2, i, native::complex_mul, "Native Unpacked Complex Multiplication");
            test_cplx_mul(out, in1, in2, i, native::packed_complex_mul, "Native Packed Complex Multiplication");
//            test_cplx_mul(out, in1, in2, i, sse::complex_mul, "SSE Unpacked Complex Multiplication");
            test_cplx_mul(out, in1, in2, i, sse::packed_complex_mul, "SSE Packed Complex Multiplication");
            test_cplx_mul(out, in1, in2, i, sse3::x64_packed_complex_mul, "SSE3 Packed Complex Multiplication");
        }

        delete [] data;

        return 0;
    }
    
}

#undef MIN_RANK
#undef MAX_RANK
