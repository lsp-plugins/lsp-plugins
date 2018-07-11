/*
 * phase_detector.hpp
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_PLUGINS_PHASE_DETECTOR_H_
#define CORE_PLUGINS_PHASE_DETECTOR_H_

#include <core/plugin.h>
#include <core/plugin_metadata.h>

namespace lsp
{
    class phase_detector: public plugin, public phase_detector_metadata
    {
        protected:
            typedef struct buffer_t
            {
                float      *pData;
                size_t      nSize;
            } buffer_t;

        protected:
            float   fTimeInterval;
            float   fReactivity;

            float  *vFunction;
            float  *vAccumulated;
            float  *vNormalized;

            size_t  nMaxVectorSize;
            size_t  nVectorSize;
            size_t  nFuncSize;

            size_t  nGapSize;
            size_t  nMaxGapSize;
            size_t  nGapOffset;

            buffer_t  vA, vB;

            float   fTau;
            float   fSelector;
            bool    bBypass;

        public:
            phase_detector();
            virtual ~phase_detector();

        protected:
            size_t fillGap(const float *a, const float *b, size_t count);
            void clearBuffers();
            void printFunction(const char *s, const float *f);
            bool setTimeInterval(float interval);
            void setReactiveInterval(float interval);

        public:
            virtual void update_settings();

            virtual void init(int sample_rate);

            virtual void destroy();

            virtual void process(size_t samples);
    };

} /* namespace ddb */

#endif /* CORE_PLUGINS_PHASE_DETECTOR_H_ */
