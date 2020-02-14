/*
 * audiofile.cpp
 *
 *  Created on: 22 февр. 2019 г.
 *      Author: sadko
 */


#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <core/sampling/Sample.h>
#include <core/LSPString.h>
#include <core/files/AudioFile.h>

using namespace lsp;
#define FREQUENCY       440.0f

UTEST_BEGIN("core.files", audiofile)

    UTEST_MAIN
    {
        LSPString path;
        Sample source, loaded;

        // Create sample
        UTEST_ASSERT(source.init(2, DEFAULT_SAMPLE_RATE, DEFAULT_SAMPLE_RATE));
        float phase = 0.0f;

        for (size_t ch=0; ch<2; ++ch) {
            float *dst = source.getBuffer(ch);
            UTEST_ASSERT(dst != NULL);

            float kf = FREQUENCY / DEFAULT_SAMPLE_RATE;

            for (size_t i=0; i<DEFAULT_SAMPLE_RATE; ++i)
                dst[i]  = sinf(phase + 2.0f * M_PI * kf * i);

            phase += 0.5f * M_PI; // sin -> cos
        }

        // Save to audio file
        AudioFile af;
        io::Path ipath;
        UTEST_ASSERT(path.fmt_utf8("tmp/utest-%s.wav", this->full_name()));
        UTEST_ASSERT(ipath.set(&path) == STATUS_OK);
        UTEST_ASSERT(af.create(&source, DEFAULT_SAMPLE_RATE) == STATUS_OK);

        UTEST_ASSERT(af.store_samples(&path, af.samples()) == STATUS_OK);
        UTEST_ASSERT(af.store_samples(path.get_utf8(), af.samples()) == STATUS_OK);
        UTEST_ASSERT(af.store_samples(&ipath, af.samples()) == STATUS_OK);

        UTEST_ASSERT(af.store_samples(&path, 0, af.samples()) == STATUS_OK);
        UTEST_ASSERT(af.store_samples(path.get_utf8(), 0, af.samples()) == STATUS_OK);
        UTEST_ASSERT(af.store_samples(&ipath, 0, af.samples()) == STATUS_OK);

        UTEST_ASSERT(af.store(path.get_utf8()) == STATUS_OK)
        UTEST_ASSERT(af.store(&ipath) == STATUS_OK);
        UTEST_ASSERT(af.store(&path) == STATUS_OK);

        af.destroy();
        UTEST_ASSERT(af.channels() == 0);

        // Load audio file
        UTEST_ASSERT(af.load(&path) == STATUS_OK);
        UTEST_ASSERT(af.channels() == source.channels());
        UTEST_ASSERT(af.samples() == source.length());
        UTEST_ASSERT(af.sample_rate() == DEFAULT_SAMPLE_RATE);
        UTEST_ASSERT(af.convert_to_sample(&loaded) == STATUS_OK);
        af.destroy();
        UTEST_ASSERT(af.channels() == 0);

        // Compare samples
        UTEST_ASSERT(source.channels() == loaded.channels());
        UTEST_ASSERT(source.length() == loaded.length());

        for (size_t i=0; i<source.channels(); ++i)
        {
            FloatBuffer src1(source.length()), src2(loaded.length());
            src1.copy(source.getBuffer(i), source.length());
            src2.copy(loaded.getBuffer(i), loaded.length());

            UTEST_ASSERT(src1.equals_absolute(src2));
        }

        source.destroy();
        loaded.destroy();

        // Do final checks
        UTEST_ASSERT(source.channels() == 0);
        UTEST_ASSERT(source.length() == 0);
        UTEST_ASSERT(loaded.channels() == 0);
        UTEST_ASSERT(loaded.length() == 0);
    }

UTEST_END

