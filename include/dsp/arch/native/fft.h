/*
 * fft.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_FFT_H_
#define DSP_ARCH_NATIVE_FFT_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
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

    void normalize_fft3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        rank            = 1 << rank;
        float k         = 1.0f / rank;
        dsp::mul_k3(dst_re, src_re, k, rank);
        dsp::mul_k3(dst_im, src_im, k, rank);
    }

    void normalize_fft2(float *re, float *im, size_t rank)
    {
        rank            = 1 << rank;
        float k         = 1.0f / rank;
        dsp::mul_k2(re, k, rank);
        dsp::mul_k2(im, k, rank);
    }

    static void do_normalize_fft(float *dst_re, float *dst_im, size_t rank)
    {
        size_t items    = 1 << rank;
        float k         = 1.0f / items;
        while (items--)
        {
            *(dst_re++)    *= k;
            *(dst_im++)    *= k;
        }
    }

    static void repack_fft(float *dst, size_t rank)
    {
        size_t iterations    = 1 << (rank - 2);

        // Re-shuffle [re0, re1, re2, re3, im0, im1, im2, im3] to [re0, im0, re1, im1, re2, im2, re3, im3]
        while (iterations--)
        {
            // Perform swaps between elements
            float r         = dst[2];
            dst[2]          = dst[1];
            dst[1]          = dst[4];
            dst[4]          = r;

            r               = dst[3];
            dst[3]          = dst[5];
            dst[5]          = dst[6];
            dst[6]          = r;

            // Move pointers
            dst            += 8;
        }
    }

    static void repack_normalize_fft(float *dst, size_t rank)
    {
        size_t iterations    = 1 << (rank - 2);
        float k              = 0.25f / iterations;

        // Re-shuffle [re0, re1, re2, re3, im0, im1, im2, im3] to [re0, im0, re1, im1, re2, im2, re3, im3]
        while (iterations--)
        {
            // Perform swaps between elements
            float r         = dst[2] * k;
            dst[2]          = dst[1] * k;
            dst[1]          = dst[4] * k;
            dst[4]          = r;

            r               = dst[3] * k;
            dst[3]          = dst[5] * k;
            dst[5]          = dst[6] * k;
            dst[6]          = r;

            dst[0]         *= k;
            dst[7]         *= k;

            // Move pointers
            dst            += 8;
        }
    }

    static inline void scramble_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        size_t items    = 1 << rank;

        // Scramble the order of samples
        if ((dst_re != src_re) && (dst_im != src_im))
        {
            #define SC_COPY(type)   \
                for (size_t i = 0; i < items; ++i) \
                { \
                    size_t j = reverse_bits(type(i), rank);    /* Reverse the order of the bits */ \
                    /* Copy the values from the reversed position */ \
                    dst_re[i]   = src_re[j]; \
                    dst_im[i]   = src_im[j]; \
                }

            // Just copy data from calculated positions
            if (rank <= (sizeof(int16_t) * 8))
            {
                if (rank <= 8)
                    SC_COPY(uint8_t)
                else
                    SC_COPY(uint16_t)
            }
            else
            {
                if (rank <= 32)
                    SC_COPY(uint32_t)
                else
                    SC_COPY(uint64_t)
            }

            #undef SC_COPY
        }
        else
        {
            // More general algorithm: first copy data
            dsp::move(dst_re, src_re, items);
            dsp::move(dst_im, src_im, items);

            #define SC_COPY(type)   \
                for (size_t i = 1; i < (items - 1); ++i) \
                { \
                    size_t j = reverse_bits(type(i), rank);    /* Reverse the order of the bits */ \
                    if (i >= j) \
                        continue; \
                    \
                    /* Copy the values from the reversed position */ \
                    float re    = dst_re[i]; \
                    float im    = dst_im[i]; \
                    dst_re[i]   = dst_re[j]; \
                    dst_im[i]   = dst_im[j]; \
                    dst_re[j]   = re; \
                    dst_im[j]   = im; \
                }

            // Reverse the source arrays
            if (rank <= (sizeof(int16_t) * 8))
            {
                if (rank <= 8)
                    SC_COPY(uint8_t)
                else
                    SC_COPY(uint16_t)
            }
            else
            {
                if (rank <= 32)
                    SC_COPY(uint32_t)
                else
                    SC_COPY(uint64_t)
            }

            #undef SC_COPY
        }
    }

    static inline void packed_scramble_fft(float *dst, const float *src, size_t rank)
    {
        size_t items    = size_t(1) << rank;

        // Scramble the order of samples
        if (dst != src)
        {
            #define SC_COPY(type)   \
                for (size_t i = 0; i < items; i ++) \
                { \
                    size_t j = reverse_bits(type(i), rank);    /* Reverse the order of the bits */ \
                    /* Copy the values from the reversed position */ \
                    dst[i*2] = src[j*2]; \
                    dst[i*2+1] = src[j*2+1]; \
                }

            // Just copy data from calculated positions
            if (rank <= (sizeof(int16_t) * 8))
            {
                if (rank <= 8)
                    SC_COPY(uint8_t)
                else
                    SC_COPY(uint16_t)
            }
            else
            {
                if (rank <= 32)
                    SC_COPY(uint32_t)
                else
                    SC_COPY(uint64_t)
            }

            #undef SC_COPY
        }
        else
        {
            // More general algorithm: first copy data
            dsp::move(dst, src, items);

            #define SC_COPY(type)   \
                for (size_t i = 1; i < items; i ++) \
                { \
                    size_t j = reverse_bits(type(i), rank);    /* Reverse the order of the bits */ \
                    if (i >= j) \
                        continue; \
                    \
                    /* Copy the values from the reversed position */ \
                    float re    = dst[i*2]; \
                    float im    = dst[i*2+1]; \
                    dst[i*2]    = dst[j*2]; \
                    dst[i*2+1]  = dst[j*2+1]; \
                    dst[j*2]    = re; \
                    dst[j*2+1]  = im; \
                }

            // Reverse the source arrays
            if (rank <= (sizeof(int16_t) * 8))
            {
                if (rank <= 8)
                    SC_COPY(uint8_t)
                else
                    SC_COPY(uint16_t)
            }
            else
            {
                if (rank <= 32)
                    SC_COPY(uint32_t)
                else
                    SC_COPY(uint64_t)
            }

            #undef SC_COPY
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

            // Re-shuffle output to store [re0, re1, re2, re3, im0, im1, im2, im3]
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

    static void start_reverse_fft(float *dst_re, float *dst_im, size_t rank)
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
            float s0_re     = dst_re[0] + dst_re[1];
            float s1_re     = dst_re[0] - dst_re[1];
            float s2_re     = dst_re[2] + dst_re[3];
            float s3_re     = dst_re[2] - dst_re[3];

            float s0_im     = dst_im[0] + dst_im[1];
            float s1_im     = dst_im[0] - dst_im[1];
            float s2_im     = dst_im[2] + dst_im[3];
            float s3_im     = dst_im[2] - dst_im[3];

            dst_re[0]       = s0_re + s2_re;
            dst_re[1]       = s1_re - s3_im;
            dst_re[2]       = s0_re - s2_re;
            dst_re[3]       = s1_re + s3_im;

            dst_im[0]       = s0_im + s2_im;
            dst_im[1]       = s1_im + s3_re;
            dst_im[2]       = s0_im - s2_im;
            dst_im[3]       = s1_im - s3_re;

            // Move pointers
            dst_re         += 4;
            dst_im         += 4;
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

    void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        // Check bounds
        if (rank <= 1)
        {
            if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src_re[1];
                float s1_im     = src_im[1];
                dst_re[1]       = src_re[0] - s1_re;
                dst_im[1]       = src_im[0] - s1_im;
                dst_re[0]       = src_re[0] + s1_re;
                dst_im[0]       = src_im[0] + s1_im;
            }
            else
            {
                dst_re[0]       = src_re[0];
                dst_im[0]       = src_im[0];
            }
            return;
        }

        // Scramble the order of samples
        scramble_fft(dst_re, dst_im, src_re, src_im, rank);

        // Perform the lowest kernel calculations
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
            for (size_t p=0; p<items; p += bs)
            {
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

    void packed_direct_fft(float *dst, const float *src, size_t rank)
    {
        // Check bounds
        if (rank <= 1)
        {
            if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src[2];
                float s1_im     = src[3];
                dst[2]          = src[0] - s1_re;
                dst[3]          = src[1] - s1_im;
                dst[0]          = src[0] + s1_re;
                dst[1]          = src[1] + s1_im;
            }
            else
            {
                dst[0]          = src[0];
                dst[1]          = src[1];
            }
            return;
        }

        // Scramble the order of samples
        packed_scramble_fft(dst, src, rank);

        // Perform the lowest kernel calculations
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

        // Fixup complex number presentation
        repack_fft(dst, rank);
    }

    void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        // Check bounds
        if (rank <= 1)
        {
            if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src_re[1];
                float s1_im     = src_im[1];
                dst_re[1]       = (src_re[0] - s1_re) * 0.5f;
                dst_im[1]       = (src_im[0] - s1_im) * 0.5f;
                dst_re[0]       = (src_re[0] + s1_re) * 0.5f;
                dst_im[0]       = (src_im[0] + s1_im) * 0.5f;
            }
            else
            {
                dst_re[0]       = src_re[0];
                dst_im[0]       = src_im[0];
            }
            return;
        }

        // Scramble the order of samples
        scramble_fft(dst_re, dst_im, src_re, src_im, rank);

        // Perform the lowest kernel calculations
        start_reverse_fft(dst_re, dst_im, rank);

        // Prepare for butterflies
        size_t items    = 1 << rank;

        float c_re[4], c_im[4], w_re[4], w_im[4];
        const float *dw     = XFFT_DW;
        const float *iw_re  = XFFT_A_RE;
        const float *iw_im  = XFFT_A_IM;

        // Iterate butterflies
        for (size_t n=4, bs=n << 1; n < items; n <<= 1, bs <<= 1)
        {
            for (size_t p=0; p<items; p += bs)
            {
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
                    c_re[0]         = w_re[0] * b_re[0] - w_im[0] * b_im[0];
                    c_re[1]         = w_re[1] * b_re[1] - w_im[1] * b_im[1];
                    c_re[2]         = w_re[2] * b_re[2] - w_im[2] * b_im[2];
                    c_re[3]         = w_re[3] * b_re[3] - w_im[3] * b_im[3];

                    c_im[0]         = w_re[0] * b_im[0] + w_im[0] * b_re[0];
                    c_im[1]         = w_re[1] * b_im[1] + w_im[1] * b_re[1];
                    c_im[2]         = w_re[2] * b_im[2] + w_im[2] * b_re[2];
                    c_im[3]         = w_re[3] * b_im[3] + w_im[3] * b_re[3];

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

        // Update amplitudes
        do_normalize_fft(dst_re, dst_im, rank);
    }

    void packed_reverse_fft(float *dst, const float *src, size_t rank)
    {
        // Check bounds
        if (rank <= 1)
        {
            if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src[2];
                float s1_im     = src[3];
                dst[2]          = src[0] - s1_re;
                dst[3]          = src[1] - s1_im;
                dst[0]          = src[0] + s1_re;
                dst[1]          = src[1] + s1_im;
            }
            else
            {
                dst[0]          = src[0];
                dst[1]          = src[1];
            }
            return;
        }

        // Scramble the order of samples
        packed_scramble_fft(dst, src, rank);

        // Perform the lowest kernel calculations
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

        // Update amplitudes
        repack_normalize_fft(dst, rank);
    }

    static void center_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        if (rank == 0)
            return;

        size_t center    = 1 << (rank - 1);
        for (size_t i=0; i < center; ++i)
        {
            float v1_re         =   src_re[i];
            float v1_im         =   src_im[i];
            float v2_re         =   src_re[center + i];
            float v2_im         =   src_im[center + i];

            dst_re[i]           =   v2_re;
            dst_im[i]           =   v2_im;
            dst_re[center + i]  =   v1_re;
            dst_im[center + i]  =   v1_im;
        }
    }

    static void combine_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        if (rank < 2)
            return;
        ssize_t  count  = 1 << rank;
        const float *tail_re = &src_re[count];
        const float *tail_im = &src_im[count];
        count >>= 1;

        for (ssize_t i=1; i<count; ++i)
        {
            dst_re[i]       = src_re[i] + tail_re[-i];
            dst_im[i]       = src_im[i] - tail_im[-i];
        }

        dsp::fill_zero(&dst_re[count+1], count-1);
        dsp::fill_zero(&dst_im[count+1], count-1);
    }

    static void packed_combine_fft(float *dst, const float *src, size_t rank)
    {
        if (rank < 2)
            return;
        ssize_t  count  = 1 << (rank+1);
        const float *tail = &src[count];
        count >>= 1;

        for (ssize_t i=1; i<count; i += 2)
        {
            dst[i]          = src[i]    + tail[-i-1];
            dst[i+1]        = src[i+1]  - tail[-i];
        }

        dsp::fill_zero(&dst[count+2], count-2);
    }
}

#endif /* DSP_ARCH_NATIVE_FFT_H_ */
