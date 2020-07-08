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
    enum depopper_mode_t
    {
        DPM_LINEAR,
        DPM_CUBIC,
        DPM_RMS
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
            size_t          nClipCounter;
            state_t         nState;
            bool            bReconfigure;
            depopper_mode_t enMode;

            // Computed parameters
            float           fTauAttack;
            float           fTauRelease;
            size_t          nFade;
            float           fPoly[4];

        protected:
            float           crossfade();

        public:
            explicit        Depopper();
            virtual        ~Depopper();

        public:
            /**
             * Initialize depopper
             */
            void            construct();

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
             * Get depopper mode
             * @return depopper mode
             */
            depopper_mode_t get_mode() const                { return enMode;            }

            /**
             * Set depopper mode
             * @param mode depopper mode
             * @return previous mode
             */
            depopper_mode_t set_mode(depopper_mode_t mode);

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
