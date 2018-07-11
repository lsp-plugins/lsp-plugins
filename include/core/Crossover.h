/*
 * Crossover.h
 *
 *  Created on: 03 авг. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_CROSSOVER_H_
#define CORE_CROSSOVER_H_

#include <core/Filter.h>

namespace lsp
{
    /** Crossover handler
     *
     */
    class ICrossHandler
    {
        public:
            ICrossHandler();
            virtual ~ICrossHandler();

        public:
            /** Signal processing callback
             *
             * @param band_id number of band that triggered processing callback
             * @param out output buffer
             * @param in input buffer
             * @param samples number of samples to process
             */
            virtual void process(size_t band_id, float *out, const float *in, size_t samples);
    };

    /** Crossover, splits signal into bands, calls processing handler (if present)
     * and mixes processed bands back after adjusting the post-processing amplification gain
     *
     */
    class Crossover
    {
        protected:
            struct splitpoint_t;
            struct band_t;
            struct task_t;

            struct splitpoint_t
            {
                Filter      sLoPass;        // Lo-pass filter
                Filter      sHiPass;        // Hi-pass filter
                band_t     *pLoBand;        // Band for lo-pass
                band_t     *pHiBand;        // Band for hi-pass
                float       fFreq;          // Split frequency
                bool        bChanged;       // Changed flag
            };

            struct band_t
            {
                splitpoint_t   *pLeft;      // Left split point
                splitpoint_t   *pRight;     // Right split point
                float          *vBuffer;    // Buffer for signal processing
                float           fAmp;       // Post-processing amplification
            };

            struct task_t
            {
                splitpoint_t   *pSplit;     // Split point
                const float    *vInBuf;     // Input buffer
                float          *vLoBuf;     // Buffer for low frequencies
                float          *vHiBuf;     // Buffer for high frequencies
            };

        protected:
            size_t          nBands;         // Number of bands
            size_t          nBufSize;       // Buffer size
            size_t          nSlope;         // Crossover slope
            size_t          nSampleRate;    // Sample rate
            band_t         *vBands;         // List of bands
            splitpoint_t   *vSplit;         // List of split points
            task_t         *vTasks;         // Split task
            float          *vBuffers;       // Buffer data
            ICrossHandler  *pHandler;       // Crossover handler

        protected:
            void            build_tasks();
            void            build_task(size_t &n_tasks, size_t left, size_t bands, task_t *task);

        public:
            Crossover();
            ~Crossover();

        public:
            /** Initialize crossover
             *
             * @param bands number of bands
             * @param buf_size signal processing buffer size
             * @return status of operation
             */
            bool init(size_t bands, size_t buf_size);

            /** Destroy crossover
             *
             */
            void destroy();

            /** Bind the signal processing handler for the crossover
             *
             * @param handler signal processing handler to bind
             */
            inline void bind(ICrossHandler *handler)    { pHandler = handler;   }

            /** Set slope of crossover filters, needs reconfiguration
             *
             * @param slope slope of crossover filters
             */
            void set_slope(size_t slope);

            /** Set gain of band
             *
             * @param band band number
             * @param gain gain
             */
            bool set_gain(size_t band, float gain);

            /** Set split frequency for the split point, needs reconfiguration
             *
             * @param sp split point number
             * @param freq split point frequency
             */
            bool set_frequency(size_t sp, float freq);

            /** Set sample rate, needs reconfiguration
             *
             * @param sr sample rate to set
             */
            inline void set_sample_rate(size_t sr) { nSampleRate = sr; }

            /** Get frequency chart of the crossover
             *
             * @param re real part of the frequency chart
             * @param im imaginary part of the frequency chart
             * @param f frequencies to calculate value
             * @param count number of dots for the chart
             */
            bool freq_chart(float *re, float *im, const float *f, size_t count);

            /** Reconfigure crossover after parameter update
             *
             */
            void reconfigure();

            /** Process data
             *
             * @param out
             * @param in
             * @param samples
             */
            void process(float *out, const float *in, size_t samples);
    };

} /* namespace lsp */

#endif /* CORE_CROSSOVER_H_ */
