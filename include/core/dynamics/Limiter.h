/*
 * Limiter.h
 *
 *  Created on: 25 нояб. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_DYNAMICS_LIMITER_H_
#define CORE_DYNAMICS_LIMITER_H_

#include <core/types.h>
#include <core/util/Delay.h>

#define LIMITER_PATCHES_MAX         256
#define LIMITER_PEAKS_MAX           32
#define LIMITER_LOG_PATCHES_MAX     128

namespace lsp
{
    enum limiter_mode_t
    {
        LM_COMPRESSOR,

        LM_HERM_THIN,
        LM_HERM_WIDE,
        LM_HERM_TAIL,
        LM_HERM_DUCK,

        LM_EXP_THIN,
        LM_EXP_WIDE,
        LM_EXP_TAIL,
        LM_EXP_DUCK,

        LM_LINE_THIN,
        LM_LINE_WIDE,
        LM_LINE_TAIL,
        LM_LINE_DUCK,

        LM_MIXED_HERM,
        LM_MIXED_EXP,
        LM_MIXED_LINE
    };
    
    class Limiter
    {
        private:
            Limiter & operator = (const Limiter &);

        protected:
            typedef void (*apply_patch_t)(void *patch, float *dst, float amp);

            enum update_t
            {
                UP_SR       = 1 << 0,
                UP_LK       = 1 << 1,
                UP_MODE     = 1 << 2,
                UP_OTHER    = 1 << 3,
                UP_THRESH   = 1 << 4,

                UP_ALL      = UP_SR | UP_LK | UP_MODE | UP_OTHER | UP_THRESH
            };

            typedef struct comp_t
            {
                float       fKS;            // Knee start
                float       fKE;            // Knee end
                float       fTauAttack;     // Attack time constant
                float       fTauRelease;    // Release time constant
                float       fEnvelope;      // Envelope
                float       fAmp;           // Amplification coefficient
                ssize_t     nCountdown;     // Countdown
                float       fSample;        // Last triggered sample
                float       vHermite[3];    // Knee hermite interpolation
            } comp_t;

            #pragma pack(push, 1)
            typedef struct peak_t
            {
                int32_t     nTime;
                float       fValue;
            } peak_t;
            #pragma pack(pop)

            typedef struct sat_t
            {
                int32_t     nAttack;        // Attack
                int32_t     nPlane;         // Plane
                int32_t     nRelease;       // Release
                int32_t     nMiddle;        // Middle

                float       vAttack[4];     // Hermite interpolation of attack
                float       vRelease[4];    // Hermite interpolation of release
            } sat_t;

            typedef struct exp_t
            {
                int32_t     nAttack;        // Attack
                int32_t     nPlane;         // Plane
                int32_t     nRelease;       // Release
                int32_t     nMiddle;        // Middle

                float       vAttack[4];     // Exponent interpolation of attack
                float       vRelease[4];    // Exponent interpolation of release
            } exp_t;

            typedef struct line_t
            {
                int32_t     nAttack;        // Attack
                int32_t     nPlane;         // Plane
                int32_t     nRelease;       // Release
                int32_t     nMiddle;        // Middle

                float       vAttack[2];     // Line interpolation of attack
                float       vRelease[2];    // Line interpolation of release
            } line_t;

            typedef struct mixed_t
            {
                comp_t      sComp;
                union
                {
                    sat_t       sSat;               // Hermite mode
                    exp_t       sExp;               // Exponent mode
                    line_t      sLine;              // Line mode
                };
            } mixed_t;

        protected:
            float       fThreshold;
            float       fLookahead;
            float       fMaxLookahead;
            float       fAttack;
            float       fRelease;
            float       fKnee;
            size_t      nMaxLookahead;
            size_t      nLookahead;
            size_t      nMaxSampleRate;
            size_t      nSampleRate;
            size_t      nUpdate;
            size_t      nMode;
            size_t      nThresh;

            // Pre-calculated parameters
            float      *vGainBuf;
            float      *vTmpBuf;
            uint8_t    *vData;

            Delay       sDelay;
            union
            {
                comp_t      sComp;              // Compressor mode
                sat_t       sSat;               // Hermite mode
                exp_t       sExp;               // Exponent mode
                line_t      sLine;              // Line mode
                mixed_t     sMixed;             // Mixed mode
            };

        protected:
            inline float    reduction(comp_t *comp);
            inline float    sat(ssize_t n);
            inline float    exp(ssize_t n);
            inline float    line(ssize_t n);
            static void     apply_sat_patch(sat_t *sat, float *dst, float amp);
            static void     apply_exp_patch(exp_t *exp, float *dst, float amp);
            static void     apply_line_patch(line_t *line, float *dst, float amp);

            static void     reset_sat(sat_t *sat);
            static void     reset_exp(exp_t *exp);
            static void     reset_line(line_t *line);
            static void     reset_comp(comp_t *comp);

            void            init_sat(sat_t *sat);
            void            init_exp(exp_t *exp);
            void            init_line(line_t *line);
            void            init_comp(comp_t *comp);

            void            process_compressor(float *dst, float *gain, const float *src, const float *sc, size_t samples);
//            void            process_hermite(float *dst, float *gain, const float *src, const float *sc, size_t samples);
//            void            process_exp(float *dst, float *gain, const float *src, const float *sc, size_t samples);
//            void            process_line(float *dst, float *gain, const float *src, const float *sc, size_t samples);

            void            process_patch(float *dst, float *gain, const float *src, const float *sc, size_t samples);
            void            process_mixed(float *dst, float *gain, const float *src, const float *sc, size_t samples);

        public:
            explicit Limiter();
            ~Limiter();

        public:
            /** Initialize limiter
             *
             * @param max_sr maximum sample rate that can be passed to limiter
             * @param max_lookahead maximum look-ahead time that can be passed to limiter [ms]
             * @return true on success
             */
            bool init(size_t max_sr, float max_lookahead);

            /** Destroy all data allocated by processor
             *
             */
            void destroy();

            /** Check if limiter is modifier
             *
             * @return true if limiter settings need to be updated
             */
            inline bool modified() const
            {
                return nUpdate != 0;
            }

            /** Update settings for limiter
             *
             */
            void update_settings();

            /** Set limiter working mode
             *
             * @param mode limiter working mode
             */
            inline void set_mode(limiter_mode_t mode)
            {
                if (mode == nMode)
                    return;
                nMode = mode;
                nUpdate |= UP_MODE;
            }

            /** Change current sample rate of processor
             *
             * @param sr sample rate to set
             */
            inline void set_sample_rate(size_t sr)
            {
                if (sr == nSampleRate)
                    return;
                nSampleRate     = sr;
                nUpdate        |= UP_SR;
            }

            /** Set limiter threshold
             *
             * @param thresh limiter threshold
             */
            inline void set_threshold(float thresh)
            {
                if (fThreshold == thresh)
                    return;
                fThreshold  = thresh;
                nUpdate    |= UP_THRESH;
            }

            /** Get threshold
             *
             * @return threshold
             */
            inline float get_threshold() const
            {
                return fThreshold;
            }

            /** Set attack time
             *
             * @param attack attack time
             */
            inline void set_attack(float attack)
            {
                if (fAttack == attack)
                    return;
                fAttack     = attack;
                nUpdate    |= UP_OTHER;
            }

            /** Set release time
             *
             * @param release release time
             */
            inline void set_release(float release)
            {
                if (fRelease == release)
                    return;
                fRelease    = release;
                nUpdate    |= UP_OTHER;
            }

            /** Set look-ahead time
             *
             * @param lk_ahead look-ahead time
             */
            inline void set_lookahead(float lk_ahead)
            {
                if (lk_ahead > fMaxLookahead)
                    lk_ahead = fMaxLookahead;
                if (fLookahead == lk_ahead)
                    return;
                fLookahead  = lk_ahead;
                nUpdate    |= UP_LK;
            }

            /** Set knee, the value not greater than 1.0
             * If value is 1.0, there is no knee at all
             * Other values indicate gain offset below the threshold.
             * The same offset above threshold will be automatically calculated.
             *
             * @param knee knee, 1.0 means no knee
             */
            inline void set_knee(float knee)
            {
                if (knee > 1.0f)
                    knee = 1.0f;
                if (fKnee == knee)
                    return;
                fKnee       = knee;
                nUpdate    |= UP_OTHER;
            }

            /** Get latency of limiter
             *
             * @return limiter's latency
             */
            inline size_t get_latency() const
            {
                return nLookahead;
            }

            /** Process data by limiter
             *
             * @param dst destination buffer with applied delay
             * @param gain output gain for VCA
             * @param src input signal buffer
             * @param sc sidechain input signal
             * @param samples number of samples to process
             */
            void process(float *dst, float *gain, const float *src, const float *sc, size_t samples);

    };

} /* namespace lsp */

#endif /* CORE_DYNAMICS_LIMITER_H_ */
