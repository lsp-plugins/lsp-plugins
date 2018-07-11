#include <stdlib.h>
#include <math.h>

#include <core/types.h>
#include <core/dsp.h>
#include <core/windows.h>
#include <core/envelope.h>
#include <core/files/AudioFile.h>

namespace resample_test
{
    using namespace lsp;

    static const size_t SR1     = 6;
    static const size_t SR2     = 12;

    int test(int argc, const char **argv)
    {
        dsp::init();

        AudioFile af;

#if 0
        // Load audio file
        if (af.load("test_data/HiHatClosed-48k-mono.wav") != STATUS_OK)
            return -1;

        // Resample audio file
        if (af.resample(44100) != STATUS_OK)
            return -2;

        // Store audio file
        if (af.store("test_data/HiHatClosed-44.1k-mono.wav") != STATUS_OK)
            return -3;
#else
        if (af.create(1, SR1, 2.0f) != STATUS_OK)
            return -1;

        // Fill with one sample
        float *dst = af.channel(0);

        dst[0] = 1.0f;
        printf("x[n] = ");
        test::dump_data(dst, af.samples());

        // Resample audio file
        if (af.resample(SR2) != STATUS_OK)
            return -2;

        printf("y[n] = ");
        test::dump_data(af.channel(0), af.samples());

        // Resample audio file again
        if (af.resample(SR1) != STATUS_OK)
            return -2;

        printf("z[n] = ");
        test::dump_data(af.channel(0), af.samples());
#endif

        // Destroy data
        af.destroy();

        return 0;
    }
    
}
