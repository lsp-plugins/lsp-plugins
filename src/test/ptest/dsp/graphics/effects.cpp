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
            hue         = dst[0] + value;
            alpha       = 0.0f;
        }
        else
        {
            hue         = dst[0] + t;
            alpha       = ((value - t) * kt);
        }

        dst[0]      = (hue > 1.0f) ? hue - 1.0f : hue;
        dst[3]      = alpha;
    }
}

static void eff_hsla_alpha(float *dst, const float *v, const dsp::hsla_alpha_eff_t *eff, size_t count)
{
    dsp::fill_hsla(dst, eff->h, eff->s, eff->l, eff->a, count);

    float value;

    for (size_t i=0; i<count; ++i, dst += 4)
    {
        value   = v[i];
        value   = (value >= 0.0f) ? 1.0f - value : 1.0f + value;
        dst[3]  = value; // Fill alpha channel
    }
}

static void eff_hsla_sat(float *dst, const float *v, const dsp::hsla_sat_eff_t *eff, size_t count)
{
    dsp::fill_hsla(dst, eff->h, eff->s, eff->l, eff->a, count);

    float value;
    float kt = 1.0f / eff->thresh;

    for (size_t i=0; i<count; ++i, dst += 4)
    {
        value   = v[i];
        value   = (value >= 0.0f) ? value : -value;

        if (value >= eff->thresh)
        {
            dst[1]     *= value;
            dst[3]      = 0.0f;
        }
        else
        {
            dst[1]     *= eff->thresh;
            dst[3]      = (eff->thresh - value) * kt;
        }
    }
}

static void eff_hsla_light(float *dst, const float *v, const dsp::hsla_light_eff_t *eff, size_t count)
{
    dsp::fill_hsla(dst, eff->h, eff->s, eff->l, eff->a, count);

    float value;
    float kt = 1.0f / eff->thresh;

    for (size_t i=0; i<count; ++i, dst += 4)
    {
        value   = v[i];
        value   = (value >= 0.0f) ? value : -value;

        if (value >= eff->thresh)
        {
            dst[2]     *= value;
            dst[3]      = 0.0f;
        }
        else
        {
            dst[2]     *= eff->thresh;
            dst[3]      = (eff->thresh - value) * kt;
        }
    }
}

namespace native
{
    void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count);
    void eff_hsla_sat(float *dst, const float *v, const dsp::hsla_sat_eff_t *eff, size_t count);
    void eff_hsla_light(float *dst, const float *v, const dsp::hsla_light_eff_t *eff, size_t count);
    void eff_hsla_alpha(float *dst, const float *v, const dsp::hsla_alpha_eff_t *eff, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count);
        void eff_hsla_sat(float *dst, const float *v, const dsp::hsla_sat_eff_t *eff, size_t count);
        void eff_hsla_light(float *dst, const float *v, const dsp::hsla_light_eff_t *eff, size_t count);
        void eff_hsla_alpha(float *dst, const float *v, const dsp::hsla_alpha_eff_t *eff, size_t count);
    }
)

IF_ARCH_X86_64(
    namespace avx2
    {
        void x64_eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count);
        void x64_eff_hsla_sat(float *dst, const float *v, const dsp::hsla_sat_eff_t *eff, size_t count);
        void x64_eff_hsla_light(float *dst, const float *v, const dsp::hsla_light_eff_t *eff, size_t count);
        void x64_eff_hsla_alpha(float *dst, const float *v, const dsp::hsla_alpha_eff_t *eff, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count);
        void eff_hsla_sat(float *dst, const float *v, const dsp::hsla_sat_eff_t *eff, size_t count);
        void eff_hsla_light(float *dst, const float *v, const dsp::hsla_light_eff_t *eff, size_t count);
        void eff_hsla_alpha(float *dst, const float *v, const dsp::hsla_alpha_eff_t *eff, size_t count);
    }
)

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

        dsp::hsla_alpha_eff_t alpha;
        alpha.h     = 0.5f;
        alpha.s     = 1.0f;
        alpha.l     = 0.5f;
        alpha.a     = 0.0f;

        dsp::hsla_sat_eff_t sat;
        sat.h       = 0.0f;
        sat.s       = 1.0f;
        sat.l       = 0.5f;
        sat.a       = 0.0f;
        sat.thresh  = 0.25f;

        dsp::hsla_light_eff_t light;
        light.h       = 0.0f;
        light.s       = 1.0f;
        light.l       = 0.5f;
        light.a       = 0.0f;
        light.thresh  = 0.25f;

        TEST_EXPORT(eff_hsla_hue);
        TEST_EXPORT(eff_hsla_sat);
        TEST_EXPORT(eff_hsla_light);
        TEST_EXPORT(eff_hsla_alpha);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("static::eff_hsla_hue", dst, src, count, &hue, eff_hsla_hue);
            call("native::eff_hsla_hue", dst, src, count, &hue, native::eff_hsla_hue);
            IF_ARCH_X86(call("sse2::eff_hsla_hue", dst, src, count, &hue, sse2::eff_hsla_hue));
            IF_ARCH_X86_64(call("avx2::x64_eff_hsla_hue", dst, src, count, &hue, avx2::x64_eff_hsla_hue));
            IF_ARCH_ARM(call("neon_d32::eff_hsla_hue", dst, src, count, &hue, neon_d32::eff_hsla_hue));
            PTEST_SEPARATOR;

            call("static::eff_hsla_sat", dst, src, count, &sat, eff_hsla_sat);
            call("native::eff_hsla_sat", dst, src, count, &sat, native::eff_hsla_sat);
            IF_ARCH_X86(call("sse2::eff_hsla_sat", dst, src, count, &sat, sse2::eff_hsla_sat));
            IF_ARCH_X86_64(call("avx2::x64_eff_hsla_sat", dst, src, count, &sat, avx2::x64_eff_hsla_sat));
            IF_ARCH_ARM(call("neon_d32::eff_hsla_sat", dst, src, count, &sat, neon_d32::eff_hsla_sat));
            PTEST_SEPARATOR;

            call("static::eff_hsla_light", dst, src, count, &light, eff_hsla_light);
            call("native::eff_hsla_light", dst, src, count, &light, native::eff_hsla_light);
            IF_ARCH_X86(call("sse2::eff_hsla_light", dst, src, count, &light, sse2::eff_hsla_light));
            IF_ARCH_X86_64(call("avx2::x64_eff_hsla_light", dst, src, count, &light, avx2::x64_eff_hsla_light));
            IF_ARCH_ARM(call("neon_d32::eff_hsla_light", dst, src, count, &light, neon_d32::eff_hsla_light));
            PTEST_SEPARATOR;

            call("static::eff_hsla_alpha", dst, src, count, &alpha, eff_hsla_alpha);
            call("native::eff_hsla_alpha", dst, src, count, &alpha, native::eff_hsla_alpha);
            IF_ARCH_X86(call("sse2::eff_hsla_alpha", dst, src, count, &alpha, sse2::eff_hsla_alpha));
            IF_ARCH_X86_64(call("avx2::x64_eff_hsla_alpha", dst, src, count, &alpha, avx2::x64_eff_hsla_alpha));
            IF_ARCH_ARM(call("neon_d32::eff_hsla_alpha", dst, src, count, &alpha, neon_d32::eff_hsla_alpha));

            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END




