/*
 * Analyzer.h
 *
 *  Created on: 14 авг. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_UTIL_ANALYZER_H_
#define CORE_UTIL_ANALYZER_H_

#include <core/types.h>
#include <core/envelope.h>
#include <core/windows.h>

namespace lsp
{
    enum freq_analyzer_flags_t
    {
        // Frequency list flags
        FRQA_SCALE_LOGARITHMIC  = 0x0000,
        FRQA_SCALE_LINEAR       = 0x0001,
        FRQA_SCALE_MASK         = 0x000f,

        // Function
        FRQA_FUNC_NEAREST       = 0x0000,
        FRQA_FUNC_MAX           = 0x0010,
        FRQA_FUNC_MIN           = 0x0020,
        FRQA_FUNC_AVG           = 0x0030,
        FRQA_FUNC_MASK          = 0x00f0,

        // Interpolation
        FRQA_INT_NONE           = 0x0000,
        FRQA_INT_LINEAR         = 0x0100,
        FRQA_INT_CUBIC          = 0x0200,
        FRQA_INT_MASK           = 0x0300
    };

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
                ssize_t     nCounter;       // FFT trigger counter
                bool        bFreeze;        // Freeze analysis
                bool        bActive;        // Enable analysis
            } channel_t;

        protected:
            size_t      nChannels;
            size_t      nMaxRank;
            size_t      nRank;
            size_t      nSampleRate;
            size_t      nBufSize;
            ssize_t     nFftPeriod;
            float       fReactivity;
            float       fTau;
            float       fRate;
            float       fShift;
            size_t      nReconfigure;
            size_t      nEnvelope;
            size_t      nWindow;
            bool        bActive;

            channel_t  *vChannels;          // List of channels
            void       *vData;              // Allocated floating-point data
            float      *vSigRe;             // Real part of signal
            float      *vFftReIm;           // Buffer for FFT transform (real part)
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
                if (nWindow == window)
                    return;

                nWindow         = window;
                nReconfigure   |= R_WINDOW;
            }

            /** Set envelope for analysis
             *
             * @param envelope envelope type
             */
            inline void set_envelope(size_t envelope)
            {
                if (nEnvelope == envelope)
                    return;

                nEnvelope       = envelope;
                nReconfigure   |= R_ENVELOPE;
            }

            /** Set shift gain for analysis
             *
             * @param envelope envelope type
             */
            inline void set_shift(float shift)
            {
                if (fShift == shift)
                    return;

                fShift          = shift;
                nReconfigure   |= R_ENVELOPE;
            }

            /** Set sample rate for analysis
             *
             * @param sr sample rate
             */
            inline void set_sample_rate(size_t sr)
            {
                if (nSampleRate == sr)
                    return;

                nSampleRate     = sr;
                nReconfigure   |= R_ALL;
            }

            /** Set-up FFT analysis rate
             *
             * @param rate FFT rate
             */
            inline void set_rate(float rate)
            {
                if (fRate == rate)
                    return;

                fRate           = rate;
                nReconfigure   |= R_COUNTERS;
            }

            /** Set-up FFT reactivity
             *
             * @param reactivity reactivity (msec)
             */
            inline void set_reactivity(float reactivity)
            {
                if (fReactivity == reactivity)
                    return;

                fReactivity     = reactivity;
                nReconfigure   |= R_TAU;
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
                else if (nRank == rank)
                    return true;
                nRank           = rank;
                nReconfigure   |= R_ALL;
                return true;
            }

            /**
             * Return current rank of analyzer
             * @return current rank of analyzer
             */
            inline size_t get_rank() const
            {
                return nRank;
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

            /** Check if channel is active
             *
             * @param channel channel to check
             * @return true if channel is active
             */
            inline bool channel_active(size_t channel) const { return (channel < nChannels) ? vChannels[channel].bActive : false; }

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

            /**
             * Get level of one frequency
             * @param channel channel number
             * @param idx frequency index
             * @return level
             */
            float get_level(size_t channel, const uint32_t idx);

            /** Get list of frequencies
             *
             * @param f frequency list
             * @param idx frequency indexes containing frequency numbers for future get_spectrum() call
             * @param start start frequency
             * @param stop stop frequency
             * @param count number of elements
             */
            void get_frequencies(float *frq, uint32_t *idx, float start, float stop, size_t count);

            /** Read the frequencies of the analyzer
             *
             * @param frq target array to store frequency value
             * @param channel channel ID of input channel
             * @param start start frequency
             * @param stop end frequency
             * @param count number of items to store in frq and amp arrays
             * @param flags additional flags
             * @return true on success
             */
            bool read_frequencies(float *frq, float start, float stop, size_t count, size_t flags = FRQA_SCALE_LOGARITHMIC);

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

#endif /* CORE_UTIL_ANALYZER_H_ */
