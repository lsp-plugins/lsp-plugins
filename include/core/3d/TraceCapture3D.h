/*
 * TraceCapture3D.h
 *
 *  Created on: 12 мая 2017 г.
 *      Author: sadko
 */

#ifndef CORE_3D_TRACECAPTURE3D_H_
#define CORE_3D_TRACECAPTURE3D_H_

#include <dsp/dsp.h>
#include <core/sampling/Sample.h>
#include <core/3d/Object3D.h>

namespace lsp
{
    enum trace_capture_type
    {
        TC3DT_OMNI,
        TC3DT_CARDIOID,
        TC3DT_BIDIRECTIONAL,
        TC3DT_SUPER_CARDIOID,
        TC3DT_HYPER_CARDIOID
    };

    class TraceCapture3D
    {
        protected:
#pragma pack(push, 1)
            typedef struct triangle_t
            {
                uint32_t        ip[3];
            } triangle_t;
#pragma pack(pop)

        protected:
            static const point3d_t tc_vp[];
            static const triangle_t tc_vt[];

        protected:
            Sample             *pSample;
            size_t              nChannel;
            size_t              nSampleRate;
            float               fRadius;
            trace_capture_type  enType;

        public:
            TraceCapture3D();
            ~TraceCapture3D();

        public:
            /** Initialize capture
             *
             * @param s sample to store data
             * @param channel channel to store data
             */
            void init(Sample *s, size_t channel);

            /** Set capture type
             *
             * @param type capture type
             */
            void set_type(trace_capture_type type) {  enType = type;  }

            /** Set sample rate
             *
             * @param sr sample rate
             */
            void set_sample_rate(size_t sr);

            /** Capture callback to process and store sample
             *
             * @param v direction vector
             * @param amplitude amplitude of the signal
             * @param delay delay of the signal in seconds
             */
            void capture(const vector3d_t *v, float amplitude, float delay);

            /** Reset the state of the capture
             *
             */
            void reset();

            /** Get radius of the capture surface
             *
             * @return radius of the capture surface
             */
            inline float get_radius() const { return fRadius; }

            /** Set radius of the capture surface
             *
             * @param radius radius of the capture surface
             */
            inline void set_radius(float radius) { fRadius = radius; }

            /** Build object surface for capturing
             *
             * @param rank surface rank
             * @return object build for capturing
             */
            Object3D *build_surface(size_t rank);
    };

} /* namespace lsp */

#endif /* CORE_3D_TRACECAPTURE3D_H_ */
