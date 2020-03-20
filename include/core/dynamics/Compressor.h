/*
 * Compressor.h
 *
 *  Created on: 16 сент. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_DYNAMICS_COMPRESSOR_H_
#define CORE_DYNAMICS_COMPRESSOR_H_

#include <core/types.h>

namespace lsp
{
    enum compressor_mode_t
    {
        CM_DOWNWARD,
        CM_UPWARD
    };

    /** Compressor class implementation
     *
     */
    class Compressor
    {
        private:
            Compressor & operator = (const Compressor &);

        protected:
            // Basic parameters
            float       fAttackThresh;
            float       fReleaseThresh;
            float       fBoostThresh;
            float       fAttack;
            float       fRelease;
            float       fKnee;
            float       fRatio;
            float       fEnvelope;

            // Pre-calculated parameters
            float       fTauAttack;
            float       fTauRelease;
            float       fXRatio;        // Compression ratio

            float       fLogTH;         // Logarithmic threshold
            float       fKS;            // Knee start
            float       fKE;            // Knee end
            float       vHermite[3];    // Knee hermite interpolation

            float       fBLogTH;        // Logarithmic boost threshold
            float       fBKS;           // Boost knee start
            float       fBKE;           // Boost knee end
            float       vBHermite[3];   // Boost hermite interpolation
            float       fBoost;         // Overall gain boost

            // Additional parameters
            size_t      nSampleRate;
            bool        bUpward;
            bool        bUpdate;

        public:
            explicit Compressor();
            ~Compressor();

        public:
            /** Check that some of compressor's parameters have been modified
             * and we need to call update_settings();
             *
             * @return true if some of compressor's parameters have been modified
             */
            inline bool modified() const
            {
                return bUpdate;
            }

            /** Update compressor's settings
             *
             */
            void update_settings();

            /** Set compressor threshold
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

            /**
             * Set boost threshold, valid for upward compression only
             * @param boost boost threshold
             */
            inline void set_boost_threshold(float boost)
            {
                if (fBoostThresh == boost)
                    return;
                fBoostThresh        = boost;
                bUpdate             = true;
            }

            /** Set compressor timings
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

            /** Set compressor's sample rate
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

            /** Set compressor's knee
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

            /** Set compression ratio
             *
             * @param ratio compression ratio
             */
            inline void set_ratio(float ratio)
            {
                if (ratio == fRatio)
                    return;
                bUpdate     = true;
                fRatio      = ratio;
            }

            /** Set compression mode: upward/downward
             *
             * @param mode compression mode
             */
            inline void set_mode(size_t mode)
            {
                bool upward = (mode == CM_UPWARD);
                if (upward == bUpward)
                    return;

                bUpward     = upward;
                bUpdate     = true;
            }

            /** Process sidechain signal
             *
             * @param out output signal gain to VCA
             * @param env envelope signal of compressor
             * @param in sidechain signal
             * @param samples number of samples to process
             */
            void process(float *out, float *env, const float *in, size_t samples);

            /** Process one sample of sidechain signal
             *
             * @param in sidechain signal
             * @param out envelope signal of compressor, may be NULL
             * @return output signal gain to VCA
             */
            float process(float *env, float in);

            /** Get compression curve
             *
             * @param out output compression value
             * @param in input compression value
             * @param dots number of input dots
             */
            void curve(float *out, const float *in, size_t dots);

            /** Get compression curve point
             *
             * @param in input level
             */
            float curve(float in);

            /** Get compressor gain reduction
             *
             * @param out output signal
             * @param in input signal
             * @param dots number of dots
             */
            void reduction(float *out, const float *in, size_t dots);

            /** Get compressor gain reduction
             *
             * @param in input level
             */
            float reduction(float in);
    };

} /* namespace lsp */

#endif /* CORE_DYNAMICS_COMPRESSOR_H_ */
