/*
 * Scene3D.h
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_3D_SCENE3D_H_
#define CORE_3D_SCENE3D_H_

#include <dsp/dsp.h>
#include <data/cvector.h>
#include <data/cstorage.h>
#include <core/3d/Object3D.h>
#include <core/3d/RaySource3D.h>
#include <core/3d/TraceMap3D.h>

namespace lsp
{
    /** 3D scene
     *
     */
    class Scene3D
    {
        private:
            cvector<Object3D>       vObjects;
            cvector<RaySource3D>    vSources;
            cvector<TraceCapture3D> vCaptures;
            cstorage<ray3d_t>       vRays;
            cstorage<point3d_t>     vPoints;
            cstorage<segment3d_t>   vSegments;

        public:
            /** Default constructor
             *
             */
            Scene3D();

            /** Destructor
             *
             */
            ~Scene3D();

            /** Destroy scene
             *
             * @param recursive destroy attached objects
             */
            void destroy(bool recursive=true);

            /** Clear scene
             *
             */
            inline void clear() { destroy(true); };

            /** Add object to scene
             *
             * @param obj object to add
             * @param capt object capture
             * @return true if object was added
             */
            bool add_object(Object3D *obj, TraceCapture3D *capt = NULL);

            /** Add ray source to scene
             *
             * @param obj ray source to add
             * @return true if ray source was added
             */
            bool add_source(RaySource3D *obj);

            /** Add ray to scene
             *
             * @param r ray to add
             * @return true if ray was added
             */
            bool add_ray(const ray3d_t *r);

            /** Add point to scene
             *
             * @param p point to add
             * @return true if point was added
             */
            bool add_point(const point3d_t *p);

            /** Add segment to scene
             *
             * @param p segment to add
             * @return true if segment was added
             */
            bool add_segment(const segment3d_t *s);

            /** Return number of objects in scene
             *
             * @return number of objects in scene
             */
            inline size_t num_objects() const { return vObjects.size(); }

            /** Return number of rays in scene
             *
             * @return number of rays in scene
             */
            inline size_t num_rays() const { return vRays.size(); }

            /** Return number of points in scene
             *
             * @return number of points in scene
             */
            inline size_t num_points() const { return vPoints.size(); }

            /** Return number of segments in scene
             *
             * @return number of segments in scene
             */
            inline size_t num_segments() const { return vSegments.size(); }

            /** Return number of ray sources in scene
             *
             * @return number of ray sources in scene
             */
            inline size_t num_sources() const { return vSources.size(); }

            /** Calculate overall number of triangles
             *
             * @return overall number of triangles
             */
            size_t num_triangles();

            /** Get object by it's index
             *
             * @param index object index
             * @return object or NULL
             */
            Object3D *get_object(size_t index);

            /** Get capture associated to object by it's index
             *
             * @param index object index
             * @return capture or NULL
             */
            TraceCapture3D *get_capture(size_t index);

            /** Get ray by it's index
             *
             * @param index ray index
             * @return ray or NULL
             */
            ray3d_t *get_ray(size_t index);

            /** Get point by it's index
             *
             * @param index point index
             * @return point or NULL
             */
            point3d_t *get_point(size_t index);

            /** Get segment by it's index
             *
             * @param index segment index
             * @return segment or NULL
             */
            segment3d_t *get_segment(size_t index);

            /** Get array of objects
             *
             * @return array of objects
             */
            inline Object3D **get_objects() { return vObjects.get_array(); }

            /** Get array of ray sources
             *
             * @return array of ray sources
             */
            inline RaySource3D **get_sources() { return vSources.get_array(); }

            /** Get array of captures
             *
             * @return array of captures attached to objects, elements may be null
             */
            inline TraceCapture3D **get_captures() { return vCaptures.get_array(); }

            /** Get array of rays
             *
             * @return array of rays
             */
            inline ray3d_t *get_rays() { return vRays.get_array(); }

            /** Get array of points
             *
             * @return array of points
             */
            inline point3d_t *get_points() { return vPoints.get_array(); }

            /** Get array of segments
             *
             * @return array of segments
             */
            inline segment3d_t *get_segments() { return vSegments.get_array(); }

            /** Build tracing map
             *
             * @return tracing map
             */
            TraceMap3D     *build_trace_map();
    };

} /* namespace lsp */

#endif /* CORE_3D_SCENE3D_H_ */
