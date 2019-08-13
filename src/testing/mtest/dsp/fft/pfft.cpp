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

static void packed_scramble_fft(float *dst, const float *src, size_t rank)
{
    size_t items    = size_t(1) << rank;

    // Scramble the order of samples
    if (dst != src)
    {
        for (size_t i = 0; i < items; i ++)
        {
            size_t j = reverse_bits(i, rank);
            dst[i*2] = src[j*2]; \
            dst[i*2+1] = src[j*2+1]; \
        }
    }
    else
    {
        for (size_t i = 1; i < items; i ++)
        {
            size_t j = reverse_bits(i, rank);
            if (i >= j)
                continue;

            /* Copy the values from the reversed position */
            float re    = dst[i*2];
            float im    = dst[i*2+1];
            dst[i*2]    = dst[j*2];
            dst[i*2+1]  = dst[j*2+1];
            dst[j*2]    = re;
            dst[j*2+1]  = im;
        }
    }
}

static void start_packed_direct_fft(float *dst, size_t rank)
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
        float s0_re     = dst[0] + dst[2];
        float s1_re     = dst[0] - dst[2];
        float s0_im     = dst[1] + dst[3];
        float s1_im     = dst[1] - dst[3];

        float s2_re     = dst[4] + dst[6];
        float s3_re     = dst[4] - dst[6];
        float s2_im     = dst[5] + dst[7];
        float s3_im     = dst[5] - dst[7];

        dst[0]          = s0_re + s2_re;
        dst[1]          = s1_re + s3_im;
        dst[2]          = s0_re - s2_re;
        dst[3]          = s1_re - s3_im;

        dst[4]          = s0_im + s2_im;
        dst[5]          = s1_im - s3_re;
        dst[6]          = s0_im - s2_im;
        dst[7]          = s1_im + s3_re;

        // Move pointers
        dst            += 8;
    }
}

static void start_packed_reverse_fft(float *dst, size_t rank)
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
        // s1'' = s1' + j * s3'
        // s2'' = s0' - s2'
        // s3'' = s1' - j * s3'
        float s0_re     = dst[0] + dst[2];
        float s1_re     = dst[0] - dst[2];
        float s0_im     = dst[1] + dst[3];
        float s1_im     = dst[1] - dst[3];

        float s2_re     = dst[4] + dst[6];
        float s3_re     = dst[4] - dst[6];
        float s2_im     = dst[5] + dst[7];
        float s3_im     = dst[5] - dst[7];

        // Re-shuffle output to store [re0, re1, re2, re3, im0, im1, im2, im3]
        dst[0]          = s0_re + s2_re;
        dst[1]          = s1_re - s3_im;
        dst[2]          = s0_re - s2_re;
        dst[3]          = s1_re + s3_im;

        dst[4]          = s0_im + s2_im;
        dst[5]          = s1_im + s3_re;
        dst[6]          = s0_im - s2_im;
        dst[7]          = s1_im - s3_re;

        // Move pointers
        dst            += 8;
    }
}

static void repack_fft(float *dst, size_t rank)
{
    size_t count = 1 << rank;
    float t[8];
    for (size_t i=0; i<count; i += 4)
    {
        t[0] = dst[0];
        t[1] = dst[1];
        t[2] = dst[2];
        t[3] = dst[3];
        t[4] = dst[4];
        t[5] = dst[5];
        t[6] = dst[6];
        t[7] = dst[7];

        dst[0] = t[0];
        dst[1] = t[4];
        dst[2] = t[1];
        dst[3] = t[5];
        dst[4] = t[2];
        dst[5] = t[6];
        dst[6] = t[3];
        dst[7] = t[7];

        dst += 8;
    }
}

static void repack_reverse_fft(float *dst, size_t rank)
{
    size_t count = 1 << rank;
    float t[8];
    float k = 1.0f / count;

    for (size_t i=0; i<count; i += 4)
    {
        t[0] = dst[0];
        t[1] = dst[1];
        t[2] = dst[2];
        t[3] = dst[3];
        t[4] = dst[4];
        t[5] = dst[5];
        t[6] = dst[6];
        t[7] = dst[7];

        dst[0] = t[0] * k;
        dst[1] = t[4] * k;
        dst[2] = t[1] * k;
        dst[3] = t[5] * k;
        dst[4] = t[2] * k;
        dst[5] = t[6] * k;
        dst[6] = t[3] * k;
        dst[7] = t[7] * k;

        dst += 8;
    }
}

static void packed_direct_fft(float *dst, const float *src, size_t rank)
{
    packed_scramble_fft(dst, src, rank);
    start_packed_direct_fft(dst, rank);

    // Prepare for butterflies
    size_t items    = size_t(1) << (rank + 1);

    float c_re[4], c_im[4], w_re[4], w_im[4];
    const float *dw     = XFFT_DW;
    const float *iw_re  = XFFT_A_RE;
    const float *iw_im  = XFFT_A_IM;

    // Iterate butterflies
    for (size_t n=8, bs=(n << 1); n < items; n <<= 1, bs <<= 1)
    {
        for (size_t p=0; p<items; p += bs)
        {
            // Set initial values of pointers
            float *a            = &dst[p];
            float *b            = &a[n];

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
                c_re[0]         = w_re[0] * b[0] + w_im[0] * b[4];
                c_re[1]         = w_re[1] * b[1] + w_im[1] * b[5];
                c_re[2]         = w_re[2] * b[2] + w_im[2] * b[6];
                c_re[3]         = w_re[3] * b[3] + w_im[3] * b[7];

                c_im[0]         = w_re[0] * b[4] - w_im[0] * b[0];
                c_im[1]         = w_re[1] * b[5] - w_im[1] * b[1];
                c_im[2]         = w_re[2] * b[6] - w_im[2] * b[2];
                c_im[3]         = w_re[3] * b[7] - w_im[3] * b[3];

                // Calculate the output values:
                // a'   = a + c
                // b'   = a - c
                b[0]            = a[0] - c_re[0];
                b[1]            = a[1] - c_re[1];
                b[2]            = a[2] - c_re[2];
                b[3]            = a[3] - c_re[3];

                b[4]            = a[4] - c_im[0];
                b[5]            = a[5] - c_im[1];
                b[6]            = a[6] - c_im[2];
                b[7]            = a[7] - c_im[3];

                a[0]            = a[0] + c_re[0];
                a[1]            = a[1] + c_re[1];
                a[2]            = a[2] + c_re[2];
                a[3]            = a[3] + c_re[3];

                a[4]            = a[4] + c_im[0];
                a[5]            = a[5] + c_im[1];
                a[6]            = a[6] + c_im[2];
                a[7]            = a[7] + c_im[3];

                // Update pointers
                a              += 8;
                b              += 8;

                if ((k += 8) >= n)
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

    repack_fft(dst, rank);
}

static void packed_reverse_fft(float *dst, const float *src, size_t rank)
{
    packed_scramble_fft(dst, src, rank);
    start_packed_reverse_fft(dst, rank);

    // Prepare for butterflies
    size_t items    = size_t(1) << (rank + 1);

    float c_re[4], c_im[4], w_re[4], w_im[4];
    const float *dw     = XFFT_DW;
    const float *iw_re  = XFFT_A_RE;
    const float *iw_im  = XFFT_A_IM;

    // Iterate butterflies
    for (size_t n=8, bs=(n << 1); n < items; n <<= 1, bs <<= 1)
    {
        for (size_t p=0; p<items; p += bs)
        {
            // Set initial values of pointers
            float *a            = &dst[p];
            float *b            = &a[n];

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
                c_re[0]         = w_re[0] * b[0] - w_im[0] * b[4];
                c_re[1]         = w_re[1] * b[1] - w_im[1] * b[5];
                c_re[2]         = w_re[2] * b[2] - w_im[2] * b[6];
                c_re[3]         = w_re[3] * b[3] - w_im[3] * b[7];

                c_im[0]         = w_re[0] * b[4] + w_im[0] * b[0];
                c_im[1]         = w_re[1] * b[5] + w_im[1] * b[1];
                c_im[2]         = w_re[2] * b[6] + w_im[2] * b[2];
                c_im[3]         = w_re[3] * b[7] + w_im[3] * b[3];

                // Calculate the output values:
                // a'   = a + c
                // b'   = a - c
                b[0]            = a[0] - c_re[0];
                b[1]            = a[1] - c_re[1];
                b[2]            = a[2] - c_re[2];
                b[3]            = a[3] - c_re[3];

                b[4]            = a[4] - c_im[0];
                b[5]            = a[5] - c_im[1];
                b[6]            = a[6] - c_im[2];
                b[7]            = a[7] - c_im[3];

                a[0]            = a[0] + c_re[0];
                a[1]            = a[1] + c_re[1];
                a[2]            = a[2] + c_re[2];
                a[3]            = a[3] + c_re[3];

                a[4]            = a[4] + c_im[0];
                a[5]            = a[5] + c_im[1];
                a[6]            = a[6] + c_im[2];
                a[7]            = a[7] + c_im[3];

                // Update pointers
                a              += 8;
                b              += 8;

                if ((k += 8) >= n)
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

    repack_reverse_fft(dst, rank);
}

IF_ARCH_ARM(
    namespace neon_d32
    {
        void packed_direct_fft(float *dst, const float *src, size_t rank);
        void packed_reverse_fft(float *dst, const float *src, size_t rank);
    }
)

IF_ARCH_X86(
    namespace sse
    {
        void packed_direct_fft(float *dst, const float *src, size_t rank);
        void packed_reverse_fft(float *dst, const float *src, size_t rank);
    }
)

MTEST_BEGIN("dsp.fft", pfft)

    MTEST_MAIN
    {
        FloatBuffer src1(BUF_SIZE*2, 64);
        FloatBuffer src2(BUF_SIZE*2, 64);
        FloatBuffer dst1(BUF_SIZE*2, 64);
        FloatBuffer dst2(BUF_SIZE*2, 64);

        // Prepare data
        for (size_t i=0; i<BUF_SIZE; ++i)
        {
            src1[i*2]           = i;
            src1[i*2+1]         = i * 0.1f;
        }
        src2.copy(src1);

        // Test
        printf("Testing packed direct FFT...\n");
        src1.dump("src1");
        packed_direct_fft(dst1, src1, RANK);
        dst1.dump("dst1");
        packed_direct_fft(src1, src1, RANK);
        src1.dump("src1");

        src2.dump("src2");

        IF_ARCH_ARM(
            neon_d32::packed_direct_fft(dst2, src2, RANK);
            dst2.dump("dst2");
            neon_d32::packed_direct_fft(src2, src2, RANK);
            src2.dump("src2");
        );

        IF_ARCH_X86(
            sse::packed_direct_fft(dst2, src2, RANK);
            dst2.dump("dst2");
            sse::packed_direct_fft(src2, src2, RANK);
            src2.dump("src2");
        );

        printf("Doing packed reverse FFT...\n");
        packed_reverse_fft(dst1, src1, RANK);
        dst1.dump("dst1");
        packed_reverse_fft(src1, src1, RANK);
        src1.dump("src1");

        IF_ARCH_ARM(
            neon_d32::packed_reverse_fft(dst2, src2, RANK);
            dst2.dump("dst2");
            neon_d32::packed_reverse_fft(src2, src2, RANK);
            src2.dump("src2");
        );

        IF_ARCH_X86(
            sse::packed_reverse_fft(dst2, src2, RANK);
            dst2.dump("dst2");
            sse::packed_reverse_fft(src2, src2, RANK);
            src2.dump("src2");
        );
    }
MTEST_END
