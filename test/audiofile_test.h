#include <core/AudioFile.h>

namespace audiofile_test
{
    const char *FILE_NAME = "/home/sadko/music/test.wav";
    const size_t DUMP_SAMPLES = 32;

    using namespace lsp;

    void dump_data(const float *data, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            if (i > 0)
                printf(", ");
            printf("%.3f", data[i]);
        }
        printf("\n");
    }

    void print_file(AudioFile &af)
    {
        size_t samples = af.samples();
        size_t dump = (samples > DUMP_SAMPLES) ? DUMP_SAMPLES : samples;
        for (size_t i=0; i<af.channels(); ++i)
        {
            const float *buf = af.channel(i);
            printf("  Channel #%d begin: \n", int(i));
            dump_data(buf, dump);
            printf("  Channel #%d middle: \n", int(i));
            dump_data(&buf[(samples - dump) >> 1], dump);
            printf("  Channel #%d end: \n", int(i));
            dump_data(&buf[samples - dump], dump);
        }
    }

    int test(int argc, const char **argv)
    {
        using namespace lsp;
        dsp::init();

        AudioFile af;
        status_t success = af.load(FILE_NAME);

        if (success != STATUS_OK)
            lsp_error("could not load audio file %s, status=%d (%s)\n", FILE_NAME, int(success), get_status(success));
        else
        {
            printf("Channels:      %ld\n", long(af.channels()));
            printf("Samples:       %ld\n", long(af.samples()));
            printf("Sample rate:   %ld\n", long(af.sample_rate()));
        }

        // Reverse file
        printf("Dump before reverse()\n");
        print_file(af);
        af.reverse();
        printf("Dump after  reverse()\n");
        print_file(af);

        af.destroy();

        return (success) ? 0 : -1;
    }
}
