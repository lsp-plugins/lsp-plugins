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
#include <stdlib.h>
#include <errno.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <sys/poll.h>

using namespace lsp;

static const color3d_t C_RED        = { 1.0f, 0.0f, 0.0f, 0.0f };
static const color3d_t C_GREEN      = { 0.0f, 1.0f, 0.0f, 0.0f };
static const color3d_t C_BLUE       = { 0.0f, 0.0f, 1.0f, 0.0f };
static const color3d_t C_MAGENTA    = { 1.0f, 0.0f, 1.0f, 0.0f };
static const color3d_t C_YELLOW     = { 1.0f, 1.0f, 0.0f, 0.0f };
static const color3d_t C_GRAY       = { 0.75f, 0.75f, 0.75f, 0.0f };

typedef struct wfront_t
{
    ray3d_t  r[3];      // Rays, counter-clockwise order
    point3d_t s;        // Source point
} wfront_t;

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

static size_t check_colocation(const vector3d_t *pl, const point3d_t *p1, const point3d_t *p2, const point3d_t *p3)
{
    size_t m = 0;
    if ((pl->dx * p1->x + pl->dy*p1->y + pl->dz*p1->z + pl->dw*p1->w) > 0.0f)
        m |= 0x01;
    if ((pl->dx * p2->x + pl->dy*p2->y + pl->dz*p2->z + pl->dw*p2->w) > 0.0f)
        m |= 0x02;
    if ((pl->dx * p3->x + pl->dy*p3->y + pl->dz*p3->z + pl->dw*p3->w) > 0.0f)
        m |= 0x04;

    return m;
}

static size_t intersect(point3d_t *p, const ray3d_t *r, const vector3d_t *pl)
{
    /*
         Line equation:
             x = x0 + dx * t
             y = y0 + dy * t
             z = z0 + dz * t
         Plane equation:
             nx*x + ny*y + nz*z + w = 0

         Then:
             nx*(x0+dx*t) + ny*(y0+dy*t) + nz*(z0+dz*t) + w = 0;
                    w + (nx*x0 + ny*y0 + nz*z0)
             t = - ----------------------------
                      (nx*dx + ny*dy + nz*dz)
     */
}

MTEST_BEGIN("3d", reflections)

    class Renderer: public X11Renderer
    {
        private:
            Scene3D     *pScene;
            wfront_t     sFront;

        public:
            explicit Renderer(Scene3D *scene, View3D *view): X11Renderer(view)
            {
                pScene = scene;

                point3d_t p[4];
                dsp::init_point_xyz(&p[0], 0.0f, 1.0f, 0.0f);
                dsp::init_point_xyz(&p[1], -1.0f, -0.5f, 0.0f);
                dsp::init_point_xyz(&p[2], 1.0f, -0.5f, 0.0f);
                dsp::init_point_xyz(&p[3], 0.0f, 0.0f, 1.0f);

                vector3d_t v[3];
                dsp::init_vector_p2(&v[0], &p[3], &p[0]);
                dsp::init_vector_p2(&v[1], &p[3], &p[1]);
                dsp::init_vector_p2(&v[2], &p[3], &p[2]);

                dsp::init_ray_pdv(&sFront.r[0], &p[0], &v[0]);
                dsp::init_ray_pdv(&sFront.r[1], &p[1], &v[1]);
                dsp::init_ray_pdv(&sFront.r[2], &p[2], &v[2]);
                sFront.s = p[3];

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
                        sFront.r[0].z.x += incr;
                        sFront.r[1].z.x += incr;
                        sFront.r[2].z.x += incr;
                        sFront.s.x += incr;
                        update_view();
                        break;
                    }

                    case XK_F2:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.r[0].z.y += incr;
                        sFront.r[1].z.y += incr;
                        sFront.r[2].z.y += incr;
                        sFront.s.y += incr;
                        update_view();
                        break;
                    }

                    case XK_F3:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.r[0].z.z += incr;
                        sFront.r[1].z.z += incr;
                        sFront.r[2].z.z += incr;
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
                            sFront.r[i].z.x -= sFront.s.x;
                            sFront.r[i].z.y -= sFront.s.y;
                            sFront.r[i].z.z -= sFront.s.z;
                        }
                        if (key == XK_F4)
                            dsp::init_matrix3d_rotate_x(&m, incr);
                        else if (key == XK_F5)
                            dsp::init_matrix3d_rotate_y(&m, incr);
                        else
                            dsp::init_matrix3d_rotate_z(&m, incr);
                        for (size_t i=0; i<3; ++i)
                        {
                            dsp::apply_matrix3d_mp1(&sFront.r[i].z, &m);
                            dsp::apply_matrix3d_mv1(&sFront.r[i].v, &m);
                        }
                        for (size_t i=0; i<3; ++i)
                        {
                            sFront.r[i].z.x += sFront.s.x;
                            sFront.r[i].z.y += sFront.s.y;
                            sFront.r[i].z.z += sFront.s.z;
                        }
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
            void    update_view()
            {
                // Clear view state
                pView->clear_all();

                // Calc scissor planes' normals
                vector3d_t pl[4];
                size_t cl[4];
                calc_plane_vector_rv(&pl[0], &sFront.r[0], &sFront.r[1].v);
                calc_plane_vector_rv(&pl[1], &sFront.r[1], &sFront.r[2].v);
                calc_plane_vector_rv(&pl[2], &sFront.r[2], &sFront.r[0].v);
                calc_plane_vector_p3(&pl[3], &sFront.r[0].z, &sFront.r[1].z, &sFront.r[2].z);

                // Generate visible triangles
                v_vertex3d_t v[3];

                for (size_t i=0, n=pScene->num_objects(); i<n; ++i)
                {
                    Object3D *obj   = pScene->get_object(i);
                    if ((obj == NULL) || (!obj->is_visible()))
                        continue;
                    matrix3d_t *om  = obj->get_matrix();

                    point3d_t *tr       = obj->get_vertexes();
                    vector3d_t *tn      = obj->get_normals();
                    vertex_index_t *vvx = obj->get_vertex_indexes();
                    vertex_index_t *vnx = obj->get_normal_indexes();

                    for (ssize_t j=0, m=obj->get_triangles_count(); j < m; ++j)
                    {
                        // Initialize points
                        v[0].p              = tr[*(vvx++)];
                        v[0].n              = tn[*(vnx++)];
                        v[0].c              = C_RED;

                        v[1].p              = tr[*(vvx++)];
                        v[1].n              = tn[*(vnx++)];
                        v[1].c              = C_GREEN;

                        v[2].p              = tr[*(vvx++)];
                        v[2].n              = tn[*(vnx++)];
                        v[2].c              = C_BLUE;

                        // Apply matrix modifications
                        dsp::apply_matrix3d_mp1(&v[0].p, om);
                        dsp::apply_matrix3d_mp1(&v[1].p, om);
                        dsp::apply_matrix3d_mp1(&v[2].p, om);

                        dsp::apply_matrix3d_mv1(&v[0].n, om);
                        dsp::apply_matrix3d_mv1(&v[1].n, om);
                        dsp::apply_matrix3d_mv1(&v[2].n, om);

                        // Check co-location
                        bool skip = false;
                        for (size_t i=0; i<4; ++i)
                            cl[i] = check_colocation(&pl[i], &v[0].p, &v[1].p, &v[2].p);

                        for (size_t i=0; i<4; ++i)
                            if (cl[i] == 0x07)
                                skip = true;

                        if (skip)
                        {
                            v[0].c              = C_GRAY;
                            v[1].c              = C_GRAY;
                            v[2].c              = C_GRAY;
                        }

                        pView->add_triangle(v);
                    }
                }

                // Draw front
                v_ray3d_t r;
                v_segment3d_t s;
                s.c = C_MAGENTA;

                for (size_t i=0; i<3; ++i)
                {
                    // State
                    r.p = sFront.r[i].z;
                    r.v = sFront.r[i].v;
                    r.c = C_MAGENTA;
                    pView->add_ray(&r);

                    s.p[0] = sFront.s;
                    s.p[1] = sFront.r[i].z;
                    pView->add_segment(&s);

                    s.p[0] = sFront.r[(i+1)%3].z;
                    pView->add_segment(&s);

                    // Normals
                    r.p = sFront.r[i].z;
                    r.v = pl[i];
                    r.v.dw = 0.0f;
                    r.c = C_YELLOW;
                    pView->add_ray(&r);

                    r.p = sFront.r[(i+1)%3].z;
                    pView->add_ray(&r);

                    r.v = pl[3];
                    r.v.dw = 0.0f;
                    pView->add_ray(&r);
                }
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



