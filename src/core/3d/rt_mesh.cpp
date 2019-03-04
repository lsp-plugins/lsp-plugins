/*
 * rt_mesh.cpp
 *
 *  Created on: 26 февр. 2019 г.
 *      Author: sadko
 */

#include <core/3d/common.h>
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
        material.flush();
    }

    rtm_vertex_t *rt_mesh_t::add_unique_vertex(const point3d_t *p)
    {
        // Try to find already existing vertex
        RT_FOREACH(rtm_vertex_t, v, vertex)
            float d = dsp::calc_distance_p2(p, v);
            if (d < DSP_3D_TOLERANCE)
                return v;
        RT_FOREACH_END

        // Vertex not found, allocate new one
        rtm_vertex_t *v = vertex.alloc();
        if (v != NULL)
        {
            v->x        = p->x;
            v->y        = p->y;
            v->z        = p->z;
            v->w        = p->w;
            v->ve       = NULL;
            v->ptag     = NULL;
            v->itag     = 0;
        }

        return v;
    }

    rtm_edge_t *rt_mesh_t::add_unique_edge(rtm_vertex_t *v1, rtm_vertex_t *v2)
    {
        // Try to find the same edge
        RT_FOREACH(rtm_edge_t, e, edge)
            if ((e->v[0] == v1) && (e->v[1] == v2))
                return e;
            else if ((e->v[1] == v1) && (e->v[0] == v2))
                return e;
        RT_FOREACH_END

        // Allocate new edge
        rtm_edge_t *e = edge.alloc();
        if (e != NULL)
        {
            e->v[0]     = v1;
            e->v[1]     = v2;
            e->vt       = NULL;
            e->ptag     = NULL;
            e->itag     = 0;

            e->vlnk[0]  = v1->ve;   // Link
            v1->ve      = e;        // Link
            e->vlnk[1]  = v2->ve;   // Link
            v2->ve      = e;        // Link
        }
        return e;
    }

    static const color3d_t *item_color(ssize_t itag)
    {
        switch (itag)
        {
            case 0x1: return &C_GREEN;  // Mesh
            case 0x2: return &C_RED;    // Object
            case 0x3: return &C_BLUE;   // Common
            default: break;
        }
        return &C_CYAN;
    }

    status_t rt_mesh_t::copy_object_data(Object3D *obj, ssize_t oid, const matrix3d_t *transform, rt_material_t *material)
    {
        // Cleanup object tags to prevent data corruption
        for (size_t i=0, n=obj->num_triangles(); i<n; ++i)
        {
            obj_triangle_t *ot  = obj->triangle(i);

            ot->e[0]->ptag      = NULL;
            ot->e[1]->ptag      = NULL;
            ot->e[2]->ptag      = NULL;

            ot->v[0]->ptag      = NULL;
            ot->v[1]->ptag      = NULL;
            ot->v[2]->ptag      = NULL;

            ot->ptag            = NULL;
        }

        // Clone triangles and apply object matrix to vertexes
        for (size_t i=0, n=obj->num_triangles(); i<n; ++i)
        {
            obj_triangle_t *st = obj->triangle(i);
            if (st->ptag != NULL) // Skip already emitted triangle
                continue;

            // Allocate triangle and store pointer
            rtm_triangle_t *dt = triangle.alloc();
            if (dt == NULL)
                return STATUS_NO_MEM;

            dt->oid     = oid;
            dt->face    = st->face;

            dt->ptag    = st;
            dt->m       = material;

            // Copy vertex data
            for (size_t j=0; j<3; ++j)
            {
                rtm_vertex_t *vx     = reinterpret_cast<rtm_vertex_t *>(st->v[j]->ptag);
                if (!vx)
                {
                    // Compute the location of vertex and add this vertex to unique list
                    point3d_t vp;
                    dsp::apply_matrix3d_mp2(&vp, st->v[j], transform);

                    if (!(vx = add_unique_vertex(&vp)))
                        return STATUS_NO_MEM;

                    st->v[j]->ptag  = vx;
                }
                vx->itag       |= 0x2;  // Vertex is part of the object
                dt->v[j]        = vx;
            }

            // Copy edge data
            for (size_t j=0; j<3; ++j)
            {
                rtm_edge_t *ex       = reinterpret_cast<rtm_edge_t *>(st->e[j]->ptag);
                if (!ex)
                {
                    rtm_vertex_t   *v1 = reinterpret_cast<rtm_vertex_t *>(st->e[j]->v[0]->ptag);
                    rtm_vertex_t   *v2 = reinterpret_cast<rtm_vertex_t *>(st->e[j]->v[1]->ptag);

                    if (!(ex = add_unique_edge(v1, v2)))
                        return STATUS_NO_MEM;

//                    ex->itag        = (ex->itag >= 1) ? 1 : 0;
                    st->e[j]->ptag  = ex;
                }

                ex->itag       |= 0x2;              // Edge is part of the object
                dt->e[j]        = ex;
                dt->elnk[j]     = ex->vt;           // Link
                ex->vt          = dt;               // Link
            }

            // Compute plane equation and store as normal
            dsp::calc_plane_p3(&dt->n, dt->v[0], dt->v[1], dt->v[2]);

            // Estimate the itag state of the triangle
            dt->itag   |= 0x2;
//            if (dt->v[0]->itag != 1)
//                dt->itag    = dt->v[0]->itag;
//            else if (dt->v[1]->itag != 1)
//                dt->itag    = dt->v[1]->itag;
//            else
//                dt->itag    = dt->v[2]->itag;
        }

//        RT_VALIDATE(
            if (!this->validate())
                return STATUS_BAD_STATE;
//        )
        RT_TRACE_BREAK(debug,
            lsp_trace("State of triangles after copy()");
            for (size_t i=0,n=vertex.size(); i<n; ++i)
            {
                rtm_vertex_t *v = vertex.get(i);
                view->add_point(v, item_color(v->itag));
            }

            for (size_t i=0,n=edge.size(); i<n; ++i)
            {
                rtm_edge_t *e = edge.get(i);
                view->add_segment(e, &C_YELLOW);
            }

            for (size_t i=0,n=triangle.size(); i<n; ++i)
            {
                rtm_triangle_t *t = triangle.get(i);
                view->add_triangle_1c(t, (t->oid == oid) ? &C_RED : &C_GREEN);
            }
        );


        return STATUS_OK;
    }

    status_t rt_mesh_t::split_edge(rtm_edge_t* e, rtm_vertex_t* sp)
    {
        status_t res;
        rtm_triangle_t *ct, *nt, *pt;
        rtm_edge_t *ne, *se;

        // Check that there is something to do
        if ((e->v[0] == sp) || (e->v[1] == sp))
            return STATUS_OK;

        // Rearrange first triangle
        if ((ct = e->vt) == NULL)
            return STATUS_OK;
        res             = arrange_triangle(ct, e);
        if (res != STATUS_OK)
            return res;

        // Allocate additional edge
        if (!(ne = add_unique_edge(sp, e->v[1])))
            return STATUS_NO_MEM;

        // Re-link v[1] of current edge to the sp (split point)
        if (!unlink_edge(e, e->v[1]))
            return STATUS_CORRUPTED;
        e->v[1]         = sp;
        e->vlnk[1]      = sp->ve;       // Link
        sp->ve          = e;            // Link

        // Initialize culled edge and link to corresponding vertexes
        ne->itag        = e->itag;

        RT_VALIDATE(
            if ((ne->v[0] == NULL) || (ne->v[1] == NULL))
                return STATUS_CORRUPTED;
        )

        // Process all triangles that are linked to the current edge
        while (true)
        {
            // Save pointer to triangle to move forward
            pt              = ct->elnk[0];  // Save pointer to pending triangle, splitting edge is always rearranged to have index 0

            // Allocate triangle and splitting edge
            if (!(nt = triangle.alloc()))
                return STATUS_NO_MEM;
            if (!(se = add_unique_edge(ct->v[2], sp)))
                return STATUS_NO_MEM;
            se->itag            = ct->itag;

            // Unlink current triangle from all edges
            if (!unlink_triangle(ct, ct->e[0]))
                return STATUS_CORRUPTED;
            if (!unlink_triangle(ct, ct->e[1]))
                return STATUS_CORRUPTED;
            if (!unlink_triangle(ct, ct->e[2]))
                return STATUS_CORRUPTED;

            // Perform triangle re-link
            if (ct->v[0] == e->v[0])
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
            else // if (ct->v[0] == e->v[1])
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

            // Update triangle tags
            if (ct->e[0]->itag != 1)
                ct->itag        = ct->e[0]->itag;
            else if (ct->e[1]->itag != 1)
                ct->itag        = ct->e[1]->itag;
            else
                ct->itag        = ct->e[2]->itag;

            if (nt->e[0]->itag != 1)
                nt->itag        = nt->e[0]->itag;
            else if (nt->e[1]->itag != 1)
                nt->itag        = nt->e[1]->itag;
            else
                nt->itag        = nt->e[2]->itag;

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
        // Check that there is something to do
        if ((t->v[0] == sp) || (t->v[1] == sp) || (t->v[2] == sp))
            return STATUS_OK;

        // Unlink triangle from all it's edges
        unlink_triangle(t, t->e[0]);
        unlink_triangle(t, t->e[1]);
        unlink_triangle(t, t->e[2]);

        // Create additional edges and link them to vertexes
        rtm_edge_t *ne[3];
        for (size_t i=0; i<3; ++i)
        {
            if (!(ne[i] = add_unique_edge(t->v[i], sp)))
                return STATUS_NO_MEM;
            ne[i]->itag     = t->v[i]->itag;
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

    status_t rt_mesh_t::solve_conflicts(ssize_t oid)
    {
        status_t res;
        vector3d_t pl;
        vector3d_t spl[3]; // Scissor planes
        float k[3];
        ssize_t l[3];

        for (size_t i=0; i<triangle.size(); ++i)
        {
            rtm_triangle_t *ct   = triangle.get(i);

            dsp::calc_plane_p3(&pl, ct->v[0], ct->v[1], ct->v[2]);
            dsp::calc_plane_v1p2(&spl[0], &pl, ct->v[0], ct->v[1]);
            dsp::calc_plane_v1p2(&spl[1], &pl, ct->v[1], ct->v[2]);
            dsp::calc_plane_v1p2(&spl[2], &pl, ct->v[2], ct->v[0]);

            // Split each edge with triangle, do not process new edges
            RT_FOREACH(rtm_edge_t, ce, edge)
                // Skip edge of the current triangle
                if ((ce == ct->e[0]) || (ce == ct->e[1]) || (ce == ct->e[2]))
                    continue;
                if ((ce->v[0] == ct->v[0]) || (ce->v[0] == ct->v[1]) || (ce->v[0] == ct->v[2]))
                    continue;
                if ((ce->v[1] == ct->v[0]) || (ce->v[1] == ct->v[1]) || (ce->v[1] == ct->v[2]))
                    continue;

                // Check co-location of edge points and triangle
                k[0]        = ce->v[0]->x*pl.dx + ce->v[0]->y*pl.dy + ce->v[0]->z*pl.dz + pl.dw;
                k[1]        = ce->v[1]->x*pl.dx + ce->v[1]->y*pl.dy + ce->v[1]->z*pl.dz + pl.dw;

                l[0]        = (k[0] <= -DSP_3D_TOLERANCE) ? 2 : (k[0] > DSP_3D_TOLERANCE) ? 0 : 1;
                l[1]        = (k[1] <= -DSP_3D_TOLERANCE) ? 2 : (k[1] > DSP_3D_TOLERANCE) ? 0 : 1;
//                etag        = (ct->oid == oid) ? 0x2 : 0x1; // edge tag mask

                // Ensure that edge intersects the plane
                rtm_vertex_t xsp, *sp;

                switch ((l[1] << 2) | l[0])
                {
                    case 0x00: case 0x0a:   // Edge is over the plane or under the plane, skip
                        continue;
                    case 0x01: case 0x09: // Edge touches the plane with p[0]
                        sp      = ce->v[0];
                        break;
                    case 0x04: case 0x06: // Edge touches the plane with p[1]
                        sp      = ce->v[1];
                        break;
                    case 0x02: case 0x08: // Edge is crossing the plane
                        sp      = &xsp;
                        dsp::calc_split_point_p2v1(sp, ce->v[0], ce->v[1], &pl); // Compute split point
                        break;

                    case 0x05:              // Edge lays on the plane, skip
                    default:
                        continue;
                }
//                sp.itag     = (ce->itag == ct->itag) ? ce->itag : 1;        // Common edge
                xsp.itag    = ce->itag | ct->itag;
                xsp.ptag    = NULL;

                // Now we need to check that intersection point lays on the triangle
                k[0]        = sp->x*spl[0].dx + sp->y*spl[0].dy + sp->z*spl[0].dz + spl[0].dw;
                k[1]        = sp->x*spl[1].dx + sp->y*spl[1].dy + sp->z*spl[1].dz + spl[1].dw;
                k[2]        = sp->x*spl[2].dx + sp->y*spl[2].dy + sp->z*spl[2].dz + spl[2].dw;

                l[0]        = (k[0] <= -DSP_3D_TOLERANCE) ? 2 : (k[0] > DSP_3D_TOLERANCE) ? 0 : 1;
                l[1]        = (k[1] <= -DSP_3D_TOLERANCE) ? 2 : (k[1] > DSP_3D_TOLERANCE) ? 0 : 1;
                l[2]        = (k[2] <= -DSP_3D_TOLERANCE) ? 2 : (k[2] > DSP_3D_TOLERANCE) ? 0 : 1;

#define TRACE_COMMON_STATE \
                RT_TRACE_BREAK(debug, \
                    lsp_trace("split point={%f, %f, %f, %f}", sp->x, sp->y, sp->z, sp->w); \
                    view->add_triangle_1c(ct, &C_GREEN); \
                    \
                    for (size_t i=0,n=vertex.size(); i<n; ++i) \
                    { \
                        rtm_vertex_t *v = vertex.get(i); \
                        view->add_point(v, item_color(v->itag)); \
                    } \
                    for (size_t i=0,n=edge.size(); i<n; ++i) \
                    { \
                        rtm_edge_t *e = edge.get(i); \
                        view->add_segment(e->v[0], e->v[1], (e == ce) ? &C_YELLOW : item_color(ce->itag)); \
                    } \
                    view->add_point(sp, &C_YELLOW); \
                );

                switch ((l[0]) | (l[1] << 2) | (l[2] << 4))
                {
                    case 0x16: // Point matches edges 1 and 2 (vertex 2)
                        TRACE_COMMON_STATE;
                        ct->v[2]->itag = xsp.itag;
                        if ((res = split_edge(ce, ct->v[2])) != STATUS_OK)  // Need to perform only split of crossing edge
                            return res;
                        continue;

                    case 0x19: // Point matches edges 0 and 2 (vertex 0)
                        TRACE_COMMON_STATE;
                        ct->v[0]->itag = xsp.itag;
                        if ((res = split_edge(ce, ct->v[0])) != STATUS_OK)  // Need to perform only split of crossing edge
                            return res;
                        continue;

                    case 0x25: // Point matches edges 0 and 1 (vertex 1)
                        TRACE_COMMON_STATE;
                        ct->v[1]->itag = xsp.itag;
                        if ((res = split_edge(ce, ct->v[1])) != STATUS_OK)  // Need to perform only split of crossing edge
                            return res;
                        continue;

                    case 0x1a: // Point lays on edge 2, split triangle's edge
                        TRACE_COMMON_STATE;
                        if (!(sp = add_unique_vertex(sp)))
                            return STATUS_NO_MEM;
                        sp->itag        = xsp.itag;
                        if ((res = split_edge(ct->e[2], sp)) == STATUS_OK)
                            res = split_edge(ce, sp);
                        break;

                    case 0x26: // Point lays on edge 1, split triangle's edge
                        TRACE_COMMON_STATE;
                        if (!(sp = add_unique_vertex(sp)))
                            return STATUS_NO_MEM;
                        sp->itag        = xsp.itag;
                        if ((res = split_edge(ct->e[1], sp)) == STATUS_OK)
                            res = split_edge(ce, sp);
                        break;

                    case 0x29: // Point lays on edge 0, split triangle's edge
                        TRACE_COMMON_STATE;
                        if (!(sp = add_unique_vertex(sp)))
                            return STATUS_NO_MEM;
                        sp->itag        = xsp.itag;
                        if ((res = split_edge(ct->e[0], sp)) == STATUS_OK)
                            res = split_edge(ce, sp);
                        break;

                    case 0x2a: // Point lays inside of the triangle, split triangle's edge
                        TRACE_COMMON_STATE;
                        if (!(sp = add_unique_vertex(sp)))
                            return STATUS_NO_MEM;
                        sp->itag        = xsp.itag;
                        if ((res = split_triangle(ct, sp)) == STATUS_OK)
                            res = split_edge(ce, sp);
                        break;

                    default: // Point is not crossing triangle
                        continue;
                }

                RT_TRACE_BREAK(debug,
                    lsp_trace("State of triangles after solve_conflicts()");
                    for (size_t i=0,n=vertex.size(); i<n; ++i)
                    {
                        rtm_vertex_t *v = vertex.get(i);
                        view->add_point(v, item_color(v->itag));
                    }

                    for (size_t i=0,n=edge.size(); i<n; ++i)
                    {
                        rtm_edge_t *e = edge.get(i);
                        view->add_segment(e, item_color(e->itag));
                    }

                    for (size_t i=0,n=triangle.size(); i<n; ++i)
                    {
                        rtm_triangle_t *t = triangle.get(i);
                        view->add_triangle_1c(t, (t->oid == oid) ? &C_RED : &C_GREEN);
                    }
                );

                // Check final result
                if (res != STATUS_OK)
                    return res;

                // Current triangle's structure has been modified, update split planes' equations
                dsp::calc_plane_v1p2(&spl[0], &pl, ct->v[0], ct->v[1]);
                dsp::calc_plane_v1p2(&spl[1], &pl, ct->v[1], ct->v[2]);
                dsp::calc_plane_v1p2(&spl[2], &pl, ct->v[2], ct->v[0]);
            RT_FOREACH_END
        }

        RT_TRACE_BREAK(debug,
            lsp_trace("State of triangles after solve_conflicts()");
            for (size_t i=0,n=vertex.size(); i<n; ++i)
            {
                rtm_vertex_t *v = vertex.get(i);
                view->add_point(v, item_color(v->itag));
            }

            for (size_t i=0,n=edge.size(); i<n; ++i)
            {
                rtm_edge_t *e = edge.get(i);
                view->add_segment(e, item_color(e->itag));
            }

            for (size_t i=0,n=triangle.size(); i<n; ++i)
            {
                rtm_triangle_t *t = triangle.get(i);
                view->add_triangle_1c(t, (t->oid == oid) ? &C_RED : &C_GREEN);
            }
        );

        return STATUS_OK;
    }

    status_t rt_mesh_t::solve_vertex_edge_conflicts()
    {
        // Step 1: split edges with points laying on them
        float el, d[2], xd;
        rtm_edge_t *ce;
        status_t res;

        for (size_t i=0,n=edge.size(); i<n; ++i)
        {
            ce      = edge.get(i);
            el      = dsp::calc_distance_p2(ce->v[0], ce->v[1]);

            RT_FOREACH(rtm_vertex_t, cv, vertex)
                if ((cv == ce->v[0]) || (cv == ce->v[1]))
                    continue;

                RT_TRACE_BREAK(debug,
                    lsp_trace("Testing remove_colinear_edges()");
                    view->add_point(cv, &C_YELLOW);
                    view->add_segment(ce, &C_YELLOW);
                );

                d[0]    = dsp::calc_distance_p2(cv, ce->v[0]);
                d[1]    = dsp::calc_distance_p2(cv, ce->v[1]);
                xd      = el - d[0] - d[1];

                if (d[0] < DSP_3D_TOLERANCE) // Point p[0] matches cv
                {
                    cv->itag   |= ce->itag;
                    continue;
                }
                else if (d[1] < DSP_3D_TOLERANCE) // Point p[1] matches cv
                {
                    cv->itag   |= ce->itag;
                    continue;
                }
                else if ((xd >= -DSP_3D_TOLERANCE) && (xd <= DSP_3D_TOLERANCE)) // Point cv lays on the edge
                {
                    cv->itag   |= ce->itag;
                    if ((res = split_edge(ce, cv)) != STATUS_OK)
                        return res;
                }
                else
                    continue;

                RT_TRACE_BREAK(debug,
                    lsp_trace("After remove_colinear_edges()");
                    for (size_t i=0,n=vertex.size(); i<n; ++i)
                    {
                        rtm_vertex_t *v = vertex.get(i);
                        view->add_point(v, item_color(v->itag));
                    }

                    for (size_t i=0,n=edge.size(); i<n; ++i)
                    {
                        rtm_edge_t *e = edge.get(i);
                        view->add_segment(e, item_color(e->itag));
                    }
                );

            RT_FOREACH_END
        }

//        RT_VALIDATE(
            if (!validate())
                return STATUS_CORRUPTED;
//        )

        return STATUS_OK;
    }

    status_t rt_mesh_t::remove_duplicate_edges()
    {
        Allocator3D<rtm_edge_t> xedge(edge.chunk_size());

//        RT_VALIDATE(
            if (!validate())
                return STATUS_CORRUPTED;
//        )

        // Generate unique set of non-duplicate edges
        RT_FOREACH(rtm_edge_t, ce, edge)
            ce->ptag    = NULL;

            // Similar edge was already processed?
            RT_FOREACH(rtm_edge_t, se, xedge)
                if (((ce->v[0] == se->v[0]) && (ce->v[1] == se->v[1])) ||
                    ((ce->v[1] == se->v[0]) && (ce->v[0] == se->v[1])))
                {
                    ce->ptag    = se;
                    RT_FOREACH_BREAK;
                }
            RT_FOREACH_END

            if (ce->ptag != NULL)
            {
                RT_VALIDATE(
                    if (linked_count(ce, ce->v[0]) > 1)
                        return STATUS_CORRUPTED;
                    if (linked_count(ce, ce->v[1]) > 1)
                        return STATUS_CORRUPTED;
                )
                if (!unlink_edge(ce, ce->v[0]))
                    return STATUS_CORRUPTED;
                if (!unlink_edge(ce, ce->v[1]))
                    return STATUS_CORRUPTED;
                continue; // No not add CE to new list of edges
            }

            // Allocate new edge and store as replacement
            rtm_edge_t *ne  = xedge.alloc(ce);
            if (!ne)
                return STATUS_NO_MEM;
            ce->ptag        = ne;
        RT_FOREACH_END

        // Nothing has changed?
        if (xedge.size() >= edge.size())
            return STATUS_OK;

        // Patch vertex pointers
        RT_FOREACH(rtm_vertex_t, v, vertex)
            v->ve           = (v->ve != NULL) ? reinterpret_cast<rtm_edge_t *>(v->ve->ptag) : NULL;
        RT_FOREACH_END

        // Patch edge pointers
        RT_FOREACH(rtm_edge_t, e, edge)
            e->vlnk[0]      = (e->vlnk[0] != NULL) ? reinterpret_cast<rtm_edge_t *>(e->vlnk[0]->ptag) : NULL;
            e->vlnk[1]      = (e->vlnk[1] != NULL) ? reinterpret_cast<rtm_edge_t *>(e->vlnk[1]->ptag) : NULL;
        RT_FOREACH_END

        // Patch triangle pointers
        RT_FOREACH(rtm_triangle_t, t, triangle)
            t->e[0]         = reinterpret_cast<rtm_edge_t *>(t->e[0]->ptag);
            t->e[1]         = reinterpret_cast<rtm_edge_t *>(t->e[1]->ptag);
            t->e[2]         = reinterpret_cast<rtm_edge_t *>(t->e[2]->ptag);
        RT_FOREACH_END

        xedge.swap(&edge);

//        RT_VALIDATE(
            if (!validate())
                return STATUS_CORRUPTED;
//        )

        return STATUS_OK;
    }

    status_t rt_mesh_t::paint_triangles_internal(ssize_t oid)
    {
        return STATUS_OK;
/*
        size_t total            = 0;
        size_t pending          = (triangle.size() + 0x3f) & (~0x3f);
        rtm_triangle_t **vt     = reinterpret_cast<rtm_triangle_t **>(::malloc(pending * sizeof(rtm_triangle_t *)));

        // Fill the array with pointers
        RT_FOREACH(rtm_triangle_t, t, triangle)
            vt[total++]             = t;
        RT_FOREACH_END

        pending     = total;

        while (total > 0)
        {
            for (size_t i=0; i<pending; )
            {
                rtm_triangle_t *ct  = vt[i];
//                ssize_t itag        = (ct->oid == oid) ? 0 : 2;

                if ((ct->e[0]->itag == 0x3) && (ct->e[1]->itag == 0x3))
                    ct->itag        = 0x3;
                else if ((ct->e[1]->itag == 0x3) && (ct->e[2]->itag == 0x3))
                    ct->itag        = 0x3;
                else if ((ct->e[2]->itag == 0x3) && (ct->e[0]->itag == 0x3))
                    ct->itag        = 0x3;
                lsp_trace("ct[%d]->itag = { 0x%x, 0x%x, 0x%x } -> 0x%x",
                        int(i), int(ct->e[0]->itag), int(ct->e[1]->itag), int(ct->e[2]->itag),
                        int(ct->itag)
                    );

                if ((ct->itag == 0x3) && ((pending--) > 0))
                    vt[i]           = vt[pending]; // remove from pending
                else
                    ++i;
            }

            if (pending >= total)   // Nothing has changed?
                break;
            total   = pending;
        }

        // Free the queue
        ::free(vt);

        RT_TRACE_BREAK(debug,
            lsp_trace("State of triangles after paint()");
            for (size_t i=0,n=vertex.size(); i<n; ++i)
            {
                rtm_vertex_t *v = vertex.get(i);
                view->add_point(v, item_color(v->itag));
            }

            for (size_t i=0,n=edge.size(); i<n; ++i)
            {
                rtm_edge_t *e = edge.get(i);
                view->add_segment(e, item_color(e->itag));
            }

            for (size_t i=0,n=triangle.size(); i<n; ++i)
            {
                rtm_triangle_t *t = triangle.get(i);
                view->add_triangle_1c(t, item_color(t->itag));
            }
        );

        return (total < triangle.size()) ? STATUS_OK : STATUS_NOT_FOUND;
        */
    }

    rt_material_t  *rt_mesh_t::build_material(rt_material_t *from, rt_material_t *to)
    {
        rtm_material_t *m;

        for (size_t i=0, n=material.size(); i<n; ++i)
        {
            m   = material.get(i);
            if ((m->from == from) && (m->to == to))
                return m;
        }

        if ((m = material.add()))
        {
            m->absorption[0]    = from->absorption[1] * to->absorption[0];
            m->absorption[1]    = from->absorption[0] * to->absorption[1];
            m->dispersion[0]    = from->dispersion[1];
            m->dispersion[1]    = to->dispersion[1];
            m->dissipation[0]   = from->dissipation[1];
            m->dissipation[1]   = to->dissipation[1];

            m->transparency[0]  = (from->absorption[1] * from->transparency[1]) * to->transparency[0];
            m->transparency[1]  = (to->absorption[1] * to->transparency[1]) * from->transparency[0];
            m->permeability     = to->permeability / from->permeability;
        }

        return m;
    }

    status_t rt_mesh_t::remove_obsolete_primitives()
    {
        // TODO
        return STATUS_OK;
    }

    status_t rt_mesh_t::add_object(Object3D *obj, ssize_t oid, const matrix3d_t *transform, rt_material_t *material)
    {
        status_t res;

        // Vertex states:
        // 0 = vertex is common to the mesh and object
        // 1 = vertex is part of the mesh
        // 2 = vertex is part of the object
        // 3 = vertex is common to the mesh and object
        RT_FOREACH(rtm_vertex_t, v, vertex)
            v->itag     = 1;    // Vertex is part of the mesh
        RT_FOREACH_END

        RT_FOREACH(rtm_edge_t, e, edge)
            e->itag     = 1;    // Edge is part of the mesh
        RT_FOREACH_END

        RT_FOREACH(rtm_triangle_t, t, triangle)
            t->itag     = 1;    // Triangle is part of the mesh
        RT_FOREACH_END

        // First, copy all primitives and detect their locations
        if ((res = copy_object_data(obj, oid, transform, material)) != STATUS_OK)
            return res;

        // Split edges with points laying on them
        if ((res = solve_vertex_edge_conflicts()) != STATUS_OK)
            return res;

        // Remove duplicate edges
        if ((res = remove_duplicate_edges()) != STATUS_OK)
            return res;

        // Now we need to solve all conflicts between edges and triangles
        if ((res = solve_conflicts(oid)) != STATUS_OK)
            return res;

        // Paint triangles
        if ((res = paint_triangles_internal(oid)) != STATUS_OK)
            return (res == STATUS_NOT_FOUND) ? STATUS_OK : res;

        // Now we need to update triangle tags and materials
        RT_FOREACH(rtm_triangle_t, t, triangle)
            if ((t->oid != oid) && (t->itag == 1))
            {
                t->itag     = 2;
                t->m        = build_material(t->m, material);
                if (!t->m)
                    return STATUS_NO_MEM;
            }
        RT_FOREACH_END;

        return remove_obsolete_primitives();
    }

    /*
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
    */

    bool rt_mesh_t::unlink_edge(rtm_edge_t *e, rtm_vertex_t *v)
    {
        for (rtm_edge_t **pcurr = &v->ve; *pcurr != NULL; )
        {
            rtm_edge_t *curr    = *pcurr;
            rtm_edge_t **pnext  = (curr->v[0] == v) ? &curr->vlnk[0] :
                                  (curr->v[1] == v) ? &curr->vlnk[1] :
                                  NULL;
            if (pnext == NULL) // Unexpected behaviour
                return false;

            if (curr == e)
            {
                *pcurr = *pnext;
                return true;
            }
            pcurr = pnext;
        }
        return false;
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
        Allocator3D<rtm_vertex_t>   vertex(vertex.chunk_size());
        Allocator3D<rtm_edge_t>     edge(edge.chunk_size());
        Allocator3D<rtm_triangle_t> triangle(triangle.chunk_size());

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

    bool rt_mesh_t::validate_list(rtm_vertex_t *v)
    {
        rtm_edge_t *e = v->ve;
        size_t n = 0;

        while (e != NULL)
        {
            if (!edge.validate(e))
                return false;

            ++n;
            if (e->v[0] == v)
                e   = e->vlnk[0];
            else if (e->v[1] == v)
                e   = e->vlnk[1];
            else
                return false;
        }

        return n > 0; // The vertex should be linked at least to one edge
    }

    ssize_t rt_mesh_t::linked_count(rtm_edge_t *e, rtm_vertex_t *v)
    {
        if ((e == NULL) || (v == NULL))
            return -1;

        size_t n = 0;
        for (rtm_edge_t *p = v->ve; p != NULL; )
        {
            if (p->v[0] == p->v[1])
                return -1;
            if (p == e)
                ++n;

            if (p->v[0] == v)
                p = p->vlnk[0];
            else if (p->v[1] == v)
                p = p->vlnk[1];
            else
                return -1;
        }

        return n;
    }

    bool rt_mesh_t::validate()
    {
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rtm_vertex_t *v      = vertex.get(i);
            if (v == NULL)
                return false;
            if (!validate_list(v))
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
                if (!edge.validate(e->vlnk[j]))
                    return false;
                if (linked_count(e, e->v[j]) != 1)
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


