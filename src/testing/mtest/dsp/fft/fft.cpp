/*
 * fft.cpp
 *
 *  Created on: 25 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/bits.h>
#include <test/mtest.h>
#include <test/FloatBuffer.h>

#define RANK        4
#define BUF_SIZE    (1 << RANK)

static const float XFFT_DW[] __lsp_aligned16 =
{
    // Re, Im
    0.0000000000000000f, 1.0000000000000000f,
    0.0000000000000000f, 1.0000000000000000f,
    0.7071067811865475f, 0.7071067811865475f,
    0.9238795325112868f, 0.3826834323650898f,
    0.9807852804032305f, 0.1950903220161283f,
    0.9951847266721969f, 0.0980171403295606f,
    0.9987954562051724f, 0.0490676743274180f,
    0.9996988186962042f, 0.0245412285229123f,
    0.9999247018391445f, 0.0122715382857199f,
    0.9999811752826011f, 0.0061358846491545f,
    0.9999952938095762f, 0.0030679567629660f,
    0.9999988234517019f, 0.0015339801862848f,
    0.9999997058628822f, 0.0007669903187427f,
    0.9999999264657179f, 0.0003834951875714f,
    0.9999999816164293f, 0.0001917475973107f,
    0.9999999954041073f, 0.0000958737990960f,
    0.9999999988510268f, 0.0000479368996031f
};

static const float XFFT_A_RE[] __lsp_aligned16 =
{
    1.0000000000000000f, 0.7071067811865475f, 0.0000000000000000f, -0.7071067811865475f,
    1.0000000000000000f, 0.9238795325112868f, 0.7071067811865475f, 0.3826834323650898f,
    1.0000000000000000f, 0.9807852804032305f, 0.9238795325112868f, 0.8314696123025452f,
    1.0000000000000000f, 0.9951847266721969f, 0.9807852804032305f, 0.9569403357322089f,
    1.0000000000000000f, 0.9987954562051724f, 0.9951847266721969f, 0.9891765099647810f,
    1.0000000000000000f, 0.9996988186962042f, 0.9987954562051724f, 0.9972904566786902f,
    1.0000000000000000f, 0.9999247018391445f, 0.9996988186962042f, 0.9993223845883495f,
    1.0000000000000000f, 0.9999811752826011f, 0.9999247018391445f, 0.9998305817958234f,
    1.0000000000000000f, 0.9999952938095762f, 0.9999811752826011f, 0.9999576445519639f,
    1.0000000000000000f, 0.9999988234517019f, 0.9999952938095762f, 0.9999894110819284f,
    1.0000000000000000f, 0.9999997058628822f, 0.9999988234517019f, 0.9999973527669782f,
    1.0000000000000000f, 0.9999999264657179f, 0.9999997058628822f, 0.9999993381915255f,
    1.0000000000000000f, 0.9999999816164293f, 0.9999999264657179f, 0.9999998345478677f,
    1.0000000000000000f, 0.9999999954041073f, 0.9999999816164293f, 0.9999999586369661f,
    1.0000000000000000f, 0.9999999988510268f, 0.9999999954041073f, 0.9999999896592415f
};

static const float XFFT_A_IM[] __lsp_aligned16 =
{
    0.0000000000000000f, 0.7071067811865475f, 1.0000000000000000f, 0.7071067811865476f,
    0.0000000000000000f, 0.3826834323650898f, 0.7071067811865475f, 0.9238795325112867f,
    0.0000000000000000f, 0.1950903220161283f, 0.3826834323650898f, 0.5555702330196022f,
    0.0000000000000000f, 0.0980171403295606f, 0.1950903220161283f, 0.2902846772544624f,
    0.0000000000000000f, 0.0490676743274180f, 0.0980171403295606f, 0.1467304744553617f,
    0.0000000000000000f, 0.0245412285229123f, 0.0490676743274180f, 0.0735645635996674f,
    0.0000000000000000f, 0.0122715382857199f, 0.0245412285229123f, 0.0368072229413588f,
    0.0000000000000000f, 0.0061358846491545f, 0.0122715382857199f, 0.0184067299058048f,
    0.0000000000000000f, 0.0030679567629660f, 0.0061358846491545f, 0.0092037547820598f,
    0.0000000000000000f, 0.0015339801862848f, 0.0030679567629660f, 0.0046019261204486f,
    0.0000000000000000f, 0.0007669903187427f, 0.0015339801862848f, 0.0023009691514258f,
    0.0000000000000000f, 0.0003834951875714f, 0.0007669903187427f, 0.0011504853371138f,
    0.0000000000000000f, 0.0001917475973107f, 0.0003834951875714f, 0.0005752427637321f,
    0.0000000000000000f, 0.0000958737990960f, 0.0001917475973107f, 0.0002876213937629f,
    0.0000000000000000f, 0.0000479368996031f, 0.0000958737990960f, 0.0001438106983686f
};

static void scramble_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
{
    size_t items    = 1 << rank;

    // Scramble the order of samples
    if ((dst_re != src_re) && (dst_im != src_im))
    {
        for (size_t i = 0; i < items; ++i)
        {
            size_t j    = reverse_bits(i, rank);
            dst_re[i]   = src_re[j];
            dst_im[i]   = src_im[j];
        }
    }
    else
    {
        for (size_t i = 1; i < (items - 1); ++i)
        {
            size_t j = reverse_bits(i, rank);
            if (i >= j)
                continue;

            float re    = dst_re[i];
            float im    = dst_im[i];
            dst_re[i]   = dst_re[j];
            dst_im[i]   = dst_im[j];
            dst_re[j]   = re;
            dst_im[j]   = im;
        }
    }
}

static void start_direct_fft(float *dst_re, float *dst_im, size_t rank)
{
    size_t iterations    = 1 << (rank - 2);
    while (iterations--)
    {
        // Perform 4-calculations
        // s0' = s0 + s1
        // s1' = s0 - s1
        // s2' = s2 + s3
        // s3' = s2 - s3
        // s0'' = s0' + s2'
        // s1'' = s1' - j * s3'
        // s2'' = s0' - s2'
        // s3'' = s1' + j * s3'
        float s0_re     = dst_re[0] + dst_re[1];
        float s1_re     = dst_re[0] - dst_re[1];
        float s2_re     = dst_re[2] + dst_re[3];
        float s3_re     = dst_re[2] - dst_re[3];

        float s0_im     = dst_im[0] + dst_im[1];
        float s1_im     = dst_im[0] - dst_im[1];
        float s2_im     = dst_im[2] + dst_im[3];
        float s3_im     = dst_im[2] - dst_im[3];

        dst_re[0]       = s0_re + s2_re;
        dst_re[1]       = s1_re + s3_im;
        dst_re[2]       = s0_re - s2_re;
        dst_re[3]       = s1_re - s3_im;

        dst_im[0]       = s0_im + s2_im;
        dst_im[1]       = s1_im - s3_re;
        dst_im[2]       = s0_im - s2_im;
        dst_im[3]       = s1_im + s3_re;

        // Move pointers
        dst_re         += 4;
        dst_im         += 4;
    }
}

static void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
{
    scramble_fft(dst_re, dst_im, src_re, src_im, rank);
    start_direct_fft(dst_re, dst_im, rank);

    // Prepare for butterflies
    size_t items    = 1 << rank;

    float c_re[4], c_im[4], w_re[4], w_im[4];
    const float *dw     = XFFT_DW;
    const float *iw_re  = XFFT_A_RE;
    const float *iw_im  = XFFT_A_IM;

    // Iterate butterflies
    for (size_t n=4, bs=n << 1; n < items; n <<= 1, bs <<= 1)
    {
//        size_t n=4, bs=n << 1;

        for (size_t p=0; p<items; p += bs)
        {
//            printf("rank=%d, iw_re={%.6f, %.6f, %.6f, %.6f}, iw_im={%.6f, %.6f, %.6f, %.6f}, dw={%.6f, %.6f}\n",
//                    int(rank),
//                    iw_re[0], iw_re[1], iw_re[2], iw_re[3],
//                    iw_im[0], iw_im[1], iw_im[2], iw_im[3],
//                    dw[0], dw[1]
//            );


            // Set initial values of pointers
            float *a_re         = &dst_re[p];
            float *a_im         = &dst_im[p];
            float *b_re         = &a_re[n];
            float *b_im         = &a_im[n];

            w_re[0]             = iw_re[0];
            w_re[1]             = iw_re[1];
            w_re[2]             = iw_re[2];
            w_re[3]             = iw_re[3];
            w_im[0]             = iw_im[0];
            w_im[1]             = iw_im[1];
            w_im[2]             = iw_im[2];
            w_im[3]             = iw_im[3];

            for (size_t k=0; ;)
            {
                // Calculate complex c = w * b
                c_re[0]         = w_re[0] * b_re[0] + w_im[0] * b_im[0];
                c_re[1]         = w_re[1] * b_re[1] + w_im[1] * b_im[1];
                c_re[2]         = w_re[2] * b_re[2] + w_im[2] * b_im[2];
                c_re[3]         = w_re[3] * b_re[3] + w_im[3] * b_im[3];

                c_im[0]         = w_re[0] * b_im[0] - w_im[0] * b_re[0];
                c_im[1]         = w_re[1] * b_im[1] - w_im[1] * b_re[1];
                c_im[2]         = w_re[2] * b_im[2] - w_im[2] * b_re[2];
                c_im[3]         = w_re[3] * b_im[3] - w_im[3] * b_re[3];

                // Calculate the output values:
                // a'   = a + c
                // b'   = a - c
                b_re[0]         = a_re[0] - c_re[0];
                b_re[1]         = a_re[1] - c_re[1];
                b_re[2]         = a_re[2] - c_re[2];
                b_re[3]         = a_re[3] - c_re[3];

                b_im[0]         = a_im[0] - c_im[0];
                b_im[1]         = a_im[1] - c_im[1];
                b_im[2]         = a_im[2] - c_im[2];
                b_im[3]         = a_im[3] - c_im[3];

                a_re[0]         = a_re[0] + c_re[0];
                a_re[1]         = a_re[1] + c_re[1];
                a_re[2]         = a_re[2] + c_re[2];
                a_re[3]         = a_re[3] + c_re[3];

                a_im[0]         = a_im[0] + c_im[0];
                a_im[1]         = a_im[1] + c_im[1];
                a_im[2]         = a_im[2] + c_im[2];
                a_im[3]         = a_im[3] + c_im[3];

                // Update pointers
                a_re           += 4;
                a_im           += 4;
                b_re           += 4;
                b_im           += 4;

                if ((k += 4) >= n)
                    break;

                // Rotate w vector
                c_re[0]         = w_re[0]*dw[0] - w_im[0]*dw[1];
                c_re[1]         = w_re[1]*dw[0] - w_im[1]*dw[1];
                c_re[2]         = w_re[2]*dw[0] - w_im[2]*dw[1];
                c_re[3]         = w_re[3]*dw[0] - w_im[3]*dw[1];

                c_im[0]         = w_re[0]*dw[1] + w_im[0]*dw[0];
                c_im[1]         = w_re[1]*dw[1] + w_im[1]*dw[0];
                c_im[2]         = w_re[2]*dw[1] + w_im[2]*dw[0];
                c_im[3]         = w_re[3]*dw[1] + w_im[3]*dw[0];

                w_re[0]         = c_re[0];
                w_re[1]         = c_re[1];
                w_re[2]         = c_re[2];
                w_re[3]         = c_re[3];

                w_im[0]         = c_im[0];
                w_im[1]         = c_im[1];
                w_im[2]         = c_im[2];
                w_im[3]         = c_im[3];
            }
        }

        dw     += 2;
        iw_re  += 4;
        iw_im  += 4;
    }
}

namespace native
{
    void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
}

IF_ARCH_X86(
    namespace sse
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    }

    namespace avx
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

        void direct_fft_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    }
)

typedef void (* direct_fft_t)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
typedef void (* reverse_fft_t)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

MTEST_BEGIN("dsp.fft", fft)

    void test_direct_fft(const char *text, direct_fft_t direct, const FloatBuffer &x_r, const FloatBuffer &x_i)
    {
        FloatBuffer src_r(BUF_SIZE, 64);
        FloatBuffer src_i(BUF_SIZE, 64);
        FloatBuffer dst1r(BUF_SIZE, 64);
        FloatBuffer dst1i(BUF_SIZE, 64);
        FloatBuffer dst2r(BUF_SIZE, 64);
        FloatBuffer dst2i(BUF_SIZE, 64);
        src_r.copy(x_r);
        src_i.copy(x_i);
        dst2r.copy(x_r);
        dst2i.copy(x_i);

        printf("Testing %s direct FFT...\n", text);
        src_r.dump("src_r");
        src_i.dump("src_i");

        direct(dst1r, dst1i, src_r, src_i, RANK);
        dst1r.dump("dst1r");
        dst1i.dump("dst1i");

        direct(dst2r, dst2i, dst2r, dst2i, RANK);
        dst1r.dump("dst2r");
        dst1i.dump("dst2i");

        MTEST_ASSERT_MSG(x_r.valid(), "x_r corrupted");
        MTEST_ASSERT_MSG(x_i.valid(), "x_i corrupted");
        MTEST_ASSERT_MSG(src_r.valid(), "src_r corrupted");
        MTEST_ASSERT_MSG(src_i.valid(), "src_i corrupted");
        MTEST_ASSERT_MSG(dst1r.valid(), "dst1r corrupted");
        MTEST_ASSERT_MSG(dst1i.valid(), "dst1i corrupted");
        MTEST_ASSERT_MSG(dst2r.valid(), "dst2r corrupted");
        MTEST_ASSERT_MSG(dst2i.valid(), "dst2i corrupted");
    }

    void test_reverse_fft(const char *text, reverse_fft_t direct, const FloatBuffer &x_r, const FloatBuffer &x_i)
    {
        FloatBuffer src_r(BUF_SIZE, 64);
        FloatBuffer src_i(BUF_SIZE, 64);
        FloatBuffer dst1r(BUF_SIZE, 64);
        FloatBuffer dst1i(BUF_SIZE, 64);
        FloatBuffer dst2r(BUF_SIZE, 64);
        FloatBuffer dst2i(BUF_SIZE, 64);
        src_r.copy(x_r);
        src_i.copy(x_i);
        dst2r.copy(x_r);
        dst2i.copy(x_i);

        printf("Testing %s reverse FFT...\n", text);
        src_r.dump("src_r");
        src_i.dump("src_i");

        direct(dst1r, dst1i, src_r, src_i, RANK);
        dst1r.dump("dst1r");
        dst1i.dump("dst1i");

        direct(dst2r, dst2i, dst2r, dst2i, RANK);
        dst1r.dump("dst2r");
        dst1i.dump("dst2i");

        MTEST_ASSERT_MSG(x_r.valid(), "x_r corrupted");
        MTEST_ASSERT_MSG(x_i.valid(), "x_i corrupted");
        MTEST_ASSERT_MSG(src_r.valid(), "src_r corrupted");
        MTEST_ASSERT_MSG(src_i.valid(), "src_i corrupted");
        MTEST_ASSERT_MSG(dst1r.valid(), "dst1r corrupted");
        MTEST_ASSERT_MSG(dst1i.valid(), "dst1i corrupted");
        MTEST_ASSERT_MSG(dst2r.valid(), "dst2r corrupted");
        MTEST_ASSERT_MSG(dst2i.valid(), "dst2i corrupted");
    }

    MTEST_MAIN
    {
        FloatBuffer src1r(BUF_SIZE, 64);
        FloatBuffer src1i(BUF_SIZE, 64);
        FloatBuffer dst1r(BUF_SIZE, 64);
        FloatBuffer dst1i(BUF_SIZE, 64);

        // Prepare data
        for (size_t i=0; i<BUF_SIZE; ++i)
        {
            src1r[i]            = i;
            src1i[i]            = i * 0.1f;
        }

        // Test direct FFT
        test_direct_fft("native", direct_fft, src1r, src1i);

        IF_ARCH_X86(
            if (TEST_SUPPORTED(sse::direct_fft))
                test_direct_fft("SSE", sse::direct_fft, src1r, src1i);
            if (TEST_SUPPORTED(avx::direct_fft))
                test_direct_fft("AVX", avx::direct_fft, src1r, src1i);
            if (TEST_SUPPORTED(avx::direct_fft))
                test_direct_fft("FMA3", avx::direct_fft_fma3, src1r, src1i);
        );

        IF_ARCH_ARM(
            if (TEST_SUPPORTED(neon_d32::direct_fft))
                test_direct_fft("NEON-D32", neon_d32::direct_fft, src1r, src1i);
        );

        IF_ARCH_AARCH64(
            if (TEST_SUPPORTED(asimd::direct_fft))
                test_direct_fft("ASIMD", asimd::direct_fft, src1r, src1i);
        );

        // Test reverse FFT
        direct_fft(dst1r, dst1i, src1r, src1i, RANK);
        printf("\n");

        test_reverse_fft("native", native::reverse_fft, dst1r, dst1i);

        IF_ARCH_X86(
            if (TEST_SUPPORTED(sse::reverse_fft))
                test_reverse_fft("SSE", sse::reverse_fft, dst1r, dst1i);
            if (TEST_SUPPORTED(avx::reverse_fft))
                test_reverse_fft("AVX", avx::reverse_fft, dst1r, dst1i);
            if (TEST_SUPPORTED(avx::reverse_fft))
                test_reverse_fft("FMA3", avx::reverse_fft_fma3, dst1r, dst1i);
        );

        IF_ARCH_ARM(
            if (TEST_SUPPORTED(neon_d32::reverse_fft))
                test_reverse_fft("NEON-D32", neon_d32::reverse_fft, dst1r, dst1i);
        );

        IF_ARCH_AARCH64(
            if (TEST_SUPPORTED(asimd::reverse_fft))
                test_reverse_fft("ASIMD", asimd::reverse_fft, dst1r, dst1i);
        );

    }
MTEST_END
