/*
 * bsp_context.cpp
 *
 *  Created on: 12 апр. 2019 г.
 *      Author: sadko
 */

#include <core/3d/common.h>
#include <core/3d/bsp_context.h>
#include <data/cstorage.h>

#define RT_FOREACH(type, var, collection) \
    for (size_t __ci=0,__ne=collection.size(), __nc=collection.chunks(); (__ci<__nc) && (__ne>0); ++__ci) \
    { \
        type *var       = collection.chunk(__ci); \
        size_t __loops  = collection.chunk_size(); \
        if (__loops > __ne) __loops = __ne; \
        __ne -= __loops; \
        for ( ; __loops > 0; ++var, --__loops) \
        {

#define RT_FOREACH_BREAK    { __ne = 0; break; }

#define RT_FOREACH_END      } }

namespace lsp
{
    bsp_context_t::bsp_context_t():
        node(256),
        triangle(1024)
    {
        root    = NULL;
        IF_RT_TRACE_Y(debug           = NULL;)
    }

    bsp_context_t::~bsp_context_t()
    {
        flush();
        IF_RT_TRACE_Y(debug           = NULL;)
    }

    void bsp_context_t::clear()
    {
        root    = NULL;
        node.clear();
        triangle.clear();
        IF_RT_TRACE_Y(debug           = NULL;)
    }

    void bsp_context_t::flush()
    {
        root    = NULL;
        node.flush();
        triangle.flush();
        IF_RT_TRACE_Y(debug           = NULL;)
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
            dsp::calc_normal3d_pv(&dt->n[0], dt->v);
            dt->n[1] = dt->n[0];
            dt->n[2] = dt->n[0];
//            dsp::apply_matrix3d_mv2(&dt->n[0], st->n[0], transform);
//            dsp::apply_matrix3d_mv2(&dt->n[1], st->n[1], transform);
//            dsp::apply_matrix3d_mv2(&dt->n[2], st->n[2], transform);

            dt->c               = *col;
            dt->oid             = oid;
            dt->face            = st->face;
        }

        return STATUS_OK;
    }

    status_t bsp_context_t::build_tree()
    {
        RT_TRACE_BREAK(debug,
                lsp_trace("Initial data (%d triangles)", int(triangle.size()));
                for (size_t i=0; i<triangle.size(); ++i)
                    trace.add_triangle(triangle.get(i));
            );

        // Build list of triangles for processing
        bsp_triangle_t *list = NULL;
        RT_FOREACH(bsp_triangle_t, t, triangle)
            t->next     = list;
            list        = t;
        RT_FOREACH_END;

        if (list == NULL) // There's nothing to do?
            return STATUS_OK;

        // Create initial task
        cvector<bsp_node_t> queue;
        if (!(root = node.alloc()))
            return STATUS_NO_MEM;
        root->in    = NULL;
        root->out   = NULL;
        root->on    = list;
        root->emit  = false;
        if (!queue.add(root))
            return STATUS_NO_MEM;

        // Do main iteration
        status_t res = STATUS_OK;
        bsp_node_t *task;
        while (queue.size() > 0)
        {
            // Get the task
            if (!queue.pop(&task))
            {
                res     = STATUS_CORRUPTED;
                break;
            }

            // Process the task
            if ((res = split(queue, task)) != STATUS_OK)
                break;
        }

        RT_TRACE_BREAK(debug,
            lsp_trace("Final BSP data (%d triangles)", int(triangle.size()));
            for (size_t i=0; i<triangle.size(); ++i)
                trace.add_triangle(triangle.get(i));
        );

        queue.flush();
        return res;
    }

    status_t bsp_context_t::split(cvector<bsp_node_t> &queue, bsp_node_t *task)
    {
        bsp_triangle_t *in = NULL, *out = NULL, *on = NULL;

        // Get current triangle
        bsp_triangle_t *ct = task->on;
        if (ct == NULL)
            return STATUS_OK;

        size_t tag;
        dsp::calc_plane_pv(&task->pl, ct->v);
        bsp_triangle_t *t0, *t1;//, *spt;

        // Add current triangle to 'on' list and walk through
        on          = ct;
        ct          = ct->next;
        on->next    = NULL;

//        RT_TRACE_BREAK(debug,
//            lsp_trace("Prepare split space into 'in', 'out' and 'on' sub-spaces");
//            for (bsp_triangle_t *st = task->on; st != NULL; st = st->next)
//            {
//                trace.add_triangle(st, (st == ct) ? &C_YELLOW : &C_GREEN);
//                if (st == ct)
//                    trace.add_plane_pv1c(st->v, &C_ORANGE);
//            }
//        );

        // Process each triangle
        while (ct != NULL)
        {
            bsp_triangle_t *nt  = ct->next;

            tag = dsp::colocation_x3_v1pv(&task->pl, ct->v);

//            RT_TRACE_BREAK(debug,
//                lsp_trace("Split triangle (tag=%02x)", int(tag));
//                trace.add_triangle(ct, &C_RED);
//                trace.add_plane_pv1c(spt->v, &C_ORANGE);
//            );

            switch (tag)
            {
                // Triangle is above
                case 0x00:  // 0 0 0
                case 0x01:  // 0 0 1
                case 0x04:  // 0 1 0
                case 0x05:  // 0 1 1
                case 0x10:  // 1 0 0
                case 0x11:  // 1 0 1
                case 0x14:  // 1 1 0
                    ct->next        = out;
                    out             = ct;
                    break;

                // Triangle is on the plane
                case 0x15:  // 1 1 1
                    ct->next        = on;
                    on              = ct;
                    break;

                // Triangle is below the plane
                case 0x16:  // 1 1 2
                case 0x19:  // 1 2 1
                case 0x1a:  // 1 2 2
                case 0x25:  // 2 1 1
                case 0x26:  // 2 1 2
                case 0x29:  // 2 2 1
                case 0x2a:  // 2 2 2
                    ct->next        = in;
                    in              = ct;
                    break;

                // 1 intersection, 1 triangle above, 1 triangle below, clockwise
                case 0x06:  // 0 1 2
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&ct->v[2], &ct->v[0], &ct->v[2], &task->pl);

                    t0->v[0]        = ct->v[2];
                    ct->next        = in;
                    in              = ct;
                    t0->next        = out;
                    out             = t0;
                    break;
                case 0x21:  // 2 0 1
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&ct->v[1], &ct->v[1], &ct->v[2], &task->pl);

                    t0->v[2]        = ct->v[1];
                    ct->next        = in;
                    in              = ct;
                    t0->next        = out;
                    out             = t0;
                    break;
                case 0x18:  // 1 2 0
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&ct->v[0], &ct->v[0], &ct->v[1], &task->pl);

                    t0->v[1]        = ct->v[0];
                    ct->next        = in;
                    in              = ct;
                    t0->next        = out;
                    out             = t0;
                    break;

                // 1 intersection, 1 triangle above, 1 triangle below, clockwise
                case 0x24:  // 2 1 0
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&ct->v[0], &ct->v[0], &ct->v[2], &task->pl);

                    t0->v[2]        = ct->v[0];
                    ct->next        = in;
                    in              = ct;
                    t0->next        = out;
                    out             = t0;
                    break;
                case 0x12:  // 1 0 2
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&ct->v[1], &ct->v[0], &ct->v[1], &task->pl);

                    t0->v[0]        = ct->v[1];
                    ct->next        = in;
                    in              = ct;
                    t0->next        = out;
                    out             = t0;
                    break;
                case 0x09:  // 0 2 1
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    dsp::calc_split_point_p2v1(&ct->v[2], &ct->v[1], &ct->v[2], &task->pl);

                    t0->v[1]        = ct->v[2];
                    ct->next        = in;
                    in              = ct;
                    t0->next        = out;
                    out             = t0;
                    break;

                // 2 intersections, 1 triangle below, 2 triangles above
                case 0x02:  // 0 0 2
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    if (!(t1 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_p2v1(&ct->v[1], &ct->v[0], &ct->v[1], &task->pl);
                    dsp::calc_split_point_p2v1(&ct->v[2], &ct->v[0], &ct->v[2], &task->pl);

                    t0->v[0]        = ct->v[1];
                    t0->v[2]        = ct->v[2];
                    t1->v[0]        = ct->v[2];

                    ct->next        = in;
                    in              = ct;
                    t0->next        = out;
                    t1->next        = t0;
                    out             = t1;
                    break;
                case 0x08:  // 0 2 0
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    if (!(t1 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_p2v1(&ct->v[0], &ct->v[1], &ct->v[0], &task->pl);
                    dsp::calc_split_point_p2v1(&ct->v[2], &ct->v[1], &ct->v[2], &task->pl);

                    t0->v[0]        = ct->v[0];
                    t0->v[1]        = ct->v[2];
                    t1->v[1]        = ct->v[0];

                    ct->next        = in;
                    in              = ct;
                    t0->next        = out;
                    t1->next        = t0;
                    out             = t1;
                    break;
                case 0x20:  // 2 0 0
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    if (!(t1 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_p2v1(&ct->v[0], &ct->v[2], &ct->v[0], &task->pl);
                    dsp::calc_split_point_p2v1(&ct->v[1], &ct->v[2], &ct->v[1], &task->pl);

                    t0->v[1]        = ct->v[1];
                    t0->v[2]        = ct->v[0];
                    t1->v[2]        = ct->v[1];

                    ct->next        = in;
                    in              = ct;
                    t0->next        = out;
                    t1->next        = t0;
                    out             = t1;
                    break;

                // 2 intersections, 1 triangle above, 2 triangles below
                case 0x28:  // 2 2 0
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    if (!(t1 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_p2v1(&ct->v[1], &ct->v[0], &ct->v[1], &task->pl);
                    dsp::calc_split_point_p2v1(&ct->v[2], &ct->v[0], &ct->v[2], &task->pl);

                    t0->v[0]        = ct->v[1];
                    t0->v[2]        = ct->v[2];
                    t1->v[0]        = ct->v[2];

                    ct->next        = out;
                    out             = ct;
                    t0->next        = in;
                    t1->next        = t0;
                    in              = t1;
                    break;

                case 0x22:  // 2 0 2
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    if (!(t1 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_p2v1(&ct->v[0], &ct->v[1], &ct->v[0], &task->pl);
                    dsp::calc_split_point_p2v1(&ct->v[2], &ct->v[1], &ct->v[2], &task->pl);

                    t0->v[0]        = ct->v[0];
                    t0->v[1]        = ct->v[2];
                    t1->v[1]        = ct->v[0];

                    ct->next        = out;
                    out             = ct;
                    t0->next        = in;
                    t1->next        = t0;
                    in              = t1;
                    break;

                case 0x0a:  // 0 2 2
                    if (!(t0 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;
                    if (!(t1 = triangle.alloc(ct)))
                        return STATUS_NO_MEM;

                    dsp::calc_split_point_p2v1(&ct->v[0], &ct->v[2], &ct->v[0], &task->pl);
                    dsp::calc_split_point_p2v1(&ct->v[1], &ct->v[2], &ct->v[1], &task->pl);

                    t0->v[1]        = ct->v[1];
                    t0->v[2]        = ct->v[0];
                    t1->v[2]        = ct->v[1];

                    ct->next        = out;
                    out             = ct;
                    t0->next        = in;
                    t1->next        = t0;
                    in              = t1;
                    break;

                default:
                    return STATUS_UNKNOWN_ERR;
            }

//            RT_TRACE_BREAK(debug,
//                lsp_trace("Split space into 'in' (GREEN), 'out' (BLUE) and 'on' (YELLOW) sub-spaces");
//                for (bsp_triangle_t *st = on; st != NULL; st = st->next)
//                    trace.add_triangle(st, &C_YELLOW);
//                for (bsp_triangle_t *st = in; st != NULL; st = st->next)
//                    trace.add_triangle(st, &C_GREEN);
//                for (bsp_triangle_t *st = out; st != NULL; st = st->next)
//                    trace.add_triangle(st, &C_BLUE);
//            );

            // Move to next triangle
            ct    = nt;
        }

        // Update task
        task->on    = on;
        if (out != NULL)
        {
            bsp_node_t *tout    = node.alloc();
            if (tout == NULL)
                return STATUS_NO_MEM;
            tout->in            = NULL;
            tout->out           = NULL;
            tout->on            = out;
            tout->emit          = false;

            if (!queue.add(tout))
                return STATUS_NO_MEM;
            task->out           = tout;
        }

        if (in != NULL)
        {
            bsp_node_t *tin     = node.alloc();
            if (tin == NULL)
                return STATUS_NO_MEM;
            tin->in            = NULL;
            tin->out           = NULL;
            tin->on            = in;
            tin->emit          = false;

            if (!queue.add(tin))
                return STATUS_NO_MEM;
            task->in           = tin;
        }

//        RT_TRACE_BREAK(debug,
//            lsp_trace("Split space into 'in' (GREEN), 'out' (BLUE) and 'on' (YELLOW) sub-spaces");
//            for (bsp_triangle_t *st = on; st != NULL; st = st->next)
//                trace.add_triangle(st, &C_YELLOW);
//            for (bsp_triangle_t *st = in; st != NULL; st = st->next)
//                trace.add_triangle(st, &C_GREEN);
//            for (bsp_triangle_t *st = out; st != NULL; st = st->next)
//                trace.add_triangle(st, &C_BLUE);
//        );

        return STATUS_OK;
    }

#if defined(LSP_RT_TRACE)
    void bsp_context_t::trace_recursive(bsp_node_t *node, const color3d_t *color)
    {
        if (node == NULL)
            return;
        if (node->out != NULL)
            trace_recursive(node->out, color);
        for (bsp_triangle_t *st=node->on; st != NULL; st = st->next)
            trace.add_triangle(st, color);
        if (node->in != NULL)
            trace_recursive(node->in, color);
    }
#endif /* LSP_RT_TRACE */

    status_t bsp_context_t::build_mesh(cstorage<v_vertex3d_t> *dst, const point3d_t *pov)
    {
        if (root == NULL)
            return STATUS_OK;

        // Create queue
        cvector<bsp_node_t> queue;
        bsp_node_t *curr    = root;
        curr->emit          = false;

        if (!queue.push(curr))
            return STATUS_NO_MEM;

        v_vertex3d_t *v[3];
        vector3d_t  pl;

        RT_TRACE_BREAK(debug,
            lsp_trace("Prepare build_mesh");
            trace.clear_all();
        );

        RT_TRACE_BREAK(debug,
            lsp_trace("Full content");
            trace_recursive(root, &C3D_GREEN);
        );

        do
        {
            // Get next task
            if (!queue.pop(&curr))
                return STATUS_NO_MEM;

//            RT_TRACE_BREAK(debug,
//                lsp_trace("Current element: in (CYAN), out (MAGENTA), on (YELLOW)");
//                trace_recursive(curr->in, &C_CYAN);
//                trace_recursive(curr->out, &C_MAGENTA);
//                for (bsp_triangle_t *st=curr->on; st != NULL; st = st->next)
//                    trace.add_triangle(st, &C_YELLOW);
//            );

            if (curr->emit)
            {
                for (bsp_triangle_t *ct=curr->on; ct != NULL; ct = ct->next)
                {
                    dsp::calc_plane_pv(&pl, ct->v);
                    float d         = pov->x*pl.dx + pov->y*pl.dy + pov->z*pl.dz + pl.dw;

                    // Allocate vertexes
                    size_t idx = dst->size();
                    v[0]    = dst->add();
                    v[1]    = dst->add();
                    v[2]    = dst->add();
                    if ((v[0] == NULL) || (v[1] == NULL) || (v[2] == NULL))
                        return STATUS_NO_MEM;

                    // Patch vertex pointers because dst may reallocate the memory
                    v[0]    = dst->at(idx);
                    v[1]    = dst->at(idx+1);
                    v[2]    = dst->at(idx+2);

                    if (d < 0.0f)
                    {
                        // Reverse order of vertex and flip normals
                        v[0]->p     = ct->v[0];
                        v[0]->c     = ct->c;
                        dsp::flip_vector_v2(&v[0]->n, &ct->n[0]);

                        v[1]->p     = ct->v[2];
                        v[1]->c     = ct->c;
                        dsp::flip_vector_v2(&v[1]->n, &ct->n[2]);

                        v[2]->p     = ct->v[1];
                        v[2]->c     = ct->c;
                        dsp::flip_vector_v2(&v[2]->n, &ct->n[1]);
                    }
                    else
                    {
                        // Emit as usual
                        v[0]->p     = ct->v[0];
                        v[0]->c     = ct->c;
                        v[0]->n     = ct->n[0];

                        v[1]->p     = ct->v[1];
                        v[1]->c     = ct->c;
                        v[1]->n     = ct->n[1];

                        v[2]->p     = ct->v[2];
                        v[2]->c     = ct->c;
                        v[2]->n     = ct->n[2];
                    }
                }
            }
            else
            {
                pl                  = curr->pl;
                float d             = pov->x*pl.dx + pov->y*pl.dy + pov->z*pl.dz + pl.dw;
                bsp_node_t *first   = (d < 0.0f) ? curr->out : curr->in;
                bsp_node_t *last    = (d < 0.0f) ? curr->in : curr->out;

//                RT_TRACE_BREAK(debug,
//                    lsp_trace("Draw order: first (GREEN), last (BLUE), on (YELLOW)");
//                    trace_recursive(first, &C_GREEN);
//                    trace_recursive(last, &C_BLUE);
//                    for (bsp_triangle_t *st=curr->on; st != NULL; st = st->next)
//                        trace.add_triangle(st, &C_YELLOW);
//                );

                if (last != NULL)
                {
                    last->emit  = false;
                    if (!(queue.push(last)))
                        return STATUS_NO_MEM;
                }
                if (curr->on != NULL)
                {
                    curr->emit  = true;
                    if (!(queue.push(curr)))
                        return STATUS_NO_MEM;
                }
                if (first != NULL)
                {
                    first->emit = false;
                    if (!(queue.push(first)))
                        return STATUS_NO_MEM;
                }
            }
        } while (queue.size() > 0);

        return STATUS_OK;
    }
}


