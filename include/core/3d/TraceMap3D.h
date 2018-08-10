/*
 * TraceMap.h
 *
 *  Created on: 20 апр. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_3D_TRACEMAP3D_H_
#define CORE_3D_TRACEMAP3D_H_

#include <core/status.h>
#include <dsp/dsp.h>
#include <core/3d/Object3D.h>
#include <core/3d/TraceCapture3D.h>
#include <core/3d/RaySource3D.h>

#include <data/cvector.h>

namespace lsp
{
    class Scene3D;

    class TraceMap3D
    {
        protected:
            typedef struct segment_t
            {
                triangle3d_t   *vTriangles;
                octant3d_t      sOctant;
                material3d_t   *pMaterial;
                size_t          nItems;
                Object3D       *pObject;
                TraceCapture3D *pCapture;
                point3d_t       sCapture;       // Capture center
                segment_t      *pNext;
            } segment_t;

            typedef struct capture3d_t: public intersection3d_t
            {
                segment_t      *pSeg[DSP_3D_MAXISECT];
            } capture3d_t;

        protected:
            segment_t              *pRoot;
            cvector<RaySource3D>    vSources;
            cvector<TraceCapture3D> vCaptures;

        protected:
            static bool has_triangle(const intersection3d_t *is, const triangle3d_t *t);
            void    raytrace_step(capture3d_t *ix, const raytrace3d_t *rt);

        public:
            TraceMap3D();
            ~TraceMap3D();

        public:
            /** Add object to raytrace map
             *
             * @param obj object to add
             * @param capt capture class to capture rays
             * @return true on success
             */
            bool    add_object(Object3D *obj, TraceCapture3D *capt = NULL);

            /** Add ray source
             *
             * @param rs ray source
             * @return true on success
             */
            bool    add_source(RaySource3D *rs);

            /** Execute raytracing algorithm
             * @param rays number of rays to use
             * @param s 3D scene for interactive debugging, NULL if no debugging needed
             * @return status of operation
             */
            status_t    raytrace(size_t rays, Scene3D *s = NULL);

            /** Destroy raytrace
             *
             */
            void    destroy();
    };

} /* namespace lsp */

#endif /* CORE_3D_TRACEMAP3D_H_ */
