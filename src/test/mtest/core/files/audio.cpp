/*
 * audio.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <test/helpers.h>
#include <core/files/AudioFile.h>

using namespace lsp;

#define DUMP_SAMPLES        32

static const char *FILE_NAME = "test_data/music/test.wav";

MTEST_BEGIN("core.files", audio)

    void print_file(AudioFile &af)
    {
        size_t samples = af.samples();
        size_t dump = (samples > DUMP_SAMPLES) ? DUMP_SAMPLES : samples;
        for (size_t i=0; i<af.channels(); ++i)
        {
            const float *buf = af.channel(i);
            printf_buffer(buf, dump, "  Channel #%d begin:", int(i));
            printf_buffer(&buf[(samples - dump) >> 1], dump, "  Channel #%d middle:", int(i));
            printf_buffer(&buf[samples - dump], dump, "  Channel #%d end: \n", int(i));
        }
    }

    MTEST_MAIN
    {
        const char *path = FILE_NAME;
        if (argc > 0)
            path = argv[0];

        AudioFile af;
        status_t success = af.load(path);

        MTEST_ASSERT_MSG(success == STATUS_OK, "Could not load audio file %s, status=%d (%s)", path, int(success), get_status(success));

        printf("Channels:      %ld\n", long(af.channels()));
        printf("Samples:       %ld\n", long(af.samples()));
        printf("Sample rate:   %ld\n", long(af.sample_rate()));

        // Reverse file
        printf("Dump before reverse()\n");
        print_file(af);
        af.reverse();
        printf("Dump after  reverse()\n");
        print_file(af);

        af.destroy();
    }

MTEST_END



