/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 28 сент. 2015 г.
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

#ifndef CORE_PLUGINS_PHASE_DETECTOR_H_
#define CORE_PLUGINS_PHASE_DETECTOR_H_

#include <core/plugin.h>
#include <metadata/plugins.h>

namespace lsp
{
    class phase_detector: public plugin_t, public phase_detector_metadata
    {
        protected:
            typedef struct buffer_t
            {
                float      *pData;
                size_t      nSize;
            } buffer_t;

        protected:
            float               fTimeInterval;
            float               fReactivity;

            float              *vFunction;
            float              *vAccumulated;
            float              *vNormalized;

            size_t              nMaxVectorSize;
            size_t              nVectorSize;
            size_t              nFuncSize;
            ssize_t             nBest;
            ssize_t             nWorst;
            ssize_t             nSelected;

            size_t              nGapSize;
            size_t              nMaxGapSize;
            size_t              nGapOffset;

            buffer_t            vA, vB;

            float               fTau;
            float               fSelector;
            bool                bBypass;

            float_buffer_t      *pIDisplay;      // Inline display buffer

        public:
            phase_detector();
            virtual ~phase_detector();

        protected:
            size_t fillGap(const float *a, const float *b, size_t count);
            void clearBuffers();
            void printFunction(const char *s, const float *f);
            bool setTimeInterval(float interval, bool force);
            void setReactiveInterval(float interval);
            void dropBuffers();

        public:
            virtual void destroy();
            virtual void update_settings();
            virtual void update_sample_rate(long sr);
            virtual void process(size_t samples);
            virtual bool inline_display(ICanvas *cv, size_t width, size_t height);
    };

} /* namespace ddb */

#endif /* CORE_PLUGINS_PHASE_DETECTOR_H_ */
