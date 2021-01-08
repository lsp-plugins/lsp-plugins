/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 14 авг. 2016 г.
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

#ifndef CORE_UTIL_ANALYZER_H_
#define CORE_UTIL_ANALYZER_H_

#include <core/types.h>
#include <core/envelope.h>
#include <core/windows.h>
#include <core/IStateDumper.h>

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
        private:
            Analyzer & operator = (const Analyzer &);

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
                float      *vBuffer;        // FFT delay buffer
                float      *vAmp;           // FFT amplitude
                float      *vData;          // FFT data
                size_t      nDelay;         // Delay in the delay buffer
                bool        bFreeze;        // Freeze analysis
                bool        bActive;        // Enable analysis
            } channel_t;

        protected:
            size_t      nChannels;          // Overall number of channels
            size_t      nMaxRank;           // Maximum FFT rank
            size_t      nRank;              // Current FFT rank
            size_t      nSampleRate;        // Sample rate
            size_t      nMaxSampleRate;     // Maximum possible sample rate
            size_t      nBufSize;           // Delay buffer size
            size_t      nCounter;           // Current counter
            size_t      nPeriod;            // FFT transform period
            size_t      nStep;              // FFT transform period
            size_t      nHead;              // Head of each delay buffer
            float       fReactivity;        // FFT reactivity
            float       fTau;               // Smooth coefficient
            float       fRate;              // FFT refresh rate
            float       fMinRate;           // Minimum possible FFT refresh rate
            float       fShift;             // Gain shift
            size_t      nReconfigure;       // Reconfiguration flags
            size_t      nEnvelope;          // Type of spectral envelope
            size_t      nWindow;            // Type of FFT window
            bool        bActive;            // Activity flag

            channel_t  *vChannels;          // List of channels
            void       *vData;              // Allocated floating-point data
            float      *vSigRe;             // Real part of signal
            float      *vFftReIm;           // Buffer for FFT transform (real part)
            float      *vWindow;            // FFT window
            float      *vEnvelope;          // FFT envelope

        public:
            explicit Analyzer();
            ~Analyzer();

            /**
             * Construct analyzer
             */
            void        construct();

            /** Destroy analyzer
             *
             */
            void        destroy();

        public:
            /** Initialize analyzer
             *
             * @param channels number of channels for analysis
             * @param max_rank maximum FFT rank
             * @param max_sr maximum sample rate
             * @param min_rate minimum refresh rate
             * @return status of operation
             */
            bool init(size_t channels, size_t max_rank, size_t max_sr, float min_rate);

            /**
             * Get overall number of channels
             * @return overall number of channels
             */
            inline size_t get_channels() const      { return nChannels; }

            /** Set window for analysis
             *
             * @param window window
             */
            void set_window(size_t window);

            /**
             * Get analyzer window
             * @return analyzer window
             */
            inline size_t get_window() const        { return nWindow; }

            /** Set envelope for analysis
             *
             * @param envelope envelope type
             */
            void set_envelope(size_t envelope);

            /**
             * Get envelope of analysis
             * @return envelope of analysis
             */
            inline size_t get_envelope() const       { return nEnvelope; }

            /** Set shift gain for analysis
             *
             * @param envelope envelope type
             */
            void set_shift(float shift);

            /**
             * Get gain shift value
             * @return gain shift value
             */
            inline float get_shift() const          { return fShift; }

            /** Set sample rate for analysis
             *
             * @param sr sample rate
             */
            void set_sample_rate(size_t sr);

            /**
             * Get sample rate
             * @return sample rate
             */
            inline size_t get_sample_rate() const { return nSampleRate; }

            /**
             * Get maximum possible sample rate
             * @return maximum possible sample rate
             */
            inline size_t get_max_sample_rate() const { return nMaxSampleRate; }

            /** Set-up FFT analysis rate
             *
             * @param rate FFT rate
             */
            void set_rate(float rate);

            /**
             * Get current refresh rate
             * @return current refresh rate
             */
            inline float get_rate() const { return fRate;   }

            /**
             * Get minimum possible rate
             * @return minimum possible rate
             */
            inline float get_min_rate() const { return fMinRate;    }

            /** Set-up FFT analysis reactivity
             *
             * @param reactivity reactivity (msec)
             */
            void set_reactivity(float reactivity);

            /**
             * Get reactivity of the analysis
             * @return reactivivy of the analysis
             */
            inline float get_reactivity() const     { return fReactivity; }

            /** Set rank of the analysis
             *
             * @param rank analysis rank
             * @return analysis rank
             */
            bool set_rank(size_t rank);

            /**
             * Return current rank of analyzer
             * @return current rank of analyzer
             */
            inline size_t get_rank() const          { return nRank; }

            /** Set analyzer activity
             *
             * @param active activity flag
             */
            inline void set_activity(bool active)   { bActive = active; }

            /** Freeze channel
             *
             * @param channel channel to freeze
             * @param freeze freeze flag
             * @return status of operation
             */
            bool freeze_channel(size_t channel, bool freeze);

            /** Enable channel
             *
             * @param channel channel to enable
             * @param enable enable flag
             * @return status of operation
             */
            bool enable_channel(size_t channel, bool enable);

            /** Check if channel is active
             *
             * @param channel channel to check
             * @return true if channel is active
             */
            inline bool channel_active(size_t channel) const { return (channel < nChannels) ? vChannels[channel].bActive : false; }

            /**
             * Reset the FFT data of analyzer
             */
            inline void reset() { nReconfigure       |= R_ANALYSIS; }

            /**
             * Process input signal
             * @param in array of pointers to buffers for all channels
             *        if pointer is NULL or the pointer to buffer is NULL, it is considered to be zero-filled
             * @param samples number of samples to process
             */
            void process(const float * const *in, size_t samples);

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
            void            reconfigure();

            /** Check that analyzer needs reconfiguration
             *
             * @return true if needs reconfiguration
             */
            inline bool     needs_reconfiguration() const   { return nReconfigure; }

            /**
             * Dump the state
             * @param dumper dumper
             */
            void            dump(IStateDumper *v) const;
    };

} /* namespace lsp */

#endif /* CORE_UTIL_ANALYZER_H_ */
