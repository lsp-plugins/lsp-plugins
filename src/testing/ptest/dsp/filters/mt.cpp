/*
 * mt.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */


#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define PERF_BUF_SIZE   0x200
#define KF              100.0f
#define TD              (2*M_PI/48000.0)

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

static const f_cascade_t test_c =
{
    1, 2, 1, 0,
    1, -2, 1, 0
};

//-----------------------------------------------------------------------------
// Performance test for bilinear transform
PTEST_BEGIN("dsp.filters", mt, 10, 10000)

    void call_x1(const char * label, size_t count, matched_transform_x1_t func)
    {
        printf("Testing %s matched transform on buffer size %d ...\n", label, int(count));

        void *p1 = NULL, *p2 = NULL;
        biquad_x1_t *dst = alloc_aligned<biquad_x1_t>(p1, count, 32);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count, 32);
        f_cascade_t *tmp = alloc_aligned<f_cascade_t>(p2, count, 32);

        for (size_t i=0; i<count; ++i)
            src[i]  = test_c;

        size_t to_copy = count * (sizeof(f_cascade_t) / sizeof(float));

        PTEST_LOOP(label,
                dsp::copy(tmp->t, src->t, to_copy);
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                native::matched_transform_x1(dst, tmp, KF, TD, count);
        );

        free_aligned(p1);
        free_aligned(p2);
    }

    PTEST_MAIN
    {
        call_x1("8 matched x1 native", PERF_BUF_SIZE, native::matched_transform_x1);
        IF_ARCH_X86(call_x1("8 matched x1 sse", PERF_BUF_SIZE, sse::matched_transform_x1));
        PTEST_SEPARATOR;
    }

PTEST_END
