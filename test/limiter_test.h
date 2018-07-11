#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <core/dsp.h>
#include <core/dynamics/Limiter.h>

#define LIMITER_BUF_SIZE        128
#define LIMITER_SAMPLE_RATE     48000
#define LIMITER_MAX_LOOKAHEAD   20
#define LOOKAHEAD_SAMPLES       32

namespace limiter_test
{
    using namespace lsp;

    void dump_buffer(const char *text, const float *b, size_t count)
    {
        printf("%s:\n", text);
        while (count--)
            printf("%.6f\n", *(b++));
    }

    void test_limiter(float *out, float *gain, float *in, size_t count)
    {
        printf("Testing Limter...\n");

        // Initialize limiter
        Limiter l;
        l.init(LIMITER_SAMPLE_RATE * 4, LIMITER_MAX_LOOKAHEAD);
        l.set_sample_rate(LIMITER_SAMPLE_RATE);
        l.set_mode(LM_HERM_THIN);
//        l.set_mode(LM_HAT);
//        l.set_mode(LM_SATURATOR);
//        l.set_mode(LM_HERM_DUCK);
//        l.set_mode(LM_EXP_THIN);
//        l.set_mode(LM_EXP_WIDE);
//        l.set_mode(LM_EXP_TAIL);
//        l.set_mode(LM_EXP_DUCK);
        l.set_threshold(0.5f);
        l.set_knee(1.0f);
        l.set_lookahead(samples_to_millis(LIMITER_SAMPLE_RATE, LOOKAHEAD_SAMPLES));
        l.set_attack(samples_to_millis(LIMITER_SAMPLE_RATE, LOOKAHEAD_SAMPLES/3));
        l.set_release(samples_to_millis(LIMITER_SAMPLE_RATE, LOOKAHEAD_SAMPLES/2));

        l.set_attack(samples_to_millis(LIMITER_SAMPLE_RATE, LOOKAHEAD_SAMPLES * 2));
        l.set_release(samples_to_millis(LIMITER_SAMPLE_RATE, LOOKAHEAD_SAMPLES * 3));

        if (l.modified())
            l.update_settings();

        // Process data (step 1)
        dump_buffer("in", in, count);
        l.process(out, gain, in, in, count);
        dump_buffer("gain", gain, count);
        dump_buffer("outd", out, count);
        dsp::mul3(out, out, gain, count);
        dump_buffer("out", out, count);

        // Process data (step 2)
        dsp::fill(in, 0.1f, count);
        dump_buffer("in", in, count);
        l.process(out, gain, in, in, count);
        dump_buffer("gain", gain, count);
        dump_buffer("outd", out, count);
        dsp::mul3(out, out, gain, count);
        dump_buffer("out", out, count);
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        float *out          = new float[LIMITER_BUF_SIZE];
        float *in           = new float[LIMITER_BUF_SIZE];
        float *gain         = new float[LIMITER_BUF_SIZE];

        // Test 1
//        dsp::fill_zero(out, LIMITER_BUF_SIZE);
//        dsp::fill_zero(in, LIMITER_BUF_SIZE);
//        dsp::fill_zero(gain, LIMITER_BUF_SIZE);
//        for (size_t i=0; i<LIMITER_BUF_SIZE - 16; ++i)
//            in[i+8] = 0.1f;
//        in[(LIMITER_BUF_SIZE>>1) + 8] = 1.0f;
//        test_limiter(out, gain, in, LIMITER_BUF_SIZE);

        // Test 2
//        dsp::fill_zero(out, LIMITER_BUF_SIZE);
//        dsp::fill_zero(in, LIMITER_BUF_SIZE);
//        dsp::fill_zero(gain, LIMITER_BUF_SIZE);
//        for (size_t i=8; i<LIMITER_BUF_SIZE; ++i)
//            in[i] = 0.1f;
//        for (size_t i=0; i<10; ++i)
//            in[(LIMITER_BUF_SIZE>>1) + 8 + i] = 1.0f;
//        test_limiter(out, gain, in, LIMITER_BUF_SIZE);

        // Test 3
//        dsp::fill_zero(out, LIMITER_BUF_SIZE);
//        dsp::fill_zero(in, LIMITER_BUF_SIZE);
//        dsp::fill_zero(gain, LIMITER_BUF_SIZE);
//        for (size_t i=8; i<LIMITER_BUF_SIZE; ++i)
//            in[i] = 0.1f;
//        for (size_t i=LIMITER_BUF_SIZE*0.25; i<LIMITER_BUF_SIZE*0.75; ++i)
//            in[i] = 1.0f;
//        test_limiter(out, gain, in, LIMITER_BUF_SIZE);

        // Test 4
        dsp::fill_zero(out, LIMITER_BUF_SIZE);
        dsp::fill_zero(in, LIMITER_BUF_SIZE);
        dsp::fill_zero(gain, LIMITER_BUF_SIZE);
        for (size_t i=8; i<LIMITER_BUF_SIZE; ++i)
            in[i] = 0.1f;
        for (size_t i=LIMITER_BUF_SIZE*0.25; i<LIMITER_BUF_SIZE*0.75; ++i)
        {
            in[i] = sinf(4.0f * M_PI * (i * 4)/LIMITER_BUF_SIZE);
        }
        test_limiter(out, gain, in, LIMITER_BUF_SIZE);

        delete [] out;
        delete [] gain;
        delete [] in;

        return 0;
    }

}

#undef LOOKAHEAD_SAMPLES
#undef LIMITER_BUF_SIZE
#undef LIMITER_SAMPLE_RATE
#undef LIMITER_MAX_LOOKAHEAD
