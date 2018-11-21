/*
 * effects.cpp
 *
 *  Created on: 21 нояб. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

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

IF_ARCH_ARM(
    namespace neon_d32
    {
        void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count);
        void eff_hsla_sat(float *dst, const float *v, const dsp::hsla_sat_eff_t *eff, size_t count);
        void eff_hsla_light(float *dst, const float *v, const dsp::hsla_light_eff_t *eff, size_t count);
        void eff_hsla_alpha(float *dst, const float *v, const dsp::hsla_alpha_eff_t *eff, size_t count);
    }
)

UTEST_BEGIN("dsp.graphics", effects)

    template <class eff_t>
        void call(const char *label, size_t align,
                void (* func1)(float *dst, const float *v, const eff_t *eff, size_t count),
                void (* func2)(float *dst, const float *v, const eff_t *eff, size_t count),
                const eff_t *eff
            )
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

//        size_t count = 4;
        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1024, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(count*4, align, mask & 0x02);
                FloatBuffer dst2(count*4, align, mask & 0x02);

                src.randomize_sign();
//                src.vfill(0, 4,
//                        0.0f, 0.25f, 0.5f, -0.5f
//                    );

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Call functions
                func1(dst1, src, eff, count);
                func2(dst2, src, eff, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-4f))
                {
                    src.dump("src");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Result of functions differs at index %d: %.5f vs %.5f",
                        int(dst1.last_diff()), dst1.get_diff(), dst2.get_diff());
                }
            }
        }
    }

    UTEST_MAIN
    {
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

        IF_ARCH_X86(call("sse::eff_hsla_hue", 16, native::eff_hsla_hue, sse2::eff_hsla_hue, &hue));
        IF_ARCH_X86(call("sse::eff_hsla_sat", 16, native::eff_hsla_sat, sse2::eff_hsla_sat, &sat));
        IF_ARCH_X86(call("sse::eff_hsla_light", 16, native::eff_hsla_light, sse2::eff_hsla_light, &light));
        IF_ARCH_X86(call("sse::eff_hsla_alpha", 16, native::eff_hsla_alpha, sse2::eff_hsla_alpha, &alpha));

        IF_ARCH_ARM(call("neon_d32::eff_hsla_hue", 16, native::eff_hsla_hue, neon_d32::eff_hsla_hue, &hue));
        IF_ARCH_ARM(call("neon_d32::eff_hsla_sat", 16, native::eff_hsla_sat, neon_d32::eff_hsla_sat, &sat));
        IF_ARCH_ARM(call("neon_d32::eff_hsla_light", 16, native::eff_hsla_light, neon_d32::eff_hsla_light, &light));
        IF_ARCH_ARM(call("neon_d32::eff_hsla_alpha", 16, native::eff_hsla_alpha, neon_d32::eff_hsla_alpha, &alpha));
    }

UTEST_END;
