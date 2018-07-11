#include <core/SamplePlayer.h>

namespace sample_player_test
{
    using namespace lsp;

    static const size_t SAMPLE_LENGTH=8;

    float samples[4][SAMPLE_LENGTH] =
    {
        { 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 2, 3, 4, 4, 3, 2, 1 },
        { 4, 3, 2, 1, 1, 2, 3, 4 },
        { 1, 2, 3, 2, 2, 3, 2, 1 }
    };

    float buffer[4];

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

    int test(int argc, const char **argv)
    {
        using namespace lsp;
        dsp::init();

        SamplePlayer sp;
        sp.init(4, 5);

        // Initialize samples
        for (size_t i=0; i<4; ++i)
        {
            Sample *s = new Sample;
            s->init(SAMPLE_LENGTH, SAMPLE_LENGTH);
            dsp::copy(s->getBuffer(0), samples[i], SAMPLE_LENGTH);
            sp.bind(i, s);
        }

        // Trigger playing
        for (size_t i=0; i<5; ++i)
            sp.play(i%4, 1.0f, i * 2);

        /* 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
         *     1 2 3 4 4 3 2 1
         *         4 3 2 1 1 2 3 4
         *             1 2 3 2 2 3 2 1
         *                 1 1 1 1 1 1 1 1
         * 0 0 1 2 7 7 7 6 7 6 6 8 3 2 1 1 0 0 0 0
         */

        /* 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
         * 1 1 1 1 1 1 1 1
         *     1 2 3 4 4 3 2 1
         *         4 3 2 1 1 2 3 4
         *             1 2 3 2 2 3 2 1
         *                 1 1 1 1 1 1 1 1
         * 1 1 2 3 8 8 8 7 7 6 6 8 3 2 1 1 0 0 0 0
         */

        // Call processing
        for (size_t processed = 0; processed <= 20; processed += sizeof(buffer)/sizeof(float))
        {
            sp.process(buffer, sizeof(buffer)/sizeof(float));
            dump_data(buffer, sizeof(buffer)/sizeof(float));
        }

        // Destroy player cascade
        sp.destroy(true);

        return 0;
    }
}
