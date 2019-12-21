/*
 * rt_mesh.cpp
 *
 *  Created on: 26 февр. 2019 г.
 *      Author: sadko
 */

#include <core/3d/rt_mesh.h>
#include <core/3d/rt_context.h>

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
    rt_mesh_t::rt_mesh_t():
        vertex(1024),
        edge(1024),
        triangle(1024)
    {
        IF_RT_TRACE_Y(
            debug = NULL;
            view = NULL;
        )
    }

    rt_mesh_t::~rt_mesh_t()
    {
        vertex.flush();
        edge.flush();
        triangle.flush();
    }

    status_t rt_mesh_t::add_object(Object3D *obj, ssize_t oid, const matrix3d_t *transform, rt_material_t *material)
    {
        // Reset tags
        obj->scene()->init_tags(NULL, 0);

        size_t start_t  = triangle.size();
        size_t start_e  = edge.size();

        // Clone triangles and apply object matrix to vertexes
        for (size_t i=0, n=obj->num_triangles(); i<n; ++i)
        {
            obj_triangle_t *st = obj->triangle(i);
            if (st == NULL)
                return STATUS_BAD_STATE;
            else if (st->ptag != NULL) // Skip already emitted triangle
                continue;

            // Allocate triangle and store pointer
            rtm_triangle_t *dt = triangle.alloc();
            if (dt == NULL)
                return STATUS_NO_MEM;

            dt->elnk[0] = NULL;
            dt->elnk[1] = NULL;
            dt->elnk[2] = NULL;
            dt->ptag    = st;
            dt->itag    = 0;
            dt->oid     = oid;
            dt->face    = st->face;
            st->ptag    = dt;
            dt->m       = material;

            // Copy data
            for (size_t j=0; j<3; ++j)
            {
                // Allocate vertex
                rtm_vertex_t *vx     = reinterpret_cast<rtm_vertex_t *>(st->v[j]->ptag);
                if (st->v[j]->ptag == NULL)
                {
                    vx              = vertex.alloc();
                    if (vx == NULL)
                        return STATUS_NO_MEM;

                    dsp::apply_matrix3d_mp2(vx, st->v[j], transform);
                    vx->ptag        = st->v[j];
                    vx->itag        = 0;

                    st->v[j]->ptag  = vx;
                }

                // Allocate edge
                rtm_edge_t *ex       = reinterpret_cast<rtm_edge_t *>(st->e[j]->ptag);
                if (ex == NULL)
                {
                    ex              = edge.alloc();
                    if (ex == NULL)
                        return STATUS_NO_MEM;

                    ex->v[0]        = NULL;
                    ex->v[1]        = NULL;
                    ex->vt          = NULL;
                    ex->ptag        = st->e[j];
                    ex->itag        = 0;

                    st->e[j]->ptag  = ex;
                }

                dt->v[j]        = vx;
                dt->e[j]        = ex;
            }

            // Compute plane equation and store as normal
            dsp::calc_plane_p3(&dt->n, dt->v[0], dt->v[1], dt->v[2]);
        }

        // Patch edge structures and link to vertexes
        for (size_t i=start_e, n=edge.size(); i<n; ++i)
        {
            rtm_edge_t *de       = edge.get(i);
            obj_edge_t *se      = reinterpret_cast<obj_edge_t *>(de->ptag);

            de->v[0]            = reinterpret_cast<rtm_vertex_t *>(se->v[0]->ptag);
            de->v[1]            = reinterpret_cast<rtm_vertex_t *>(se->v[1]->ptag);
        }

        // Patch triangle structures and link to edges
        for (size_t i=start_t, n=triangle.size(); i<n; ++i)
        {
            rtm_triangle_t *dt  = triangle.get(i);
            obj_triangle_t *st  = reinterpret_cast<obj_triangle_t *>(dt->ptag);

            dt->v[0]            = reinterpret_cast<rtm_vertex_t *>(st->v[0]->ptag);
            dt->v[1]            = reinterpret_cast<rtm_vertex_t *>(st->v[1]->ptag);
            dt->v[2]            = reinterpret_cast<rtm_vertex_t *>(st->v[2]->ptag);

            dt->e[0]            = reinterpret_cast<rtm_edge_t *>(st->e[0]->ptag);
            dt->e[1]            = reinterpret_cast<rtm_edge_t *>(st->e[1]->ptag);
            dt->e[2]            = reinterpret_cast<rtm_edge_t *>(st->e[2]->ptag);

            // Link triangle to the edge
            dt->elnk[0]         = dt->e[0]->vt;
            dt->elnk[1]         = dt->e[1]->vt;
            dt->elnk[2]         = dt->e[2]->vt;

            dt->e[0]->vt        = dt;
            dt->e[1]->vt        = dt;
            dt->e[2]->vt        = dt;
        }

        return STATUS_OK;
    }

    bool rt_mesh_t::unlink_triangle(rtm_triangle_t *t, rtm_edge_t *e)
    {
        for (rtm_triangle_t **pcurr = &e->vt; *pcurr != NULL; )
        {
            rtm_triangle_t *curr = *pcurr;
            rtm_triangle_t **pnext = (curr->e[0] == e) ? &curr->elnk[0] :
                                    (curr->e[1] == e) ? &curr->elnk[1] :
                                    (curr->e[2] == e) ? &curr->elnk[2] :
                                    NULL;
            if (pnext == NULL) // Unexpected behaviour
                return false;
            if (curr == t)
            {
                *pcurr = *pnext;
                return true;
            }
            pcurr = pnext;
        }
        return false;
    }

    status_t rt_mesh_t::arrange_triangle(rtm_triangle_t *ct, rtm_edge_t *e)
    {
        rtm_vertex_t *tv;
        rtm_edge_t *te;
        rtm_triangle_t *tt;

        // Rotate triangle to make ct->e[0] == e
        if (ct->e[1] == e) // Rotate clockwise
        {
            tv              = ct->v[0];
            ct->v[0]        = ct->v[1];
            ct->v[1]        = ct->v[2];
            ct->v[2]        = tv;

            te              = ct->e[0];
            ct->e[0]        = ct->e[1];
            ct->e[1]        = ct->e[2];
            ct->e[2]        = te;

            tt              = ct->elnk[0];
            ct->elnk[0]     = ct->elnk[1];
            ct->elnk[1]     = ct->elnk[2];
            ct->elnk[2]     = tt;
        }
        else if (ct->e[2] == e) // Rotate counter-clockwise
        {
            tv              = ct->v[2];
            ct->v[2]        = ct->v[1];
            ct->v[1]        = ct->v[0];
            ct->v[0]        = tv;

            te              = ct->e[2];
            ct->e[2]        = ct->e[1];
            ct->e[1]        = ct->e[0];
            ct->e[0]        = te;

            tt              = ct->elnk[2];
            ct->elnk[2]     = ct->elnk[1];
            ct->elnk[1]     = ct->elnk[0];
            ct->elnk[0]     = tt;
        }
        else if (ct->e[0] != e)
            return STATUS_BAD_STATE;

        return STATUS_OK;
    }

    status_t rt_mesh_t::split_edge(rtm_edge_t* e, rtm_vertex_t* sp)
    {
        status_t res;
        rtm_triangle_t *ct, *nt, *pt;
        rtm_edge_t *ne, *se;

        // Rearrange first triangle
        if ((ct = e->vt) == NULL)
            return STATUS_OK;
        res             = arrange_triangle(ct, e);
        if (res != STATUS_OK)
            return res;

        // Allocate edges
        ne              = edge.alloc();
        if (ne == NULL)
            return STATUS_NO_MEM;

        // Initialize culled edge and link to corresponding vertexes
        ne->v[0]        = sp;
        ne->v[1]        = e->v[1];
        ne->vt          = NULL;
        ne->ptag        = NULL;
        ne->itag        = e->itag;

        e->v[1]         = sp;

        RT_VALIDATE(
            if ((ne->v[0] == NULL) || (ne->v[1] == NULL))
                return STATUS_CORRUPTED;
        )

        // Process all triangles
        while (true)
        {
//            RT_TRACE_BREAK(debug,
//                view->add_triangle_1c(ct, &C_RED);
//                view->add_point(sp, &C_YELLOW);
//            );

            // Save pointer to triangle to move forward
            pt              = ct->elnk[0];  // Save pointer to pending triangle, splitting edge is always rearranged to have index 0

            // Allocate triangle and splitting edge
            nt              = triangle.alloc();
            se              = edge.alloc();
            if ((nt == NULL) || (se == NULL))
                return STATUS_NO_MEM;

            // Initialize splitting edge and link to it's vertexes
            se->v[0]        = ct->v[2];
            se->v[1]        = sp;
            se->vt          = NULL;
            se->ptag        = NULL;
            se->itag        = e->itag;

            // Unlink current triangle from all edges
            if (!unlink_triangle(ct, ct->e[0]))
                return STATUS_CORRUPTED;
            if (!unlink_triangle(ct, ct->e[1]))
                return STATUS_CORRUPTED;
            if (!unlink_triangle(ct, ct->e[2]))
                return STATUS_CORRUPTED;

            if (e->v[0] == ct->v[0])
            {
                // Initialize new triangle
                nt->v[0]        = sp;
                nt->v[1]        = ct->v[1];
                nt->v[2]        = ct->v[2];
                nt->e[0]        = ne;
                nt->e[1]        = ct->e[1];
                nt->e[2]        = se;
                nt->n           = ct->n;
                nt->ptag        = NULL;
                nt->itag        = ct->itag;
                nt->oid         = ct->oid;
                nt->face        = ct->face;
                nt->m           = ct->m;

                // Update current triangle
              //ct->v[0]        = ct->v[0];
                ct->v[1]        = sp;
              //ct->v[2]        = ct->v[2];
              //ct->e[0]        = e;
                ct->e[1]        = se;
              //ct->e[2]        = ct->e[2];
              //ct->n           = ct->n;
              //ct->itag        = ct->itag;
            }
            else if (e->v[0] == ct->v[1])
            {
                // Initialize new triangle
                nt->v[0]        = sp;
                nt->v[1]        = ct->v[2];
                nt->v[2]        = ct->v[0];
                nt->e[0]        = se;
                nt->e[1]        = ct->e[2];
                nt->e[2]        = ne;
                nt->n           = ct->n;
                nt->ptag        = NULL;
                nt->itag        = ct->itag;
                nt->oid         = ct->oid;
                nt->face        = ct->face;
                nt->m           = ct->m;

                // Update current triangle
                ct->v[0]        = sp;
              //ct->v[1]        = ct->v[1];
              //ct->v[2]        = ct->v[2];
              //ct->e[0]        = e;
              //ct->e[1]        = ct->e[1];
                ct->e[2]        = se;
              //ct->n           = ct->n;
              //ct->itag        = ct->itag;
            }
            else
                return STATUS_BAD_STATE;

//            RT_TRACE_BREAK(debug,
//                view->add_triangle_1c(ct, &C_GREEN);
//                view->add_triangle_1c(nt, &C_BLUE);
//                view->add_point(sp, &C_YELLOW);
//                view->add_segment(se, &C_YELLOW);
//                view->add_segment(ne, &C_CYAN);
//                view->add_segment(e, &C_MAGENTA);
//            );

            // Link edges to new triangles
            nt->elnk[0]     = nt->e[0]->vt;
            nt->elnk[1]     = nt->e[1]->vt;
            nt->elnk[2]     = nt->e[2]->vt;
            nt->e[0]->vt    = nt;
            nt->e[1]->vt    = nt;
            nt->e[2]->vt    = nt;

            ct->elnk[0]     = ct->e[0]->vt;
            ct->elnk[1]     = ct->e[1]->vt;
            ct->elnk[2]     = ct->e[2]->vt;
            ct->e[0]->vt    = ct;
            ct->e[1]->vt    = ct;
            ct->e[2]->vt    = ct;

            // Move to next triangle
            if (pt == NULL)
                break;

            // Re-arrange next triangle and edges
            ct              = pt;
            res             = arrange_triangle(ct, e);
            if (res != STATUS_OK)
                return res;
        }

        // Now the edge 'e' is stored in context but not linked to any primitive
        return STATUS_OK;
    }

    status_t rt_mesh_t::split_triangle(rtm_triangle_t* t, rtm_vertex_t* sp)
    {
        // Unlink triangle from all it's edges
        unlink_triangle(t, t->e[0]);
        unlink_triangle(t, t->e[1]);
        unlink_triangle(t, t->e[2]);

        // Create additional edges and link them to vertexes
        rtm_edge_t *ne[3];
        for (size_t i=0; i<3; ++i)
        {
            rtm_edge_t *e    = edge.alloc();
            if (e == NULL)
                return STATUS_NO_MEM;
            ne[i]           = e;

            e->v[0]         = t->v[i];
            e->v[1]         = sp;
            e->vt           = NULL;
            e->ptag         = NULL;
            e->itag         = 0;
        }

        // Allocate additional triangles
        rtm_triangle_t *nt[3];
        nt[0]       = triangle.alloc();
        nt[1]       = triangle.alloc();
        nt[2]       = t;
        if ((nt[0] == NULL) || (nt[1] == NULL))
            return STATUS_NO_MEM;

        // Now bind edges and vertexes to triangles
        nt[0]->v[0]     = t->v[1];
        nt[0]->v[1]     = t->v[2];
        nt[0]->v[2]     = sp;
        nt[0]->e[0]     = t->e[1];
        nt[0]->e[1]     = ne[2];
        nt[0]->e[2]     = ne[1];
        nt[0]->n        = t->n;
        nt[0]->ptag     = NULL;
        nt[0]->itag     = t->itag;
        nt[0]->oid      = t->oid;
        nt[0]->face     = t->face;
        nt[0]->m        = t->m;

        nt[1]->v[0]     = t->v[2];
        nt[1]->v[1]     = t->v[0];
        nt[1]->v[2]     = sp;
        nt[1]->e[0]     = t->e[2];
        nt[1]->e[1]     = ne[0];
        nt[1]->e[2]     = ne[2];
        nt[1]->n        = t->n;
        nt[1]->ptag     = NULL;
        nt[1]->itag     = t->itag;
        nt[1]->oid      = t->oid;
        nt[1]->face     = t->face;
        nt[1]->m        = t->m;

      //nt[2]->v[0]     = t->v[0];
      //nt[2]->v[1]     = t->v[1];
        nt[2]->v[2]     = sp;
      //nt[2]->e[0]     = t->e[0];
        nt[2]->e[1]     = ne[1];
        nt[2]->e[2]     = ne[0];
      //nt[2]->n        = t->n;
      //nt[2]->ptag     = NULL;
      //nt[2]->itag     = t->itag;
      //nt[2]->oid      = t->oid;
      //nt[2]->face     = t->face;

        // Re-link triangles to edges
        for (size_t i=0; i<3; ++i)
        {
            rtm_triangle_t *ct   = nt[i];

            ct->elnk[0]     = ct->e[0]->vt;
            ct->elnk[1]     = ct->e[1]->vt;
            ct->elnk[2]     = ct->e[2]->vt;
            ct->e[0]->vt    = ct;
            ct->e[1]->vt    = ct;
            ct->e[2]->vt    = ct;
        }

        return STATUS_OK;
    }

    status_t rt_mesh_t::solve_conflicts()
    {
        status_t res;
        vector3d_t pl;
        vector3d_t spl[3]; // Scissor planes
        size_t tag;

        // Intialize tags
        RT_FOREACH(rtm_edge_t, e, edge)
            e->itag     = 0;
        RT_FOREACH_END;

        for (size_t i=0; i<triangle.size(); )
        {
            rtm_triangle_t *ct  = triangle.get(i++);
            ct->itag            = i;
        }

        for (size_t i=0; i<triangle.size(); ++i)
        {
            rtm_triangle_t *ct   = triangle.get(i);
            dsp::calc_plane_p3(&pl, ct->v[0], ct->v[1], ct->v[2]);
            dsp::calc_plane_v1p2(&spl[0], &pl, ct->v[0], ct->v[1]);
            dsp::calc_plane_v1p2(&spl[1], &pl, ct->v[1], ct->v[2]);
            dsp::calc_plane_v1p2(&spl[2], &pl, ct->v[2], ct->v[0]);

            // Split each edge with triangle, do not process new edges
            RT_FOREACH(rtm_edge_t, ce, edge)
                // Interact only ONCE with a specific triangle
                if (ce->itag >= ct->itag)
                    continue;

                if ((ce == ct->e[0]) || (ce == ct->e[1]) || (ce == ct->e[2]))
                    continue;
                else if ((ce->v[0] == ct->v[0]) || (ce->v[0] == ct->v[1]) || (ce->v[0] == ct->v[2]))
                    continue;
                else if ((ce->v[1] == ct->v[0]) || (ce->v[1] == ct->v[1]) || (ce->v[1] == ct->v[2]))
                    continue;

                // Ensure that edge intersects the plane
                rtm_vertex_t sp, *spp;
                tag         = dsp::colocation_x2_v1p2(&pl, ce->v[0], ce->v[1]);
                bool touch  = true;

                switch (tag)
                {
                    case 0x01: case 0x09: // Edge touches the plane with p[0]
                        sp      = *(ce->v[0]);
                        break;
                    case 0x04: case 0x06: // Edge touches the plane with p[1]
                        sp      = *(ce->v[1]);
                        break;
                    case 0x02: case 0x08: // Edge is crossing the plane
                        touch   = false;
                        dsp::calc_split_point_p2v1(&sp, ce->v[0], ce->v[1], &pl); // Compute split point
                        break;

                    default:
                        continue;
                }

            #define TRACE_CONFLICT_BEFORE \

            #define TRACE_CONFLICT_BEFORE1 \
                RT_TRACE_BREAK(debug, \
                    lsp_trace("split point={%f, %f, %f, %f}", sp.x, sp.y, sp.z, sp.w); \
                    view->add_triangle_1c(ct, &C_GREEN); \
                    view->add_segment(ct->e[0], &C_RED); \
                    view->add_segment(ct->e[1], &C_GREEN); \
                    view->add_segment(ct->e[2], &C_BLUE); \
                    \
                    view->add_segment(ce, &C_YELLOW); \
                    view->add_point(&sp, &C_YELLOW); \
                );

            #define TRACE_CONFLICT_AFTER \

            #define TRACE_CONFLICT_AFTER1 \
                RT_TRACE_BREAK(debug, \
                    lsp_trace("Prepare solve conflicts step (%d triangles)", int(triangle.size())); \
                    for (size_t i=0,n=triangle.size(); i<n; ++i) \
                        debug->trace.add_triangle_3c(triangle.get(i), &C_RED, &C_GREEN, &C_BLUE); \
                );

                // Now, analyze co-location of split point and triangle
                tag      = dsp::colocation_x3_vvp1(spl, &sp);
                switch (tag)
                {
                    case 0x16: // Point matches edges 1 and 2 (vertex 2)
                        TRACE_CONFLICT_BEFORE;
                        ce->itag    = ct->itag;
                        if (!touch)
                        {
                            if ((res = split_edge(ce, ct->v[2])) != STATUS_OK) // Need to perform only split of crossing edge
                                return res;
                        }
                        TRACE_CONFLICT_AFTER;
                        continue;

                    case 0x19: // Point matches edges 0 and 2 (vertex 0)
                        TRACE_CONFLICT_BEFORE;
                        ce->itag    = ct->itag;
                        if (!touch)
                        {
                            if ((res = split_edge(ce, ct->v[0])) != STATUS_OK) // Need to perform only split of crossing edge
                                return res;
                        }
                        TRACE_CONFLICT_AFTER;
                        continue;

                    case 0x25: // Point matches edges 0 and 1 (vertex 1)
                        TRACE_CONFLICT_BEFORE;
                        ce->itag    = ct->itag;
                        if (!touch)
                        {
                            if ((res = split_edge(ce, ct->v[1])) != STATUS_OK) // Need to perform only split of crossing edge
                                return res;
                        }
                        TRACE_CONFLICT_AFTER;
                        continue;

                    case 0x1a: // Point lays on edge 2, split triangle's edge
                        TRACE_CONFLICT_BEFORE;
                        ce->itag    = ct->itag;
                        if ((spp = vertex.alloc(&sp)) == NULL)
                            return STATUS_NO_MEM;
                        if ((res = split_edge(ct->e[2], spp)) == STATUS_OK)
                            res     = split_edge(ce, spp);
                        TRACE_CONFLICT_AFTER;
                        break;

                    case 0x26: // Point lays on edge 1, split triangle's edge
                        TRACE_CONFLICT_BEFORE;
                        ce->itag    = ct->itag;
                        if ((spp = vertex.alloc(&sp)) == NULL)
                            return STATUS_NO_MEM;
                        if ((res = split_edge(ct->e[1], spp)) == STATUS_OK)
                            res     = split_edge(ce, spp);
                        TRACE_CONFLICT_AFTER;
                        break;

                    case 0x29: // Point lays on edge 0, split triangle's edge
                        TRACE_CONFLICT_BEFORE;
                        ce->itag    = ct->itag;
                        if ((spp = vertex.alloc(&sp)) == NULL)
                            return STATUS_NO_MEM;
                        if ((res = split_edge(ct->e[0], spp)) == STATUS_OK)
                            res     = split_edge(ce, spp);
                        TRACE_CONFLICT_AFTER;
                        break;

                    case 0x2a: // Point lays inside of the triangle, split triangle's edge
                        ce->itag    = ct->itag;
                        TRACE_CONFLICT_BEFORE;
                        if ((spp = vertex.alloc(&sp)) == NULL)
                            return STATUS_NO_MEM;
                        if ((res = split_triangle(ct, spp)) == STATUS_OK)
                            res     = split_edge(ce, spp);
                        TRACE_CONFLICT_AFTER;
                        break;

                    default: // Point is not crossing triangle
                        continue;
                }

                // Check final result
                if (res != STATUS_OK)
                    return res;

                // Current triangle's structure has been modified, update split planes' equations
                dsp::calc_plane_v1p2(&spl[0], &pl, ct->v[0], ct->v[1]);
                dsp::calc_plane_v1p2(&spl[1], &pl, ct->v[1], ct->v[2]);
                dsp::calc_plane_v1p2(&spl[2], &pl, ct->v[2], ct->v[0]);
            RT_FOREACH_END
        }

        RT_VALIDATE
        (
            if (!validate())
                return STATUS_CORRUPTED;
        );

        return STATUS_OK;
    }

    ssize_t rt_mesh_t::linked_count(rtm_triangle_t *t, rtm_edge_t *e)
    {
        if ((t == NULL) || (e == NULL))
            return -1;

        size_t n = 0;
        for (rtm_triangle_t *p = e->vt; p != NULL; )
        {
            if ((p->e[0] == p->e[1]) || (p->e[0] == p->e[2]) || (p->e[1] == p->e[2]))
                return -1;

            if (p == t)
                ++n;

            if (p->e[0] == e)
                p = p->elnk[0];
            else if (p->e[1] == e)
                p = p->elnk[1];
            else if (p->e[2] == e)
                p = p->elnk[2];
            else
                return -1;
        }

        return n;
    }

    bool rt_mesh_t::validate_list(rtm_edge_t *e)
    {
        rtm_triangle_t *t = e->vt;
        size_t n = 0;

        while (t != NULL)
        {
            if (!triangle.validate(t))
                return false;

            ++n;
            if (t->e[0] == e)
                t   = t->elnk[0];
            else if (t->e[1] == e)
                t   = t->elnk[1];
            else if (t->e[2] == e)
                t   = t->elnk[2];
            else
                return false;
        }

        if (n <= 0)
        {
            lsp_trace("Edge has no link with triangle");
        }

        return n > 0; // The edge should be linked at least to one triangle
    }

    status_t rt_mesh_t::copy(rt_mesh_t *src)
    {
        Allocator3D<rtm_vertex_t>   vertex(1024);
        Allocator3D<rtm_edge_t>     edge(1024);
        Allocator3D<rtm_triangle_t> triangle(1024);

        // Clone vertexes
        RT_FOREACH(rtm_vertex_t, sv, src->vertex)
            rtm_vertex_t *dv = vertex.alloc(sv);
            if (dv == NULL)
                return STATUS_NO_MEM;

            sv->ptag    = dv;
            dv->ptag    = sv;
        RT_FOREACH_END

        // Clone edges
        RT_FOREACH(rtm_edge_t, se, src->edge)
            rtm_edge_t *de = edge.alloc(se);
            if (de == NULL)
                return STATUS_NO_MEM;

            se->ptag    = de;
            de->ptag    = se;
            de->vt      = NULL;
        RT_FOREACH_END

        // Clone triangles
        RT_FOREACH(rtm_triangle_t, st, src->triangle)
            rtm_triangle_t *dt = triangle.alloc(st);
            if (dt == NULL)
                return STATUS_NO_MEM;

            st->ptag    = dt;
            dt->ptag    = st;
            dt->elnk[0] = NULL;
            dt->elnk[1] = NULL;
            dt->elnk[2] = NULL;
        RT_FOREACH_END;

        // Patch edge structures and link to vertexes
        RT_FOREACH(rtm_edge_t, ex, edge)
            rtm_edge_t *se   = reinterpret_cast<rtm_edge_t *>(ex->ptag);
            if (se == NULL) // Edge does not need patching
                continue;

            // Patch vertex pointers if needed
            ex->v[0]        = (se->v[0]->ptag != NULL) ? reinterpret_cast<rtm_vertex_t *>(se->v[0]->ptag) : se->v[0];
            ex->v[1]        = (se->v[1]->ptag != NULL) ? reinterpret_cast<rtm_vertex_t *>(se->v[1]->ptag) : se->v[1];
        RT_FOREACH_END

        // Link triangle structures to edges
        RT_FOREACH(rtm_triangle_t, tx, triangle)
            rtm_triangle_t *st  = reinterpret_cast<rtm_triangle_t *>(tx->ptag);
            if (st == NULL) // Triangle does not need patching
                continue;

            tx->v[0]            = reinterpret_cast<rtm_vertex_t *>(st->v[0]->ptag);
            tx->v[1]            = reinterpret_cast<rtm_vertex_t *>(st->v[1]->ptag);
            tx->v[2]            = reinterpret_cast<rtm_vertex_t *>(st->v[2]->ptag);

            tx->e[0]            = reinterpret_cast<rtm_edge_t *>(st->e[0]->ptag);
            tx->e[1]            = reinterpret_cast<rtm_edge_t *>(st->e[1]->ptag);
            tx->e[2]            = reinterpret_cast<rtm_edge_t *>(st->e[2]->ptag);

            // Link triangle to the edge
            tx->elnk[0]         = tx->e[0]->vt;
            tx->elnk[1]         = tx->e[1]->vt;
            tx->elnk[2]         = tx->e[2]->vt;

            tx->e[0]->vt        = tx;
            tx->e[1]->vt        = tx;
            tx->e[2]->vt        = tx;
        RT_FOREACH_END

        // Perform data swap
        vertex.swap(&this->vertex);
        edge.swap(&this->edge);
        triangle.swap(&this->triangle);

        RT_VALIDATE(
            if (!src->validate())
                return STATUS_CORRUPTED;
            if (!validate())
                return STATUS_CORRUPTED;
        );

        return STATUS_OK;
    }

    bool rt_mesh_t::validate()
    {
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rtm_vertex_t *v      = vertex.get(i);
            if (v == NULL)
                return false;
        }

        for (size_t i=0, n=edge.size(); i<n; ++i)
        {
            rtm_edge_t *e        = edge.get(i);
            if (e == NULL)
                return false;
            if (!validate_list(e))
                return false;

            for (size_t j=0; j<2; ++j)
            {
                if (e->v[j] == NULL)
                    return false;
                if (!vertex.validate(e->v[j]))
                    return false;
            }
        }

        for (size_t i=0, n=triangle.size(); i<n; ++i)
        {
            rtm_triangle_t *t    = triangle.get(i);
            if (t == NULL)
                return false;

            for (size_t j=0; j<3; ++j)
            {
                if (t->v[j] == NULL)
                    return false;
                if (t->e[j] == NULL)
                    return false;
                if (!vertex.validate(t->v[j]))
                    return false;
                if (!edge.validate(t->e[j]))
                    return false;
                if (!triangle.validate(t->elnk[j]))
                    return false;
                if (linked_count(t, t->e[j]) != 1)
                    return false;
            }
        }

        return true;
    }
}
