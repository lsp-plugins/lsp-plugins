/*
 * oscilloscope.cpp
 *
 *  Created on: 16 Feb 2020
 *      Author: crocoduck
 */

#include <test/mtest.h>
#include <test/helpers.h>
#include <core/util/Oscillator.h>
#include <core/util/Oscilloscope.h>

using namespace lsp;

#define MAX_FNAME_LENGTH 100

MTEST_BEGIN("core.util", oscilloscope)

    void write_buffer(const char *filePath, const char *description, const float *buf, size_t count)
    {
        printf("Writing %s to file %s\n", description, filePath);

        FILE *fp = NULL;
        fp = fopen(filePath, "w");

        if (fp == NULL)
            return;

        while (count--)
            fprintf(fp, "%.30f\n", *(buf++));

        if(fp)
            fclose(fp);
    }

    void run_test(float *oscilloscopeSrc, float *oscilloscopeDst, float *sweepDst, Oscillator &sOscillator, Oscilloscope &sOscilloscope, size_t oscillatorCount, size_t sweepCount, size_t nSweeps)
    {
        char *fName = new char[MAX_FNAME_LENGTH];

        float sweepMark = 1.0f;

        while (sweepMark <= nSweeps)
        {
            sOscillator.process_overwrite(oscilloscopeSrc, oscillatorCount);
            dsp::mul_k2(oscilloscopeSrc, sweepMark, oscillatorCount);

            sOscilloscope.process(oscilloscopeDst, oscilloscopeSrc, oscillatorCount);

            float *sweepBuf = sOscilloscope.get_sweep_buffer();

            if (sOscilloscope.get_sweep_complete())
            {
                snprintf(fName, MAX_FNAME_LENGTH, "tmp/sweep_%lu.csv", (unsigned long)sweepMark);
                dsp::copy(sweepDst, sweepBuf, sweepCount);
                write_buffer(fName, "Sweep", sweepDst, sweepCount);
                sOscilloscope.set_sweep_complete(false);
                ++sweepMark;
            }
        }

        delete [] fName;
    }

MTEST_MAIN
{
    size_t          nTestSigBufLen  = 1000;

    size_t          nSampleRate     = 48000;
    fg_function_t   enTestSigFunc   = FG_SINE;
    float           fTestSigFreq    = 1000.0f;

    over_mode_t     enOverMode      = OM_LANCZOS_8X3;
    float           fPreTrigger     = 3.0f / fTestSigFreq;
    float           fPostTrigger    = 3.0f / fTestSigFreq;
    trg_type_t      enTrigType      = TRG_TYPE_SIMPLE_RISING_EDGE;

    size_t          nSweeps         = 10;

    Oscillator sOscillator;
    sOscillator.init();
    sOscillator.set_sample_rate(nSampleRate);
    sOscillator.set_function(enTestSigFunc);
    sOscillator.set_frequency(fTestSigFreq);
    sOscillator.set_amplitude(1.0f);
    sOscillator.update_settings();

    Oscilloscope sOscilloscope;
    sOscilloscope.init();
    sOscilloscope.set_sample_rate(nSampleRate);
    sOscilloscope.set_oversampler_mode(enOverMode);
    sOscilloscope.set_pre_trigger_time(fPreTrigger);
    sOscilloscope.set_post_trigger_time(fPostTrigger);
    sOscilloscope.set_trigger_type(enTrigType);
    sOscilloscope.update_settings();

    size_t nSweepSigBufLen = sOscilloscope.get_sweep_length();

    float *in       = new float[nTestSigBufLen];
    float *out      = new float[nTestSigBufLen];
    float *sweep    = new float[nSweepSigBufLen];

    run_test(out, in, sweep, sOscillator, sOscilloscope, nTestSigBufLen, nSweepSigBufLen, nSweeps);

    delete [] in;
    delete [] out;
    delete [] sweep;

    sOscillator.destroy();
    sOscilloscope.destroy();
}

MTEST_END
