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
                ST_FADE,    // Fade is currently in progress, not reached fade out threshold
                ST_OPENED,  // Fade is opened, listening for signal fall-off
                ST_WAIT     // Wait until signal falls below the threshold
            };

        protected:
            typedef struct fade_t
            {
                depopper_mode_t enMode;         // Fade mode
                float           fThresh;        // Threshold
                float           fTime;          // Fade time
                float           fDelay;         // Fade delay
                ssize_t         nSamples;       // Fade length in samples
                ssize_t         nDelay;         // Protection delay
                float           fPoly[4];       // Fade polynom
            } fade_t;

        protected:
            size_t          nSampleRate;        // Sample rate
            state_t         nState;             // Fade state

            float           fLookMax;           // Maximum lookahead value
            ssize_t         nLookMin;           // Minimum lookahead offset
            ssize_t         nLookMax;           // Maximum lookahead offset
            ssize_t         nLookOff;           // Current lookahead offset
            ssize_t         nLookCount;         // Number of lookahead samples

            // Signal envelope computing
            float           fRmsMax;            // Maximum permitted RMS period
            float           fRmsLength;         // RMS estimation period
            ssize_t         nRmsMin;            // Minimum rms offset
            ssize_t         nRmsMax;            // Maximum rms offset
            ssize_t         nRmsOff;            // Current rms offset
            ssize_t         nRmsLen;            // Number of rms samples
            float           fRmsNorm;           // Norming coefficient

            // Computed parameters
            ssize_t         nCounter;           // Fade-in counter
            ssize_t         nDelay;             // Crossing delay
            float           fRms;               // Rms value

            // Fade data
            fade_t          sFadeIn;
            fade_t          sFadeOut;

            // Buffers
            float          *pGainBuf;           // Gain buffer
            float          *pRmsBuf;            // Rms estimation buffer
            uint8_t        *pData;

            // Reconfiguration
            bool            bReconfigure;       // Reconfiguration flag

        protected:
            float           calc_rms(float s);
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
             * @param max_fade maximum possible fade out in milliseconds
             * @param max_rms maximum RMS estimation time in milliseconds
             * @return true on success
             */
            bool            init(size_t srate, float max_fade, float max_rms);

            /**
             * Check whether the module needs reconfiguration
             * @return true if the module needs reconfiguration
             */
            inline bool     needs_reconfiguration() const       { return bReconfigure;          }

            /**
             * Perform the reconfiguration
             */
            void            reconfigure();

            /**
             * Get fade in time (in milliseconds)
             * @return fade in time (in milliseconds)
             */
            inline float    get_fade_in_time() const            { return sFadeIn.fTime;         }

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
            inline float    get_fade_out_time() const           { return sFadeOut.fTime;        }

            /**
             * Set fade out time (in milliseconds)
             * @param time fade out time (in milliseconds)
             * @return previously used fade time
             */
            float           set_fade_out_time(float time);

            /**
             * Get fade in transition delay
             * @return fade in delay
             */
            inline float    get_fade_in_delay() const           { return sFadeIn.fDelay;        }

            /**
             * Set fade in transition delay
             * @param delay delay
             * @return previous value
             */
            float           set_fade_in_delay(float delay);

            /**
             * Get fade out transition delay
             * @return fade out delay
             */
            inline float    get_fade_out_delay() const          { return sFadeOut.fDelay;       }

            /**
             * Set fade out transition delay
             * @param delay delay
             * @return previous value
             */
            float           set_fade_out_delay(float delay);

            /**
             * Get fade in threshold
             * @return fade in threshold
             */
            inline float    get_fade_in_threshold() const       { return sFadeIn.fThresh;       }

            /**
             * Set fade in threshold
             * @param thresh fade in threshold
             */
            float           set_fade_in_threshold(float thresh);

            /**
             * Get fade out threshold
             * @return fade out threshold
             */
            inline float    get_fade_out_threshold() const      { return sFadeOut.fThresh;      }

            /**
             * Set fade out threshold
             * @param thresh fade out threshold
             */
            float           set_fade_out_threshold(float thresh);

            /**
             * Get RMS estimation time length
             * @return RMS estimation time length
             */
            inline float    rms_length() const                  { return fRmsLength;            }

            /**
             * Set RMS estimation time length
             * @param length RMS estimation time length
             * @return previous value
             */
            float           set_rms_length(float length);

            /**
             * Get fade in mode
             * @return fade in mode
             */
            depopper_mode_t get_fade_in_mode() const            { return sFadeIn.enMode;        }

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
            depopper_mode_t get_fade_out_mode() const           { return sFadeOut.enMode;       }

            /**
             * Set fade out mode
             * @param mode fade out mode
             * @return previous mode
             */
            depopper_mode_t set_fade_out_mode(depopper_mode_t mode);

            /**
             * Set release time in milliseconds
             * @param release release time in milliseconds
             * @return previous release time
             */
            float           set_release(float release);

            /**
             * Process the signal, the gain should be applied respecite to the returned latency
             * @param env envelope output
             * @param gain output gain of the depopper that should be applied to the signal
             * @param src source buffer to read the signal
             * @param count number of samples to process
             */
            void            process(float *env, float *gain, const float *src, size_t count);

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
