/*
 * rt_spline.h
 *
 *  Created on: 25 мар. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_3D_RT_SPLINE_H_
#define CORE_3D_RT_SPLINE_H_

#include <core/3d/common.h>
#include <data/cvector.h>

namespace lsp
{
    struct rt_debug_t;

    typedef struct rt_spline_t
    {
        public:
            rtm_vertex_t   *start;
            rtm_vertex_t   *end;
            cvector<rtm_vertex_t> vertex;

        protected:
            ssize_t        find_matching_edge(rtm_vertex_t *v0, rtm_vertex_t *v1);

        public:
            explicit rt_spline_t();
            ~rt_spline_t();

            /**
             * Clear spline
             */
            void clear();

            /**
             * Take all data from source spline and clear source spline
             * @param src source spline
             */
            void take(rt_spline_t *src);

            /**
             * Swap contents with another spline
             * @param dst target spline to swap
             */
            void swap(rt_spline_t *dst);

            /**
             * Reverse direction of the spline
             */
            void reverse();

            /**
             * check if spline is closed
             * @return true if spline is closed
             */
            inline bool closed() const { return (start == end) && (start != NULL); }

            /**
             * Copy contents of the spline
             * @param src source spline to copy
             * @return status of operation
             */
            status_t copy(rt_spline_t *src);

            /**
             * Link data from another spline to the current spline
             * @param src source spline to link
             * @return status of operation: STATUS_OK if linked, STATUS_CLOSED if the spline has been closed, STATUS_FAILED if can not link
             */
            status_t link(rt_spline_t *src);

            /**
             * Add edge to the spline
             * @param v0 vertex 0 of the edge
             * @param v1 vertex 1 of the edge
             * @return status of operation: STATUS_OK if added, STATUS_CLOSED if the spline has been closed, error otherwise
             */
            status_t add(rtm_vertex_t *v0, rtm_vertex_t *v1);

            /**
             * Test triangle for clockwise order
             * @param t triangle to test
             * @return status of operation: STATUS_OK if matched, STATUS_NOT_FOUND if not matched, error otherwise
             */
            status_t test(rtm_triangle_t *t);

    } rt_spline_t;
}

#endif /* INCLUDE_CORE_3D_RT_SPLINE_T_H_ */
