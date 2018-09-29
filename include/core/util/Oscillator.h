/*
 * Oscillator.h
 *
 *  Created on: 20 Mar 2017
 *      Author: crocoduck
 */

#ifndef CORE_UTIL_OSCILLATOR_H_
#define CORE_UTIL_OSCILLATOR_H_

#include <core/types.h>
#include <math.h>
#include <core/util/Oversampler.h>

namespace lsp
{
    enum fg_function_t
    {
        FG_SINE,            // Pure Math waves          //0
        FG_COSINE,                                      //1
        FG_SQUARED_SINE,                                //2
        FG_SQUARED_COSINE,                              //3
        FG_RECTANGULAR,                                 //4
        FG_SAWTOOTH,                                    //5
        FG_TRAPEZOID,                                   //6
        FG_PULSETRAIN,                                  //7
        FG_PARABOLIC,                                   //8
        FG_BL_RECTANGULAR,  // Band limited waves       //9
        FG_BL_SAWTOOTH,                                 //10
        FG_BL_TRAPEZOID,                                //11
        FG_BL_PULSETRAIN,                               //12
        FG_BL_PARABOLIC,                                //13
        FG_MAX
    };

    enum dc_reference_t
    {
        DC_WAVEDC,                                      // DC Offset with respect wave's natural DC (0 fDCOffset <=> Wave DC)
        DC_ZERO,                                        // DC Offset with respect 0 DC (0 fDCOffset <=> 0 Overall DC)
        DC_MAX
    };

    class Oscillator
    {
        protected:
            typedef uint32_t phacc_t;

            typedef struct squared_sinusoid_t
            {
                bool            bInvert;                // If true, invert the sign (phase) of the wave.
                float           fAmplitude;             // Signed amplitude of the wave.
                float           fWaveDC;                // Natural DC value of the wave.
            } squared_sinusoud_t;

            typedef struct rectangular_t
            {
                float           fDutyRatio;             // Fraction of the period over which the wave is positive
                phacc_t         nDutyWord;              // Word expressing the phase interval in which the rectangular wave is positive.
                float           fWaveDC;                // DC value of the wave.
                float           fBLPeakAtten;           // Value of attenuation to bring peak of band limited wave to 1.0f.
            } rectangular_t;

            typedef struct sawtooth_t
            {
                float           fWidth;                 // Fraction of the period at which the tooth peaks
                phacc_t         nWidthWord;             // Word expressing the phase value in which the sawtooth wave peaks.
                float           fCoeffs[4];             // Coefficients of the sawtooth waves lines - ascending: [0] * p + [1]; descending: [2] * p + [3]; with p value of the phase accumulator.
                float           fWaveDC;                // Natural DC value of the wave.
                float           fBLPeakAtten;           // Value of attenuation to bring peak of band limited wave to 1.0f.
            } sawtooth_t;

            typedef struct trapezoid_t
            {
                float           fRaiseRatio;            // Fraction of half period at which the wave ramps up.
                float           fFallRatio;             // Fraction of half period at which the wave ramps down.
                phacc_t         nPoints[4];             // Points of the vertices of the trapezoids along the period, as phase accumulator words.
                float           fCoeffs[4];             // Coefficients of the trapezoid waves lines - ascending: [0] * p; descending: [1] * p + [2]; ascending [1] * p + [3]; with p value of the phase accumulator.
                float           fWaveDC;                // Natural DC value of the wave.
                float           fBLPeakAtten;           // Value of attenuation to bring peak of band limited wave to 1.0f.
            } trapezoid_t;

            typedef struct pulse_t
            {
                float           fPosWidthRatio;         // Fraction of half period in which the positive pulse is active.
                float           fNegWidthRatio;         // Fraction of half period in which the negative pulse is active.
                phacc_t         nTrainPoints[3];        // Points of the vertices of the pulses along the period.
                float           fWaveDC;                // Natural DC value of the wave.
                float           fBLPeakAtten;           // Value of attenuation to bring peak of band limited wave to 1.0f.
            } pulse_t;

            typedef struct parabolic_t
            {
                bool            bInvert;                // If true, invert the sign (phase) of the wave.
                float           fAmplitude;
                float           fWidth;                 // For parabolic waves, fraction of the period in which the parabola is contained.
                phacc_t         nWidthWord;             // The above expressed as a phase accumulator word
                float           fWaveDC;                // Natural DC value of the wave.
                float           fBLPeakAtten;           // Value of attenuation to bring peak of band limited wave to 1.0f.
            } parabolic_t;

        private:
            fg_function_t       enFunction;             // Function for the oscillator.
            float               fAmplitude;             // Amplitude of the oscillator. [ Gain ]
            float               fFrequency;             // Oscillator frequency. [Hz]
            float               fDCOffset;              // DC offset [ Gain ]
            dc_reference_t      enDCReference;          // Select DC Offset reference. If true, wave DC will be shifted to 0.
            float               fReferencedDC;          // DC offset with reference to the specified value.
            float               fInitPhase;             // Additional phase factor. [rad]

            size_t              nSampleRate;            // Sample Rate. [Hz]
            phacc_t             nPhaseAcc;              // Phase accumulator variable.
            uint8_t             nPhaseAccBits;          // Number of bits in the phase accumulator.
            uint8_t             nPhaseAccMaxBits;       // Maximum number of bits available for the phase accumulator.
            phacc_t             nPhaseAccMask;          // Bit mask for the phase accumulator.
            float               fAcc2Phase;             // Factor converting from phase accumulator values to [rad] phase values.

            phacc_t             nFreqCtrlWord;          // Frequency control word for the phase accumulator
            phacc_t             nInitPhaseWord;         // Word expressing the initial phase. Depends upon fInitPhase.

            squared_sinusoid_t  sSquaredSinusoid;
            rectangular_t       sRectangular;
            sawtooth_t          sSawtooth;
            trapezoid_t         sTrapezoid;
            pulse_t             sPulse;
            parabolic_t         sParabolic;

            float              *vProcessBuffer;         // Buffers
            float              *vSynthBuffer;
            uint8_t            *pData;

            Oversampler         sOver;                  // Oversampler for Band Limited synthesis.
            Oversampler         sOverGetPeriods;        // Oversampler for get_periods method.
            size_t              nOversampling;          // Hold oversampling factor. This assumes all the oversamples used here will have the same factor.
            over_mode_t         enOverMode;             // Oversampler mode.
            phacc_t             nFreqCtrlWord_Over;     // Frequency control word for the oversampled phase accumulator.

            bool                bSync;                  // Flag that indicates that generator needs update

        public:
            Oscillator();
            ~Oscillator();

        public:

            /** Initialise Oscillator
             *
             */
            bool init();

            /** Destroy Oscillator
             *
             */
            void destroy();

            /** Check that generator needs settings update
             *
             * @return true if generator needs settings update
             */
            inline bool needs_update() const
            {
                return bSync;
            }

            /** This method should be called if needs_update() returns true
             * before calling process() methods
             *
             */
            void update_settings();

            /** Set number of bits of the phase accumulator
             *
             * @param nBits number of bits
             */
            inline void set_phase_accumulator_bits(uint8_t nBits)
            {
                if ((nPhaseAccBits == nBits) || (nBits > nPhaseAccMaxBits)) // It's just my preference to use brackets
                    return;

                nPhaseAccBits   = nBits;
                nPhaseAcc       = 0;
                bSync           = true;
            }

            /** Set sample rate for the function generator
             *
             * @param sr sample rate
             */
            inline void set_sample_rate(size_t sr)
            {
                if (nSampleRate == sr)
                    return;

                nSampleRate = sr;
                nPhaseAcc   = 0;
                bSync       = true;
            }

            /** Reset the samples counter
             *
             */
            inline void reset_phase_accumulator() { nPhaseAcc = 0; }

            /** Set the function of the oscillator:
             *
             * @param function output function of the generator
             */
            inline void set_function(fg_function_t function)
            {
                if ((function < FG_SINE) || (function >= FG_MAX))
                    return;

                enFunction  = function;
                bSync       = true;
            }

            /** Set the frequency of the oscillator:
             *
             * @param frequency of the oscilator. [Hz]
             */
            inline void set_frequency(float frequency)
            {
                if (fFrequency == frequency)
                    return;

                fFrequency      = frequency;
                bSync           = true;
            }

            /** Get the exact current frequency of the oscillator:
             *
             */
            inline float get_exact_frequency() const
            {
                return nSampleRate * nFreqCtrlWord * (1.0f / (nPhaseAccMask + 1.0f));
            }

            /** Set the phase factor of the oscillator:
             *
             *  @ param phase phase factor of the oscillator [rad].
             */
            inline void set_phase(float phase)
            {

                if (fInitPhase == phase)
                    return;

                fInitPhase = phase;
                bSync = true;
            }

            /** Get the exact current phase factor:
             *
             */
            inline float get_exact_phase() const
            {
                double x = (nPhaseAccMask + 1.0);
                return round(x * fInitPhase * 0.5f * M_1_PI) * (2.0 * M_PI / x);
            }

            /** Set the DC offset
             *
             * @param dcOffset DC offset of the wave
             */
            inline void set_dc_offset(float dcOffset)
            {
                if (fDCOffset == dcOffset)
                    return;

                fDCOffset = dcOffset;
            }

            /** Set the DC Reference
             *
             * @param dcReference
             */
            inline void set_dc_reference(dc_reference_t dcReference)
            {
                if ((dcReference < DC_WAVEDC) || (dcReference >= DC_MAX))
                    return;

                enDCReference   = dcReference;
                bSync           = true;
            }

            /** Set inversion value for squared sinusoids
             *
             * @param invert boolean expressing if inverted
             */
            inline void set_squared_sinusoid_inversion(bool invert)
            {
                if (sSquaredSinusoid.bInvert == invert)
                    return;

                sSquaredSinusoid.bInvert = invert;
                bSync = true;
            }

            inline void set_parabolic_inversion(bool invert)
            {
                if (sParabolic.bInvert == invert)
                    return;

                sParabolic.bInvert = invert;
                bSync = true;
            }

            /** Set the duty ratio for rectangular waves
             *
             * @param dutyRatio duty ratio of the rectangular wave.
             */
            inline void set_duty_ratio(float dutyRatio)
            {
                if ((sRectangular.fDutyRatio == dutyRatio) || (dutyRatio < 0.0f) || (dutyRatio > 1.0f))
                    return;

                sRectangular.fDutyRatio = dutyRatio;
                bSync = true;
            }

            /** Set the width for sawtooth waves
             *
             * @ param width width of sawtooth wave
             */
            inline void set_width(float width)
            {
                if (width < 0.0f)
                    width = 0.0f;
                else if (width > 1.0f)
                    width = 1.0f;
                if (sSawtooth.fWidth == width)
                    return;

                sSawtooth.fWidth = width;
                bSync = true;
            }

            /** Set raise and fall ratios for the trapezoid wave
             *
             * @ param raise raise ratio of the trapezoid
             * @ param fall fall ratio of the trepezoid
             */
            inline void set_trapezoid_ratios(float raise, float fall)
            {
                if (raise < 0.0f)
                    raise = 0.0f;
                else if (raise > 1.0f)
                    raise = 1.0f;

                if (fall < 0.0f)
                    fall = 0.0f;
                else if (fall > 1 - raise)
                    fall = 1 - raise;

                if ((raise == sTrapezoid.fRaiseRatio) && (fall == sTrapezoid.fFallRatio))
                    return;

                sTrapezoid.fRaiseRatio      = raise;
                sTrapezoid.fFallRatio       = fall;
                bSync                       = true;

            }

            /** Set width ratios for pulse train
             *
             * @param posWidthRatio ratio of the positive amplitude pulse
             * @param negWidthRatio ratio of the negative amplitude pulse
             */
            inline void set_pulsetrain_ratios(float posWidthRatio, float negWidthRatio)
            {
                if (posWidthRatio < 0.0f)
                    posWidthRatio = 0.0f;
                else if (posWidthRatio > 1.0f)
                    posWidthRatio = 1.0f;

                if (negWidthRatio < 0.0f)
                    negWidthRatio = 0.0f;
                else if (negWidthRatio > 1.0f)
                    negWidthRatio = 1.0f;

                if ((posWidthRatio == sPulse.fPosWidthRatio) && (negWidthRatio == sPulse.fNegWidthRatio))
                    return;

                sPulse.fPosWidthRatio   = posWidthRatio;
                sPulse.fNegWidthRatio   = negWidthRatio;
                bSync                   = true;

            }

            /** Set parabolic wave width
             *
             * @param width width of parabolic wave
             */
            inline void set_parabolic_width(float width)
            {
                if (width < 0.0f)
                    width = 0.0f;
                else if (width > 1.0f)
                    width = 1.0f;

                if (sParabolic.fWidth == width)
                    return;

                sParabolic.fWidth   = width;
                bSync               = true;
            }

            /** Set Oversampler mode
             *
             * @param mode oversampler mode
             */
            inline void set_oversampler_mode(over_mode_t mode)
            {
                if (mode == enOverMode)
                    return;

                enOverMode = mode;
                bSync = true;
            }

            /** Set the amplitude of the oscillator:
             *
             * @param amplitude amplitude of the oscillator. [ ]
             */
            inline void set_amplitude(float amplitude)
            {
                if (fAmplitude == amplitude)
                    return;

                fAmplitude = amplitude;
                bSync = true;
            }

            /** Return a given number of periods of the output waves.
             *
             * @param dst output wave destination
             * @param periods number of periods in the wave
             * @param periodsSkip number or initial periods to skip
             * @param samples number of samples in the wave
             */
            void get_periods(float *dst, size_t periods, size_t periodsSkip, size_t samples);

            /** Output wave to the destination buffer in
             * additive mode
             *
             * @param dst output wave destination
             * @param src input source, allowed to be NULL
             * @param count number of samples to synthesise
             */
            void process_add(float *dst, const float *src, size_t count);

            /** Output wave to the destination buffer in
             * multiplicative mode
             *
             * @param dst output wave destination
             * @param src input source, allowed to be NULL
             * @param count number of samples to process
             */
            void process_mul(float *dst, const float *src, size_t count);

            /** Output wave to a destination buffer overwriting its content
             *
             * @param dst output wave destination
             * @param src input source, allowed to be NULLL
             * @param count number of samples to process
             */
            void process_overwrite(float *dst, size_t count);

        protected:

            /** Synthesize the required wave and write its sample to internal
             * buffer.
             *
             * @param os oversampler to use for the processing
             * @param count number of samples to process
             */
            void do_process(Oversampler *os, float * dst, size_t count);

    };

}

#endif /* CORE_UTIL_OSCILLATOR_H_ */
