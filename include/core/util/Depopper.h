/*
 * Depopper.h
 *
 *  Created on: 4 июл. 2020 г.
 *      Author: sadko
 */

#ifndef CORE_UTIL_DEPOPPER_H_
#define CORE_UTIL_DEPOPPER_H_

#include <core/types.h>
#include <core/IStateDumper.h>

namespace lsp
{
    enum depopper_mode_t
    {
        DPM_LINEAR,
        DPM_CUBIC,
        DPM_SINE,
        DPM_GAUSSIAN,
        DPM_PARABOLIC
    };
    
    /**
     * This module is designed to prevent loud popping sounds
     * when the signal level raises dramatically
     */
    class Depopper
    {
        private:
            Depopper & operator = (const Depopper &);

        protected:
            enum state_t
            {
                ST_CLOSED,  // Fade is closed, listening for income signal raise
                ST_FADE1,   // Fade is currently in progress, not reached fade out threshold
                ST_FADE2,   // Fade is currently in progress, reached fade out threshold
                ST_OPENED,  // Fade is opened, listening for signal fall-off
                ST_WAIT     // Wait until signal falls below the threshold
            };

        protected:
            typedef struct fade_t
            {
                depopper_mode_t enMode;         // Fade mode
                float           fThresh;        // Threshold
                float           fTime;          // Fade time
                ssize_t         nSamples;       // Fade length in samples
                float           fPoly[4];       // Fade polynom
            } fade_t;

        protected:
            size_t          nSampleRate;
            state_t         nState;             // Fade state
            float           fMaxLookahead;
            size_t          nLookahead;
            float           fAttack;
            float           fRelease;
            float           fEnvelope;
            bool            bReconfigure;

            // Computed parameters
            float           fTauAttack;
            float           fTauRelease;
            ssize_t         nCounter;           // Fade-in counter

            fade_t          sFadeIn;
            fade_t          sFadeOut;

            float          *pGainBuf;           // Gain buffer
            uint8_t        *pData;

        protected:
            float           crossfade(fade_t *fade, float x);
            void            calc_fade(fade_t *fade, bool in);
            void            apply_fadeout(float *dst, ssize_t samples);

            static void     dump_fade(IStateDumper *v, const char *name, const fade_t *fade);


        public:
            explicit        Depopper();
            virtual        ~Depopper();

            /**
             * Initialize depopper
             */
            void            construct();

            /**
             * Destroy depopper
             */
            void            destroy();

        public:
            /**
             * Initialize
             * @param srate sample rate
             * @param max_lookahead maximum lookahead in milliseconds
             * @return true on success
             */
            bool            init(size_t srate, float max_lookahead);

            /**
             * Check whether the module needs reconfiguration
             * @return true if the module needs reconfiguration
             */
            inline bool     needs_reconfiguration() const   { return bReconfigure;    }

            /**
             * Perform the reconfiguration
             */
            void            reconfigure();

            /**
             * Get fade in time (in milliseconds)
             * @return fade in time (in milliseconds)
             */
            inline float    get_fade_in_time() const        { return sFadeIn.fTime;         }

            /**
             * Set fade in time (in milliseconds)
             * @param time fade in time (in milliseconds)
             * @return previously used fade time
             */
            float           set_fade_in_time(float time);

            /**
             * Get fade out time (in milliseconds)
             * @return fade out time (in milliseconds)
             */
            inline float    get_fade_out_time() const        { return sFadeOut.fTime;       }

            /**
             * Set fade out time (in milliseconds)
             * @param time fade out time (in milliseconds)
             * @return previously used fade time
             */
            float           set_fade_out_time(float time);

            /**
             * Get fade in threshold
             * @return fade in threshold
             */
            inline float    get_fade_in_threshold() const       { return sFadeIn.fThresh;        }

            /**
             * Set fade in threshold
             * @param thresh fade in threshold
             */
            float           set_fade_in_threshold(float thresh);

            /**
             * Get fade out threshold
             * @return fade out threshold
             */
            inline float    get_fade_out_threshold() const       { return sFadeOut.fThresh;        }

            /**
             * Set fade out threshold
             * @param thresh fade out threshold
             */
            float           set_fade_out_threshold(float thresh);

            /**
             * Get attack time in milliseconds
             * @return attack time in milliseconds
             */
            inline float    attack() const                  { return fAttack;           }

            /**
             * Set attack time in milliseconds
             * @param attack attack time in milliseconds
             * @return previous attack time
             */
            float           set_attack(float attack);

            /**
             * Get fade in mode
             * @return fade in mode
             */
            depopper_mode_t get_fade_in_mode() const        { return sFadeIn.enMode;    }

            /**
             * Set fade in mode
             * @param mode fade in mode
             * @return previous mode
             */
            depopper_mode_t set_fade_in_mode(depopper_mode_t mode);

            /**
             * Get fade out mode
             * @return fade in mode
             */
            depopper_mode_t get_fade_out_mode() const       { return sFadeOut.enMode;    }

            /**
             * Set fade out mode
             * @param mode fade out mode
             * @return previous mode
             */
            depopper_mode_t set_fade_out_mode(depopper_mode_t mode);

            /**
             * Set release time in milliseconds
             * @return release time in milliseconds
             */
            inline float    release() const                 { return fRelease;          }

            /**
             * Set release time in milliseconds
             * @param release release time in milliseconds
             * @return previous release time
             */
            float           set_release(float release);

            /**
             * Process the signal, the gain should be applied respecite to the returned latency
             * @param gain output gain of the depopper that should be applied to the signal
             * @param src source buffer to read the signal
             * @param count number of samples to process
             */
            void            process(float *gain, const float *src, size_t count);

            /**
             * Dump internal state
             * @param v state dumper
             */
            void            dump(IStateDumper *v) const;

            /**
             * Get latency
             * @return latency
             */
            inline size_t   latency() const                 { return sFadeOut.nSamples; }
    };

} /* namespace lsp */

#endif /* CORE_UTIL_DEPOPPER_H_ */
