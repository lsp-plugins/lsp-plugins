/*
 * rt_spline.h
 *
 *  Created on: 25 мар. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_3D_RT_SPLINE_H_
#define CORE_3D_RT_SPLINE_H_

#include <core/3d/common.h>
#include <data/cstorage.h>

namespace lsp
{
    struct rt_debug_t;

    typedef struct rt_spline_t
    {
        public:
            rtm_vertex_t   *start;
            rtm_vertex_t   *end;
            cstorage<rtm_edge_t> edges;

        public:
            explicit rt_spline_t();
            ~rt_spline_t();

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
             * Copy contents of the spline
             * @param src source spline to copy
             * @return status of operation
             */
            status_t copy(rt_spline_t *src);

            /**
             * Add edge to the spline
             * @param edge edge to add
             * @return status of operation: STATUS_OK if added, STATUS_CLOSED if the spline has been closed, error otherwise
             */
            status_t add(rtm_edge_t *edge);

            /**
             * Test triangle for clockwise order and apply it to the spline
             * @param t triangle to test
             * @return status of operation: STATUS_OK if matched, STATUS_NOT_FOUND if not matched, error otherwise
             */
            status_t test(rtm_triangle_t *t);

    } rt_spline_t;
}

#endif /* INCLUDE_CORE_3D_RT_SPLINE_T_H_ */
