/*
 * effects.cpp
 *
 *  Created on: 21 нояб. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 6
#define MAX_RANK 16

static void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count)
{
    dsp::fill_hsla(dst, eff->h, eff->s, eff->l, eff->a, count);

    float value, hue, alpha;
    float t     = 1.0f - eff->thresh;
    float kt    = 1.0f / eff->thresh;

    for (size_t i=0; i<count; ++i, dst += 4)
    {
        value   = v[i];
        value   = (value >= 0.0f) ? 1.0f - value : 1.0f + value;

        if (value < t)
        {
            hue         = eff->h + value;
            alpha       = 0.0f;
        }
        else
        {
            hue         = eff->h + t;
            alpha       = ((value - t) * kt);
        }

        dst[0]      = (hue > 1.0f) ? hue - 1.0f : hue;
        dst[3]      = alpha;
    }
}

namespace native
{
    void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count);
    }
)

//IF_ARCH_ARM(
//    namespace neon_d32
//    {
//        void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count);
//    }
//)

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.graphics", effects, 5, 5000)

template <class eff_t>
    void call(const char *label, float *dst, const float *src, size_t count,
            const eff_t *eff,
            void (* func)(float *dst, const float *v, const eff_t *eff, size_t count)
        )
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s pixels...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, eff, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;
        float *dst          = alloc_aligned<float>(data, buf_size * 5, 64);
        float *src          = &dst[buf_size * 4];

        dsp::hsla_hue_eff_t hue;
        hue.h       = 0.0f;
        hue.s       = 1.0f;
        hue.l       = 0.5f;
        hue.a       = 0.0f;
        hue.thresh  = 0.33333333333f;

        TEST_EXPORT(eff_hsla_hue);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("static::eff_hsla_hue", dst, src, count, &hue, eff_hsla_hue);
            call("native::eff_hsla_hue", dst, src, count, &hue, native::eff_hsla_hue);

            IF_ARCH_X86(call("sse2::eff_hsla_hue", dst, src, count, &hue, sse2::eff_hsla_hue));

//            IF_ARCH_ARM(call("neon_d32::eff_hsla_hue", dst, src, count, &hue, neon_d32::eff_hsla_hue));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END




