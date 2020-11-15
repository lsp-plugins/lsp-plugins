/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 26 февр. 2019 г.
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

#ifndef INCLUDE_CORE_3D_RT_PLAN_H_
#define INCLUDE_CORE_3D_RT_PLAN_H_

#include <core/3d/common.h>
#include <core/3d/Allocator3D.h>

namespace lsp
{
    /**
     * This is a space cutting plan for the raytracing algorithm
     */
    typedef struct rt_plan_t
    {
        private:
            rt_plan_t & operator = (const rt_plan_t &);

        public:
            Allocator3D<rt_split_t>     items;

        public:
            explicit rt_plan_t();
            ~rt_plan_t();

        public:
            /**
             * Clear plan: clear underlying structures
             */
            inline void     clear() { items.clear(); };

            /**
             * Flush plan: clear underlying structures and release memory
             */
            inline void     flush() { items.flush(); };

            /**
             * Check that the cutting plan is empty
             * @return true if the cutting plan is empty
             */
            inline bool     is_empty() const { return items.size() == 0; }

            /**
             * Swap contents with another plan
             * @param dst target plan to perform swap
             */
            inline void     swap(rt_plan_t *dst) { items.swap(&dst->items);  }

            /**
             * Split raytrace plan and keep the only edges that are below the cutting plane
             * @param pl cutting plane
             * @return status of operation
             */
            status_t        cut_out(const vector3d_t *pl);

            /**
             * Split raytrace plan and keep the only edges that are above the cutting plane
             * @param pl cutting plane
             * @return status of operation
             */
            status_t        cut_in(const vector3d_t *pl);

            /**
             * Split raytrace plan and keep the only edges that are below the cutting plane,
             * store all edges above the cutting plane to the other plan passed as parameter
             * @param out plan to store all edges above the cutting plane
             * @param pl cutting plane
             * @return status of operation
             */
            status_t        split(rt_plan_t *out, const vector3d_t *pl);

            /**
             * Add triangle to the plan
             * @param pv three triangle points
             * @param sp the equation of split plane provided by triangle
             * @return status of operation
             */
            status_t        add_triangle(const point3d_t *pv);

            /**
             * Add triangle to the plan
             * @param t triangle to add
             * @param sp the equation of split plane provided by triangle
             * @return status of operation
             */
            status_t        add_triangle(const rtm_triangle_t *t);

            /**
             * Add triangle to the plan
             * @param pv three triangle points
             * @param sp the equation of split plane provided by triangle
             * @return status of operation
             */
            rt_split_t     *add_edge(const point3d_t *pv);

            /**
             * Add triangle to the plan
             * @param pv three triangle points
             * @param sp the equation of split plane provided by triangle
             * @return status of operation
             */
            rt_split_t     *add_edge(const point3d_t *p1, const point3d_t *p2);

    } rt_plan_t;
}



#endif /* INCLUDE_CORE_3D_RT_PLAN_H_ */
