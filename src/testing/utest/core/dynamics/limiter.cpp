/*
 * limiter.cpp
 *
 *  Created on: 12 февр. 2020 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/helpers.h>
#include <test/FloatBuffer.h>
#include <core/dynamics/Limiter.h>
#include <core/io/OutSequence.h>

#define SRATE       48000
#define BUF_SIZE    4096

using namespace lsp;

UTEST_BEGIN("core.dynamics", limiter)

    void test_triangle_peak_classic()
    {
        FloatBuffer in(BUF_SIZE);
        FloatBuffer out(BUF_SIZE);
        FloatBuffer gain(BUF_SIZE);

        // Prepare buffer
        in.fill_zero();
        {
            float s = 0.0f, step = 0.05f;
            int i=0;
            for ( ; s < 0.999f; s += step)
                in[i++] = s;
            for ( ; s > 0.001f; s -= step)
                in[i++] = s;
        }

        // Initialize limiter
        Limiter l;
        UTEST_ASSERT(l.init(SRATE*4, 20.0f));
        l.set_sample_rate(SRATE);
        l.set_mode(LM_COMPRESSOR);
        l.set_knee(1.0f);
        l.set_threshold(0.5f);
        l.set_attack(1.5);
        l.set_release(1.5);
        l.set_lookahead(5); // 5 ms lookahead
        UTEST_ASSERT(l.modified());
        l.update_settings();

        l.process(out, gain, in, in, BUF_SIZE);
        ssize_t latency = l.get_latency();
        UTEST_ASSERT(latency == ssize_t(5.0f * SRATE * 0.001f));

        // Save output
        LSPString path;
        UTEST_ASSERT(path.fmt_utf8("tmp/utest-tpc-%s.csv", this->full_name()));
        io::OutSequence fd;
        UTEST_ASSERT(fd.open(&path, io::File::FM_WRITE_NEW, "UTF-8") == STATUS_OK);

        LSPString line;
        UTEST_ASSERT(fd.write_ascii("in;gain;out\n") == STATUS_OK);
        for (size_t i=0; i<BUF_SIZE; ++i)
        {
            UTEST_ASSERT(line.fmt_utf8("%.6f;%.6f;%.6f\n", in[i], gain[i], out[i]));
            UTEST_ASSERT(fd.write(&line) == STATUS_OK);
        }

        UTEST_ASSERT(fd.close() == STATUS_OK);

        UTEST_ASSERT(dsp::max(out, BUF_SIZE) < 0.6f);
        UTEST_ASSERT(dsp::min(out, BUF_SIZE) >= 0.0f);

        UTEST_ASSERT(dsp::max(gain, BUF_SIZE) >= 1.0f);
        UTEST_ASSERT(dsp::min(gain, BUF_SIZE) >= 0.0f);
        UTEST_ASSERT(float_equals_adaptive(gain.get(0), 1.0f));
        UTEST_ASSERT(float_equals_adaptive(gain.get(BUF_SIZE-1), 1.0f));

        ssize_t i1 = dsp::max_index(in, BUF_SIZE);
        ssize_t i2 = dsp::max_index(out, BUF_SIZE);
        UTEST_ASSERT(latency == (i2-i1));

        l.destroy();
    }

    UTEST_MAIN
    {
        test_triangle_peak_classic();
    }

UTEST_END
