#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>
#include <core/windows.h>
#include <core/envelope.h>

#define CASCADES            7
#define BIQUAD_X1_FLOATS    (sizeof(biquad_x1_t) / sizeof(float))
#define BIQUAD_X2_FLOATS    (sizeof(biquad_x2_t) / sizeof(float))
#define BIQUAD_X4_FLOATS    (sizeof(biquad_x4_t) / sizeof(float))
#define BIQUAD_X8_FLOATS    (sizeof(biquad_x8_t) / sizeof(float))
#define CASCADE_FLOATS      (sizeof(f_cascade_t) / sizeof(float))

namespace native
{
    void matched_transform_x1(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count);
//    void test_matched_transform_x1(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count);
}

namespace sse
{
    void matched_transform_x1(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count);
//    void test_matched_transform_x1(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count);
}

namespace mt_test
{
    using namespace lsp;
    using namespace test;

    void test_x1()
    {
        FBuffer src(CASCADE_FLOATS * CASCADES, true);
        FBuffer tmp(CASCADE_FLOATS * CASCADES, true);
        FBuffer dst1(BIQUAD_X1_FLOATS * CASCADES, true);
        FBuffer dst2(BIQUAD_X1_FLOATS * CASCADES, true);
//        FBuffer dst3(BIQUAD_X1_FLOATS * CASCADES, true);
//        FBuffer dst4(BIQUAD_X1_FLOATS * CASCADES, true);

        f_cascade_t *bc = src.data<f_cascade_t>();
        for (size_t i=0; i<CASCADES; ++i)
        {
            float kt = i * 0.1;
            float kb = i * 0.05;
            bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
            bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
        }

        bc = tmp.data<f_cascade_t>();

        dsp::copy(tmp.data(), src.data(), CASCADE_FLOATS * CASCADES);
        native::matched_transform_x1(dst1.data<biquad_x1_t>(), bc, 1000.0f, 2*M_PI/48000.0, CASCADES);
//        dsp::copy(tmp.data(), src.data(), CASCADE_FLOATS * CASCADES);
//        native::test_matched_transform_x1(dst2.data<biquad_x1_t>(), bc, 1000.0f, 2*M_PI/48000.0, CASCADES);
        dsp::copy(tmp.data(), src.data(), CASCADE_FLOATS * CASCADES);
        sse::matched_transform_x1(dst2.data<biquad_x1_t>(), bc, 1000.0f, 2*M_PI/48000.0, CASCADES);
//        dsp::copy(tmp.data(), src.data(), CASCADE_FLOATS * CASCADES);
//        sse::test_matched_transform_x1(dst4.data<biquad_x1_t>(), bc, 1000.0f, 2*M_PI/48000.0, CASCADES);

        for (size_t i=0; i<CASCADES; ++i)
        {
            printf("i=%d\n", int(i));
            src.dump ("  src ", i * CASCADE_FLOATS, CASCADE_FLOATS);
            dst1.dump("  dst1", i * BIQUAD_X1_FLOATS, BIQUAD_X1_FLOATS);
            dst2.dump("  dst2", i * BIQUAD_X1_FLOATS, BIQUAD_X1_FLOATS);
//            dst3.dump("  dst3", i * BIQUAD_X1_FLOATS, BIQUAD_X1_FLOATS);
//            dst4.dump("  dst4", i * BIQUAD_X1_FLOATS, BIQUAD_X1_FLOATS);
        }
    }

    int test(int argc, const char **argv)
    {
        dsp::context_t ctx;

        dsp::init();
        dsp::start(&ctx);

        test_x1();

        dsp::finish(&ctx);

        return 0;
    }
}

#undef CASCADES
#undef BIQUAD_X1_FLOATS
#undef BIQUAD_X2_FLOATS
#undef BIQUAD_X4_FLOATS
#undef BIQUAD_X8_FLOATS
#undef CASCADE_FLOATS
