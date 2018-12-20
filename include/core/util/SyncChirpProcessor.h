/*
 * SyncChirpProcessor.h
 *
 *  Created on: 12 Jul 2017
 *      Author: crocoduck
 */

#ifndef CORE_UTIL_SYNCCHIRPPROCESSOR_H_
#define CORE_UTIL_SYNCCHIRPPROCESSOR_H_

#include <core/types.h>
#include <core/sampling/SamplePlayer.h>
#include <core/status.h>
#include <core/files/AudioFile.h>
#include <core/util/Oversampler.h>
#include <core/windows.h>

namespace lsp
{
    enum scp_method_t
    {
        SCP_SYNTH_SIMPLE,                           // Pure math chirp and inverse filter
        SCP_SYNTH_CHIRPBANDLIMITED,                 // Band limited chirp, pure math inverse filter
        SCP_SYNTH_BANDLIMITED,                      // Band limited chirp and inverse filter
        SCP_SYNTH_MAX
    };

    enum scp_fade_t
    {
        SCP_FADE_NONE,                              // No fade in - fade out
        SCP_FADE_RAISED_COSINES,                    // Raised cosine like shapes for fade in and fade out
        SCP_FADE_MAX
    };

    enum scp_rtcalc_t
    {
        SCP_RT_EDT_0,                               // Early Decay Time with 0 dB upper limit
        SCP_RT_EDT_1,                               // Early Decay Time with 1 dB upper limit
        SCP_RT_T_10,                                // T 19
        SCP_RT_T_20,                                // T 20
        SCP_RT_T_30,                                // T 30
        SCP_RT_MAX,

        SCP_RT_DEFAULT          = SCP_RT_EDT_0
    };

    class SyncChirpProcessor
    {

        protected:

            // Chirp parameters:
            typedef struct chirp_t
            {
                // Tunable properties:
                scp_method_t    enMethod;           // Synthesis method
                double          initialFrequency;   // Initial chirp frequency [Hz]
                double          finalFrequency;     // Final chirp frequency [Hz]
                float           fDuration;          // Chirp duration [s]
                float           fAlpha;             // Chirp amplitude. This parameter is here as the inverse filter needs scaling too to be matched to the chirp.

                // Calculated properties:
                float           fDurationCoarse;    // Pre-optimisation value of fDuration [s]
                size_t          nDuration;          // Chirp duration [samples]
                size_t          nTimeLags;          // Number of higher order responses time lags calculated
                size_t          nOrder;             // Number of harmonics of the initial frequency covered by the chirp
                double          beta;               // Initial angular frequency [rad /s]
                double          gamma;              // Exponential time constant [1 / s]
                double          delta;              // Chirp constant
                float           fConvScale;         // Scale factor for convolution

                bool            bAsymptotic;        // True if the chirp is asymptotic. The theory of nonlinear profiling holds for asymptotic chirps.

                bool            bRecalculate;       // If true, recalculate the chirp related parameters
                bool            bReconfigure;       // If true, reconfigure all the time series
            } chirp_t;

            // Fader parameters:
            typedef struct fader_t
            {
                // Tunable properties:
                scp_fade_t      enMethod;           // Fading method
                float           fFadeIn;            // Fade in time [s]
                float           fFadeOut;           // Fade out time [s]

                // Calculated properties:
                size_t          nFadeIn;            // Fade in time [samples]
                size_t          nFadeIn_Over;       // Fade in time, oversampled [samples]
                size_t          nFadeOut;           // Fade out time [samples]
                size_t          nFadeOut_Over;      // Fade out time, oversampled [samples]
            } fader_t;

            // Convolution Parameters
            typedef struct conv_t
            {
            	size_t          nChannels;          // Number of channels of convolution result
            	size_t          nPartitionSize;     // Size of the partition used to compute convolution [samples]
            	size_t 			nConvRank; 			// Rank of single partition convolution
            	size_t 			nImage; 			// Size of single partition convolution images [samples]

            	size_t 			nAllocationSize; 	// Number of samples to allocate for convolution result AudioFile object [samples]
            	size_t 		   *vPartitions; 		// Number of partitions used to cover each single padded input time series for each channel
            	size_t 		   *vPaddedLengths;     // Length of each input time series for each channel, padded with zeros so to be long an integer number of partition sizes [samples] - The zero pad is not real (allocated), but convolution happens as if it was.
            	size_t 		   *vInversePrepends; 	// Length of the zero prepending at the inverse filter, so that the total length of prepend + filter is the same as the padded channel length [samples] - The zero prepend is not real (allocated), but convolution happens as if it was.
            	size_t 		   *vConvLengths;       // For each channel in the convolution result, the length of the result stored therein, counted from the beginning [samples]
            	size_t         *vAlignOffsets;      // For each channel in the convolution result, the offset with which the result has to be stored so that all the origins of times are aligned [samples]
            	uint8_t        *pData;

            	float 		   *vInPart; 			// Holds a single input time series partition
            	float          *vInvPart; 			// Holds a single inverse filter partition
				float          *vInImage; 			// Holds a single input time series FFT image
				float 	       *vInvImage; 			// Holds a single inverse filter FFT image
				float 		   *vTemp; 				// Holds temporary data
            	uint8_t 	   *pTempData;
            	bool 			bReallocateTemp;
            } conv_t;

            // Convolution Result Post-processing values:
            typedef struct crpostproc_t
            {
                // Background Noise Properties
                double          noiseLevel;        // Background noise level [dB]
                double          noiseValue;        // Background noise level [linear scale]

                // Optimal Convolution Result Positive Time Backwards Integration limit
                float           fIrLimit;           // Integration Limit [s]
                size_t          nIrLimit;           // Integration Limit [samples]

                // Background Noise Properties, normalised by Positive Time Convolution Result Total Energy
                // (energy within origin of time and the optimal limit)
                double          noiseLevelNorm;
                double          noiseValueNorm;

                bool            bLowNoise;          // If true, the noise was low enough for the requested RT calculation

                // Reverberation time, through Convolution Result Positive Time Backwards Integration
                size_t          nRT;                // Reverberation time [samples]
                float           fRT;                // Reverberation time [s]
                float           fCorrelation;       // Reverberation regression line correlation coefficient

                // Matrices for nonlinear identification procedure:
                size_t          nHamOrder;          // Order of the Hammerstain model to identify
                size_t          nHwinSize;          // Size of the window to separate higher order responses
                size_t          nWinRank;           // Rank of the window (power of two expressing the window size)
                double          mCoeffsReDet;       // Determinant of matrix of Linear System Coefficients (Real Part)
                double          mCoeffsImDet;       // Determinant of matrix of Linear System Coefficients (Imaginary Part)
                float          *mCoeffsRe;          // Matrix of real parts of Linear System Coefficients
                float          *mCoeffsIm;          // Matrix of imag parts of Linear System Coefficients
                float          *mHigherRe;          // Matrix of real parts of higher order frequency responses
                float          *mHigherIm;          // Matrix of imag parts of higher order frequency responses
                float          *mKernelsRe;         // Matrix of real parts of model kernels
                float          *mKernelsIm;         // Matrix of imag parts of model kernels
                float          *vTemprow1Re;        // Temporary rows for kernel matrix calculation
                float          *vTemprow1Im;
                float          *vTemprow2Re;
                float          *vTemprow2Im;
                uint8_t        *pData;
            } crpostproc_t;

        private:

            size_t              nSampleRate;

            chirp_t             sChirpParams;

            fader_t             sFader;

            conv_t 				sConvParams;

            crpostproc_t        sCRPostProc;

            Sample             *pChirp;
            Sample             *pInverseFilter;
            AudioFile          *pConvResult;

            Oversampler         sOver1;             // Oversampler for Band Limited chirp synthesis.
            Oversampler         sOver2;             // Oversampler for Band Limited inverse filter synthesis.
            over_mode_t         enOverMode;         // Oversampler mode.
            size_t              nOversampling;      // Hold oversampling factor. This assumes all the oversamples used here will have the same factor.

            float              *vOverBuffer1;       // Temporary buffer for oversampled synthesis
            float              *vOverBuffer2;       // Temporary buffer for oversampled synthesis
            float              *vEnvelopeBuffer;    // Buffer for Convolution Result Envelope processing
            uint8_t            *pData;

            bool                bSync;

        public:

            SyncChirpProcessor();
            ~SyncChirpProcessor();

        protected:

            /** Calculate the best partition size for convolution. Also computes rank and image size.
             *
             * @param partSizeLimit max partition size
             */
            void calculateConvolutionPartitionSize(size_t partSizeLimit);

            /** Allocate arrays for channel dependent properties.
             *
             * @param nchannels number of channels in the convolution result.
             */
            status_t allocateConvolutionParameters(size_t nchannels);

            /** Destroy arrays for channel dependent properties.
             *
             */
            void destroyConvolutionParameters();

            /** Calculate convolution parameters from convolution operation input.
             *
             * @param data array of pointers to mono Sample objects, as many as the allocated number of channels for the result.
             * @param offset 0 time index for the time series
             */
            void calculateConvolutionParameters(Sample **data, size_t *offset);

            /** Allocate temporary arrays for convolution.
             *
             */
            status_t allocateConvolutionTempArrays();

            /** Destroy temporary arrays for convolution.
             *
             */
            void destroyConvolutionTempArrays();

            /** Allocate memory for the convolution result
             *
             * @param size_t sampleRate sample rate of the convolution result
             * @param size_t nchannels number of channels of the convolution result
             * @param size_t count number of samples to be allocated
             * @return status
             */
            status_t allocateConvolutionResult(size_t sampleRate, size_t nchannels, size_t count);

            /** Convolve a time series with the inverse filter
             *
             * @param data pointer to Sample object containing the time series
             * @param offset 0 time index for the time series
             * @param channel channel destination in the multichannel convolution result
             * @return status
             */
            status_t do_linear_convolution(Sample *data, size_t offset, size_t channel);

            /** Allocate memory for the nonlinear identification matrices
             *
             * @param size_t order order of the Hammerstein model
             * @param size_t windowSize size of the window to isolate the higher order responses
             * @return status
             */
            status_t allocateIdentificationMatrices(size_t order, size_t windowSize);

            /** Destroy the nonlinear identification matrices
             *
             */
            void destroyIdentificationMatrices();

            /** Convert from matrix subscript to allocated memory index for coefficients matrices
             *
             *  @param size_t r row subscript
             *  @param size_t c column subscript
             */
            inline size_t sub2ind_Coeffs(size_t r, size_t c);

            /** Convert from matrix subscript to allocated memory index for data matrices (Higher and Kernels)
             *
             *  @param size_t r row subscript
             *  @param size_t c column subscript
             */
            inline size_t sub2ind_Data(size_t r, size_t c);

            /** Fill matrices of coefficients with their values
             *
             */
            void fillCoefficientsMatrices();

            /** Solve identification problem
             *
             */
            void solve();

            /** Force DC Blocking in identified Hammerstein Kernels.
             *
             */
            void force_kernels_DC_block();

            /** Window Higher Order Responses
             *
             * @param channel channel in the convolution result
             * @param doInnerSmoothing if true, it will apply a fade in and fadeout to the the higher order response
             * @param nFadeIn number of samples for the inner fade in
             * @param nFadeOut number of samples for the inner fade out
             * @param windowType type of smoothing window to be applied to the whole of the higher order response vector
             */
            void windowHigherOrderResponses(size_t channel, bool doInnerSmoothing, size_t nFadeIn, size_t nFadeOut, windows::window_t windowType);

            /** Calculate a sample of a synchronized chirp wave
             *
             * @param size_t sampleRate sample rate of the chirp wave
             * @param size_t chirpIdx index (sample number)
             * @return sample number [chirpIdx] of the chirp wave
             */
            inline double calculate_chirp_sample(size_t sampleRate, size_t chirpIdx);

            /** Calculate a sample of a matched synchronized chirp inverse filter (uses sample from generating chirp)
             *
             * @param sampleRate sample rate of the generating chirp wave (same as sample rate of the inverse filter)
             * @param chirpValue value of the generating chirp wave sample
             * @param chirpIdx index (sample number) of the generating chirp wave sample
             * @return sample number [L - chirpIdx - 1] of the inverse filter, where L is the length of the generating chirp
             */
            inline double calculate_inverse_filter_sample(size_t sampleRate, double chirpValue, size_t chirpIdx);

            /** Calculate a sample of the fade-out fade-in window
             *
             * @param windowIdx index (sample number)
             * @return sample number [windowIdx] of the fade-out fade-in window
             */
            float calculate_fading_window_sample(size_t windowIdx);

            /** Profile Convolution Result Background Noise (by looking at the negative times of the Convolution Result)
             *
             * @param head sample of the convolution result at which start analysis [samples]
             * @param count number of samples to process [samples]
             * @param limit number of negative time samples to use in the assessment [samples]
             * @return status
             */
            status_t profile_background_noise(size_t channel, size_t head, size_t count);

            /** Calibrate backwards integration limit. This will calculate the limit for a backwards integration which starts
             *  at the supplied head parameter
             *
             * @param channel channel in the convolution result
             * @param head sample of the convolution result from which to start analysis [samples]
             * @oaram windowSize size of the window used for the envelope follower [samples]
             * @param tolerance level above background noise below which, even if convolution result peaks are found, the convolution result is considered faded into noise [dB]
             * @return status
             */
            status_t calibrate_backwards_integration_limit(size_t channel, size_t head, size_t windowSize, double tolerance);

            /** Calculate reverberation time of the positive time response by backward integration, relative to head.
             *
             * @param channel channel in the convolution result
             * @param head sample of the convolution result from which to start analysis [samples]
             * @param decayThreshold IR level threshold below which reverberation is considered complete [dB]
             * @param highRegLevel higher level of the limits at which regression line fitting is to be performed [dB]
             * @param lowRegLevel lower level of the limits at which regression line fitting is to be performed [dB]
             * @param limit limit sample up to which the squared impulse response is summed, with respect head [samples]
             * @return status
             */
            status_t calculate_reverberation_time(size_t channel, size_t head, double decayThreshold, double highRegLevel, double lowRegLevel, size_t limit);

            /** Calculate reverberation time of the positive time response by backward integration, relative to head.
             *
             * @param channel channel in the convolution result
             * @param head sample of the convolution result from which to start analysis [samples]
             * @param rtCalc reverberation time calculation method
             * @param limit integration limit [samples]
             * @return status
             */
            status_t calculate_reverberation_time(size_t channel, size_t head, scp_rtcalc_t rtCalc, size_t limit);

            /** Calculate the binomial coefficient n over k (n choose k)
             *
             * @param n top integer or set size
             * @param k bottom integer or subset size
             * @return value of the binomial coefficient given an and k
             */
            double nchoosek(size_t n, size_t k);

        public:

            /** Allocate and calculate time series
             *
             * @return status
             */
            status_t reconfigure();

            /** Do multiple convolutions, each for each channel in the convolution result
             *
             * @param data array of pointers to mono Sample objects, as many as the allocated number of channels for the result.
             * @param offset 0 time index for the time series
             * @param nchannels number of channels in the convolution result
             * @param partSizeLimit maximum size for convolution partision size
             */
            status_t do_linear_convolutions(Sample **data, size_t *offset, size_t nchannels, size_t partSizeLimit);

            /** Postprocess the Linear Convolution result
             *
             * @param channel channel in the convolution result
             * @param offset samples offset from the middle of the convolution result [samples]
             * @param rtCalc reverberation time calculation method
             * @oaram windowSize size of the window used for the envelope follower [s]
             * @param tolerance level above background noise below which, even if convolution result peaks are found, the convolution result is considered faded into noise [dB]
             *
             */
            status_t postprocess_linear_convolution(size_t channel, ssize_t offset, scp_rtcalc_t rtCalc, float windowSize, double tolerance);

            /** Postprocess the Nonlinear Convolution result
             *
             * @param channel channel in the convolution result
             * @param order order of the model
             * @param nFadeIn number of samples for the inner fade in
             * @param nFadeOut number of samples for the inner fade out
             * @param windowType type of smoothing window to be applied to the whole of the higher order response vector
             * @param nWindowRank rank of resulting FIR responses kernels (exponent of 2 defining their length: 2^nWindowRank)
             * @return status
             */
            status_t postprocess_nonlinear_convolution(size_t channel, size_t order, bool doInnerSmoothing, size_t nFadeIn, size_t nFadeOut, windows::window_t windowType, size_t nWindowRank);

            /** Save linear convolution result to file, any wanted interval
             *
             * @param path to file
             * @param head number of the first frame to be saved
             * @param count number of frames to be saved
             * @return status
             */
            status_t save_linear_convolution(const char *path, size_t head, size_t count);

            /** Save linear convolution result to file, any wanted interval, starting point specified
             * as offset from middle of convolution result.
             *
             * @param path to file
             * @param offset frames offset from the middle frame
             * @param count number of frames to be saved
             * @return status
             */
            status_t save_linear_convolution(const char *path, ssize_t offset, size_t count);

            /** Save linear convolution result to file, positive time only
             *
             * @param path to file
             * @param count number of frames to be saved
             * @return status
             */
            status_t save_linear_convolution(const char *path, size_t count);

            /** Save nonlinear convolution result to file
             *
             * @path path to file
             * @param offset frames offset from the middle frame (stored as a value only)
             * @return status
             */
            status_t save_to_lspc(const char *path, ssize_t offset = 0);

            /** Load convolution result and chirp parameters from lspc file
             *
             * @path path to file
             * @return status
             */
            status_t load_from_lspc(const char *path);

        public:

            /** Initialise SynchronizedChirp
             *
             */
            bool init();

            /** Destroy SynchronizedChirp
             *
             */
            void destroy();

            /** Check that SynchronizedChirp needs settings update
             *
             * @return true if SynchronizedChirp needs setting update
             */
            inline bool needs_update() const
            {
                return bSync;
            }

            /** Update SynchronizedChirp stateful settings
             *
             */
            void update_settings();

            /** Set sample rate for SynchronizedChirp
             *
             * @param sr sample rate
             */
            inline void set_sample_rate(size_t sr)
            {
                if (nSampleRate == sr)
                    return;

                nSampleRate                     = sr;
                sChirpParams.bRecalculate       = true;
                sChirpParams.bReconfigure       = true;
                bSync                           = true;
            }

            /** Set chirp synthesis method
             *
             * @param method synthesis method
             */
            inline void set_chirp_synthesis_method(scp_method_t method)
            {
                if ((method < SCP_SYNTH_SIMPLE) || (method >= SCP_SYNTH_MAX))
                    return;

                sChirpParams.enMethod           = method;
                sChirpParams.bReconfigure       = true;
            }

            /** Set chirp initial frequency
             *
             * @param initialFrequency initial frequency in Hertz
             */
            inline void set_chirp_initial_frequency(double initialFrequency)
            {
                if (sChirpParams.initialFrequency == initialFrequency)
                    return;

                sChirpParams.initialFrequency   = initialFrequency;
                sChirpParams.bRecalculate       = true;
                sChirpParams.bReconfigure       = true;
                bSync                           = true;
            }

            /** Set chirp final frequency
             *
             * @param finalFrequency final frequency in Hertz
             */
            inline void set_chirp_final_frequency(double finalFrequency)
            {
                if (sChirpParams.finalFrequency == finalFrequency)
                    return;

                sChirpParams.finalFrequency     = finalFrequency;
                sChirpParams.bRecalculate       = true;
                sChirpParams.bReconfigure       = true;
                bSync                           = true;
            }

            /** Set chirp duration in seconds
             *
             * @param duration chirp duration in seconds
             */
            inline void set_chirp_duration(float duration)
            {
                if ((sChirpParams.fDurationCoarse <= duration) && (sChirpParams.fDuration >= duration)) // Same coarse value => same optimized value
                    return;

                sChirpParams.fDuration          = duration;
                sChirpParams.bRecalculate       = true;
                sChirpParams.bReconfigure       = true;
                bSync                           = true;
            }

            /** Set chirp amplitude
             *
             * @param amplitude chirp amplitude
             */
            inline void set_chirp_amplitude(float amplitude)
            {
                if (sChirpParams.fAlpha == amplitude)
                    return;

                sChirpParams.fAlpha             = amplitude;
                sChirpParams.bReconfigure       = true;
                bSync                           = true;
            }

            /** Set fader fading method
             *
             * @param method fading method
             */
            inline void set_fader_fading_method(scp_fade_t method)
            {
                if ((method < SCP_FADE_NONE) || (method >= SCP_FADE_MAX))
                    return;

                sFader.enMethod                 = method;
                sChirpParams.bReconfigure       = true;
            }

            /** Set fader fade in time
             *
             * @param fadeIn fading in time [s]
             */
            inline void set_fader_fadein(float fadeIn)
            {
                if (sFader.fFadeIn == fadeIn)
                    return;

                sFader.fFadeIn                  = fadeIn;
                sChirpParams.bReconfigure       = true;
                bSync                           = true;
            }

            /** Set fader fade out time
             *
             * @param fadeOut fading out time [s]
             */
            inline void set_fader_fadeout(float fadeOut)
            {
                if (sFader.fFadeOut == fadeOut)
                    return;

                sFader.fFadeOut                 = fadeOut;
                sChirpParams.bReconfigure       = true;
                bSync                           = true;
            }

            /** Get chirp initial frequency
             *
             *  @return chirp initial frequency
             */
            inline double get_chirp_initial_frequency() const
            {
                return sChirpParams.initialFrequency;
            }

            /** Get chirp final frequency
             *
             * @return chirp final frequency
             */
            inline double get_chirp_final_frequency() const
            {
                return sChirpParams.finalFrequency;
            }

            /** Get chirp amplitude
             *
             * @return chirp amplitude
             */
            inline float get_chirp_alpha() const
            {
                return sChirpParams.fAlpha;
            }

            /** Get chirp gamma factor
             *
             * @return chirp gamma
             */
            inline double get_chirp_gamma() const
            {
                return sChirpParams.gamma;
            }

            /** Get chirp delta factor
             *
             * @return chirp delta
             */
            inline double get_chirp_delta() const
            {
                return sChirpParams.delta;
            }

            /** Get chirp duration
             *
             * @return chirp duration in seconds
             */
            inline float get_chirp_duration_seconds() const
            {
                return sChirpParams.fDuration;
            }

            /** Get integration limit time in seconds
             *
             */
            inline float get_integration_limit_seconds() const
            {
                return sCRPostProc.fIrLimit;
            }

            /** Return whether the background noise was optimal for the requested RT measurement
             *
             */
            inline bool get_background_noise_optimality() const
            {
                return sCRPostProc.bLowNoise;
            }

            /** Get reverberation time in samples
             *
             */
            inline size_t get_reverberation_time_samples() const
            {
                return sCRPostProc.nRT;
            }

            /** Get reverberation time in seconds
             *
             */
            inline float get_reverberation_time_seconds() const
            {
                return sCRPostProc.fRT;
            }

            /** Get reverberation regression line correlation coefficient
             *
             */
            inline float get_reverberation_correlation() const
            {
                return sCRPostProc.fCorrelation;
            }

            /** Get coefficients matrix real part
             *
             */
            inline float * get_coefficients_matrix_real_part() const
            {
                return sCRPostProc.mCoeffsRe;
            }

            /** Get coefficients matrix imaginary part
             *
             */
            inline float * get_coefficients_matrix_imaginary_part() const
            {
                return sCRPostProc.mCoeffsIm;
            }

            /** Get higher order responses matrix real part
             *
             */
            inline float * get_higher_matrix_real_part() const
            {
                return sCRPostProc.mHigherRe;
            }

            /** Get higher order responses matrix imaginary part
             *
             */
            inline float * get_higher_matrix_imaginary_part() const
            {
                return sCRPostProc.mHigherIm;
            }

            /** Get kernels matrix real part
             *
             */
            inline float * get_kernels_matrix_real_part() const
            {
                return sCRPostProc.mKernelsRe;
            }

            /** Get kernels matrix imaginary part
             *
             */
            inline float * get_kernels_matrix_imaginary_part() const
            {
                return sCRPostProc.mKernelsIm;
            }

            /** Fill a matrix with kernel FIR taps
             *
             * dst destination matrix. Must be size of order by windowSize.
             * return status
             */
            status_t fill_with_kernel_taps(float *dst);

            /** Get a single kernel FIR taps
             * dst destination buffer. Must be at least windowSize long.
             * order requested order.
             * return status
             */
            status_t get_kernel_fir(float *dst, size_t order);

            /** Get chirp
             *
             * @return pointer to chirp Sample object
             */
            inline Sample * get_chirp() const
            {
                return pChirp;
            }

            /** Get inverse filter
             *
             * @return pointer to inverse filter Sample object
             */
            inline Sample * get_inverse_filter() const
            {
                return pInverseFilter;
            }

            /** Get convolution result
             *
             * @return pointer to convolution result Sample object
             */
            inline AudioFile * get_convolution_result() const
            {
                return pConvResult;
            }

            /** Get convolution result positive time length in seconds
             *
             * @return length convolution result positive time length in seconds
             */
            inline float get_convolution_result_positive_time_length() const
            {
                size_t dataLength   = pConvResult->samples();
                size_t head         = (dataLength / 2) - 1;

                return samples_to_seconds(nSampleRate, dataLength - head);
            }

            /** Get convolution result samples for plots, arbitrary initial head
             *
             * @param channel channel in the convolution result
             * @param dst pointer to destination data
             * @param head sample of the convolution result from which to start collecting plot data
             * @param convLimit sample of the convolution result up to which the plot data are extracted, relative to head
             * @param plotCount requested samples for plot
             */
            void get_convolution_result_plottable_samples(size_t channel, float *dst, size_t head, size_t convLimit, size_t plotCount, bool normalize = true);

            /** Get convolution result samples for plots, arbitrary initial offset
             *
             * @param channel channel in the convolution result
             * @param dst pointer to destination data
             * @param offset samples offset from the middle of the convolution result from which to start collecting plot data
             * @param convLimit sample of the convolution result up to which the plot data are extracted, relative to head
             * @param plotCount requested samples for plot
             * @param normalize if true, normalise the plot samples with respect convolution result peak
             */
            void get_convolution_result_plottable_samples(size_t channel, float *dst, ssize_t offset, size_t convLimit, size_t plotCount, bool normalize = true);

            /** Get convolution result samples for plots, starting from middle of convolution result
             *
             * @param channel channel in the convolution result
             * @param dst pointer to destination data
             * @param convLimit sample of the convolution result up to which the plot data are extracted, relative to middle
             * @param plotCount requested samples for plot
             */
            void get_convolution_result_plottable_samples(size_t channel, float *dst, size_t convLimit, size_t plotCount, bool normalize = true);

            /** Set Oversampler mode
             *
             * @param mode oversampler mode
             */
            inline void set_oversampler_mode(over_mode_t mode)
            {
                if (mode == enOverMode)
                    return;

                enOverMode                      = mode;
                sChirpParams.bReconfigure       = true;
                bSync                           = true;
            }
    };
}

#endif /* CORE_UTIL_SYNCCHIRPPROCESSOR_H_ */
