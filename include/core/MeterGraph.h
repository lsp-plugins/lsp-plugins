/*
 * Meter.h
 *
 *  Created on: 20 мая 2016 г.
 *      Author: sadko
 */

#ifndef CORE_METER_GRAPH_H_
#define CORE_METER_GRAPH_H_

#include <core/types.h>
#include <core/ShiftBuffer.h>

namespace lsp
{
    enum meter_method_t
    {
        MM_MAXIMUM,
        MM_MINIMUM
    };

    class MeterGraph
    {
        private:
            ShiftBuffer         sBuffer;
            float               fCurrent;
            size_t              nCount;
            size_t              nPeriod;
            bool                bMinimize;

        public:
            MeterGraph();
            ~MeterGraph();

        public:
            bool init(size_t frames, size_t period);
            void destroy();

            inline void set_method(meter_method_t m) { bMinimize = (m == MM_MINIMUM); };

            inline float *data()    { return sBuffer.head();   }

            /** Process single sample
             *
             * @param sample sample to process
             */
            void process(float sample);

            /** Process multiple samples
             *
             * @param s array of samples
             * @param n number of samples to process
             */
            void process(const float *s, size_t n);

            /** Get current level
             *
             * @return current level
             */
            inline float level() const { return sBuffer.last(); }

            /** Fill graph with specific level
             *
             * @param level level
             */
            inline void fill(float level) { sBuffer.fill(level); };
    };
}

#endif /* CORE_METER_GRAPH_H_ */
