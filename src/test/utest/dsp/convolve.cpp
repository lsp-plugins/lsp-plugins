/*
 * convolve.cpp
 *
 *  Created on: 7 сент. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count);
}

static void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
{
    for (size_t i=0; i<count; ++i)
    {
        for (size_t j=0; j<length; ++j)
            dst[i+j] += src[i] * conv[j];
    }
}

typedef void (* convolve_t)(float *dst, const float *src, const float *conv, size_t length, size_t count);

UTEST_BEGIN("dsp", convolve)
    void call(const char *label, size_t align, convolve_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        for (size_t mask=0; mask <= 0x07; ++mask)
        {
            UTEST_FOREACH(count, /*0, 1, 2, 3,*/ 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
            {
                FloatBuffer src(count, align, mask & 0x01);
                src.fill_zero();
                src[0] = 1.0f;

                UTEST_FOREACH(length, /* 0, 1, 2, 3, 4,*/ 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
                {
                    printf("Tesing convolution length=%d on buffer length=%d\n", int(length), int(count));

                    ssize_t clen = count + length - 1;
                    FloatBuffer conv(length, align, mask & 0x02);
                    FloatBuffer dst1((clen > 0) ? clen : 0, align, mask & 0x04);
                    dst1.fill_zero();
                    FloatBuffer dst2(dst1);

                    convolve(dst1, src, conv, length, count);
                    func(dst2, src, conv, length, count);

                    UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                    UTEST_ASSERT_MSG(conv.valid(), "Convolution buffer corrupted");
                    UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                    UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                    // Compare buffers
                    if (!dst1.equals_absolute(dst2))
                    {
                        src.dump("src ");
                        conv.dump("conv");
                        dst1.dump("dst1");
                        dst2.dump("dst2");
                        UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                    }
                }
            }
        }
    }

    UTEST_MAIN
    {
        call("native:convolve", 16, native::convolve);
    }

UTEST_END;


