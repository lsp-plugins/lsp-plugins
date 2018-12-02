/*
 * sync_chirp.cpp
 *
 *  Created on: 27 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <test/helpers.h>
#include <core/windows.h>
#include <core/util/SyncChirpProcessor.h>
#include <core/util/ResponseTaker.h>

using namespace lsp;

#define MAX_FNAME_LENGTH 100

MTEST_BEGIN("core.util", sync_chirp)

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

    void write_matrix(const char *description, const char *filePath, const float *matrix, size_t rows, size_t columns)
    {
        printf("Writing matrix %s to file %s\n", description, filePath);

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

    void test_time_series(float *out, float *in, size_t count, SyncChirpProcessor &sc, ResponseTaker *rtArray, size_t nChannels, scp_rtcalc_t enAlgo, float prWsize, double prTol)
    {
        printf("Testing time series generation...\n");

        if (sc.needs_update())
            sc.update_settings();

        sc.reconfigure();

        printf("Chirp properties:\n");
        printf("Initial Frequency:\t%.3f Hz\n", sc.get_chirp_initial_frequency());
        printf("Final Frequency:  \t%.3f Hz\n", sc.get_chirp_final_frequency());
        printf("Alpha:            \t%.3f\n", sc.get_chirp_alpha());
        printf("Gamma:            \t%.3f s\n", sc.get_chirp_gamma());
        printf("Delta:            \t%.3f rad\n", sc.get_chirp_delta());
        printf("Duration:         \t%.7f s\n", sc.get_chirp_duration_seconds());

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            if (rtArray[ch].needs_update())
            	rtArray[ch].update_settings();

            rtArray[ch].reconfigure(sc.get_chirp());

            rtArray[ch].start_capture();
        }

        bool exitLoop = false;

        while (!exitLoop)
        {
        	exitLoop = true;

        	for (size_t ch = 0; ch < nChannels; ++ch)
        	{
        		float *inPtr 	= &in[ch * count];
        		float *outPtr 	= &out[ch * count];

        		rtArray[ch].process(outPtr, inPtr, count);
        		dsp::copy(inPtr, outPtr, count);

        		exitLoop = exitLoop && rtArray[ch].cycle_complete();
        	}

        }

        Sample *data = sc.get_chirp();
        write_buffer("tmp/syncChirp.csv", "sync chirp samples", data->getBuffer(0), data->length());

        data = sc.get_inverse_filter();
        write_buffer("tmp/inverseFilter.csv", "inverse filter samples", data->getBuffer(0), data->length());

        char *fName         = new char[MAX_FNAME_LENGTH];
        Sample **dataArray 	= new Sample*[nChannels];
        size_t *offsets 	= new size_t[nChannels];

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
        	dataArray[ch] 	= rtArray[ch].get_capture();
        	offsets[ch] 	= rtArray[ch].get_capture_start();

            snprintf(fName, MAX_FNAME_LENGTH, "tmp/chirpCapture%lu.csv", ch);
            write_buffer(fName, "Capture", dataArray[ch]->getBuffer(0), dataArray[ch]->length());
        }

        sc.do_linear_convolutions(dataArray, offsets, nChannels, 32768);

        AudioFile *conv = sc.get_convolution_result();
        float *fRT      = new float[nChannels];
        float *fcR      = new float[nChannels];
        float *fiL      = new float[nChannels];

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            snprintf(fName, MAX_FNAME_LENGTH, "tmp/result%lu.csv", ch);
            write_buffer(fName, "Convolution Result", conv->channel(ch), conv->samples());

            sc.postprocess_linear_convolution(ch, 0, enAlgo, prWsize, prTol);

            fRT[ch] = sc.get_reverberation_time_seconds();
            fcR[ch] = sc.get_reverberation_correlation();
            fiL[ch] = sc.get_integration_limit_seconds();
        }

        write_buffer("tmp/fReverbTimes.csv", "Reverberation Times [s]", fRT, nChannels);
        write_buffer("tmp/fCorrCoeffs.csv", "Correlation Coefficients", fcR, nChannels);
        write_buffer("tmp/fIntLimits.csv", "Backward Integration Times [s]", fiL, nChannels);

        sc.save_linear_convolution("tmp/impulseResponse.wav", -1);
        sc.save_to_lspc("tmp/allData.lspc", 0);

        status_t readStatus = sc.load_from_lspc("tmp/allData.lspc");
        MTEST_ASSERT(readStatus == STATUS_OK);

        delete [] fName;

        for (size_t ch = 0; ch < nChannels; ++ch)
		{
			dataArray[ch]->destroy();
		}
        delete [] dataArray;

        delete [] offsets;

        conv->destroy();

        delete [] fRT;
        delete [] fcR;
        delete [] fiL;
    }

//    void test_coefficients_matrices(size_t order, size_t nTaps, size_t offset, float amplitude)
//    {
//        printf("Testing Coefficients matrices generation...");
//
//        SyncChirpProcessor  sc;
//        sc.init();
//        sc.set_chirp_amplitude(amplitude);
//        sc.update_settings();
//
//        // The coefficients matrix in unaffected by smoothing.
//        sc.postprocess_nonlinear_convolution(order, false, 1, 1, windows::RECTANGULAR, nTaps);
//
//        float *coeffRe = sc.get_coefficients_matrix_real_part();
//        float *coeffIm = sc.get_coefficients_matrix_imaginary_part();
//
//        write_matrix("mCoeffsRe", "/tmp/mCoeffsRe.csv", coeffRe, order, order);
//        write_matrix("mCoeffsIm", "/tmp/mCoeffsIm.csv", coeffIm, order, order);
//    }

//    void test_lspc_read(const char *lspcPath)
//    {
//        printf("Testing lspc file read...");
//
//        SyncChirpProcessor  sc;
//        sc.init();
//        sc.update_settings();
//
//        sc.load_from_lspc(lspcPath);
//    }

//    void test_nonlinear_identification(const char *lspcPath, size_t sampleRate, size_t order, bool doInnerSmoothing, size_t nFadeIn, size_t nFadeOut, size_t windowOrder, size_t offset)
//    {
//        printf("Testing nonlinear identification procedure...\n");
//
//        float fSampleRate = sampleRate;
//        write_buffer("/tmp/fSampleRate.csv", "Sample Rate", &fSampleRate, 1);
//
//        float fOrder = order;
//        write_buffer("/tmp/fOrder.csv", "Identification Order", &fOrder, 1);
//
//        float fWindowOrder = windowOrder;
//        write_buffer("/tmp/fWindowOrder.csv", "Window Order", &fWindowOrder, 1);
//
//        float fOffset = offset;
//        write_buffer("/tmp/fOffset.csv", "Window Offset", &fOffset, 1);
//
//        SyncChirpProcessor  sc;
//        sc.init();
//        sc.set_sample_rate(sampleRate);
//        sc.update_settings();
//
//        status_t status = sc.load_from_lspc(lspcPath);
//
//        if (status != STATUS_OK)
//            return;
//
//        size_t taps = 1 << windowOrder;
//
//        AudioFile *conv = sc.get_convolution_result();
//        write_buffer("/tmp/cResult.csv", "Convolution Result", conv->channel(0), conv->samples());
//
//        status = sc.postprocess_nonlinear_convolution(order, doInnerSmoothing, nFadeIn, nFadeOut, windows::HANN, windowOrder);
//
//        if (status != STATUS_OK)
//            return;
//
//        float *coeffRe      = sc.get_coefficients_matrix_real_part();
//        float *coeffIm      = sc.get_coefficients_matrix_imaginary_part();
//
//        write_matrix("mCoeffsRe", "/tmp/mCoeffsRe.csv", coeffRe, order, order);
//        write_matrix("mCoeffsIm", "/tmp/mCoeffsIm.csv", coeffIm, order, order);
//
//        float *higherRe     = sc.get_higher_matrix_real_part();
//        float *higherIm     = sc.get_higher_matrix_imaginary_part();
//
//        write_matrix("mHigherRe", "/tmp/mHigherRe.csv", higherRe, order, taps);
//        write_matrix("mHigherIm", "/tmp/mHigherIm.csv", higherIm, order, taps);
//
//        float *kernelsRe    = sc.get_kernels_matrix_real_part();
//        float *kernelsIm    = sc.get_kernels_matrix_imaginary_part();
//
//        write_matrix("mKernelsRe", "/tmp/mKernelsRe.csv", kernelsRe, order, taps);
//        write_matrix("mKernelsIm", "/tmp/mKernelsIm.csv", kernelsIm, order, taps);
//
//        // Kernels Taps
//        float *kernels = new float[order * taps];
//
//        sc.fill_with_kernel_taps(kernels);
//        write_matrix("mKernelsTaps", "/tmp/mKernelsTaps.csv", kernels, order, taps);
//
//        // Chirp initial frequency
//        float initialFrequency = sc.get_chirp_initial_frequency();
//        write_buffer("/tmp/fInitialFrequency.csv", "Initial Frequency", &initialFrequency, 1);
//
//        // Chirp initial frequency
//        float finalFrequency = sc.get_chirp_final_frequency();
//        write_buffer("/tmp/fFinalFrequency.csv", "Final Frequency", &finalFrequency, 1);
//
//        // Chirp amplitude
//        float alpha = sc.get_chirp_alpha();
//        write_buffer("/tmp/fAlpha.csv", "Chirp Alpha", &alpha, 1);
//
//        // Chirp gamma
//        float gamma = sc.get_chirp_gamma();
//        write_buffer("/tmp/gamma.csv", "Chirp Gamma", &gamma, 1);
//
//        // Chirp delta
//        // Chirp gamma
//        float delta = sc.get_chirp_delta();
//        write_buffer("/tmp/delta.csv", "Chirp Delta", &delta, 1);
//
//        delete [] kernels;
//    }

    MTEST_MAIN
    {
		size_t          nSampleRate     = 48000;
		size_t 			nChannels 		= 2;
		size_t          nLatency        = 4096;
		double          initialFreq     = 1.0;
		double          finalFreq       = 23000.0;
		float           duration        = 10.0f;
		float           amplitude       = 1.0f;
		float           tail            = 1.0f;
		scp_method_t    method          = SCP_SYNTH_BANDLIMITED;
		over_mode_t     overMode        = OM_LANCZOS_8X2;
		scp_fade_t      fadeMode        = SCP_FADE_RAISED_COSINES;
		float           fadeIn          = 0.020f;
		float           fadeOut         = 0.020f;
		float           rFadeIn         = 0.020f;
		float           rPause          = 0.020f;
		scp_rtcalc_t    enAlgo          = SCP_RT_T_20;
		float           prWsize         = 0.085f;
		double          prTol           = 3.0;

        SyncChirpProcessor  sc;
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

        float *in   = new float[nChannels * nLatency];
        float *out  = new float[nChannels * nLatency];

        ResponseTaker *rtArray = new ResponseTaker[nChannels];

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
            rtArray[ch].init();
        	rtArray[ch].set_sample_rate(nSampleRate);
        	rtArray[ch].set_op_fading(rFadeIn);
        	rtArray[ch].set_op_pause(rPause);
        	rtArray[ch].set_op_tail(tail);
        	rtArray[ch].set_latency_samples(nLatency);
        }

        test_time_series(out, in, nLatency, sc, rtArray, nChannels, enAlgo, prWsize, prTol);

//        size_t          iSampleRate         = 48000; // For identification
//        size_t          order               = 9;
//        bool            doInnerSmoothing    = true;
//        size_t          nFadeIn             = 50000;
//        size_t          nFadeOut            = 50000;
//        size_t          windowOrder         = 12;
//        size_t          offset              = 1 << (windowOrder - 1);
//
//        // test_coefficients_matrices(order, windowOrder, offset, amplitude);
//
//        const char *lspcPath            = "test_data/profiler_mono/straightWire.lspc";
//
//        test_lspc_read(lspcPath);
//
//        test_nonlinear_identification(lspcPath, iSampleRate, order, doInnerSmoothing, nFadeIn, nFadeOut, windowOrder, offset);

        delete [] out;
        delete [] in;

        sc.destroy();

        for (size_t ch = 0; ch < nChannels; ++ch)
        {
			rtArray[ch].destroy();
        }

        delete [] rtArray;

    }

MTEST_END




