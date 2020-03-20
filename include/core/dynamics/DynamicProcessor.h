/*
 * DynamicProcessor.h
 *
 *  Created on: 19 окт. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_DYNAMICS_DYNAMICPROCESSOR_H_
#define CORE_DYNAMICS_DYNAMICPROCESSOR_H_

#include <core/types.h>

namespace lsp
{
    #define DYNAMIC_PROCESSOR_DOTS      4
    #define DYNAMIC_PROCESSOR_RANGES    (DYNAMIC_PROCESSOR_DOTS + 1)
    
    typedef struct dyndot_t
    {
        float   fInput;         // Negative value means off
        float   fOutput;        // Negative value means off
        float   fKnee;          // Negative value means off
    } dyndot_t;

    class DynamicProcessor
    {
        private:
            DynamicProcessor & operator = (const DynamicProcessor &);

        protected:
            typedef struct spline_t
            {
                float       fPreRatio;      // Pre-knee ratio
                float       fPostRatio;     // Post-knee ratio
                float       fKneeStart;     // Start knee threshold
                float       fKneeStop;      // Stop knee threshold
                float       fThresh;        // Logarithmic threshold
                float       fMakeup;        // Makeup gain of the knee
                float       vHermite[4];    // Hermite interpolation
            } spline_t;

            typedef struct reaction_t
            {
                float       fLevel;
                float       fTau;
            } reaction_t;

            enum counters_t
            {
                CT_SPLINES,
                CT_ATTACK,
                CT_RELEASE,

                CT_TOTAL
            };

        protected:
            // Input parameters
            dyndot_t    vDots[DYNAMIC_PROCESSOR_DOTS];
            float       vAttackLvl[DYNAMIC_PROCESSOR_DOTS];
            float       vReleaseLvl[DYNAMIC_PROCESSOR_DOTS];
            float       vAttackTime[DYNAMIC_PROCESSOR_RANGES];
            float       vReleaseTime[DYNAMIC_PROCESSOR_RANGES];
            float       fInRatio;   // Input ratio
            float       fOutRatio;  // Output ratio

            // Processing parameters
            spline_t    vSplines[DYNAMIC_PROCESSOR_DOTS];
            reaction_t  vAttack[DYNAMIC_PROCESSOR_RANGES];
            reaction_t  vRelease[DYNAMIC_PROCESSOR_RANGES];
            uint8_t     fCount[CT_TOTAL];  // Number of elements for AttackLvl, ReleaseLvl, ... etc

            // Dynamic patameters
            float       fEnvelope;

            // Additional parameters
            size_t      nSampleRate;
            bool        bUpdate;

        protected:
//            static inline float     spline_curve(const spline_t *s, float x);
            static inline float     spline_amp(const spline_t *s, float x);
            static inline float     spline_model(const spline_t *s, float x);
            void                    sort_reactions(reaction_t *s, size_t count);
            void                    sort_splines(spline_t *s, size_t count);
            static inline float     solve_reaction(const reaction_t *s, float x, size_t count);

        public:
            explicit DynamicProcessor();
            ~DynamicProcessor();

        public:
            /** Check that some of processor's parameters have been modified
             * and we need to call update_settings();
             *
             * @return true if some of processor's parameters have been modified
             */
            inline bool modified() const
            {
                return bUpdate;
            }

            /** Update processor's settings
             *
             */
            void update_settings();

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

            /** Get input ratio
             *
             * @return input ratio
             */
            inline float get_in_ratio() const
            {
                return fInRatio;
            }

            /** Set input ratio
             *
             * @param ratio input ratio
             */
            inline void set_in_ratio(float ratio)
            {
                if (fInRatio == ratio)
                    return;
                fInRatio = ratio;
                bUpdate = true;
            }

            /** Get output ratio
             *
             * @return output ratio
             */
            inline float get_out_ratio() const
            {
                return fOutRatio;
            }

            /** Set output ratio
             *
             * @param ratio output ratio
             */
            inline void set_out_ratio(float ratio)
            {
                if (fOutRatio == ratio)
                    return;
                fOutRatio = ratio;
                bUpdate = true;
            }

            /** Get dot configuration
             *
             * @param id identifier of dot
             * @param dst pointer to store data
             * @return status of operation
             */
            inline bool get_dot(size_t id, dyndot_t *dst) const
            {
                if ((id >= DYNAMIC_PROCESSOR_DOTS) || (dst == NULL))
                    return false;
                *dst    = vDots[id];
                return true;
            }

            /** Set dot configuration
             *
             * @param id identifier of dot
             * @param src new configuration pointer or NULL
             * @return status of operation
             */
            bool set_dot(size_t id, const dyndot_t *src);

            /** Set dot configuration
             *
             * @param id identifier of dot
             * @param in input level
             * @param out output level
             * @param knee knee size
             * @return status of operation
             */
            bool set_dot(size_t id, float in, float out, float knee);

            /** Get attack level
             *
             * @param id split level
             * @return attack level
             */
            inline float get_attack_level(size_t id) const
            {
                return (id >= DYNAMIC_PROCESSOR_DOTS) ? -1.0f : vAttackLvl[id];
            }

            /** Set attack level
             *
             * @param id split level
             * @param value level value
             */
            inline void set_attack_level(size_t id, float value)
            {
                if ((id >= DYNAMIC_PROCESSOR_DOTS) || (vAttackLvl[id] == value))
                    return;
                vAttackLvl[id] = value;
                bUpdate = true;
            }

            /** Get release level
             *
             * @param id split level
             * @return attack level
             */
            inline float get_release_level(size_t id) const
            {
                return (id >= DYNAMIC_PROCESSOR_DOTS) ? -1.0f : vReleaseLvl[id];
            }

            /** Set release level
             *
             * @param id split level
             * @param value level value
             */
            inline void set_release_level(size_t id, float value)
            {
                if ((id >= DYNAMIC_PROCESSOR_DOTS) || (vReleaseLvl[id] == value))
                    return;
                vReleaseLvl[id] = value;
                bUpdate = true;
            }

            /** Get attack time of the specified range
             *
             * @param id identifier of the range
             * @return attack time
             */
            inline float get_attack_time(size_t id) const
            {
                return (id >= DYNAMIC_PROCESSOR_RANGES) ? -1.0f : vAttackTime[id];
            }

            /** Set attack time
             *
             * @param id identifier of the range
             * @param value attack time value
             */
            inline void set_attack_time(size_t id, float value)
            {
                if ((id >= DYNAMIC_PROCESSOR_RANGES) || (vAttackTime[id] == value))
                    return;
                vAttackTime[id] = value;
                bUpdate = true;
            }

            /** Get release time of the specified range
             *
             * @param id identifier of the range
             * @return release time
             */
            inline float get_release_time(size_t id) const
            {
                return (id >= DYNAMIC_PROCESSOR_RANGES) ? -1.0f : vReleaseTime[id];
            }

            /** Set release time
             *
             * @param id identifier of the range
             * @param value attack time value
             */
            inline void set_release_time(size_t id, float value)
            {
                if ((id >= DYNAMIC_PROCESSOR_RANGES) || (vReleaseTime[id] == value))
                    return;
                vReleaseTime[id] = value;
                bUpdate = true;
            }

            /** Process sidechain signal
             *
             * @param out output signal gain to VCA
             * @param env envelope signal of processor
             * @param in sidechain signal
             * @param samples number of samples to process
             */
            void process(float *out, float *env, const float *in, size_t samples);

            /** Process one sample of sidechain signal
             *
             * @param in sidechain signal
             * @param out envelope signal of processor, may be NULL
             * @return output signal gain to VCA
             */
            float process(float *env, float in);

            /** Get dynamic curve
             *
             * @param out output compression value
             * @param in input compression value
             * @param dots number of input dots
             */
            void curve(float *out, const float *in, size_t dots);

            /** Get dynamic curve point
             *
             * @param in input level
             */
            float curve(float in);

            /** Get dynamic curve model
             *
             * @param out output compression value
             * @param in input compression value
             * @param dots number of input dots
             */
            void model(float *out, const float *in, size_t dots);

            /** Get dynamic curve point
             *
             * @param in input level
             */
            float model(float in);

            /** Get dynamic gain reduction
             *
             * @param out output signal
             * @param in input signal
             * @param dots number of dots
             */
            void reduction(float *out, const float *in, size_t dots);

            /** Get dynamic gain reduction
             *
             * @param in input level
             */
            float reduction(float in);
    };

} /* namespace lsp */

#endif /* CORE_DYNAMICS_DYNAMICPROCESSOR_H_ */
