/*
 * reflections3d.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <test/mtest/3d/common/X11Renderer.h>
#include <core/files/Model3DFile.h>

#include <core/types.h>
#include <core/debug.h>
#include <core/sugar.h>
#include <core/status.h>
#include <stdlib.h>
#include <errno.h>
#include <data/cstorage.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <sys/poll.h>

namespace mtest
{
    using namespace lsp;

    static const color3d_t C_RED        = { 1.0f, 0.0f, 0.0f, 0.0f };
    static const color3d_t C_GREEN      = { 0.0f, 1.0f, 0.0f, 0.0f };
    static const color3d_t C_BLUE       = { 0.0f, 0.0f, 1.0f, 0.0f };
    static const color3d_t C_MAGENTA    = { 1.0f, 0.0f, 1.0f, 0.0f };
    static const color3d_t C_YELLOW     = { 1.0f, 1.0f, 0.0f, 0.0f };
    static const color3d_t C_ORANGE     = { 1.0f, 0.5f, 0.0f, 0.0f };
    static const color3d_t C_GRAY       = { 0.75f, 0.75f, 0.75f, 0.0f };

    typedef struct wfront_t
    {
        point3d_t p[3];     // Three points showing directions
        point3d_t s;        // Source point
    } wfront_t;

    typedef struct object_t
    {
        bound_box3d_t               box;      // Bounding box for each object
        size_t                      nt;     // Number of triangles
        v_triangle3d_t             *t;      // List of triangles for each object
    } object_t;

    enum context_state_t
    {
        S_SCAN_SCENE,
        S_CULL_FRONT,
        S_CULL_BACK
    };

    typedef struct context_t
    {
        wfront_t                    front;      // Wave front
        cstorage<v_triangle3d_t>    source;     // Triangles for processing
        cstorage<v_triangle3d_t>   *matched;    // List of matched triangles (for debug)
        cstorage<v_triangle3d_t>   *ignored;    // List of ignored triangles (for debug)
        cvector<object_t>          *scene;      // Overall scene
        context_state_t             state;      // Context state
    } context_t;

    static void destroy_tasks(cvector<context_t> &tasks)
    {
        for (size_t i=0, n=tasks.size(); i<n; ++i)
        {
            context_t *ctx = tasks.get(i);
            if (ctx == NULL)
                continue;

            ctx->ignored    = NULL;
            ctx->matched    = NULL;
            ctx->scene      = NULL;
            ctx->source.flush();

            delete ctx;
        }

        tasks.flush();
    }

    static void inv_normal(vector3d_t *v)
    {
        v->dx = - v->dy;
        v->dy = - v->dy;
        v->dz = - v->dz;
    }

    static void calc_plane_vector_pv(vector3d_t *v, const point3d_t *p)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = p[1].x - p[0].x;
        d[0].dy     = p[1].y - p[0].y;
        d[0].dz     = p[1].z - p[0].z;
        d[0].dw     = p[1].w - p[0].w;

        d[1].dx     = p[2].x - p[0].x;
        d[1].dy     = p[2].y - p[0].y;
        d[1].dz     = p[2].z - p[0].z;
        d[1].dw     = p[2].w - p[0].w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = + d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = - d[0].dx*d[1].dz + d[0].dz*d[1].dx;
        v->dz       = + d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        dsp::normalize_vector(v);
        v->dw       = - ( v->dx * p[0].x + v->dy * p[0].y + v->dz * p[0].z); // Parameter for the plane equation
    }

    static void calc_plane_vector_p3(vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = p1->x - p0->x;
        d[0].dy     = p1->y - p0->y;
        d[0].dz     = p1->z - p0->z;
        d[0].dw     = p1->w - p0->w;

        d[1].dx     = p2->x - p0->x;
        d[1].dy     = p2->y - p0->y;
        d[1].dz     = p2->z - p0->z;
        d[1].dw     = p2->w - p0->w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = + d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = - d[0].dx*d[1].dz + d[0].dz*d[1].dx;
        v->dz       = + d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        dsp::normalize_vector(v);
        v->dw       = - ( v->dx * p0->x + v->dy * p0->y + v->dz * p0->z); // Parameter for the plane equation
    }

    /**
     * Calculate vector that represents plane equation
     * @param v target vector to store equation
     * @param r ray representing first vector
     * @param u another vector representing plane
     */
    static void calc_plane_vector_rv(vector3d_t *v, const ray3d_t *r, const vector3d_t *u)
    {
        // Calculate edge parameters
        const vector3d_t  *w  = &r->v;
        const point3d_t *p    = &r->z;

        // Do vector multiplication to calculate the normal vector
        v->dx       = + w->dy * u->dz - w->dz*u->dy;
        v->dy       = - w->dx * u->dz + w->dz*u->dx;
        v->dz       = + w->dx * u->dy - w->dy*u->dx;
        dsp::normalize_vector(v);
        v->dw       = - ( v->dx * p->x + v->dy * p->y + v->dz * p->z); // Parameter for the plane equation
    }

    /**
     * Split triangle with plane, generates output set of triangles into out (triangles above split plane)
     * and in (triangles below split plane). For every triangle, points 1 and 2 are the points that
     * lay on the split plane, the first triangle ALWAYS has 2 common points with plane (1 and 2)
     *
     * @param out array of vertexes above plane
     * @param n_out counter of vertexes above plane (multiple of 3), should be initialized
     * @param in array of vertexes below plane
     * @param n_in counter of vertexes below plane (multiple of 3), should be iniitialized
     * @param pl plane equation
     * @param pv triangle to perform the split
     */
    static void split_triangle(
            v_triangle3d_t *out,
            size_t *n_out,
            v_triangle3d_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const v_triangle3d_t *pv
        )
    {
        point3d_t sp[2];    // Split point
        vector3d_t d[2];    // Delta vector
        point3d_t p[3];     // Triangle sources
        float k[3];         // Co-location of points
        float t[2];

        in     += *n_in;
        out    += *n_out;

        p[0]    = pv->p[0];
        p[1]    = pv->p[1];
        p[2]    = pv->p[2];

        k[0]    = pl->dx*p[0].x + pl->dy*p[0].y + pl->dz*p[0].z + pl->dw;
        k[1]    = pl->dx*p[1].x + pl->dy*p[1].y + pl->dz*p[1].z + pl->dw;
        k[2]    = pl->dx*p[2].x + pl->dy*p[2].y + pl->dz*p[2].z + pl->dw;

        // Check that the whole triangle lies above the plane or below the plane
        if (k[0] < 0.0f)
        {
            if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                in[0].p[0]      = p[0];
                in[0].p[1]      = p[1];
                in[0].p[2]      = p[2];

                in[0].n[0]      = pv->n[0];
                in[0].n[1]      = pv->n[1];
                in[0].n[2]      = pv->n[2];
                *n_in          += 1;
                return;
            }
        }
        else if (k[0] > 0.0f)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                out[0].p[0]     = p[0];
                out[0].p[1]     = p[1];
                out[0].p[2]     = p[2];

                out[0].n[0]     = pv->n[0];
                out[0].n[1]     = pv->n[1];
                out[0].n[2]     = pv->n[2];

                *n_out         += 1;
                return;
            }
        }
        else // (k[0] == 0)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                out[0].p[0]     = p[0];
                out[0].p[1]     = p[1];
                out[0].p[2]     = p[2];

                out[0].n[0]     = pv->n[0];
                out[0].n[1]     = pv->n[1];
                out[0].n[2]     = pv->n[2];

                *n_out         += 1;
                return;
            }
            else if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                in[0].p[0]      = p[0];
                in[0].p[1]      = p[1];
                in[0].p[2]      = p[2];

                in[0].n[0]      = pv->n[0];
                in[0].n[1]      = pv->n[1];
                in[0].n[2]      = pv->n[2];

                *n_in          += 1;
                return;
            }
        }

        // There is an intersection with plane, we need to analyze it
        // Rotate triangle until vertex 0 is above the split plane
        while (k[0] <= 0)
        {
            t[0]    = k[0];
            sp[0]   = p[0];

            k[0]    = k[1];
            p[0]    = p[1];
            k[1]    = k[2];
            p[1]    = p[2];
            k[2]    = t[0];
            p[2]    = sp[0];
        }

        // Now we have p[0] guaranteed to be above plane, analyze p[1] and p[2]
        if (k[1] < 0.0f) // k[1] < 0
        {
            d[0].dx = p[0].x - p[1].x;
            d[0].dy = p[0].y - p[1].y;
            d[0].dz = p[0].z - p[1].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = p[0].x + d[0].dx * t[0];
            sp[0].y = p[0].y + d[0].dy * t[0];
            sp[0].z = p[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

            if (k[2] < 0.0f) // (k[1] < 0) && (k[2] < 0)
            {
                d[1].dx = p[0].x - p[2].x;
                d[1].dy = p[0].y - p[2].y;
                d[1].dz = p[0].z - p[2].z;

                t[1]    = -k[0] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[0].x + d[1].dx * t[1];
                sp[1].y = p[0].y + d[1].dy * t[1];
                sp[1].z = p[0].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 1 triangle above plane, 2 below
                out[0].p[0]     = p[0];
                out[0].p[1]     = sp[0];
                out[0].p[2]     = sp[1];

                out[0].n[0]     = pv->n[0];
                out[0].n[1]     = pv->n[1];
                out[0].n[2]     = pv->n[2];

                *n_out         += 1;

                in[0].p[0]      = p[1];
                in[0].p[1]      = sp[1];
                in[0].p[2]      = sp[0];
                in[1].p[0]      = p[2];
                in[1].p[1]      = sp[1];
                in[1].p[2]      = p[1];

                in[0].n[0]      = pv->n[0];
                in[0].n[1]      = pv->n[1];
                in[0].n[2]      = pv->n[2];
                in[1].n[0]      = pv->n[0];
                in[1].n[1]      = pv->n[1];
                in[1].n[2]      = pv->n[2];

                *n_in          += 2;
            }
            else if (k[2] > 0.0f) // (k[1] < 0) && (k[2] > 0)
            {
                d[1].dx = p[2].x - p[1].x;
                d[1].dy = p[2].y - p[1].y;
                d[1].dz = p[2].z - p[1].z;

                t[1]    = -k[2] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[2].x + d[1].dx * t[1];
                sp[1].y = p[2].y + d[1].dy * t[1];
                sp[1].z = p[2].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 2 triangles above plane, 1 below
                out[0].p[0]     = p[2];
                out[0].p[1]     = sp[0];
                out[0].p[2]     = sp[1];
                out[1].p[0]     = p[0];
                out[1].p[1]     = sp[0];
                out[1].p[2]     = p[2];

                out[0].n[0]     = pv->n[0];
                out[0].n[1]     = pv->n[1];
                out[0].n[2]     = pv->n[2];
                out[1].n[0]     = pv->n[0];
                out[1].n[1]     = pv->n[1];
                out[1].n[2]     = pv->n[2];

                *n_out         += 2;

                in[0].p[0]      = p[1];
                in[0].p[1]      = sp[1];
                in[0].p[2]      = sp[0];

                in[0].n[0]      = pv->n[0];
                in[0].n[1]      = pv->n[1];
                in[0].n[2]      = pv->n[2];

                *n_in          += 1;
            }
            else // (k[1] < 0) && (k[2] == 0)
            {
                // 1 triangle above plane, 1 below
                out[0].p[0]     = p[0];
                out[0].p[1]     = sp[0];
                out[0].p[2]     = p[2];

                out[0].n[0]     = pv->n[0];
                out[0].n[1]     = pv->n[1];
                out[0].n[2]     = pv->n[2];
                *n_out         += 1;

                in[0].p[0]      = p[1];
                in[0].p[1]      = p[2];
                in[0].p[2]      = sp[0];

                in[0].n[0]      = pv->n[0];
                in[0].n[1]      = pv->n[1];
                in[0].n[2]      = pv->n[2];

                *n_in          += 1;
            }
        }
        else // (k[1] >= 0) && (k[2] < 0)
        {
            d[0].dx = p[0].x - p[2].x;
            d[0].dy = p[0].y - p[2].y;
            d[0].dz = p[0].z - p[2].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = p[0].x + d[0].dx * t[0];
            sp[0].y = p[0].y + d[0].dy * t[0];
            sp[0].z = p[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

            if (k[1] > 0.0f) // (k[1] > 0) && (k[2] < 0)
            {
                d[1].dx = p[1].x - p[2].x;
                d[1].dy = p[1].y - p[2].y;
                d[1].dz = p[1].z - p[2].z;

                t[1]    = -k[1] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[1].x + d[1].dx * t[1];
                sp[1].y = p[1].y + d[1].dy * t[1];
                sp[1].z = p[1].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 2 triangles above plane, 1 below
                out[0].p[0]     = p[0];
                out[0].p[1]     = sp[1];
                out[0].p[2]     = sp[0];
                out[1].p[0]     = p[1];
                out[1].p[1]     = sp[1];
                out[1].p[2]     = p[0];

                out[0].n[0]     = pv->n[0];
                out[0].n[1]     = pv->n[1];
                out[0].n[2]     = pv->n[2];
                out[1].n[0]     = pv->n[0];
                out[1].n[1]     = pv->n[1];
                out[1].n[2]     = pv->n[2];

                *n_out         += 2;

                in[0].p[0]      = p[2];
                in[0].p[1]      = sp[0];
                in[0].p[2]      = sp[1];

                in[0].n[0]      = pv->n[0];
                in[0].n[1]      = pv->n[1];
                in[0].n[2]      = pv->n[2];

                *n_in          += 1;
            }
            else // (k[1] == 0) && (k[2] < 0)
            {
                // 1 triangle above plane, 1 triangle below plane
                out[0].p[0]     = p[0];
                out[0].p[1]     = p[1];
                out[0].p[2]     = sp[0];

                out[0].n[0]     = pv->n[0];
                out[0].n[1]     = pv->n[1];
                out[0].n[2]     = pv->n[2];

                *n_out         += 1;

                in[0].p[0]      = p[2];
                in[0].p[1]      = sp[0];
                in[0].p[2]      = p[1];

                in[0].n[0]      = pv->n[0];
                in[0].n[1]      = pv->n[1];
                in[0].n[2]      = pv->n[2];

                *n_in          += 1;
            }
        }
    }

    /**
     * Project triangle to the plane
     * @param pv array of 3 points to store projected points
     * @param fp focus point, the point where all projective lines do intersect
     * @param pl plane equation vector
     * @param tv triangle points
     */
    static void project_triangle(
        point3d_t *pv,
        const point3d_t *fp,
        const vector3d_t *pl,
        const point3d_t *tv
    )
    {
        vector3d_t d[3];
        float k, t[3];

        d[0].dx     = fp->x - tv[0].x;
        d[0].dy     = fp->y - tv[0].y;
        d[0].dz     = fp->z - tv[0].z;
        d[0].dw     = 0.0f;

        d[1].dx     = fp->x - tv[1].x;
        d[1].dy     = fp->y - tv[1].y;
        d[1].dz     = fp->z - tv[1].z;
        d[1].dw     = 0.0f;

        d[2].dx     = fp->x - tv[2].x;
        d[2].dy     = fp->y - tv[2].y;
        d[2].dz     = fp->z - tv[2].z;
        d[2].dw     = 0.0f;

        k           = - (pl->dx*fp->x + pl->dy*fp->y + pl->dz*fp->z + pl->dw);
        t[0]        = k / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);
        t[1]        = k / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);
        t[2]        = k / (pl->dx*d[2].dx + pl->dy*d[2].dy + pl->dz*d[2].dz);

        pv[0].x     = fp->x + t[0] * d[0].dx;
        pv[0].y     = fp->y + t[0] * d[0].dy;
        pv[0].z     = fp->z + t[0] * d[0].dz;
        pv[0].w     = 1.0f;

        pv[1].x     = fp->x + t[1] * d[1].dx;
        pv[1].y     = fp->y + t[1] * d[1].dy;
        pv[1].z     = fp->z + t[1] * d[1].dz;
        pv[1].w     = 1.0f;

        pv[2].x     = fp->x + t[2] * d[2].dx;
        pv[2].y     = fp->y + t[2] * d[2].dy;
        pv[2].z     = fp->z + t[2] * d[2].dz;
        pv[2].w     = 1.0f;
    }

    static const size_t bbox_map[] =
    {
        0, 1, 2,
        0, 2, 3,
        6, 5, 4,
        6, 4, 7,
        1, 0, 4,
        1, 4, 5,
        3, 2, 6,
        3, 6, 7,
        1, 5, 2,
        2, 5, 6,
        0, 3, 4,
        3, 7, 4
    };

    static void destroy_scene(cvector<object_t> &list)
    {
        for (size_t i=0, n=list.size(); i<n; ++i)
        {
            object_t *obj = list.get(i);
            if (obj != NULL)
                free(obj);
        }
        list.flush();
    }

    static status_t prepare_scene(cstorage<v_triangle3d_t> &ignored, cvector<object_t> &scene, Scene3D *s)
    {
        status_t res        = STATUS_OK;
        size_t hsize        = ALIGN_SIZE(sizeof(object_t), DEFAULT_ALIGN);

        cvector<object_t> ol;

        for (size_t i=0, n=s->num_objects(); i<n; ++i)
        {
            // Get scene object
            Object3D *obj   = s->get_object(i);
            if ((obj == NULL) || (!obj->is_visible()))
                continue;

            // Add bounding box
            bound_box3d_t *bbox = s->get_bound_box(i);
            if (bbox == NULL)
                continue;

            // Initialize pointers
            matrix3d_t *om      = obj->get_matrix();
            point3d_t *tr       = obj->get_vertexes();
            vector3d_t *tn      = obj->get_normals();
            vertex_index_t *vvx = obj->get_vertex_indexes();
            vertex_index_t *vnx = obj->get_normal_indexes();

            // Allocate object descriptor
            size_t nt           = obj->get_triangles_count();
            size_t osize        = hsize + ALIGN_SIZE(sizeof(v_triangle3d_t) * nt, DEFAULT_ALIGN);
            object_t *o         = reinterpret_cast<object_t *>(malloc(osize));
            if (o == NULL)
            {
                res             = STATUS_NO_MEM;
                break;
            }
            if (!ol.add(o))
            {
                free(o);
                res             = STATUS_NO_MEM;
                break;
            }

            o->t                = reinterpret_cast<v_triangle3d_t *>(reinterpret_cast<uint8_t *>(o) + hsize);
            o->box              = *bbox;
            o->nt               = nt;

            // Apply object matrix to vertexes and produce final array
            v_triangle3d_t *t   = o->t;

            for (size_t j=0; j < nt; ++j, ++t)
            {
                dsp::apply_matrix3d_mp2(&t->p[0], &tr[*(vvx++)], om);
                dsp::apply_matrix3d_mp2(&t->p[1], &tr[*(vvx++)], om);
                dsp::apply_matrix3d_mp2(&t->p[2], &tr[*(vvx++)], om);

                dsp::apply_matrix3d_mv2(&t->n[0], &tn[*(vnx++)], om);
                dsp::apply_matrix3d_mv2(&t->n[1], &tn[*(vnx++)], om);
                dsp::apply_matrix3d_mv2(&t->n[2], &tn[*(vnx++)], om);
            }
        }

        if (res == STATUS_OK)
            ol.swap_data(&scene);

        destroy_scene(ol);
        return res;
    }

    /**
     * Scan scene for triangles laying inside the viewing area of wave front
     * @param ctx wave front context
     * @return statuf of operation
     */
    static status_t scan_scene(context_t *ctx)
    {
        // Check crossing with bounding box
        vector3d_t pl[4];
        calc_plane_vector_p3(&pl[0], &ctx->front.s, &ctx->front.p[0], &ctx->front.p[1]);
        calc_plane_vector_p3(&pl[1], &ctx->front.s, &ctx->front.p[1], &ctx->front.p[2]);
        calc_plane_vector_p3(&pl[2], &ctx->front.s, &ctx->front.p[2], &ctx->front.p[0]);
        calc_plane_vector_p3(&pl[3], &ctx->front.p[0], &ctx->front.p[1], &ctx->front.p[2]);

        v_triangle3d_t out[16], buf1[16], buf2[16], *q, *in, *tmp;
        size_t n_out, n_buf1, n_buf2, *n_q, *n_in, *n_tmp;

        // Check for crossing with all bounding boxes
        for (size_t i=0, n=ctx->scene->size(); i<n; ++i)
        {
            object_t *obj = ctx->scene->get(i);
            if (obj->nt < 16)
            {
                if (obj->nt == 0)
                    continue;

                if (!ctx->source.append(obj->t, obj->nt))
                    return STATUS_NO_MEM;
                continue;
            }

            // Cull each triangle of bounding box with four scissor planes
            for (size_t j=0, m = sizeof(bbox_map)/sizeof(size_t); j < m; )
            {
                // Initialize input and queue buffer
                q = buf1, in = buf2;
                n_q = &n_buf1, n_in = &n_buf2;

                // Put to queue with updated matrix
                *n_q        = 1;
                n_out       = 0;
                q->p[0]     = obj->box.p[bbox_map[j++]];
                q->p[1]     = obj->box.p[bbox_map[j++]];
                q->p[2]     = obj->box.p[bbox_map[j++]];

                // Cull triangle with planes
                for (size_t k=0; ; )
                {
                    // Reset counters
                    *n_in   = 0;

                    // Split all triangles:
                    // Put all triangles above the plane to out
                    // Put all triangles below the plane to in
                    for (size_t l=0; l < *n_q; ++l)
                        split_triangle(out, &n_out, in, n_in, &pl[k], &q[l]);

                    // Interrupt cycle if there is no data to process
                    if ((*n_in <= 0) || ((++k) >= 4))
                       break;

                    // Swap buffers buf0 <-> buf1
                    n_tmp = n_in, tmp = in;
                    n_in = n_q, in = q;
                    n_q = n_tmp, q = tmp;
                }

                if (*n_in > 0) // Is there intersection with bounding box?
                    break;
            }
            if (*n_in > 0)
            {
                if (!ctx->source.append(obj->t, obj->nt))
                    return STATUS_NO_MEM;
            }
            else if (!ctx->ignored->append(obj->t, obj->nt))
                return STATUS_NO_MEM;
        }

        return STATUS_OK;
    }

    static status_t cull_front(context_t *ctx)
    {
        v_triangle3d_t out[16], buf1[16], buf2[16], *q, *in, *tmp;
        size_t n_out, n_buf1, n_buf2, *n_q, *n_in, *n_tmp;
        vector3d_t pl[4];

        cstorage<v_triangle3d_t> source;
        source.swap(&ctx->source);

        calc_plane_vector_p3(&pl[0], &ctx->front.s, &ctx->front.p[0], &ctx->front.p[1]);
        calc_plane_vector_p3(&pl[1], &ctx->front.s, &ctx->front.p[1], &ctx->front.p[2]);
        calc_plane_vector_p3(&pl[2], &ctx->front.s, &ctx->front.p[2], &ctx->front.p[0]);
        calc_plane_vector_p3(&pl[3], &ctx->front.p[0], &ctx->front.p[1], &ctx->front.p[2]);

        // Cull each triangle with four scissor planes
        for (ssize_t i=0, n=source.size(); i < n; ++i)
        {
            // Initialize input and queue buffer
            q = buf1, in = buf2;
            n_q = &n_buf1, n_in = &n_buf2;

            // Put triangle to queue
            *n_q        = 1;
            n_out       = 0;
            *q          = *(source.at(i)); // Get next triangle for processing

            // Cull triangle with planes
            for (size_t k=0; ; )
            {
                // Reset counters
                *n_in   = 0;
                // Split all triangles:
                // Put all triangles above the plane to out
                // Put all triangles below the plane to in
                for (size_t l=0; l < *n_q; ++l)
                    split_triangle(out, &n_out, in, n_in, &pl[k], &q[l]);

                // Interrupt cycle if there is no data to process
                if ((*n_in <= 0) || ((++k) >= 4))
                   break;

                // Swap buffers buf0 <-> buf1
                n_tmp = n_in, tmp = in;
                n_in = n_q, in = q;
                n_q = n_tmp, q = tmp;
            }

            // Emit all triangles above the plane (outside vision) as ignored
            for (size_t l=0; l < n_out; ++l)
            {
                if (!ctx->ignored->add(&out[l]))
                    return STATUS_NO_MEM;
            }

            // The final set of triangles inside vision is in 'q' buffer, put them as visible
            for (size_t l=0; l < *n_in; ++l)
            {
                if (!ctx->source.add(&in[l]))
                    return STATUS_NO_MEM;
            }
        }

        return STATUS_OK;
    }

    static status_t split_space(cvector<context_t> &tasks, context_t *ctx, const vector3d_t *pl, bool keep)
    {
        v_triangle3d_t out[2], in[2], sout[2], sin[2], t;
        size_t n_out, n_in, n_sout, n_sin;
        vector3d_t spl, npl;

        // First, split front triangle into sub-triangles
        t.p[0] = ctx->front.p[0];
        t.p[1] = ctx->front.p[1];
        t.p[2] = ctx->front.p[2];
        dsp::init_normal3d_dxyz(&t.n[0], 0.0f, 0.0f, 0.0f);
        dsp::init_normal3d_dxyz(&t.n[0], 0.0f, 0.0f, 0.0f);
        dsp::init_normal3d_dxyz(&t.n[0], 0.0f, 0.0f, 0.0f);

        n_out   = 0;
        n_in    = 0;
        spl     = *pl;
        split_triangle(out, &n_out, in, &n_in, &spl, &t);

        // Analyze result: out and in may be in range of [0..2]
        if (out <= 0) // There are no triangles outside
            return STATUS_OK; // Nothing to do
        else if (in <= 0) // There are no triangle inside
        {
            ctx->source.clear(); // All triangles are outside
            return STATUS_OK;
        }
        else if ((n_in == 2) && (n_out == 1)) // Need to invert space ?
        {
            inv_normal(&spl);           // Invert normal of splitting plane
            // Swap state of 'in' and 'out' arrays
            t       = out[0];
            out[1]  = in[1];
            out[0]  = in[0];
            in[0]   = t;
            n_in    = 1;
            n_out   = 2;
        }

        cstorage<v_triangle3d_t> source, clipped;
        source.swap(&ctx->source);

        // Perform split using main plane
        if (keep)
        {
            for (size_t i=0, nt=source.size(); i<nt; ++i)
            {
                n_sin = 0, n_sout = 0;
                split_triangle(sout, &n_sout, sin, &n_sin, &spl, source.at(i));

                // Add generated triangles to target buffers
                for (size_t l=0; l < n_sout; ++l)
                {
                    if (!clipped.add(&sout[l]))
                        return STATUS_NO_MEM;
                }
                for (size_t l=0; l < n_sin; ++l)
                {
                    if (!ctx->source.add(&sin[l]))
                        return STATUS_NO_MEM;
                }
            }
        }
        else
        {
            for (size_t i=0, nt=source.size(); i<nt; ++i)
            {
                n_sin = 0, n_sout = 0;
                split_triangle(sout, &n_sout, sin, &n_sin, &spl, source.at(i));

                // Add generated triangles to target buffer, ignore 'out' triangles
                for (size_t l=0; l < n_sin; ++l)
                {
                    if (!ctx->source.add(&sin[l]))
                        return STATUS_NO_MEM;
                }
            }
        }
        source.flush(); // Drop all temporary data

        // Now we have triangles in ctx->source and t_out, analyze split state
        // Update triangle of current context
        ctx->front.p[0]     = in[0].p[0];
        ctx->front.p[1]     = in[0].p[1];
        ctx->front.p[2]     = in[0].p[2];

        // Is there any data for analysis?
        if (clipped.size() <= 0)
            return STATUS_OK;

        cstorage<v_triangle3d_t> space1, space2;

        // Are there two triangles that should be generated?
        if (n_out == 2)
        {
            // There are two triangles above the split plane, perform second split
            // Prepare split plane and ensure it's direction
            calc_plane_vector_p3(&npl, &ctx->front.s, &out[1].p[1], &out[1].p[2]);
            float a = (npl.dx * spl.dx + npl.dy * spl.dy + npl.dz * spl.dz + npl.dw * spl.dw);
            if (a < 0.0f)
                inv_normal(&npl);

            // Do second split
            for (size_t i=0, nt=clipped.size(); i<nt; ++i)
            {
                n_sin = 0, n_sout = 0;
                split_triangle(sout, &n_sout, sin, &n_sin, &npl, clipped.at(i));

                // Add generated triangles to target buffers
                for (size_t l=0; l < n_sout; ++l)
                {
                    if (!space2.add(&sout[l]))
                        return STATUS_NO_MEM;
                }
                for (size_t l=0; l < n_sin; ++l)
                {
                    if (!space1.add(&sin[l]))
                        return STATUS_NO_MEM;
                }
            }
            clipped.flush();
        }
        else
            space1.swap(&clipped);

        // Non-empty space for context 1?
        if (space1.size() > 0)
        {
            context_t *sctx = new context_t;
            if (ctx == NULL)
                return STATUS_NO_MEM;

            // Initialize context with clipped sub-set
            sctx->front.p[0]    = out[0].p[0];
            sctx->front.p[1]    = out[0].p[1];
            sctx->front.p[2]    = out[0].p[2];
            sctx->matched       = ctx->matched;
            sctx->ignored       = ctx->ignored;
            sctx->scene         = ctx->scene;
            sctx->source.swap(&space1);
        }

        // Non-empty space for context 2?
        if (space2.size() > 0)
        {
            context_t *sctx = new context_t;
            if (ctx == NULL)
                return STATUS_NO_MEM;

            // Initialize context with clipped sub-set
            sctx->front.p[0]    = out[1].p[0];
            sctx->front.p[1]    = out[1].p[1];
            sctx->front.p[2]    = out[1].p[2];
            sctx->matched       = ctx->matched;
            sctx->ignored       = ctx->ignored;
            sctx->scene         = ctx->scene;
            sctx->source.swap(&space2);
        }

        return STATUS_OK;
    }

    /**
     * Perform binary split of the space
     * @param out list of context tasks outside the space
     * @param in list of context tasks inside the space
     * @param ctx context to apply split
     * @param pl plane to use splitting
     * @return status of operation
     */
    static status_t split_binary(
            cvector<context_t> &out,
            cvector<context_t> &in,
            context_t *ctx,
            const vector3d_t *pl
        )
    {
        return STATUS_OK; // TODO
    }

    /**
     * Perform binary space culling and return culled triangle to it's queue
     * @param tasks list of tasks to save contexts
     * @param ctx context to process
     * @param ct triangle to split and return to queue
     * @param pl culling plane
     * @return status of operation
     */
    static status_t cull_binary(
            cvector<context_t> &tasks,
            context_t *ctx,
            const v_triangle3d_t *ct,
            const vector3d_t *pl
        )
    {
        return STATUS_OK; // TODO
    }

    static status_t cull_back(cvector<context_t> &tasks, context_t *ctx)
    {
        vector3d_t pl[4];
        v_triangle3d_t t;

        // Select triangle from queue that will be used for culling
        while (true)
        {
            if (ctx->source.size() <= 0)
            {
                delete ctx;
                return STATUS_OK;
            }
            else if (!ctx->source.remove(0, &t))
                return STATUS_UNKNOWN_ERR;

            // Compute culling planes
            calc_plane_vector_p3(&pl[3], &t.p[0], &t.p[1], &t.p[2]);

            // Re-arrange normals if it is required
            float a = (pl[3].dx * pl[4].dx + pl[3].dy * pl[4].dy + pl[3].dz * pl[4].dz + pl[3].dw * pl[4].dw);
            if (a > 0.0f) // Normals of projection plane and culling plane have same directions
            {
                // compute plane equations
                if (ctx->source.size() > 0)
                {
                    calc_plane_vector_p3(&pl[0], &ctx->front.s, &t.p[0], &t.p[1]);
                    calc_plane_vector_p3(&pl[1], &ctx->front.s, &t.p[1], &t.p[2]);
                    calc_plane_vector_p3(&pl[2], &ctx->front.s, &t.p[2], &t.p[0]);
                }
            }
            else if (a < 0.0f) // Normals of projection plane and culling plane have opposite directions
            {
                inv_normal(&pl[4]);
                // compute plane equations
                if (ctx->source.size() > 0)
                {
                    calc_plane_vector_p3(&pl[0], &ctx->front.s, &t.p[1], &t.p[0]);
                    calc_plane_vector_p3(&pl[1], &ctx->front.s, &t.p[2], &t.p[1]);
                    calc_plane_vector_p3(&pl[2], &ctx->front.s, &t.p[0], &t.p[2]);
                }
            }
            else // We skip this triangle because it's perpendicular to the source
                continue;

            delete ctx;
            return ctx->matched->add(&t) ? STATUS_OK : STATUS_NO_MEM;
        }

        // Now we have 3 planes for culling and one plane for back-culling
        cvector<context_t> inside, queue; // Store all triangle contexts that are inside the culling space

        // Put context to queue
        if (!inside.add(ctx))
        {
            delete ctx;
            return STATUS_NO_MEM;
        }

        // Operate with each plane
        size_t count;

        for (size_t i=0; i<3; ++i)
        {
            // Check if there is still data for processing
            count = inside.size();
            if (count <= 0)
                return STATUS_OK;

            // Obtain number of contexts in queue
            for (size_t j=0; j < count; ++j)
            {
                // Fetch new context
                ctx = inside.get(0);
                if (!inside.remove(size_t(0), true))
                {
                    delete ctx;
                    return STATUS_CORRUPTED;
                }

                // Perform binary space split
                status_t res = split_binary(tasks, queue, ctx, &pl[i]); // TODO: implement this function
                if (res != STATUS_OK)
                {
                    delete ctx;
                    destroy_tasks(inside);
                    destroy_tasks(queue);
                    return res;
                }
            }

            // Now we have all pending items in 'queue' array, take them for the next iteration
            inside.swap_data(&queue);
        }

        // Cut all triangles behind the space, partition the cutting triangle and put triangle back to the queue
        count = inside.size();
        if (count <= 0)
            return STATUS_OK;

        for (size_t i=0; i<count; ++i)
        {
            // Fetch new context
            ctx = inside.get(0);
            if (!inside.remove(size_t(0), true))
            {
                delete ctx;
                return STATUS_CORRUPTED;
            }

            status_t res = cull_binary(tasks, ctx, &t, &pl[i]); // TODO: implement this function
            if (res != STATUS_OK)
            {
                delete ctx;
                destroy_tasks(inside);
                return res;
            }
        }

        return STATUS_OK;
    }

    static status_t perform_raytrace(
            cvector<context_t> &tasks
        )
    {
        context_t *ctx = NULL;

        while (tasks.size() > 0)
        {
            // Get next context from queue
            if (!tasks.pop(&ctx))
                return STATUS_CORRUPTED;

            // Check that we need to perform a scan
            switch (ctx->state)
            {
                case S_SCAN_SCENE:
                    scan_scene(ctx);
                    ctx->state = S_CULL_FRONT;
                    break;

                case S_CULL_FRONT:
                    cull_front(ctx);
                    if (ctx->source.size() == 1)
                    {
                        bool success = (ctx->source.size() > 0) ?
                                (ctx->matched->add(ctx->source.get(0)) != NULL) : true;
                        delete ctx;
                        ctx = NULL;
                        if (!success)
                            return STATUS_NO_MEM;
                        continue;
                    }
                    ctx->state = S_CULL_BACK;
                    break;

                case S_CULL_BACK:
                    cull_back(tasks, ctx);
                    ctx = NULL;
                    // TODO: replace this stub with back-culling algorithm
//                    if (!ctx->matched->add_all(&ctx->source))
//                        return STATUS_NO_MEM;
//                    delete ctx;
//                    ctx = NULL;
                    break;
            }

            // Post-process context state
            if (ctx == NULL)
                continue;
            else if (ctx->source.size() <= 0)
                delete ctx;
            else if (!tasks.push(ctx))
            {
                delete ctx;
                return STATUS_NO_MEM;
            }
        }

#if 0
            // Context is in final state?
            size_t n = ctx->source.size();
            if (n <= 0)
            {
                ctx->source.flush();
                delete ctx;
                continue;
            }

            // Remove the triangle that caused the culling
            if (!ctx->source.remove(0, &t))
                return STATUS_CORRUPTED;

            // Compute the normal and angle
            calc_plane_vector_pv(&pl[3], t.p);
            calc_plane_vector_p3(&pl[4], &ctx->front.r[0].z, &ctx->front.r[1].z, &ctx->front.r[2].z);
            float a = (pl[3].dx * pl[4].dx + pl[3].dy * pl[4].dy + pl[3].dz * pl[4].dz + pl[3].dw * pl[4].dw);

            // There are no more triangles in the space?
            if (n <= 1)
            {
                if (a != 0.0f) // We don't need to process perpendicular triangles
                {
                    if (!ctx->matched->add(&t)) // Emit the triangle!
                        return STATUS_NO_MEM;
                }
                ctx->source.flush();
                delete ctx;
                continue;
            }

            // We need to split 3D space into sub-spaces
            calc_plane_vector_p3(&pl[0], &t.p[0], &t.p[1], &ctx->front.s);
            calc_plane_vector_p3(&pl[1], &t.p[1], &t.p[2], &ctx->front.s);
            calc_plane_vector_p3(&pl[2], &t.p[2], &t.p[0], &ctx->front.s);
            if (a < 0.0f) // We need to invert clipping plane normals if we see back side of triangle
            {
                inv_normal(&pl[0]);
                inv_normal(&pl[1]);
                inv_normal(&pl[2]);
                inv_normal(&pl[3]);
            }

            for (size_t i= (a != 0.0f) ? 0 : 2; i<4; ++i)
            {
                // Split space into sub-spaces
                res = split_space(tasks, ctx, &pl[i], i < 3);
                if (res != STATUS_OK)
                    return res;

                // There are no triangles left inside the context?
                if (ctx->source.size() <= 0)
                {
                    // Immediately add selected triangle to matched
                    if (!ctx->matched->add(&t))
                        return STATUS_NO_MEM;

                    // Destroy context
                    ctx->source.flush();
                    delete ctx;
                    goto NEXT_LOOP;
                }
            }

            // There are some triangles left inside the context
            // Add current triangle as the last and submit context to the queue
            if (!ctx->source.append(&t))
                return STATUS_NO_MEM;
            if (!tasks.push(ctx))
                return STATUS_NO_MEM;
#endif

        return STATUS_OK;
    }
} // Namespace mtest

MTEST_BEGIN("3d", reflections)

    class Renderer: public X11Renderer
    {
        private:
            Scene3D        *pScene;
            wfront_t        sFront;
            bool            bBoundBoxes;

        public:
            explicit Renderer(Scene3D *scene, View3D *view): X11Renderer(view)
            {
                pScene = scene;
                bBoundBoxes = true;

                dsp::init_point_xyz(&sFront.p[0], 0.0f, 1.0f, 0.0f);
                dsp::init_point_xyz(&sFront.p[1], -1.0f, -0.5f, 0.0f);
                dsp::init_point_xyz(&sFront.p[2], 1.0f, -0.5f, 0.0f);
                dsp::init_point_xyz(&sFront.s, 0.0f, 0.0f, 1.0f);

                update_view();
            }

            virtual ~Renderer()
            {
            }

        public:
            virtual void on_key_press(const XKeyEvent &ev, KeySym key)
            {
                switch (key)
                {
                    case XK_F1:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.p[0].x += incr;
                        sFront.p[1].x += incr;
                        sFront.p[2].x += incr;
                        sFront.s.x += incr;
                        update_view();
                        break;
                    }

                    case XK_F2:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.p[0].y += incr;
                        sFront.p[1].y += incr;
                        sFront.p[2].y += incr;
                        sFront.s.y += incr;
                        update_view();
                        break;
                    }

                    case XK_F3:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.p[0].z += incr;
                        sFront.p[1].z += incr;
                        sFront.p[2].z += incr;
                        sFront.s.z += incr;
                        update_view();
                        break;
                    }

                    case XK_F4:
                    case XK_F5:
                    case XK_F6:
                    {
                        matrix3d_t m;
                        float incr = (ev.state & ShiftMask) ? M_PI/16.0f : -M_PI/16.0f;

                        for (size_t i=0; i<3; ++i)
                        {
                            sFront.p[i].x -= sFront.s.x;
                            sFront.p[i].y -= sFront.s.y;
                            sFront.p[i].z -= sFront.s.z;
                        }
                        if (key == XK_F4)
                            dsp::init_matrix3d_rotate_x(&m, incr);
                        else if (key == XK_F5)
                            dsp::init_matrix3d_rotate_y(&m, incr);
                        else
                            dsp::init_matrix3d_rotate_z(&m, incr);
                        for (size_t i=0; i<3; ++i)
                            dsp::apply_matrix3d_mp1(&sFront.p[i], &m);
                        for (size_t i=0; i<3; ++i)
                        {
                            sFront.p[i].x += sFront.s.x;
                            sFront.p[i].y += sFront.s.y;
                            sFront.p[i].z += sFront.s.z;
                        }
                        update_view();
                        break;
                    }

                    case 'b':
                    {
                        bBoundBoxes = ! bBoundBoxes;
                        update_view();
                        break;
                    }

                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                    {
                        Object3D *obj = pScene->get_object(key - '0');
                        if (obj != NULL)
                        {
                            obj->set_visible(!obj->is_visible());
                            update_view();
                        }
                        break;
                    }
                    default:
                        X11Renderer::on_key_press(ev, key);
                        break;
                }
            }

        protected:
            status_t    update_view()
            {
                v_segment3d_t s;
                v_vertex3d_t v[3];

                // Clear view state
                pView->clear_all();

                // List of ignored and matched triangles
                cstorage<v_triangle3d_t> ignored, matched;
                cvector<object_t> scene;
                cvector<context_t> tasks;

                // Create initial context
                context_t *ctx = new context_t;
                if (ctx == NULL)
                    return STATUS_NO_MEM;

                ctx->state      = S_SCAN_SCENE;
                ctx->front      = sFront;
                ctx->ignored    = &ignored;
                ctx->matched    = &matched;
                ctx->scene      = &scene;

                // Add context to tasks
                if (!tasks.add(ctx))
                {
                    delete ctx;
                    return STATUS_NO_MEM;
                }

                // Prepare scene for analysis
                status_t res = prepare_scene(ignored, scene, pScene);
                if (res != STATUS_OK)
                {
                    tasks.flush();
                    delete ctx;
                    return res;
                }

                // Render bounding boxes of the scene
                s.c = C_ORANGE;
                for (size_t i=0, n=scene.size(); i<n; ++i)
                {
                    object_t *o = scene.at(i);
                    bound_box3d_t *bbox = &o->box;

                    if (bBoundBoxes)
                    {
                        for (size_t i=0; i<4; ++i)
                        {
                            s.p[0] = bbox->p[i];
                            s.p[1] = bbox->p[(i+1)%4];
                            pView->add_segment(&s);
                            s.p[0] = bbox->p[i];
                            s.p[1] = bbox->p[i+4];
                            pView->add_segment(&s);
                            s.p[0] = bbox->p[i+4];
                            s.p[1] = bbox->p[(i+1)%4 + 4];
                            pView->add_segment(&s);
                        }
                    }
                }

                // Clear allocated resources, tasks and ctx should be already deleted
                res = perform_raytrace(tasks);

                destroy_tasks(tasks);
                destroy_scene(scene);

                // Build final scene from matched and ignored items
                for (size_t i=0, m=ignored.size(); i < m; ++i)
                {
                    v_triangle3d_t *t = ignored.at(i);
                    v[0].p     = t->p[0];
                    v[0].n     = t->n[0];
                    v[0].c     = C_GRAY;

                    v[1].p     = t->p[1];
                    v[1].n     = t->n[1];
                    v[1].c     = C_GRAY;

                    v[2].p     = t->p[2];
                    v[2].n     = t->n[2];
                    v[2].c     = C_GRAY;

                    pView->add_triangle(v);
                }
                ignored.flush();

                for (size_t i=0, m=matched.size(); i < m; ++i)
                {
                    v_triangle3d_t *t = matched.at(i);
                    v[0].p     = t->p[0];
                    v[0].n     = t->n[0];
                    v[0].c     = C_RED;

                    v[1].p     = t->p[1];
                    v[1].n     = t->n[1];
                    v[1].c     = C_GREEN;

                    v[2].p     = t->p[2];
                    v[2].n     = t->n[2];
                    v[2].c     = C_BLUE;

                    pView->add_triangle(v);
                }
                matched.flush();

//                    project_triangle(&out[0], &wf->s, &pl[3], &t.p[0]);
//                    vs.p[0]             = out[0];
//                    vs.p[1]             = out[1];
//                    view->add_segment(&vs);
//
//                    vs.p[0]             = out[1];
//                    vs.p[1]             = out[2];
//                    view->add_segment(&vs);
//
//                    vs.p[0]             = out[2];
//                    vs.p[1]             = out[0];
//                    view->add_segment(&vs);

                // Calc scissor planes' normals
                vector3d_t pl[4];
                calc_plane_vector_p3(&pl[0], &sFront.s, &sFront.p[0], &sFront.p[1]);
                calc_plane_vector_p3(&pl[1], &sFront.s, &sFront.p[1], &sFront.p[2]);
                calc_plane_vector_p3(&pl[2], &sFront.s, &sFront.p[2], &sFront.p[0]);
                calc_plane_vector_p3(&pl[3], &sFront.p[0], &sFront.p[1], &sFront.p[2]);

                // Draw front
                v_ray3d_t r;
                s.c = C_MAGENTA;

                for (size_t i=0; i<3; ++i)
                {
                    // State
                    r.p = sFront.p[i];
                    dsp::init_vector_p2(&r.v, &sFront.s, &r.p);
                    r.c = C_MAGENTA;
                    pView->add_ray(&r);

                    s.p[0] = sFront.s;
                    s.p[1] = sFront.p[i];
                    pView->add_segment(&s);

                    s.p[0] = sFront.p[(i+1)%3];
                    pView->add_segment(&s);

                    // Normals
                    r.p = sFront.p[i];
                    r.v = pl[i];
                    r.v.dw = 0.0f;
                    r.c = C_YELLOW;
                    pView->add_ray(&r);

                    r.p = sFront.p[(i+1)%3];
                    pView->add_ray(&r);

                    r.v = pl[3];
                    r.v.dw = 0.0f;
                    pView->add_ray(&r);
                }

                return res;
            }
    };

    MTEST_MAIN
    {
        const char *scene_file = (argc < 1) ? "res/test/3d/cross.obj" : argv[0];

        // Load scene
        Scene3D s;
        View3D v;
        status_t res = Model3DFile::load(&s, scene_file, true);
        MTEST_ASSERT_MSG(res == STATUS_OK, "Error loading scene from file %s", scene_file);

        // Initialize renderer
        Renderer r(&s, &v);
        MTEST_ASSERT_MSG(r.init() == STATUS_OK, "Error initializing renderer");
        r.run();
        r.destroy();

        // Destroy scene
        s.destroy();
    }

MTEST_END



