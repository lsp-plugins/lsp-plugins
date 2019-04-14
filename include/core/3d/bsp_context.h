/*
 * bsp_context.h
 *
 *  Created on: 12 апр. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_3D_BSP_CONTEXT_H_
#define CORE_3D_BSP_CONTEXT_H_

#include <core/types.h>
#include <core/sugar.h>
#include <core/3d/common.h>
#include <core/3d/Allocator3D.h>
#include <core/3d/Object3D.h>

namespace lsp
{
    typedef struct bsp_node_t
    {
        vector3d_t      pl;
        bsp_node_t     *in;
        bsp_node_t     *out;
        bsp_triangle_t *on;
    } bsp_node_t;

    typedef struct bsp_context_t
    {
        private:
            bsp_context_t & operator = (const bsp_context_t &);

        public:
            Allocator3D<bsp_node_t>     node;
            Allocator3D<bsp_triangle_t> triangle;
            bsp_node_t                 *root;

        public:
            explicit bsp_context_t();
            ~bsp_context_t();

        protected:
            status_t split(cvector<bsp_node_t> &queue, bsp_node_t *task);

        public:
            void clear();
            void flush();

            inline void swap(bsp_context_t *dst)
            {
                ::swap(root, dst->root);
                node.swap(&dst->node);
                triangle.swap(&dst->triangle);
            }

            /**
             * Add object to context
             * @param obj object to add
             * @param oid unique id to identify the object
             * @param col object color
             * @return status of operation
             */
            inline status_t add_object(Object3D *obj, ssize_t oid, const color3d_t *col)
            {
                return add_object(obj, oid, obj->matrix(), col);
            }

            /**
             * Add object to context
             * @param obj object to add
             * @param oid unique id to identify the object
             * @param transform transformation matrix to apply to object
             * @param col object color
             * @return status of operation
             */
            status_t add_object(Object3D *obj, ssize_t oid, const matrix3d_t *transform, const color3d_t *col);

            /**
             * Build the BSP tree
             * @return status of operation
             */
            status_t build_tree();

    } bsp_context_t;
}


#endif /* CORE_3D_BSP_CONTEXT_H_ */
