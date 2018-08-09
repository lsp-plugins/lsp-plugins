/*
 * synchronizedchirp_test.h
 *
 *  Created on: 14 Jul 2017
 *      Author: crocoduck
 */

#include <core/SyncChirpProcessor.h>
#include <core/ResponseTaker.h>
#include <stdio.h>

#define SYNCHRONIZEDCHIRP_BUF_SIZE      4096
#define SYNCHRONIZEDCHIRP_SAMPLE_RATE   192000

namespace synchronizedchirp_test
{
    using namespace lsp;

    void dump_buffer(const char *text, const float *buf, size_t count)
    {
        printf("dump of buffer %s:\n", text);
        while (count--)
            printf("%.30f\n", *(buf++));
    }

    void write_buffer(const char *filePath, const char *text, const float *buf, size_t count)
    {
        printf("Writing buffer %s:\n", text);

        FILE *fp = NULL;
        fp = fopen(filePath, "w");

        if (fp == NULL)
            return;

        while (count--)
            fprintf(fp, "%.30f\n", *(buf++));

        if(fp)
            fclose(fp);
    }

    void write_matrix(const char *text, const char *filePath, const float *matrix, size_t rows, size_t columns)
    {
        printf("Writing matrix %s:\n", text);

        FILE *fp = NULL;

        fp = fopen(filePath, "w");

        if (fp == NULL)
            return;

        for (size_t r = 0; r < rows; ++ r)
        {
            for (size_t c = 0; c < columns; ++c)
            {
                fprintf(fp, "%.30f", matrix[r * columns + c]);

                if (c == (columns - 1))
                    fprintf(fp, "\n");
                else
                    fprintf(fp, ", ");
            }
        }

        if(fp)
            fclose(fp);
    }

    void test_time_series(float *out, float *in, size_t count, size_t nLatency, size_t nSampleRate, double initialFreq, double finalFreq, float duration, float amplitude, float tail, scp_method_t method, over_mode_t overMode, scp_fade_t fadeMode, float fadeIn, float fadeOut)
    {
        printf("Testing time series generation...");

        SyncChirpProcessor  sc;
        ResponseTaker       rt;

        sc.init();
        sc.set_sample_rate(nSampleRate);
        sc.set_chirp_initial_frequency(initialFreq);
        sc.set_chirp_final_frequency(finalFreq);
        sc.set_chirp_duration(duration);
        sc.set_chirp_amplitude(amplitude);
        sc.set_chirp_synthesis_method(method);
        sc.set_fader_fading_method(fadeMode);
        sc.set_fader_fadein(fadeIn);
        sc.set_fader_fadeout(fadeOut);
        sc.set_oversampler_mode(overMode);

        if (sc.needs_update())
            sc.update_settings();

        rt.set_sample_rate(nSampleRate);
//        rt.set_op_fading();
//        rt.set_op_pause();
        rt.set_op_tail(tail);
        rt.set_latency_samples(nLatency);

        if (rt.needs_update())
            rt.update_settings();

        sc.reconfigure();
        rt.reconfigure(sc.get_chirp());

        rt.start_capture();

        while (!rt.cycle_complete())
        {
            rt.process(out, in, count);
            dsp::copy(in, out, count);
        }

        Sample *data = sc.get_chirp();
        write_buffer("/tmp/chirp.csv", "Chirp", data->getBuffer(0), data->length());

        data = sc.get_inverse_filter();
        write_buffer("/tmp/inverseFilter.csv", "Inverse Filter", data->getBuffer(0), data->length());

//        data = sc.get_time_lags();
//        write_buffer("Time Lags", data->getBuffer(0), data->length());

        data = rt.get_capture();
//        write_buffer("Capture", data->getBuffer(0), data->length());

        sc.do_linear_convolution(data, rt.get_capture_start());

//        AudioFile *conv = sc.get_convolution_result();
//        write_buffer("Convolution", conv->channel(0), conv->samples());

//        sc.save_linear_convolution("/tmp/positiveTimeResponse.wav");
//        sc.save_nonlinear_convolution("/tmp/allTimeResponse.wav");
    }

    void test_coefficients_matrices(size_t order, size_t nTaps, size_t offset, float amplitude)
    {
        printf("Testing Coefficients matrices generation...");

        SyncChirpProcessor  sc;
        sc.init();
        sc.set_chirp_amplitude(amplitude);
        sc.update_settings();

        // The coefficients matrix in unaffected by smoothing.
        sc.postprocess_nonlinear_convolution(order, false, 1, 1, windows::RECTANGULAR, nTaps);

        float *coeffRe = sc.get_coefficients_matrix_real_part();
        float *coeffIm = sc.get_coefficients_matrix_imaginary_part();

        write_matrix("mCoeffsRe", "/tmp/mCoeffsRe.csv", coeffRe, order, order);
        write_matrix("mCoeffsIm", "/tmp/mCoeffsIm.csv", coeffIm, order, order);
    }

    void test_lspc_read(const char *lspcPath)
    {
        printf("Testing lspc file read...");

        SyncChirpProcessor  sc;
        sc.init();
        sc.update_settings();

        sc.load_from_lspc(lspcPath);
    }

    void test_nonlinear_identification(const char *lspcPath, size_t sampleRate, size_t order, bool doInnerSmoothing, size_t nFadeIn, size_t nFadeOut, size_t windowOrder, size_t offset)
    {
        printf("Testing nonlinear identification procedure...\n");

        float fSampleRate = sampleRate;
        write_buffer("/tmp/fSampleRate.csv", "Sample Rate", &fSampleRate, 1);

        float fOrder = order;
        write_buffer("/tmp/fOrder.csv", "Identification Order", &fOrder, 1);

        float fWindowOrder = windowOrder;
        write_buffer("/tmp/fWindowOrder.csv", "Window Order", &fWindowOrder, 1);

        float fOffset = offset;
        write_buffer("/tmp/fOffset.csv", "Window Offset", &fOffset, 1);

        SyncChirpProcessor  sc;
        sc.init();
        sc.set_sample_rate(sampleRate);
        sc.update_settings();

        status_t status = sc.load_from_lspc(lspcPath);

        if (status != STATUS_OK)
            return;

        size_t taps = 1 << windowOrder;

        AudioFile *conv = sc.get_convolution_result();
        write_buffer("/tmp/cResult.csv", "Convolution Result", conv->channel(0), conv->samples());

        status = sc.postprocess_nonlinear_convolution(order, doInnerSmoothing, nFadeIn, nFadeOut, windows::HANN, windowOrder);

        if (status != STATUS_OK)
            return;

        float *coeffRe      = sc.get_coefficients_matrix_real_part();
        float *coeffIm      = sc.get_coefficients_matrix_imaginary_part();

        write_matrix("mCoeffsRe", "/tmp/mCoeffsRe.csv", coeffRe, order, order);
        write_matrix("mCoeffsIm", "/tmp/mCoeffsIm.csv", coeffIm, order, order);

        float *higherRe     = sc.get_higher_matrix_real_part();
        float *higherIm     = sc.get_higher_matrix_imaginary_part();

        write_matrix("mHigherRe", "/tmp/mHigherRe.csv", higherRe, order, taps);
        write_matrix("mHigherIm", "/tmp/mHigherIm.csv", higherIm, order, taps);

        float *kernelsRe    = sc.get_kernels_matrix_real_part();
        float *kernelsIm    = sc.get_kernels_matrix_imaginary_part();

        write_matrix("mKernelsRe", "/tmp/mKernelsRe.csv", kernelsRe, order, taps);
        write_matrix("mKernelsIm", "/tmp/mKernelsIm.csv", kernelsIm, order, taps);

        // Kernels Taps
        float *kernels = new float[order * taps];

        sc.fill_with_kernel_taps(kernels);
        write_matrix("mKernelsTaps", "/tmp/mKernelsTaps.csv", kernels, order, taps);

        // Chirp initial frequency
        float initialFrequency = sc.get_chirp_initial_frequency();
        write_buffer("/tmp/fInitialFrequency.csv", "Initial Frequency", &initialFrequency, 1);

        // Chirp initial frequency
        float finalFrequency = sc.get_chirp_final_frequency();
        write_buffer("/tmp/fFinalFrequency.csv", "Final Frequency", &finalFrequency, 1);

        // Chirp amplitude
        float alpha = sc.get_chirp_alpha();
        write_buffer("/tmp/fAlpha.csv", "Chirp Alpha", &alpha, 1);

        // Chirp gamma
        float gamma = sc.get_chirp_gamma();
        write_buffer("/tmp/gamma.csv", "Chirp Gamma", &gamma, 1);

        // Chirp delta
        // Chirp gamma
        float delta = sc.get_chirp_delta();
        write_buffer("/tmp/delta.csv", "Chirp Delta", &delta, 1);

        delete [] kernels;
    }

    int test(int argc, const char **argv)
    {
        dsp_context_t ctx;

        srand(static_cast<size_t>(time(0)));

        dsp::init();
        dsp::start(&ctx);

        float *in   = new float[SYNCHRONIZEDCHIRP_BUF_SIZE];
        float *out  = new float[SYNCHRONIZEDCHIRP_BUF_SIZE];

//        size_t          sampleRate      = SYNCHRONIZEDCHIRP_SAMPLE_RATE;
//        size_t          nLatency        = SYNCHRONIZEDCHIRP_BUF_SIZE;
//        double          initialFreq     = 1.0;
//        double          finalFreq       = 25000.0;
//        float           duration        = 10.0f;
//        float           amplitude       = 1.0f;
//        float           tail            = 1.0f;
//        scp_method_t    method          = SCP_SYNTH_BANDLIMITED;
//        over_mode_t     overMode        = OM_LANCZOS_8X2;
//        scp_fade_t      fadeMode        = SCP_FADE_RAISED_COSINES;
//        float           fadeIn          = 0.020f;
//        float           fadeOut         = 0.020f;

        // test_time_series(out, in, SYNCHRONIZEDCHIRP_BUF_SIZE, nLatency, nSampleRate, initialFreq, finalFreq, duration, amplitude, tail, method, overMode, fadeMode, fadeIn, fadeOut);

        size_t          iSampleRate         = 48000; // For identification
        size_t          order               = 9;
        bool            doInnerSmoothing    = true;
        size_t          nFadeIn             = 50000;
        size_t          nFadeOut            = 50000;
        size_t          windowOrder         = 12;
        size_t          offset              = 1 << (windowOrder - 1);

        // test_coefficients_matrices(order, windowOrder, offset, amplitude);

        const char *lspcPath            = "test_data/profiler_mono/straightWire.lspc";

        test_lspc_read(lspcPath);

        test_nonlinear_identification(lspcPath, iSampleRate, order, doInnerSmoothing, nFadeIn, nFadeOut, windowOrder, offset);

        delete [] out;
        delete [] in;

        dsp::finish(&ctx);

        return 0;
    }
}

#undef SYNCHRONIZEDCHIRP_BUF_SIZE
#undef SYNCHRONIZEDCHIRP_SAMPLE_RATE
