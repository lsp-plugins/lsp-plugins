/*
 * sanitize.cpp
 *
 *  Created on: 25 мар. 2020 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/ByteBuffer.h>

namespace native
{
    void sanitize1(float *dst, size_t count);
    void sanitize2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void sanitize1(float *dst, size_t count);
        void sanitize2(float *dst, const float *src, size_t count);
    }

    namespace avx
    {
        void sanitize1(float *dst, size_t count);
        void sanitize2(float *dst, const float *src, size_t count);
    }

    namespace avx2
    {
        void sanitize1(float *dst, size_t count);
        void sanitize2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void sanitize1(float *dst, size_t count);
        void sanitize2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void sanitize1(float *dst, size_t count);
        void sanitize2(float *dst, const float *src, size_t count);
    }
)

typedef void (* sanitize1_t)(float *dst, size_t count);
typedef void (* sanitize2_t)(float *dst, const float *src, size_t count);

UTEST_BEGIN("dsp.float", sanitize)

    void init_buf(ByteBuffer &buf, size_t count)
    {
        uint32_t *ival = buf.data<uint32_t>();
        float    *fval = buf.data<float>();

        for (size_t i=0; i<count; ++i)
        {
            switch (i % 10)
            {
                case 0:
                    fval[i]          = (rand() * 2.0f) / RAND_MAX;
                    break;
                case 1:
                    ival[i]         = 0x7f800000; // + Infinity
                    break;
                case 2:
                    fval[i]         = - (rand() * 2.0f) / RAND_MAX;
                    break;
                case 3:
                    ival[i]         = 0xff800000; // - Infinity
                    break;
                case 4:
                    ival[i]         = 0x80004000; // -Denormal
                    break;
                case 5:
                    ival[i]         = 0x00004000; // +Denormal
                    break;
                case 6:
                    ival[i]         = 0xff804000; // -SNaN
                    break;
                case 7:
                    ival[i]         = 0x7f804000; // +SNaN
                    break;
                case 8:
                    ival[i]         = 0xffc04000; // -QNaN
                    break;
                case 9:
                    ival[i]         = 0x7fc04000; // +QNaN
                    break;
                default:
                    ival[i]         = 0;
                    break;
            }
        }
    }

    void check_buffer(const char *slabel, const char *dlabel, ByteBuffer & src, ByteBuffer &dst, size_t count)
    {
        uint32_t *a = src.data<uint32_t>();
        uint32_t *b = dst.data<uint32_t>();

        UTEST_ASSERT_MSG(src.valid(), "Source buffer '%s' corrupted", slabel);
        UTEST_ASSERT_MSG(dst.valid(), "Destination buffer '%s' corrupted", dlabel);
        for (size_t i=0, n=count; i<n; ++i)
        {
            switch (i % 10)
            {
                case 0: case 2:
                    UTEST_ASSERT_MSG(b[i] == a[i],
                            "Invalid buffer %s contents at element %d: 0x%08lx, expected to be 0x%08lx",
                            dlabel, int(i), (unsigned long)(b[i]), (unsigned long)(a[i])
                        );
                    break;
                default:
                    UTEST_ASSERT_MSG(b[i] == (a[i] & 0x80000000),
                            "Invalid buffer %s contents at element %d: 0x%08lx, expected to be 0x%08lx",
                            dlabel, int(i), (unsigned long)(b[i]), (unsigned long)(a[i] & 0x80000000)
                        );
                    break;
            }
        }
    }

    void call(const char *label, size_t align, sanitize2_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                        32, 64, 65, 100, 768, 999, 1024, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                ByteBuffer src(count * sizeof(float), align, mask & 0x01);
                init_buf(src, count);
                ByteBuffer dst1(count * sizeof(float), align, mask & 0x02);
                ByteBuffer dst2(dst1);

                // Call functions
                native::sanitize2(dst1.data<float>(), src.data<float>(), count);
                func(dst2.data<float>(), src.data<float>(), count);

                check_buffer("src", "dst1", src, dst1, count);
                check_buffer("src", "dst2", src, dst2, count);
            }
        }
    }

    void call(const char *label, size_t align, sanitize1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1024, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                ByteBuffer dst1(count * sizeof(float), align, mask & 0x02);
                init_buf(dst1, count);
                ByteBuffer dst2(dst1);
                ByteBuffer src(dst1);

                // Call functions
                native::sanitize1(dst1.data<float>(), count);
                func(dst2.data<float>(), count);

                check_buffer("src", "dst1", src, dst1, count);
                check_buffer("src", "dst2", src, dst2, count);
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        IF_ARCH_X86(CALL(sse2::sanitize1, 16));
        IF_ARCH_X86(CALL(sse2::sanitize2, 16));

        IF_ARCH_X86(CALL(avx::sanitize1, 16));
        IF_ARCH_X86(CALL(avx::sanitize2, 16));

        IF_ARCH_X86(CALL(avx2::sanitize1, 32));
        IF_ARCH_X86(CALL(avx2::sanitize2, 32));

        IF_ARCH_ARM(CALL(neon_d32::sanitize1, 16));
        IF_ARCH_ARM(CALL(neon_d32::sanitize2, 16));

        IF_ARCH_AARCH64(CALL(asimd::sanitize1, 16));
        IF_ARCH_AARCH64(CALL(asimd::sanitize2, 16));
    }

UTEST_END;


