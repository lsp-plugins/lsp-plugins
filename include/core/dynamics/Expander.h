/*
 * Expander.h
 *
 *  Created on: 2 нояб. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_DYNAMICS_EXPANDER_H_
#define CORE_DYNAMICS_EXPANDER_H_

#include <core/types.h>

namespace lsp
{
    enum expander_mode_t
    {
        EM_DOWNWARD,
        EM_UPWARD
    };
    
    class Expander
    {
        private:
            Expander & operator = (const Expander &);

        protected:
            // Basic parameters
            float       fAttackThresh;
            float       fReleaseThresh;
            float       fAttack;
            float       fRelease;
            float       fKnee;
            float       fRatio;
            float       fEnvelope;

            // Pre-calculated parameters
            float       fTauAttack;
            float       fTauRelease;
            float       vHermite[3];    // Knee hermite interpolation
            float       fLogKS;         // Knee start
            float       fLogKE;         // Knee end
            float       fLogTH;         // Logarithmic threshold

            // Additional parameters
            size_t      nSampleRate;
            bool        bUpdate;
            bool        bUpward;

        public:
            explicit Expander();
            ~Expander();

        public:
            /** Check that some of parameters have been modified
             * and we need to call update_settings();
             *
             * @return true if some of parameters have been modified
             */
            inline bool modified() const
            {
                return bUpdate;
            }

            inline bool is_upward() const
            {
                return bUpward;
            }

            inline bool is_downward() const
            {
                return !bUpward;
            }

            /** Update expander settings
             *
             */
            void update_settings();

            /** Set threshold
             *
             * @param attack the attack threshold
             * @param release the release threshold (relative to attack, must be positive, less or equal to 1.0)
             */
            inline void set_threshold(float attack, float release)
            {
                if ((fAttackThresh == attack) && (fReleaseThresh == release))
                    return;
                fAttackThresh       = attack;
                fReleaseThresh      = release;
                bUpdate             = true;
            }

            /** Set timings
             *
             * @param attack attack time (ms)
             * @param release release time (ms)
             */
            inline void set_timings(float attack, float release)
            {
                if ((fAttack == attack) && (fRelease == release))
                    return;
                fAttack     = attack;
                fRelease    = release;
                bUpdate     = true;
            }

            /** Set attack time
             *
             * @param attack attack time (ms)
             */
            inline void set_attack(float attack)
            {
                if (fAttack == attack)
                    return;
                fAttack     = attack;
                bUpdate     = true;
            }

            /** Set release time
             *
             * @param release release time (ms)
             */
            inline void set_release(float release)
            {
                if (fRelease == release)
                    return;
                fRelease    = release;
                bUpdate     = true;
            }

            /** Set sample rate
             *
             * @param sr sample rate
             */
            inline void set_sample_rate(size_t sr)
            {
                if (sr == nSampleRate)
                    return;
                nSampleRate = sr;
                bUpdate     = true;
            }

            /** Set knee
             *
             * @param knee (in gain units)
             */
            inline void set_knee(float knee)
            {
                if (knee == fKnee)
                    return;
                fKnee       = knee;
                bUpdate     = true;
            }

            /** Set ratio
             *
             * @param ratio expansion ratio
             */
            inline void set_ratio(float ratio)
            {
                if (ratio == fRatio)
                    return;
                bUpdate     = true;
                fRatio      = ratio;
            }

            /** Set expander mode: upward/downward
             *
             * @param mode expander mode
             */
            inline void set_mode(size_t mode)
            {
                bool upward = (mode == EM_UPWARD);
                if (upward == bUpward)
                    return;

                bUpward     = upward;
                bUpdate     = true;
            }

            /** Process sidechain signal
             *
             * @param out output signal gain to VCA
             * @param env envelope signal of expander
             * @param in sidechain signal
             * @param samples number of samples to process
             */
            void process(float *out, float *env, const float *in, size_t samples);

            /** Process one sample of sidechain signal
             *
             * @param s sidechain signal
             * @param out envelope signal of expander, may be NULL
             * @return output signal gain to VCA
             */
            float process(float *env, float s);

            /** Get expansion curve
             *
             * @param out output expansion value
             * @param in input expansion value
             * @param dots number of input dots
             */
            void curve(float *out, const float *in, size_t dots);

            /** Get expansion curve point
             *
             * @param in input level
             */
            float curve(float in);

            /** Get gain amplification
             *
             * @param out output signal
             * @param in input signal
             * @param dots number of dots
             */
            void amplification(float *out, const float *in, size_t dots);

            /** Get gain amplification
             *
             * @param in input level
             */
            float amplification(float in);
    };

} /* namespace lsp */

#endif /* CORE_DYNAMICS_EXPANDER_H_ */
