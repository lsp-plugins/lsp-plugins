/*
 * View3D.h
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_3D_VIEW3D_H_
#define CORE_3D_VIEW3D_H_

#include <core/3d/common.h>
#include <core/3d/rt_plan.h>
#include <data/cstorage.h>

namespace lsp
{
    using namespace lsp;

    typedef enum v3dflags_t
    {
        V3D_VERTEXES    = 1 << 0,
        V3D_RAYS        = 1 << 1,
        V3D_POINTS      = 1 << 2,
        V3D_SEGMENTS    = 1 << 3,
        V3D_VERTEXES2   = 1 << 4,
    } v3dflags_t;

    class View3D
    {
        private:
            cstorage<v_vertex3d_t>      vVertexes;  // First-order draw
            cstorage<v_ray3d_t>         vRays;
            cstorage<v_point3d_t>       vPoints;
            cstorage<v_segment3d_t>     vSegments;
            cstorage<v_vertex3d_t>      vVertexes2; // Second-order draw

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
             * Swap contents with destination view
             * @param dst destination view
             */
            void swap(View3D *dst);

            /**
             * Add all data
             * @param src source for the trace
             */
            void add_all(const View3D *src);

            /**
             * Cleanup view
             */
            inline void clear_all() { clear(V3D_VERTEXES | V3D_RAYS | V3D_POINTS | V3D_SEGMENTS | V3D_VERTEXES2); };

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

            /**
             * Return number of second-pass vertexes
             * @return number of second-pass vertexes
             */
            inline size_t num_vertexes2() const { return vVertexes2.size(); }

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

            /** Add point to scene
             *
             * @param p point to add
             * @param c color
             * @return true if point was added
             */
            bool add_point(const point3d_t *p, const color3d_t *c);

            /** Add segment to scene
             *
             * @param p segment to add
             * @return true if segment was added
             */
            bool add_segment(const v_segment3d_t *s);

            /**
             * Add segment to scene
             * @param s segment to add
             * @return true if segment was added
             */
            bool add_segment(const rtm_edge_t *s, const color3d_t *c);

            /**
             * Add segment to scene
             * @param s segment to add
             * @return true if segment was added
             */
            bool add_segment(const rtm_edge_t *s, const color3d_t *c1, const color3d_t *c2);

            /**
             * Add segment to scene
             * @param p1 point 1
             * @param p2 point 2
             * @param c color
             * @return status of operation
             */
            bool add_segment(const point3d_t *p1, const point3d_t *p2, const color3d_t *c);

            bool add_segment(const rt_split_t *s, const color3d_t *c);

            /**
             * Add triangle to scene
             * @param vi array of 3 elements indicating triangle corners
             * @return true if triangle was added
             */
            bool add_triangle(const v_vertex3d_t *vi);

            /**
             * Add triangle to scene
             * @param vi triangle
             * @return true on success
             */
            bool add_triangle(const obj_triangle_t *vi, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2);

            bool add_triangle(const bsp_triangle_t *t);
            bool add_triangle(const bsp_triangle_t *t, const color3d_t *c);

            /**
             * Add context view
             * @param v context view to add
             */
            void add_view_1c(const rt_view_t *v, const color3d_t *c);

            void add_view_3c(const rt_view_t *v, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2);

            /**
             * Add triangle
             * @param t triangle
             * @param c triangle color
             * @return status of operation
             */
            bool add_triangle_1c(const v_triangle3d_t *t, const color3d_t *c);

            bool add_triangle_pvnc1(const point3d_t *t, const vector3d_t *n, const color3d_t *c);

            bool add_triangle_pvnc3(const point3d_t *t, const vector3d_t *n, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2);

            bool add_triangle_3c(const v_triangle3d_t *t, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2);

            bool add_triangle_3c(const obj_triangle_t *t, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2);

            bool add_triangle_1c(const obj_triangle_t *t, const color3d_t *c);

            bool add_triangle_1c(const rt_triangle_t *t, const color3d_t *c);

            bool add_triangle_3c(const rtm_triangle_t *t, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2);

            bool add_triangle_3c(const rt_triangle_t *t, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2);

            bool add_triangle_1c(const rtm_triangle_t *t, const color3d_t *c);

            /**
             * Add plane triangle
             * @param t triangle
             * @param c triangle color
             * @return status of operation
             */
            bool add_plane_pv1c(const point3d_t *t, const color3d_t *c);

            bool add_plane_pvn1c(const point3d_t *t, const vector3d_t *n, const color3d_t *c);

            /**
             * Add plane triangle
             * @param t triangle
             * @param c triangle color
             * @return status of operation
             */
            bool add_plane_3p1c(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const color3d_t *c);

            bool add_plane_3pn1c(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const vector3d_t *n, const color3d_t *c);

            void add_plane_2pn1c(const point3d_t *p1, const point3d_t *p2, const vector3d_t *n, const color3d_t *c);

            bool add_plane_sp3p1c(const point3d_t *sp, const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const color3d_t *c);

            /**
             * Add triangle
             * @param t triangle
             * @param c color
             * @return status of operation
             */
            bool add_triangle_pv1c(const point3d_t *pv, const color3d_t *c);

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

            /**
             * Get vertex by it's index
             * @param index vertex index
             * @return vertex or NULL
             */
            v_vertex3d_t *get_vertex2(size_t index);

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
            inline v_vertex3d_t *get_vertexes2() { return vVertexes2.get_array(); }

            /**
             * Dump plan to view
             * @param view view to dump
             * @param c mesh color
             * @return
             */
            void dump(rt_plan_t *plan, const color3d_t *c);

            /**
             * Get pointer to storage of vertexes
             * @return pointer to storage of vertexes
             */
            inline cstorage<v_vertex3d_t> *vertexes() { return &vVertexes; }

            /**
             * Get pointer to storage of second-order vertexes
             * @return pointer to storage of second-order vertexes
             */
            inline cstorage<v_vertex3d_t> *vertexes2() { return &vVertexes2; }
    };

} /* namespace mtest */

#endif /* CORE_3D_VIEW3D_H_ */
