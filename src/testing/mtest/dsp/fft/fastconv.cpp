/*
 * fastconv.cpp
 *
 *  Created on: 31 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/mtest.h>
#include <test/FloatBuffer.h>
#include <core/debug.h>

#define RANK        6
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

static void fastconv_parse(float *dst, const float *src, size_t rank)
{
    // Prepare for butterflies
    float c_re[4], c_im[4], w_re[4], w_im[4];
    const float *dw     = &XFFT_DW[(rank - 3) << 1];
    const float *iw_re  = &XFFT_A_RE[(rank-3) << 2];
    const float *iw_im  = &XFFT_A_IM[(rank-3) << 2];
    size_t items        = size_t(1) << (rank + 1);
    size_t bs           = items;
    size_t n            = bs >> 1;

    // Iterate first cycle
    if (n > 4)
    {
        // ONE LARGE CYCLE
        // Set initial values of pointers
        float *a            = dst;
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
            // Calculate the output values:
            // a'   = a + 0
            // b'   = (a-0) * w
            a[0]            = src[0];
            a[1]            = src[1];
            a[2]            = src[2];
            a[3]            = src[3];

            a[4]            = 0.0f;
            a[5]            = 0.0f;
            a[6]            = 0.0f;
            a[7]            = 0.0f;

            // Calculate complex c = w * b
            b[0]            = w_re[0] * a[0];
            b[1]            = w_re[1] * a[1];
            b[2]            = w_re[2] * a[2];
            b[3]            = w_re[3] * a[3];

            b[4]            = -w_im[0] * a[0];
            b[5]            = -w_im[1] * a[1];
            b[6]            = -w_im[2] * a[2];
            b[7]            = -w_im[3] * a[3];

            // Update pointers
            a              += 8;
            b              += 8;
            src            += 4;

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

        dw     -= 2;
        iw_re  -= 4;
        iw_im  -= 4;

        n >>= 1;
        bs >>= 1;
    }
    else
    {
        // Unpack 4x real to 4x split complex
        dst[0]      = src[0];
        dst[1]      = src[1];
        dst[2]      = src[2];
        dst[3]      = src[3];

        dst[4]      = 0.0f;
        dst[5]      = 0.0f;
        dst[6]      = 0.0f;
        dst[7]      = 0.0f;
    }

    // Iterate butterflies
    for (; n > 4; n >>= 1, bs >>= 1)
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
                // Calculate the output values:
                // c    = a - b
                // a'   = a + b
                // b'   = c * w
                c_re[0]         = a[0] - b[0];
                c_re[1]         = a[1] - b[1];
                c_re[2]         = a[2] - b[2];
                c_re[3]         = a[3] - b[3];

                c_im[0]         = a[4] - b[4];
                c_im[1]         = a[5] - b[5];
                c_im[2]         = a[6] - b[6];
                c_im[3]         = a[7] - b[7];

                a[0]            = a[0] + b[0];
                a[1]            = a[1] + b[1];
                a[2]            = a[2] + b[2];
                a[3]            = a[3] + b[3];

                a[4]            = a[4] + b[4];
                a[5]            = a[5] + b[5];
                a[6]            = a[6] + b[6];
                a[7]            = a[7] + b[7];

                // Calculate complex c = w * b
                b[0]            = w_re[0] * c_re[0] + w_im[0] * c_im[0];
                b[1]            = w_re[1] * c_re[1] + w_im[1] * c_im[1];
                b[2]            = w_re[2] * c_re[2] + w_im[2] * c_im[2];
                b[3]            = w_re[3] * c_re[3] + w_im[3] * c_im[3];

                b[4]            = w_re[0] * c_im[0] - w_im[0] * c_re[0];
                b[5]            = w_re[1] * c_im[1] - w_im[1] * c_re[1];
                b[6]            = w_re[2] * c_im[2] - w_im[2] * c_re[2];
                b[7]            = w_re[3] * c_im[3] - w_im[3] * c_re[3];

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

        dw     -= 2;
        iw_re  -= 4;
        iw_im  -= 4;
    }

    // Add two last stages
    for (size_t i=0; i<items; i += 8)
    {
        // s0' = s0 + s2            = (r0 + r2) + j*(i0 + i2)
        // s1' = s1 + s3            = (r1 + r3) + j*(i1 + i3)
        // s2' = s0 - s2            = 1*((r0 - r2) + j*(i0 - i2)) = (r0 - r2) + j*(i0 - i2)
        // s3' = -j*(s1 - s3)       = -j*((r1 - r3) + j*(i1 - i3)) = (i1 - i3) - j*(r1 - r3)

        // s0" = s0' + s1'          = (r0 + r2) + j*(i0 + i2) + (r1 + r3) + j*(i1 + i3)
        // s1" = s0' - s1'          = (r0 + r2) + j*(i0 + i2) - (r1 + r3) - j*(i1 + i3)
        // s2" = s2' + s3'          = (r0 - r2) + j*(i0 - i2) + (i1 - i3) - j*(r1 - r3)
        // s3" = s2' - s3'          = (r0 - r2) + j*(i0 - i2) - (i1 - i3) + j*(r1 - r3)

        float r0k       = dst[0] + dst[2];
        float r1k       = dst[0] - dst[2];
        float r2k       = dst[1] + dst[3];
        float r3k       = dst[1] - dst[3];

        float i0k       = dst[4] + dst[6];
        float i1k       = dst[4] - dst[6];
        float i2k       = dst[5] + dst[7];
        float i3k       = dst[5] - dst[7];

//        dst[0]          = r0k;
//        dst[1]          = r2k;
//        dst[2]          = r1k;
//        dst[3]          = i3k;
//
//        dst[4]          = i0k;
//        dst[5]          = i2k;
//        dst[6]          = i1k;
//        dst[7]          = r3k;

        dst[0]          = r0k + r2k;
        dst[1]          = r0k - r2k;
        dst[2]          = r1k + i3k;
        dst[3]          = r1k - i3k;

        dst[4]          = i0k + i2k;
        dst[5]          = i0k - i2k;
        dst[6]          = i1k - r3k;
        dst[7]          = i1k + r3k;

        dst            += 8;
    }

    // Now all complex numbers are stored in the following rormat:
    // [r0 r1 r2 r3 i0 i1 i2 i3  r4 r5 r6 r7 i4 i5 i6 i7  ... ]
}

static void fastconv_restore(float *dst, float *tmp, size_t rank)
{
    float c_re[4], c_im[4], w_re[4], w_im[4];

    // Prepare for butterflies
    size_t last     = size_t(1) << rank;
    size_t items    = last << 1;
    size_t n        = 8;
    size_t bs       = n << 1;

    // Add two last stages
    float *d        = tmp;

    // All complex numbers are stored in the following format:
    // [r0 r1 r2 r3 i0 i1 i2 i3  r4 r5 r6 r7 i4 i5 i6 i7  ... ]
    for (size_t i=0; i<items; i += 8)
    {
        // s0' = s0 + s1        = (r0 + r1) + j*(i0 + i1)
        // s1' = s0 - s1        = (r0 - r1) + j*(i0 - i1)
        // s2' = s2 + s3        = (r2 + r3) + j*(i2 + i3)
        // s3' = s2 - s3        = (r2 - r3) + j*(i2 - i3)

        // s0" = s0' + s2'      = (r0 + r1) + j*(i0 + i1) + (r2 + r3) + j*(i2 + i3)
        // s1" = s1' + j*s3'    = (r0 - r1) + j*(i0 - i1) - (i2 - i3) + j*(r2 - r3)
        // s2" = s0' - s2'      = (r0 + r1) + j*(i0 + i1) - (r2 + r3) - j*(i2 + i3)
        // s3" = s1' - j*s3'    = (r0 - r1) + j*(i0 - i1) + (i2 - i3) - j*(r2 - r3)

        float r0k       = d[0] + d[1];
        float r1k       = d[0] - d[1];
        float r2k       = d[2] + d[3];
        float r3k       = d[2] - d[3];

        float i0k       = d[4] + d[5];
        float i1k       = d[4] - d[5];
        float i2k       = d[6] + d[7];
        float i3k       = d[6] - d[7];

        d[0]            = r0k + r2k;
        d[1]            = r1k - i3k;
        d[2]            = r0k - r2k;
        d[3]            = r1k + i3k;

        d[4]            = i0k + i2k;
        d[5]            = i1k + r3k;
        d[6]            = i0k - i2k;
        d[7]            = i1k - r3k;

        d              += 8;
    }

    const float *dw     = XFFT_DW;
    const float *iw_re  = XFFT_A_RE;
    const float *iw_im  = XFFT_A_IM;

    // Iterate butterflies
    while (n < last)
    {
//        lsp_dumpf("iw_re", "%.6f", iw_re, 4);
//        lsp_dumpf("iw_im", "%.6f", iw_im, 4);
//        lsp_dumpf("dw   ", "%.6f", dw, 2);

        for (size_t p=0; p<items; p += bs)
        {
            // Set initial values of pointers
            float *a            = &tmp[p];
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
        n     <<= 1;
        bs    <<= 1;
    }

    if (n < items)
    {
        // ONE LARGE CYCLE
        // Set initial values of pointers
        float *a            = tmp;
        float *b            = &a[n];
        float *d1           = dst;
        float *d2           = &d1[n>>1];
        float kn            = 1.0f / last;

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

            // Calculate the output values:
            // a'   = a + c
            // b'   = a - c
            d1[0]           = kn*(a[0] + c_re[0]);
            d1[1]           = kn*(a[1] + c_re[1]);
            d1[2]           = kn*(a[2] + c_re[2]);
            d1[3]           = kn*(a[3] + c_re[3]);

            d2[0]           = kn*(a[0] - c_re[0]);
            d2[1]           = kn*(a[1] - c_re[1]);
            d2[2]           = kn*(a[2] - c_re[2]);
            d2[3]           = kn*(a[3] - c_re[3]);

            // Update pointers
            a              += 8;
            b              += 8;
            d1             += 4;
            d2             += 4;

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
    else
    {
        // Add real result to the target (ignore complex result)
        float kn     = 1.0f / last;

        for (size_t i=0; i<items; i += 8)
        {
            dst[0]     += tmp[0] * kn;
            dst[1]     += tmp[1] * kn;
            dst[2]     += tmp[2] * kn;
            dst[3]     += tmp[3] * kn;

            dst        += 4;
            tmp        += 8;
        }
    }
}

namespace native
{
    void fastconv_parse(float *dst, const float *src, size_t rank);
    void fastconv_restore(float *dst, float *tmp, size_t rank);
    void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
    void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
}

IF_ARCH_X86(
    namespace sse
    {
        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_restore(float *dst, float *tmp, size_t rank);
        void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
    }

    namespace avx
    {
        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_restore(float *dst, float *tmp, size_t rank);
        void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);

        void fastconv_parse_fma3(float *dst, const float *src, size_t rank);
        void fastconv_restore_fma3(float *dst, float *tmp, size_t rank);
        void fastconv_apply_fma3(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        void fastconv_parse_apply_fma3(float *dst, float *tmp, const float *c, const float *src, size_t rank);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_restore(float *dst, float *tmp, size_t rank);
        void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_restore(float *dst, float *tmp, size_t rank);
        void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
    }
)

typedef void (* fastconv_parse_t)(float *dst, const float *src, size_t rank);
typedef void (* fastconv_restore_t)(float *dst, float *tmp, size_t rank);
typedef void (* fastconv_apply_t)(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
typedef void (* fastconv_parse_apply_t)(float *dst, float *tmp, const float *c, const float *src, size_t rank);

MTEST_BEGIN("dsp.fft", fastconv)

    void test_fastconv_parse(const char *text, fastconv_parse_t parse, FloatBuffer &src)
    {
        printf("testing %s fastconv_parse...\n", text);
        FloatBuffer samp(BUF_SIZE >> 1, 64);
        FloatBuffer conv(BUF_SIZE << 1, 64);
        samp.copy(src);

        samp.dump("samp");
        parse(conv, samp, RANK);
        conv.dump("conv");

        MTEST_ASSERT_MSG(samp.valid(), "samp corrupted");
        MTEST_ASSERT_MSG(conv.valid(), "conv corrupted");
    }

    void test_fastconv_restore(const char *text,
            fastconv_parse_t parse, fastconv_restore_t restore,
            FloatBuffer &src, FloatBuffer &dst)
    {
        printf("testing %s fastconv_parse + fastconv_restore...\n", text);
        FloatBuffer samp(BUF_SIZE >> 1, 64);
        FloatBuffer conv(BUF_SIZE << 1, 64);
        FloatBuffer rest(BUF_SIZE, 64);

        samp.copy(src);
        rest.copy(dst);
        samp.dump("samp");
        samp.dump("rest");

        parse(conv, samp, RANK);
        conv.dump("conv");

        restore(rest, conv, RANK);
        conv.dump("rtmp");
        rest.dump("rest");

        MTEST_ASSERT_MSG(samp.valid(), "samp corrupted");
        MTEST_ASSERT_MSG(conv.valid(), "conv corrupted");
        MTEST_ASSERT_MSG(rest.valid(), "rest corrupted");
    }

    void test_fastconv_apply(const char *text,
            fastconv_parse_t parse, fastconv_apply_t apply,
            FloatBuffer &src1, FloatBuffer &src2, FloatBuffer &dst)
    {
        printf("testing %s fastconv_parse + fastconv_apply...\n", text);
        FloatBuffer samp1(BUF_SIZE >> 1, 64);
        FloatBuffer samp2(BUF_SIZE >> 1, 64);
        FloatBuffer conv1(BUF_SIZE << 1, 64);
        FloatBuffer conv2(BUF_SIZE << 1, 64);
        FloatBuffer rest(BUF_SIZE, 64);
        FloatBuffer temp(BUF_SIZE << 1, 64);

        samp1.copy(src1);
        samp2.copy(src2);
        rest.copy(dst);
        samp1.dump("samp1");
        samp2.dump("samp2");
        rest.dump ("dest ");

        parse(conv1, samp1, RANK);
        parse(conv2, samp2, RANK);
        conv1.dump("conv1");
        conv2.dump("conv2");

        apply(rest, temp, conv1, conv2, RANK);
        temp.dump ("temp ");
        rest.dump ("rest ");

        MTEST_ASSERT_MSG(samp1.valid(), "samp1 corrupted");
        MTEST_ASSERT_MSG(samp2.valid(), "samp2 corrupted");
        MTEST_ASSERT_MSG(conv1.valid(), "conv1 corrupted");
        MTEST_ASSERT_MSG(conv2.valid(), "conv2 corrupted");
        MTEST_ASSERT_MSG(rest.valid(), "rest corrupted");
        MTEST_ASSERT_MSG(temp.valid(), "temp corrupted");
    }

    void test_fastconv_parse_apply(const char *text,
            fastconv_parse_t parse, fastconv_parse_apply_t papply,
            FloatBuffer &src1, FloatBuffer &src2, FloatBuffer &dst)
    {
        printf("testing %s fastconv_parse + fastconv_parse_apply...\n", text);
        FloatBuffer samp1(BUF_SIZE >> 1, 64);
        FloatBuffer samp2(BUF_SIZE >> 1, 64);
        FloatBuffer conv (BUF_SIZE << 1, 64);
        FloatBuffer rest(BUF_SIZE, 64);
        FloatBuffer temp(BUF_SIZE << 1, 64);

        samp1.copy(src1);
        samp2.copy(src2);
        rest.copy(dst);
        samp1.dump("samp1");
        samp2.dump("samp2");
        rest.dump ("dest ");

        parse(conv, samp1, RANK);
        conv.dump("conv ");

        papply(rest, temp, conv, samp2, RANK);
        temp.dump ("temp ");
        rest.dump ("rest ");

        MTEST_ASSERT_MSG(samp1.valid(), "samp1 corrupted");
        MTEST_ASSERT_MSG(samp2.valid(), "samp2 corrupted");
        MTEST_ASSERT_MSG(conv.valid(), "conv corrupted");
        MTEST_ASSERT_MSG(rest.valid(), "rest corrupted");
        MTEST_ASSERT_MSG(temp.valid(), "temp corrupted");
    }

    MTEST_MAIN
    {
        FloatBuffer src1(BUF_SIZE >> 1, 64);
        FloatBuffer src2(BUF_SIZE >> 1, 64);
        FloatBuffer dst(BUF_SIZE, 64);

        FloatBuffer samp1(BUF_SIZE >> 1, 64);
        FloatBuffer samp2(BUF_SIZE >> 1, 64);
        FloatBuffer conv1(BUF_SIZE << 1, 64);
        FloatBuffer conv2(BUF_SIZE << 1, 64);
        FloatBuffer rest1(BUF_SIZE, 64);
        FloatBuffer rest2(BUF_SIZE, 64);
        FloatBuffer temp(BUF_SIZE << 1, 64);

        // Prepare data
        for (size_t i=0; i<(BUF_SIZE >> 1); ++i)
        {
            src1[i]         = i;
            src2[i]         = 0.1 * i;
        }
        for (size_t i=0; i<BUF_SIZE; ++i)
            dst[i]          = 10.0 * i;
        samp2.copy(samp1);

        // Test parse
        test_fastconv_parse("native", fastconv_parse, src1);
        IF_ARCH_X86(
            if (TEST_SUPPORTED(sse::fastconv_parse))
                test_fastconv_parse("SSE", sse::fastconv_parse, src1);
            if (TEST_SUPPORTED(avx::fastconv_parse))
                test_fastconv_parse("AVX", avx::fastconv_parse, src1);
            if (TEST_SUPPORTED(avx::fastconv_parse_fma3))
                test_fastconv_parse("FMA3", avx::fastconv_parse_fma3, src1);
        );
        IF_ARCH_ARM(
            if (TEST_SUPPORTED(neon_d32::fastconv_parse))
                test_fastconv_parse("NEON-D32", neon_d32::fastconv_parse, src1);
        );
        IF_ARCH_AARCH64(
            if (TEST_SUPPORTED(asimd::fastconv_parse))
                test_fastconv_parse("ASIMD", asimd::fastconv_parse, src1);
        );

        // Test parse + restore
        printf("\n");
        test_fastconv_restore("native", fastconv_parse, fastconv_restore, src1, dst);

        IF_ARCH_X86(
            if (TEST_SUPPORTED(sse::fastconv_parse) && TEST_SUPPORTED(sse::fastconv_restore))
                test_fastconv_restore("SSE", sse::fastconv_parse, sse::fastconv_restore, src1, dst);
            if (TEST_SUPPORTED(avx::fastconv_parse) && TEST_SUPPORTED(avx::fastconv_restore))
                test_fastconv_restore("AVX", avx::fastconv_parse, avx::fastconv_restore, src1, dst);
            if (TEST_SUPPORTED(avx::fastconv_parse_fma3) && TEST_SUPPORTED(avx::fastconv_restore_fma3))
                test_fastconv_restore("FMA3", avx::fastconv_parse_fma3, avx::fastconv_restore_fma3, src1, dst);
        );
        IF_ARCH_ARM(
            if (TEST_SUPPORTED(neon_d32::fastconv_parse) && TEST_SUPPORTED(neon_d32::fastconv_restore))
                test_fastconv_restore("NEON-D32", neon_d32::fastconv_parse, neon_d32::fastconv_restore, src1, dst);
        );
        IF_ARCH_AARCH64(
            if (TEST_SUPPORTED(asimd::fastconv_parse) && TEST_SUPPORTED(asimd::fastconv_restore))
                test_fastconv_restore("ASIMD", asimd::fastconv_parse, asimd::fastconv_restore, src1, dst);
        );

        // Test parse + apply
        printf("\n");
        test_fastconv_apply("native", native::fastconv_parse, native::fastconv_apply, src1, src2, dst);

        IF_ARCH_X86(
            if (TEST_SUPPORTED(sse::fastconv_parse) && TEST_SUPPORTED(sse::fastconv_apply))
                test_fastconv_apply("SSE", sse::fastconv_parse, sse::fastconv_apply, src1, src2, dst);
            if (TEST_SUPPORTED(avx::fastconv_parse) && TEST_SUPPORTED(avx::fastconv_apply))
                test_fastconv_apply("AVX", avx::fastconv_parse, avx::fastconv_apply, src1, src2, dst);
            if (TEST_SUPPORTED(avx::fastconv_parse_fma3) && TEST_SUPPORTED(avx::fastconv_apply_fma3))
                test_fastconv_apply("FMA3", avx::fastconv_parse_fma3, avx::fastconv_apply_fma3, src1, src2, dst);
        );
        IF_ARCH_ARM(
            if (TEST_SUPPORTED(neon_d32::fastconv_parse) && TEST_SUPPORTED(neon_d32::fastconv_apply))
                test_fastconv_apply("NEON-D32", neon_d32::fastconv_parse, neon_d32::fastconv_apply, src1, src2, dst);
        );
        IF_ARCH_AARCH64(
            if (TEST_SUPPORTED(asimd::fastconv_parse) && TEST_SUPPORTED(asimd::fastconv_apply))
                test_fastconv_apply("ASIMD", asimd::fastconv_parse, asimd::fastconv_apply, src1, src2, dst);
        );

        // Test parse + parse_apply
        printf("\n");
        test_fastconv_parse_apply("native", native::fastconv_parse, native::fastconv_parse_apply, src1, src2, dst);

        IF_ARCH_X86(
            if (TEST_SUPPORTED(sse::fastconv_parse) && TEST_SUPPORTED(sse::fastconv_parse_apply))
                test_fastconv_parse_apply("SSE", sse::fastconv_parse, sse::fastconv_parse_apply, src1, src2, dst);
            if (TEST_SUPPORTED(avx::fastconv_parse) && TEST_SUPPORTED(avx::fastconv_parse_apply))
                test_fastconv_parse_apply("AVX", avx::fastconv_parse, avx::fastconv_parse_apply, src1, src2, dst);
            if (TEST_SUPPORTED(avx::fastconv_parse_fma3) && TEST_SUPPORTED(avx::fastconv_parse_apply_fma3))
                test_fastconv_parse_apply("FMA3", avx::fastconv_parse_fma3, avx::fastconv_parse_apply_fma3, src1, src2, dst);
        );

        IF_ARCH_ARM(
            if (TEST_SUPPORTED(neon_d32::fastconv_parse) && TEST_SUPPORTED(neon_d32::fastconv_parse_apply))
                test_fastconv_parse_apply("NEON-D32", neon_d32::fastconv_parse, neon_d32::fastconv_parse_apply, src1, src2, dst);
        );

        IF_ARCH_AARCH64(
            if (TEST_SUPPORTED(asimd::fastconv_parse) && TEST_SUPPORTED(asimd::fastconv_parse_apply))
                test_fastconv_parse_apply("ASIMD", asimd::fastconv_parse, asimd::fastconv_parse_apply, src1, src2, dst);
        );
    }
MTEST_END

