/*
 * convolver.cpp
 *
 *  Created on: 8 сент. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <test/helpers.h>
#include <core/util/Convolver.h>

using namespace lsp;

#define CONV_SIZE       0x2000
#define SRC_SIZE        0x2000
#define SRC2_SIZE       0x20

static void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
{
    for (size_t i=0; i<count; ++i)
    {
        for (size_t j=0; j<length; ++j)
            dst[i+j] += src[i] * conv[j];
    }
}

UTEST_BEGIN("core.util", convolver)
    void convolve(Convolver &conv, float *dst, const float *src, size_t count, size_t step)
    {
        for (size_t i=0; i<count;)
        {
            size_t todo = count - i;
            if (todo > step)
                todo = step;
            if (i == 248)
                printf("debug\n");
            conv.process(&dst[i], &src[i], todo);
            i += todo;
        }
    }

    void test_small()
    {
        Convolver c;

        FloatBuffer conv(0x1f);
        FloatBuffer src(SRC_SIZE + conv.size());
        FloatBuffer dst1(src.size());
        FloatBuffer dst2(dst1);
        FloatBuffer dst3(dst1);

        // Initialize data
        for (size_t i=0; i<conv.size(); ++i)
            conv[i] = i + 1;
        src.fill_zero();
        for (size_t i=0, j=0; i<SRC_SIZE; i+=5, ++j)
            src[i] = ((j % 3) == 0) ? 1.0f :
                     ((j % 3) == 1) ? 0.1f : 0.01f;

        dst1.fill_zero();
        dst2.fill_zero();
        dst3.fill_zero();

        UTEST_ASSERT(c.init(conv, conv.size(), 9, 0));
        ::convolve(dst1, src, conv, conv.size(), SRC_SIZE);
        dsp::convolve(dst2, src, conv, conv.size(), SRC_SIZE);
        convolve(c, dst3, src, src.size(), 31);

        UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(conv.valid(), "Convolution 1 buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
        UTEST_ASSERT_MSG(dst3.valid(), "Destination buffer 3 corrupted");

        if ((!dst2.equals_relative(dst1, 1e-4)) || (!dst3.equals_relative(dst2, 1e-4)))
        {
            src.dump("src ");
            conv.dump("conv");
            dst1.dump("dst1");
            dst2.dump("dst2");
            dst3.dump("dst3");
            size_t index = dst2.last_diff();
            UTEST_FAIL_MSG("Output of convolver is invalid, started at sample=%d: %.5f vs %.5f",
                    int(index), dst2[index], dst3[index]);
        }

        c.destroy();
    }

    void test_large()
    {
        Convolver c;

        FloatBuffer conv(CONV_SIZE);
        FloatBuffer src(SRC2_SIZE + conv.size());
        FloatBuffer dst1(src.size());
        FloatBuffer dst2(dst1);
        FloatBuffer dst3(dst1);
        dsp::fill_zero(src.data(SRC2_SIZE), src.size() - SRC2_SIZE);

//        for (size_t i=0; i<conv.size(); ++i)
//            conv[i] = i + 1;
//        src.fill_zero();
//        src[0] = 1.0f;

        dst1.fill_zero();
        dst2.fill_zero();
        dst3.fill_zero();

        UTEST_ASSERT(c.init(conv, conv.size(), 10, 0));
        ::convolve(dst1, src, conv, conv.size(), SRC2_SIZE);
        dsp::convolve(dst2, src, conv, conv.size(), SRC2_SIZE);
        convolve(c, dst3, src, src.size(), 31);

        UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(conv.valid(), "Convolution 1 buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
        UTEST_ASSERT_MSG(dst3.valid(), "Destination buffer 3 corrupted");

        if ((!dst2.equals_absolute(dst1, 1e-4)) || (!dst3.equals_absolute(dst2, 1e-4)))
        {
            src.dump("src ");
            conv.dump("conv");
            dst1.dump("dst1");
            dst2.dump("dst2");
            dst3.dump("dst3");
            size_t index = dst2.last_diff();
            UTEST_FAIL_MSG("Output of convolver is invalid, started at sample=%d: %.5f vs %.5f",
                    int(index), dst2[index], dst3[index]);
        }

        c.destroy();
    }

    UTEST_MAIN
    {
        test_small();
        test_large();
    }
UTEST_END;


