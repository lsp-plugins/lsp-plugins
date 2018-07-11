/*
 * Analyzer.h
 *
 *  Created on: 14 авг. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_ANALYZER_H_
#define CORE_ANALYZER_H_

#include <core/types.h>
#include <core/envelope.h>
#include <core/windows.h>

namespace lsp
{
    class Analyzer
    {
        protected:
            enum reconfigure_flags
            {
                R_ENVELOPE  = 1<<0,
                R_WINDOW    = 1<<1,
                R_ANALYSIS  = 1<<2,
                R_TAU       = 1<<3,
                R_COUNTERS  = 1<<4,

                R_ALL       = R_ENVELOPE | R_WINDOW | R_ANALYSIS | R_TAU | R_COUNTERS
            };

            typedef struct channel_t
            {
                float      *vBuffer;        // FFT buffer
                float      *vAmp;           // FFT amplitude
                size_t      nCounter;       // FFT trigger counter
                bool        bFreeze;        // Freeze analysis
                bool        bActive;        // Enable analysis
            } channel_t;

        protected:
            size_t      nChannels;
            size_t      nMaxRank;
            size_t      nRank;
            size_t      nSampleRate;
            size_t      nBufSize;
            size_t      nFftPeriod;
            float       fReactivity;
            float       fTau;
            float       fRate;
            float       fShift;
            size_t      nReconfigure;
            size_t      nEnvelope;
            size_t      nWindow;
            bool        bActive;

            channel_t  *vChannels;          // List of channels
            float      *vSigRe;             // Real part of signal
            float      *vSigIm;             // Imaginary part of signal (always zero)
            float      *vFftRe;             // Buffer for FFT transform (real part)
            float      *vFftIm;             // Buffer for FFT transfor (imaginary part)
            float      *vWindow;            // FFT window
            float      *vEnvelope;          // FFT envelope

        public:
            Analyzer();
            ~Analyzer();

        public:
            /** Initialize analyzer
             *
             * @param channels number of channels for analysis
             * @param max_rank maximum FFT rank
             * @return status of operation
             */
            bool init(size_t channels, size_t max_rank);

            /** Destroy analyzer
             *
             */
            void destroy();

            /** Set window for analysis
             *
             * @param window window
             */
            inline void set_window(size_t window)
            {
                if (nWindow != window)
                {
                    nWindow         = window;
                    nReconfigure   |= R_WINDOW;
                }
            }

            /** Set envelope for analysis
             *
             * @param envelope envelope type
             */
            inline void set_envelope(size_t envelope)
            {
                if (nEnvelope != envelope)
                {
                    nEnvelope       = envelope;
                    nReconfigure   |= R_ENVELOPE;
                }
            }

            /** Set shift gain for analysis
             *
             * @param envelope envelope type
             */
            inline void set_shift(float shift)
            {
                if (fShift != shift)
                {
                    fShift          = shift;
                    nReconfigure   |= R_ENVELOPE;
                }
            }

            /** Set sample rate for analysis
             *
             * @param sr sample rate
             */
            inline void set_sample_rate(size_t sr)
            {
                if (nSampleRate != sr)
                {
                    nSampleRate     = sr;
                    nReconfigure   |= R_ALL;
                }
            }

            /** Set-up FFT analysis rate
             *
             * @param rate FFT rate
             */
            inline void set_rate(float rate)
            {
                if (fRate != rate)
                {
                    fRate           = rate;
                    nReconfigure   |= R_COUNTERS;
                }
            }

            /** Set-up FFT reactivity
             *
             * @param reactivity reactivity (msec)
             */
            inline void set_reactivity(float reactivity)
            {
                if (fReactivity != reactivity)
                {
                    fReactivity     = reactivity;
                    nReconfigure   |= R_TAU;
                }
            }

            /** Set rank of the analysis
             *
             * @param rank analysis rank
             * @return analysis rank
             */
            inline bool set_rank(size_t rank)
            {
                if ((rank < 2) || (rank > nMaxRank))
                    return false;
                nRank           = rank;
                nReconfigure   |= R_ALL;
                return true;
            }

            /** Set analyzer activity
             *
             * @param active activity flag
             */
            inline void set_activity(bool active)
            {
                bActive         = active;
            }

            /** Freeze channel
             *
             * @param channel channel to freeze
             * @param freeze freeze flag
             * @return status of operation
             */
            inline bool freeze_channel(size_t channel, bool freeze)
            {
                if (channel >= nChannels)
                    return false;
                vChannels[channel].bFreeze      = freeze;
                return true;
            }

            /** Enable channel
             *
             * @param channel channel to enable
             * @param enable enable flag
             * @return status of operation
             */
            inline bool enable_channel(size_t channel, bool enable)
            {
                if (channel >= nChannels)
                    return false;
                vChannels[channel].bActive      = enable;
                return true;
            }

            inline void reset()
            {
                nReconfigure       |= R_ANALYSIS;
            }

            /** Process data
             *
             * @param channel ID of input channel
             * @param in data for processing
             * @param samples number of samples to process
             */
            void process(size_t channel, const float *in, size_t samples);

            /** Read spectrum data
             *
             * @param channel channel
             * @param out output buffer
             * @param idx array of frequency numbers
             * @param count size of input and output arrays
             */
            bool get_spectrum(size_t channel, float *out, const uint32_t *idx, size_t count);

            /** Get list of frequencies
             *
             * @param f frequency list
             * @param idx frequency indexes
             * @param start start frequency
             * @param stop stop frequency
             * @param count number of elements
             */
            void get_frequencies(float *frq, uint32_t *idx, float start, float stop, size_t count);

            /** Reconfigure analyzer
             *
             */
            void reconfigure();

            /** Check that analyzer needs reconfiguration
             *
             * @return true if needs reconfiguration
             */
            inline bool needs_reconfiguration() const
            {
                return nReconfigure;
            }
    };

} /* namespace lsp */

#endif /* CORE_ANALYZER_H_ */
