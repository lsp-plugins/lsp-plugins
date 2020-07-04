/*
 * Depopper.h
 *
 *  Created on: 4 июл. 2020 г.
 *      Author: sadko
 */

#ifndef CORE_UTIL_DEPOPPER_H_
#define CORE_UTIL_DEPOPPER_H_

#include <core/types.h>

namespace lsp
{
    
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
                ST_CLOSED,  // Depopper is closed, listening for income signal raise
                ST_OPENED,  // Depopper is opened, listening for signal fall-off
                ST_FADING   // Depopper is currently fading in
            };

        protected:
            size_t          nSampleRate;
            float           fFadeTime;
            float           fThreshold;
            float           fAttack;
            float           fRelease;
            float           fEnvelope;
            float           fClip;
            float           fClipStart;
            float           fClipInc;
            state_t         nState;
            bool            bReconfigure;

            // Computed parameters
            size_t          nFadeSamples;
            float           fTauAttack;
            float           fTauRelease;

        public:
            explicit        Depopper();
            virtual        ~Depopper();

        public:
            /**
             * Initialize depopper
             */
            void            init();

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
             * Set sample rate
             * @param sr sample rate
             * @return previous sample rate
             */
            size_t          set_sample_rate(size_t sr);

            /**
             * Get sample rate
             * @return sample rate
             */
            inline size_t   get_sample_rate() const         { return nSampleRate;       }

            /**
             * Get fade time (in milliseconds)
             * @return fade time (in milliseconds)
             */
            inline float    get_fade_time() const           { return fFadeTime;         }

            /**
             * Set fade time (in milliseconds)
             * @param time fade time (in milliseconds)
             * @return previously used fade time
             */
            float           set_fade_time(float time);

            /**
             * Get threshold
             * @return threshold
             */
            inline float    get_threshold() const           { return fThreshold;        }

            /**
             * Set threshold
             * @param thresh threshold
             */
            float           set_threshold(float thresh);

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
             * Check that depopper is currently closed
             * @return true if depopper is currently closed
             */
            inline bool     is_closed() const               { return nState == ST_CLOSED; }

            /**
             * Check that depopper is currently opened
             * @return true if depopper is currently opened
             */
            inline bool     is_opened() const               { return nState == ST_OPENED; }

            /**
             * Check that depopper is currently fading
             * @return true if depopper is currently fading
             */
            inline bool     is_fading() const               { return nState == ST_FADING; }

            /**
             * Process the signal
             * @param gain output gain of the depopper that should be applied to the signal
             * @param src source buffer to read the signal
             * @param count number of samples to process
             */
            void            process(float *gain, const float *src, size_t count);

    };

} /* namespace lsp */

#endif /* CORE_UTIL_DEPOPPER_H_ */
