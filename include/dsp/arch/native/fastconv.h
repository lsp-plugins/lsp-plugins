/*
 * fastconv.h
 *
 *  Created on: 03 мар. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_FASTCONV_H_
#define DSP_ARCH_NATIVE_FASTCONV_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void fastconv_parse(float *dst, const float *src, size_t rank)
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

    void fastconv_parse_internal(float *dst, const float *src, size_t rank)
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
            float *d        = dst;

            for (size_t i=0; i<items; i += 16)
            {
                d[0]    = src[0];
                d[1]    = src[1];
                d[2]    = src[2];
                d[3]    = src[3];

                d[4]    = 0.0f;
                d[5]    = 0.0f;
                d[6]    = 0.0f;
                d[7]    = 0.0f;

                d      += 8;
                src    += 4;
            }
            for (size_t i=0; i<items; i += 16)
            {
                d[0]    = 0.0f;
                d[1]    = 0.0f;
                d[2]    = 0.0f;
                d[3]    = 0.0f;

                d[4]    = 0.0f;
                d[5]    = 0.0f;
                d[6]    = 0.0f;
                d[7]    = 0.0f;

                d      += 8;
            }
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

        // Now all complex numbers are stored in the following rormat:
        // [r0 r1 r2 r3 i0 i1 i2 i3  r4 r5 r6 r7 i4 i5 i6 i7  ... ]
    }

    void fastconv_restore(float *dst, float *tmp, size_t rank)
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
                dst[0]      = tmp[0] * kn;
                dst[1]      = tmp[1] * kn;
                dst[2]      = tmp[2] * kn;
                dst[3]      = tmp[3] * kn;

                dst        += 4;
                tmp        += 8;
            }
        }
    }

    void fastconv_restore_internal(float *dst, float *tmp, size_t rank)
    {
        float c_re[4], c_im[4], w_re[4], w_im[4];

        // Prepare for butterflies
        size_t last     = size_t(1) << rank;
        size_t items    = last << 1;
        size_t n        = 8;
        size_t bs       = n << 1;

        const float *dw     = XFFT_DW;
        const float *iw_re  = XFFT_A_RE;
        const float *iw_im  = XFFT_A_IM;

        // Iterate butterflies
        while (n < last)
        {
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
                d1[0]          += kn*(a[0] + c_re[0]);
                d1[1]          += kn*(a[1] + c_re[1]);
                d1[2]          += kn*(a[2] + c_re[2]);
                d1[3]          += kn*(a[3] + c_re[3]);

                d2[0]          += kn*(a[0] - c_re[0]);
                d2[1]          += kn*(a[1] - c_re[1]);
                d2[2]          += kn*(a[2] - c_re[2]);
                d2[3]          += kn*(a[3] - c_re[3]);

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
            float kn    = 1.0f / last;

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

    void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank)
    {
        // Parse convolution data
        fastconv_parse_internal(tmp, src, rank);

        size_t items    = size_t(1) << (rank + 1);

        float rx[4], ix[4];

        // All complex numbers are stored in the following format:
        // [r0 r1 r2 r3 i0 i1 i2 i3  r4 r5 r6 r7 i4 i5 i6 i7  ... ]
        float *s        = tmp;

        for (size_t i=0; i<items; i += 8)
        {
            // Complete direct FFT

            // s0' = s0 + s2            = (r0 + r2) + j*(i0 + i2)
            // s1' = s1 + s3            = (r1 + r3) + j*(i1 + i3)
            // s2' = s0 - s2            = 1*((r0 - r2) + j*(i0 - i2)) = (r0 - r2) + j*(i0 - i2)
            // s3' = -j*(s1 - s3)       = -j*((r1 - r3) + j*(i1 - i3)) = (i1 - i3) - j*(r1 - r3)

            // s0" = s0' + s1'          = (r0 + r2) + j*(i0 + i2) + (r1 + r3) + j*(i1 + i3)
            // s1" = s0' - s1'          = (r0 + r2) + j*(i0 + i2) - (r1 + r3) - j*(i1 + i3)
            // s2" = s2' + s3'          = (r0 - r2) + j*(i0 - i2) + (i1 - i3) - j*(r1 - r3)
            // s3" = s2' - s3'          = (r0 - r2) + j*(i0 - i2) - (i1 - i3) + j*(r1 - r3)

            rx[0]           = s[0] + s[2];
            rx[1]           = s[0] - s[2];
            rx[2]           = s[1] + s[3];
            rx[3]           = s[1] - s[3];

            ix[0]           = s[4] + s[6];
            ix[1]           = s[4] - s[6];
            ix[2]           = s[5] + s[7];
            ix[3]           = s[5] - s[7];

            s[0]            = rx[0] + rx[2];
            s[1]            = rx[0] - rx[2];
            s[2]            = rx[1] + ix[3];
            s[3]            = rx[1] - ix[3];

            s[4]            = ix[0] + ix[2];
            s[5]            = ix[0] - ix[2];
            s[6]            = ix[1] - rx[3];
            s[7]            = ix[1] + rx[3];

            // Complex mul
            rx[0]           = s[0]*c[0] - s[4]*c[4];
            rx[1]           = s[1]*c[1] - s[5]*c[5];
            rx[2]           = s[2]*c[2] - s[6]*c[6];
            rx[3]           = s[3]*c[3] - s[7]*c[7];

            ix[0]           = s[0]*c[4] + s[4]*c[0];
            ix[1]           = s[1]*c[5] + s[5]*c[1];
            ix[2]           = s[2]*c[6] + s[6]*c[2];
            ix[3]           = s[3]*c[7] + s[7]*c[3];

            s[0]            = rx[0];
            s[1]            = rx[1];
            s[2]            = rx[2];
            s[3]            = rx[3];

            s[4]            = ix[0];
            s[5]            = ix[1];
            s[6]            = ix[2];
            s[7]            = ix[3];

            // Prepare reverse FFT

            // s0' = s0 + s1        = (r0 + r1) + j*(i0 + i1)
            // s1' = s0 - s1        = (r0 - r1) + j*(i0 - i1)
            // s2' = s2 + s3        = (r2 + r3) + j*(i2 + i3)
            // s3' = s2 - s3        = (r2 - r3) + j*(i2 - i3)

            // s0" = s0' + s2'      = (r0 + r1) + j*(i0 + i1) + (r2 + r3) + j*(i2 + i3)
            // s1" = s1' + j*s3'    = (r0 - r1) + j*(i0 - i1) - (i2 - i3) + j*(r2 - r3)
            // s2" = s0' - s2'      = (r0 + r1) + j*(i0 + i1) - (r2 + r3) - j*(i2 + i3)
            // s3" = s1' - j*s3'    = (r0 - r1) + j*(i0 - i1) + (i2 - i3) - j*(r2 - r3)

            rx[0]           = s[0] + s[1];
            rx[1]           = s[0] - s[1];
            rx[2]           = s[2] + s[3];
            rx[3]           = s[2] - s[3];

            ix[0]           = s[4] + s[5];
            ix[1]           = s[4] - s[5];
            ix[2]           = s[6] + s[7];
            ix[3]           = s[6] - s[7];

            s[0]            = rx[0] + rx[2];
            s[1]            = rx[1] - ix[3];
            s[2]            = rx[0] - rx[2];
            s[3]            = rx[1] + ix[3];

            s[4]            = ix[0] + ix[2];
            s[5]            = ix[1] + rx[3];
            s[6]            = ix[0] - ix[2];
            s[7]            = ix[1] - rx[3];

            s              += 8;
            c              += 8;
        }

        // Do reverse FFT transformation
        fastconv_restore_internal(dst, tmp, rank);
    }

    void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank)
    {
        size_t items    = size_t(1) << (rank + 1);

        float rx[4], ix[4];

        // Do complex multiplication
        float *d        = tmp;
        for (size_t i=0; i<items; i += 8)
        {
            rx[0]       = c1[0]*c2[0] - c1[4]*c2[4];
            rx[1]       = c1[1]*c2[1] - c1[5]*c2[5];
            rx[2]       = c1[2]*c2[2] - c1[6]*c2[6];
            rx[3]       = c1[3]*c2[3] - c1[7]*c2[7];

            ix[0]       = c1[0]*c2[4] + c1[4]*c2[0];
            ix[1]       = c1[1]*c2[5] + c1[5]*c2[1];
            ix[2]       = c1[2]*c2[6] + c1[6]*c2[2];
            ix[3]       = c1[3]*c2[7] + c1[7]*c2[3];

            d[0]        = rx[0];
            d[1]        = rx[1];
            d[2]        = rx[2];
            d[3]        = rx[3];

            d[4]        = ix[0];
            d[5]        = ix[1];
            d[6]        = ix[2];
            d[7]        = ix[3];

            // Prepare reverse FFT
            // s0' = s0 + s1        = (r0 + r1) + j*(i0 + i1)
            // s1' = s0 - s1        = (r0 - r1) + j*(i0 - i1)
            // s2' = s2 + s3        = (r2 + r3) + j*(i2 + i3)
            // s3' = s2 - s3        = (r2 - r3) + j*(i2 - i3)

            // s0" = s0' + s2'      = (r0 + r1) + j*(i0 + i1) + (r2 + r3) + j*(i2 + i3)
            // s1" = s1' + j*s3'    = (r0 - r1) + j*(i0 - i1) - (i2 - i3) + j*(r2 - r3)
            // s2" = s0' - s2'      = (r0 + r1) + j*(i0 + i1) - (r2 + r3) - j*(i2 + i3)
            // s3" = s1' - j*s3'    = (r0 - r1) + j*(i0 - i1) + (i2 - i3) - j*(r2 - r3)

            rx[0]       = d[0] + d[1];
            rx[1]       = d[0] - d[1];
            rx[2]       = d[2] + d[3];
            rx[3]       = d[2] - d[3];

            ix[0]       = d[4] + d[5];
            ix[1]       = d[4] - d[5];
            ix[2]       = d[6] + d[7];
            ix[3]       = d[6] - d[7];

            d[0]        = rx[0] + rx[2];
            d[1]        = rx[1] - ix[3];
            d[2]        = rx[0] - rx[2];
            d[3]        = rx[1] + ix[3];

            d[4]        = ix[0] + ix[2];
            d[5]        = ix[1] + rx[3];
            d[6]        = ix[0] - ix[2];
            d[7]        = ix[1] - rx[3];

            d          += 8;
            c1         += 8;
            c2         += 8;
        }

        // Do reverse FFT transformation
        fastconv_restore_internal(dst, tmp, rank);
    }
}

#endif /* DSP_ARCH_NATIVE_FASTCONV_H_ */
