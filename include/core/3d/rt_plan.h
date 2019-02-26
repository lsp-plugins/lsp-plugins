/*
 * rt_plan.h
 *
 *  Created on: 26 февр. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_3D_RT_PLAN_H_
#define INCLUDE_CORE_3D_RT_PLAN_H_

#include <core/3d/common.h>
#include <core/3d/Allocator3D.h>

namespace lsp
{
    typedef struct rt_plan_t
    {
        public:
            Allocator3D<rt_split_t>     items;

        public:
            explicit rt_plan_t();
            ~rt_plan_t();

        protected:
            status_t    perform_split(const vector3d_t *pl);

        public:
            /**
             * Split raytrace plan and keep the only edges that are below the cutting plane
             * @param pl cutting plane
             * @return status of operation
             */
            status_t    split_out(const vector3d_t *pl);

            /**
             * Split raytrace plan and keep the only edges that are above the cutting plane
             * @param pl cutting plane
             * @return status of operation
             */
            status_t    split_in(const vector3d_t *pl);

            /**
             * Split raytrace plan and keep the only edges that are below the cutting plane,
             * store all edges above the cutting plane to the other plan passed as parameter
             * @param out plan to store all edges above the cutting plane
             * @param pl cutting plane
             * @return status of operation
             */
            status_t    split(rt_plan_t *out, const vector3d_t *pl);

            /**
             * Add triangle to the plan
             * @param pv three triangle points
             * @return status of operation
             */
            status_t    add_triangle(const point3d_t *pv);

            /**
             * Add edge to the plan
             * @param pv array of two edge points
             * @return status of operation
             */
            status_t    add_edge(const point3d_t *pv);

            void        swap(rt_plan_t *dst);
    } rt_plan_t;
}



#endif /* INCLUDE_CORE_3D_RT_PLAN_H_ */
