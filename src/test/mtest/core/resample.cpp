/*
 * resample.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <core/files/AudioFile.h>

using namespace lsp;

static const char *SRC_FILE = "test_data/source.wav";
static const char *DST_FILE = "test_data/destination.wav";

MTEST_BEGIN("core", resample)

    MTEST_MAIN
    {
        const char *src = (argc >= 1) ? SRC_FILE : argv[0];
        const char *dst = (argc >= 2) ? DST_FILE : argv[1];

        AudioFile af;

        MTEST_ASSERT(af.load(src) == STATUS_OK); // Load audio file
        size_t sr = (af.sample_rate() == 44100) ? 48000 : 44100;
        printf("Resampling %d -> %d", int(af.sample_rate()), int(sr));

        MTEST_ASSERT(af.resample(sr) == STATUS_OK); // Resample audio file
        MTEST_ASSERT(af.store(dst) == STATUS_OK); // Store file

        // Destroy data
        af.destroy();
    }

MTEST_END


