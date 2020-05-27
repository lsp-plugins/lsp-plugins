/*
 * Sweeper.h
 *
 *  Created on: 10 May 2020
 *      Author: crocoduck
 */

#ifndef CORE_UTIL_SWEEPER_H_
#define CORE_UTIL_SWEEPER_H_

#include <core/types.h>

namespace lsp
{

    class SweepGenerator
    {
        protected:

            typedef struct phase_acc_t
            {
                size_t  nAccumulator;
                size_t  nMaxBits;
                size_t  nBits;
                size_t  nMask;
                size_t  nCtrl;
                float   fMultiplier;
            } phase_acc_t;

        private:
        SweepGenerator & operator = (const SweepGenerator &);

        private:
            bool bSync;

            size_t nSampleRate;

            phase_acc_t sPhaseAcc;

            float fSweepDuration;
            float fSweepPeak;

        public:
            explicit SweepGenerator();
            ~SweepGenerator();

        public:

            /* Initialise SweepGenerator.
             *
             */
            void init();

            /* Destroy SweepGenerator
             *
             */
            void destroy();

            /** Reset the SweepGenerator
             *
             */
            inline void reset_sweep()
            {
                sPhaseAcc.nAccumulator = 0;
            }

            /** Check that SweepGenerator needs settings update
             *
             * @return true if SweepGenerator needs setting update
             */
            inline bool needs_update() const
            {
                return bSync;
            }

            /** Update SweepGenerator stateful settings
             *
             */
            void update_settings();

            /** Set sample rate for the LatencyDetector
             *
             * @param sr sample rate
             */
            inline void set_sample_rate(size_t sr)
            {
                if (nSampleRate == sr)
                    return;

                nSampleRate = sr;
                bSync = true;
            }

            /** Set the number of bits in the phase accumulator
             *
             * @param nBits number of bits
             */
            inline void set_bits(size_t nBits)
            {
                if (nBits == sPhaseAcc.nBits)
                    return;

                sPhaseAcc.nBits = nBits;
                bSync = true;
            }

            /** Set the SweepGenerator sweep duration
             *
             * @param duration sweep duration in seconds
             */
            inline void set_sweep_duration(float duration)
            {
                if (duration == fSweepDuration)
                    return;

                fSweepDuration = duration;
                bSync = true;
            }

            /** Get the SweepGenerator sweep duration
             *
             */
            inline float get_sweep_duration() const
            {
                return fSweepDuration;
            }

            /** Get the SweepGenerator sweep length
             *
             */
            inline size_t get_sweep_length() const
            {
                return seconds_to_samples(nSampleRate, fSweepDuration);
            }

            /** Set the SweepGenerator peak
             *
             * @param peak sweep peak value
             */
            inline void set_sweep_peak(float peak)
            {
                if (peak == fSweepPeak)
                    return;

                fSweepPeak = peak;
                bSync = true;
            }

            /** Get the SweepGenerator peak
             *
             */
            inline float get_sweep_peak() const
            {
                return fSweepPeak;
            }

            /** Sweep samples in a destination buffer
             *
             * @param dst output buffer destination
             * @param count number of samples to process
             */
            void sweep(float *dst, size_t count);
    };
}

#endif /* CORE_UTIL_SWEEPER_H_ */
