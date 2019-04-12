/*
 * bsp_context_t.cpp
 *
 *  Created on: 12 апр. 2019 г.
 *      Author: sadko
 */

#include <core/3d/bsp_context.h>

namespace lsp
{
    bsp_context_t::bsp_context_t()
    {
        root    = NULL;
    }

    bsp_context_t::~bsp_context_t()
    {
        flush();
    }

    void bsp_context_t::clear()
    {
        root    = NULL;
        for (size_t i=0, n=node.size(); i<n; ++i)
        {
            bsp_node_t *n = node.get(i);
            n->match.flush();       // Flush contents
            n->~bsp_node_t();       // Destroy node
        }
        node.clear();
        triangle.clear();
    }

    void bsp_context_t::flush()
    {
        root    = NULL;
        for (size_t i=0, n=node.size(); i<n; ++i)
        {
            bsp_node_t *n = node.get(i);
            n->match.flush();       // Flush contents
            n->~bsp_node_t();       // Destroy node
        }
        node.flush();
        triangle.flush();
    }

    status_t bsp_context_t::add_object(Object3D *obj, ssize_t oid, const matrix3d_t *transform, const color3d_t *col)
    {
        for (size_t i=0, n=obj->num_triangles(); i<n; ++i)
        {
            obj_triangle_t *st  = obj->triangle(i);
            bsp_triangle_t *dt  = triangle.alloc();
            if (dt == NULL)
                return STATUS_NO_MEM;

            dsp::apply_matrix3d_mp2(&dt->v[0], st->v[0], transform);
            dsp::apply_matrix3d_mp2(&dt->v[1], st->v[1], transform);
            dsp::apply_matrix3d_mp2(&dt->v[2], st->v[2], transform);
            dsp::apply_matrix3d_mv2(&dt->n[0], st->n[0], transform);
            dsp::apply_matrix3d_mv2(&dt->n[1], st->n[1], transform);
            dsp::apply_matrix3d_mv2(&dt->n[2], st->n[2], transform);
            dt->oid             = oid;
            dt->face            = st->face;
        }

        return STATUS_OK;
    }

    status_t bsp_context_t::build_tree()
    {
        // Create initial task
        cvector<bsp_task_t> queue;
        bsp_task_t *t = new bsp_task_t();
        if (t == NULL)
            return STATUS_NO_MEM;

        if (!(t->node = node.alloc()))
        {
            delete t;
            return STATUS_NO_MEM;
        }

        t->mesh.swap(&triangle);
        if (!queue.add(t))
        {
            delete t;
            queue.flush();
            return STATUS_NO_MEM;
        }

        // Cleanup state
        clear();

        // Do main iteration
        status_t res = STATUS_OK;
        while (queue.size() > 0)
        {
            // Get the task
            if (!queue.pop(&t))
            {
                res     = STATUS_CORRUPTED;
                break;
            }

            // Process the task
            if ((res = split(queue, t)) != STATUS_OK)
            {
                t->mesh.flush();
                delete t;
                break;
            }
        }

        // Destroy tasks
        for (size_t i=0, n=queue.size(); i<n; ++i)
        {
            t = queue.get(i);
            t->mesh.flush();
            delete t;
        }
        queue.flush();

        return res;
    }

    status_t bsp_context_t::split(cvector<bsp_task_t> &queue, bsp_task_t *task)
    {
        // TODO
        return STATUS_OK;
    }
}


