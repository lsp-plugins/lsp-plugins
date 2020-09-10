/*
 * Crossover.h
 *
 *  Created on: 03 авг. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_UTIL_CROSSOVER_H_
#define CORE_UTIL_CROSSOVER_H_

#include <core/IStateDumper.h>
#include <core/filters/Filter.h>
#include <core/filters/Equalizer.h>

namespace lsp
{
    /*
         The overall schema of signal processing by the crossover for 4 bands
         (filters are following in order of the increasing frequency):

        INPUT = Input signal passed to the process() method
        LPF   = Low-pass filter
        HPF   = High-pass filter
        APF   = All-pass filter
        OUT   = Output signal for the particular band passed to the crossover_func_t callback function

       ┌─────┐     ┌─────┐     ┌─────┐     ┌─────┐     ┌─────┐
       │INPUT│──┬─►│LPF 0│────►│APF 1│────►│APF 2│────►│OUT 0│
       └─────┘  │  └─────┘     └─────┘     └─────┘     └─────┘
                │
                │
                │  ┌─────┐     ┌─────┐     ┌─────┐     ┌─────┐
                └─►│HPF 0│──┬─►│LPF 1│────►│APF 2│────►│OUT 1│
                   └─────┘  │  └─────┘     └─────┘     └─────┘
                            │
                            │
                            │  ┌─────┐     ┌─────┐     ┌─────┐
                            └─►│HPF 1│──┬─►│LPF 2│────►│OUT 2│
                               └─────┘  │  └─────┘     └─────┘
                                        │
                                        │
                                        │  ┌─────┐     ┌─────┐
                                        └─►│HPF 2│────►│OUT 3│
                                           └─────┘     └─────┘
     */

    /**
     * Crossover callback function for processing band signal
     *
     * @param object the object that handles callback
     * @param subject the subject that is used to handle callback
     * @param band number of the band
     * @param data the output band signal produced by crossover,
     *        is valid only until the function returns
     * @param count number of samples in the data buffer
     */
    typedef void (* crossover_func_t)(void *object, void *subject, size_t band, const float *data, size_t count);

    /** Crossover, splits signal into bands, calls processing handler (if present)
     * and mixes processed bands back after adjusting the post-processing amplification gain
     *
     */
    class Crossover
    {
        private:
            Crossover & operator = (const Crossover &);

        protected:
            typedef struct split_t
            {
                Equalizer           sLPF;           // Lo-pass filter
                Filter              sHPF;           // Hi-pass filter with all-pass filters

                size_t              nBandId;        // Number of split point
                size_t              nSlope;         // Filter slope (0 = off)
                float               fFreq;          // Frequency
            } split_t;

            typedef struct band_t
            {
                float               fGain;          // Output gain of the band
                float               fStart;         // Start frequency of the band
                float               fEnd;           // End frequency of the band
                bool                bEnabled;       // Enabled flag
                split_t            *pStart;         // Pointer to starting split point
                split_t            *pEnd;           // Pointer to ending split point

                crossover_func_t    pFunc;          // Function
                void               *pObject;        // Bound object
                void               *pSubject;       // Bound subject
                size_t              nId;            // Number of the band
            } band_t;

            enum reconfigure_t
            {
                R_GAIN          = 1 << 0,           // We can reconfigure band gain in softer mode
                R_SPLIT         = 1 << 1,           // Need to reconfigure filter order

                R_ALL           = R_GAIN | R_SPLIT
            } reconfigure_t;

        protected:
            size_t          nReconfigure;   // Change flag
            size_t          nSplits;        // Number of splits
            size_t          nBufSize;       // Buffer size
            size_t          nSampleRate;    // Sample rate

            band_t         *vBands;         // List of bands
            split_t        *vSplit;         // List of split points
            split_t       **vPlan;          // Split plan
            size_t          nPlanSize;      // Size of plan

            float          *vLpfBuf;        // Buffer for LPF
            float          *vHpfBuf;        // Buffer for HPF
            uint8_t        *pData;          // Unaligned data

        protected:
            // count is guaranteed to be not greater than nBufSize
            bool            freq_chart_internal(float *tf, const float *f, size_t band, size_t count);

        public:
            explicit Crossover();
            ~Crossover();

            /** Construct crossover
             *
             */
            void            construct();

            /** Destroy crossover
             *
             */
            void            destroy();

            /** Initialize crossover
             *
             * @param bands number of bands
             * @param buf_size maximum signal processing buffer size
             * @return status of operation
             */
            bool            init(size_t bands, size_t buf_size);

        public:
            /**
             * Get number of bands
             * @return number of bands
             */
            inline size_t   num_bands() const                       { return nSplits+1;     }

            /**
             * Get number of split points
             * @return number of split points
             */
            inline size_t   num_splits() const                      { return nSplits;       }

            /**
             * Get maximum buffer size for one iteration, if the provided
             * buffer is greater than max_buffer_size, the signal will be processed
             * in more than one iteration.
             *
             * @return maximum buffer size
             */
            inline size_t   max_buffer_size() const                 { return nBufSize;      }

            /** Set slope of crossover
             *
             * @param sp split point number
             * @param slope slope of crossover filters
             */
            void            set_slope(size_t sp, size_t slope);

            /**
             * Get slope of the split point
             * @param sp split point number
             * @return slope of the split point, 0 means split point is off,
             *         negative value means invalid index
             */
            ssize_t         get_slope(size_t sp) const;

            /** Set frequency of split point
             *
             * @param sp split point number
             * @param freq split frequency of the split point
             */
            void            set_frequency(size_t sp, float freq);

            /**
             * Get split frequency of the split point
             * @param sp split point number
             * @return split frequency of the split point, negative value
             *         means invalid index
             */
            float           get_frequency(size_t sp) const;

            /**
             * Set gain of the specific output band
             * @param band band number
             * @param gain gain of the band
             */
            void            set_gain(size_t band, float gain);

            /**
             * Get gain of the specific output band
             * @param band band number
             * @return gain of the band, negative value on invalid index
             */
            float           get_gain(size_t band) const;

            /**
             * Get start frequency of the band, may call reconfigure()
             * @param band band number
             * @return start frequency of the band or negative value on invalid index
             */
            float           get_band_start(size_t band);

            /**
             * Get end frequency of the band, may call reconfigure()
             * @param band band number
             * @return end frequency of the band or negative value on invalid index
             */
            float           get_band_end(size_t band);

            /**
             * Check that the band is active (always true for band 0), may call reconfigure()
             * @param band band number
             * @return true if band is active
             */
            bool            band_active(size_t band);

            /**
             * Set band signal handler
             * @param band band number
             * @param func handler function
             * @param object object to pass to function
             * @param subject subject to pass to function
             * @return false if invalid band number has been specified
             */
            bool            set_handler(size_t band, crossover_func_t func, void *object, void *subject);

            /**
             * Unset band signal handler
             * @param band band number
             * @return false if invalid band number has been specified
             */
            bool            unset_handler(size_t band);

            /** Set sample rate, needs reconfiguration
             *
             * @param sr sample rate to set
             */
            void            set_sample_rate(size_t sr);

            /**
             * Get sample rate of the crossover
             * @return sample rate
             */
            inline size_t   get_sample_rate()                   { return nSampleRate;           }

            /** Get frequency chart of the crossover
             *
             * @param band number of the band
             * @param re real part of the frequency chart
             * @param im imaginary part of the frequency chart
             * @param f frequencies to calculate value
             * @param count number of points for the chart
             * @return false if invalid band index is specified
             */
            bool            freq_chart(size_t band,  float *re, float *im, const float *f, size_t count);

            /** Get frequency chart of the crossover
             *
             * @param band number of the band
             * @param c transfer function (packed complex numbers)
             * @param f frequencies to calculate value
             * @param count number of points for the chart
             * @return false if invalid band index is specified
             */
            bool            freq_chart(size_t band, float *c, const float *f, size_t count);

            /**
             * Check that we need to call reconfigure()
             * @return true if we need to call reconfigure()
             */
            inline bool     needs_reconfiguration() const       { return nReconfigure != 0;     }

            /** Reconfigure crossover after parameter update
             *
             */
            void            reconfigure();

            /** Process data and issue callbacks, automatically calls reconfigure()
             * if the reconfiguration is required
             *
             * @param in input buffer to process data
             * @param samples number of samples to process
             */
            void            process(const float *in, size_t samples);

            /**
             * Dump the state
             * @param dumper dumper
             */
            void            dump(IStateDumper *v) const;
    };

} /* namespace lsp */

#endif /* CORE_UTIL_CROSSOVER_H_ */
