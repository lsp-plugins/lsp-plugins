/*
 * View.h
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#ifndef TEST_MTEST_3D_COMMON_VIEW3D_H_
#define TEST_MTEST_3D_COMMON_VIEW3D_H_

#include <dsp/dsp.h>
#include <data/cstorage.h>

namespace mtest
{
    using namespace lsp;

#pragma pack (push, 1)
    typedef struct v_vertex3d_t
    {
        point3d_t   p;      // Position
        vector3d_t  n;      // Normal
        color3d_t   c;      // Color
    } v_vertex3d_t;

    typedef struct v_triangle3d_t
    {
        point3d_t   p[3];   // Positions
        vector3d_t  n[3];   // Normals
        color3d_t   c[3];   // Colors
    } v_triangle3d_t;

    typedef struct v_point3d_t
    {
        point3d_t   p;      // Position
        color3d_t   c;      // Color
    } v_point3d_t;

    typedef struct v_ray3d_t
    {
        point3d_t   p;      // Position
        vector3d_t  v;      // Direction
        color3d_t   c;      // Color
    } v_ray3d_t;

    typedef struct v_segment3d_t
    {
        point3d_t   p[2];   // Position
        color3d_t   c;      // Color
    } v_segment3d_t;

    typedef uint32_t            index3d_t;
#pragma pack (pop)

    typedef enum v3dflags_t
    {
        V3D_VERTEXES    = 1 << 0,
        V3D_RAYS        = 1 << 1,
        V3D_POINTS      = 1 << 2,
        V3D_SEGMENTS    = 1 << 3
    } v3dflags_t;

    class View3D
    {
        private:
            cstorage<v_vertex3d_t>      vVertexes;
            cstorage<v_ray3d_t>         vRays;
            cstorage<v_point3d_t>       vPoints;
            cstorage<v_segment3d_t>     vSegments;

        public:
            explicit View3D();
            virtual ~View3D();

        public:
            /**
             * Cleanup view
             * @param flags flags indicating what to cleanup
             */
            void clear(size_t flags);

            /**
             * Cleanup view
             */
            inline void clear_all() { clear(V3D_VERTEXES | V3D_RAYS | V3D_POINTS | V3D_SEGMENTS); };

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

            /**
             * Return number of vertexes
             * @return number of vertexes
             */
            inline size_t num_vertexes() const { return vVertexes.size(); }

            /** Add ray to scene
             *
             * @param r ray to add
             * @return true if ray was added
             */
            bool add_ray(const v_ray3d_t *r);

            /** Add point to scene
             *
             * @param p point to add
             * @return true if point was added
             */
            bool add_point(const v_point3d_t *p);

            /** Add segment to scene
             *
             * @param p segment to add
             * @return true if segment was added
             */
            bool add_segment(const v_segment3d_t *s);

            /**
             * Add triangle to scene
             * @param vi array of 3 elements indicating triangle corners
             * @return true if triangle was added
             */
            bool add_triangle(const v_vertex3d_t *vi);

            /**
             * Add triangle to scene
             * @param t triangle to add
             * @return true if triangle was added
             */
            bool add_triangle(const v_triangle3d_t *t);

            /**
             * Add triangle to scene and automatically generate indexes
             */
            bool add_triangle(const v_vertex3d_t *v1, const v_vertex3d_t *v2, const v_vertex3d_t *v3);

            /** Get ray by it's index
             *
             * @param index ray index
             * @return ray or NULL
             */
            v_ray3d_t *get_ray(size_t index);

            /** Get point by it's index
             *
             * @param index point index
             * @return point or NULL
             */
            v_point3d_t *get_point(size_t index);

            /** Get segment by it's index
             *
             * @param index segment index
             * @return segment or NULL
             */
            v_segment3d_t *get_segment(size_t index);

            /**
             * Get vertex by it's index
             * @param index vertex index
             * @return vertex or NULL
             */
            v_vertex3d_t *get_vertex(size_t index);

            /** Get array of rays
             *
             * @return array of rays
             */
            inline v_ray3d_t *get_rays() { return vRays.get_array(); }

            /** Get array of points
             *
             * @return array of points
             */
            inline v_point3d_t *get_points() { return vPoints.get_array(); }

            /** Get array of segments
             *
             * @return array of segments
             */
            inline v_segment3d_t *get_segments() { return vSegments.get_array(); }

            /**
             * Get array of vertexes
             * @return array of vertexes
             */
            inline v_vertex3d_t *get_vertexes() { return vVertexes.get_array(); }
    };

} /* namespace mtest */

#endif /* TEST_MTEST_3D_COMMON_VIEW3D_H_ */
