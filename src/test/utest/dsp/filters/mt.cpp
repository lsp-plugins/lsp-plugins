/*
 * mt.cpp
 *
 *  Created on: 27 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

#define CASCADES            11
#define BIQUAD_X1_FLOATS    (sizeof(biquad_x1_t) / sizeof(float))
#define BIQUAD_X2_FLOATS    (sizeof(biquad_x2_t) / sizeof(float))
#define BIQUAD_X4_FLOATS    (sizeof(biquad_x4_t) / sizeof(float))
#define BIQUAD_X8_FLOATS    (sizeof(biquad_x8_t) / sizeof(float))
#define CASCADE_FLOATS      (sizeof(f_cascade_t) / sizeof(float))

namespace native
{
    void matched_transform_x1(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void matched_transform_x1(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count);
    }
)

typedef void (* matched_transform_x1_t)(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count);

UTEST_BEGIN("dsp.filters", mt)

    void call(const char *text, matched_transform_x1_t f1, matched_transform_x1_t f2)
    {
        if (!UTEST_SUPPORTED(f1))
            return;
        if (!UTEST_SUPPORTED(f2))
            return;

        printf("Testing %s matched transformation\n", text);

        float td = 2.0*M_PI/48000.0;
        FloatBuffer src1(CASCADE_FLOATS * CASCADES, 64, true);
        f_cascade_t *bc = src1.data<f_cascade_t>();
        for (size_t i=0; i<CASCADES; ++i)
        {
            float kt = i * 0.1;
            float kb = i * 0.05;
            bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
            bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
        }

        FloatBuffer src2(src1); // Copy of src1
        FloatBuffer dst1(BIQUAD_X1_FLOATS * CASCADES, 64, true);
        FloatBuffer dst2(BIQUAD_X1_FLOATS * CASCADES, 64, true);

        f1(dst1.data<biquad_x1_t>(), bc, 1.5f, td, CASCADES);
        f2(dst2.data<biquad_x1_t>(), bc, 1.5f, td, CASCADES);

        UTEST_ASSERT_MSG(src1.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(src2.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
        if (!dst1.equals_relative(dst2, 1e-4f))
        {
            src1.dump("src1");
            src2.dump("src2");
            dst1.dump("dst1");
            dst2.dump("dst2");
            UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("mt_sse_x1", native::matched_transform_x1, sse::matched_transform_x1));
    }

UTEST_END;




