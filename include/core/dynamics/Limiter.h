/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 25 нояб. 2016 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CORE_DYNAMICS_LIMITER_H_
#define CORE_DYNAMICS_LIMITER_H_

#include <core/types.h>
#include <core/IStateDumper.h>
#include <core/util/Delay.h>

#define LIMITER_PATCHES_MAX         256
#define LIMITER_PEAKS_MAX           32
#define LIMITER_LOG_PATCHES_MAX     128

namespace lsp
{
    enum limiter_mode_t
    {
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
        LM_LINE_DUCK
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
                UP_ALR      = 1 << 5,

                UP_ALL      = UP_SR | UP_LK | UP_MODE | UP_OTHER | UP_THRESH | UP_ALR
            };

            typedef struct alr_t
            {
                float       fKS;            // Knee start
                float       fKE;            // Knee end
                float       fGain;          // Maximum output gain
                float       fTauAttack;     // Attiack time constant
                float       fTauRelease;    // Release time constant
                float       vHermite[3];    // Hermite approximation
                float       fAttack;        // Attack
                float       fRelease;       // Release
                float       fEnvelope;      // Envelope
                bool        bEnable;        // Enable ALR
            } alr_t;

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

        protected:
            float       fThreshold;
            float       fReqThreshold;
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
            alr_t       sALR;

            // Pre-calculated parameters
            float      *vGainBuf;
            float      *vTmpBuf;                // Temporary buffer to store the actual sidechain value
            uint8_t    *vData;

            Delay       sDelay;
            union
            {
                sat_t       sSat;               // Hermite mode
                exp_t       sExp;               // Exponent mode
                line_t      sLine;              // Line mode
            };

        protected:
            inline float    sat(ssize_t n);
            inline float    exp(ssize_t n);
            inline float    line(ssize_t n);
            static void     apply_sat_patch(sat_t *sat, float *dst, float amp);
            static void     apply_exp_patch(exp_t *exp, float *dst, float amp);
            static void     apply_line_patch(line_t *line, float *dst, float amp);

            static void     reset_sat(sat_t *sat);
            static void     reset_exp(exp_t *exp);
            static void     reset_line(line_t *line);

            void            init_sat(sat_t *sat);
            void            init_exp(exp_t *exp);
            void            init_line(line_t *line);

            void            process_alr(float *gbuf, const float *sc, size_t samples);

            static void     dump(IStateDumper *v, const char *name, const sat_t *sat);
            static void     dump(IStateDumper *v, const char *name, const exp_t *exp);
            static void     dump(IStateDumper *v, const char *name, const line_t *line);

        public:
            explicit Limiter();
            ~Limiter();

            /**
             * Construct the object
             */
            void construct();

            /** Destroy all data allocated by processor
             *
             */
            void destroy();

        public:
            /** Initialize limiter
             *
             * @param max_sr maximum sample rate that can be passed to limiter
             * @param max_lookahead maximum look-ahead time that can be passed to limiter [ms]
             * @return true on success
             */
            bool init(size_t max_sr, float max_lookahead);

            /** Check if limiter is modifier
             *
             * @return true if limiter settings need to be updated
             */
            inline bool modified() const                            { return nUpdate != 0;          }

            /** Update settings for limiter
             *
             */
            void update_settings();

            /**
             * Get limiter mode
             * @return limiter mode
             */
            inline limiter_mode_t get_mode() const                  { return limiter_mode_t(nMode); }

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

            /** Get threshold
             *
             * @return threshold
             */
            inline float        get_threshold() const               { return fReqThreshold;     }

            /** Set limiter threshold
             *
             * @param thresh limiter threshold
             * @param immediate immediately set threshold, no need to perform automated gain lowering
             */
            float               set_threshold(float thresh, bool immediate);

            /**
             * Get attack time
             * @return attack time
             */
            inline float        get_attack() const                  { return fAttack;           }

            /** Set attack time
             *
             * @param attack attack time
             */
            float               set_attack(float attack);

            /**
             * Get release time
             * @return attack time
             */
            inline float        get_release() const                 { return fRelease;          }

            /** Set release time
             *
             * @param release release time
             */
            float               set_release(float release);

            /**
             * Get lookahead time
             * @return lookahead time
             */
            inline float        get_lookahead() const               { return fLookahead;        }

            /** Set look-ahead time
             *
             * @param lk_ahead look-ahead time
             */
            float               set_lookahead(float lk_ahead);

            /**
             * Get maximum possible latency according to the configuration
             * @return maximum possible latency
             */
            inline size_t       max_latency() const                 { return nMaxLookahead;     }

            /**
             * Get knee of the limiter
             * @return knee of the limiter
             */
            inline float        get_knee() const                    { return fKnee;             }

            /** Set knee, the value not greater than 1.0
             * If value is 1.0, there is no knee at all
             * Other values indicate gain offset below the threshold.
             * The same offset above threshold will be automatically calculated.
             *
             * @param knee knee, 1.0 means no knee
             */
            float               set_knee(float knee);

            /** Get latency of limiter
             *
             * @return limiter's latency
             */
            inline size_t       get_latency() const                 { return nLookahead;        }

            /**
             * Get automatic level regulation attack
             * @return automatic level regulation attack
             */
            inline float        get_alr_attack() const              { return sALR.fAttack;      }

            /**
             * Set automatic level regulation attack
             * @param attack attack value
             * @return previous value
             */
            float               set_alr_attack(float attack);

            /**
             * Get automatic level regulation release
             * @return automatic level regulation attack
             */
            inline float        get_alr_release() const             { return sALR.fRelease;     }

            /**
             * Set automatic level regulation release
             * @param attack attack value
             * @return previous value
             */
            float               set_alr_release(float attack);

            /**
             * Check that automatic level regulation is turned on
             * @return true if automatic level regulation is turned on
             */
            inline bool         get_alr() const                     { return sALR.bEnable;      }

            /** Enable automatic level regulation
             *
             * @param enable enable flag
             * @return previous value
             */
            bool                set_alr(bool enable);

            /** Process data by limiter
             *
             * @param dst destination buffer with applied delay
             * @param gain output gain for VCA
             * @param src input signal buffer
             * @param sc sidechain input signal
             * @param samples number of samples to process
             */
            void                process(float *dst, float *gain, const float *src, const float *sc, size_t samples);

            /**
             * Dump internal state
             * @param v state dumper
             */
            void                dump(IStateDumper *v) const;
    };

} /* namespace lsp */

#endif /* CORE_DYNAMICS_LIMITER_H_ */
