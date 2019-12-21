/*
 * reflections3d.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <testing/mtest/3d/common/X11Renderer.h>
#include <core/files/Model3DFile.h>
#include <core/3d/raytrace.h>
#include <core/3d/rt_context.h>
#include <core/3d/RayTrace3D.h>

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
#include <sys/poll.h>

//#define TEST_DEBUG

#ifndef TEST_DEBUG
//    #define BREAKPOINT_STEP     -1
    #define BREAKPOINT_STEP     0
//    #define BREAKPOINT_STEP     0

/*
        dsp::init_point_xyz(&front.p[0], -0.980776, -0.195088, 0.000000); \
        dsp::init_point_xyz(&front.p[1], 0.685477, -0.883233, 0.000000); \
        dsp::init_point_xyz(&front.p[2], 0.295300, 1.078322, 0.000000); \
        dsp::init_point_xyz(&front.s, 0.000000, 0.000000, 1.000000);
*/

#else /* DEBUG */
    #define BREAKPOINT_STEP     0

    #define INIT_FRONT(front) \
        dsp::init_point_xyz(&front.s, 0.0f, 0.0f, -0.75f); \
        dsp::init_point_xyz(&front.p[0], 0.378978, -0.513494, -1.580061); \
        dsp::init_point_xyz(&front.p[1], 0.453656, -0.486792, -1.562277); \
        dsp::init_point_xyz(&front.p[2], 0.352607, -0.494265, -1.620808);
#endif /* DEBUG */

namespace mtest
{
    using namespace lsp;

    void draw_barycentric(View3D *v)
    {
        float t[3], a[3];
        point3d_t p[3], sp;

        t[0] = 1.0f;
        t[1] = 2.0f;
        t[2] = -1.0f;

        dsp::init_point_xyz(&p[0], -1, -1, t[0]);
        dsp::init_point_xyz(&p[1], 1, 1, t[1]);
        dsp::init_point_xyz(&p[2], -1, 1, t[2]);
        v->add_triangle_pv1c(p, &C3D_RED);
        p[0].z = 0.0f;
        p[1].z = 0.0f;
        p[2].z = 0.0f;

        float s     = dsp::calc_area_pv(p);

        for (sp.x=-1.0f; sp.x<=1.0f; sp.x += 0.1f)
            for (sp.y=-1.0f; sp.y<=1.0f; sp.y += 0.1f)
            {
                sp.z    = 0.0f;
                a[0]    = dsp::calc_area_p3(&sp, &p[1], &p[2]);
                a[1]    = dsp::calc_area_p3(&sp, &p[2], &p[0]);
                a[2]    = dsp::calc_area_p3(&sp, &p[0], &p[1]);

                sp.z    = (t[0]*a[0] + t[1]*a[1] + t[2]*a[2])/ s;

                v->add_point(&sp, &C3D_YELLOW);
            }
    }

} // Namespace mtest

MTEST_BEGIN("3d", reflections)

    class Renderer: public X11Renderer
    {
        private:
            Scene3D        *pScene;
            ssize_t         nTrace;
            ray3d_t         sSource;
            ray3d_t         sCapture;
            bool            bBoundBoxes;
            bool            bDrawFront;
            bool            bDrawMatched;
            bool            bDrawIgnored;
            bool            bDrawDebug;

        public:
            explicit Renderer(Scene3D *scene, View3D *view): X11Renderer(view)
            {
                pScene = scene;
                bBoundBoxes     = false;
                bDrawFront      = true;
                bDrawMatched    = true;
                bDrawIgnored    = true;
                bDrawDebug      = true;
                nTrace          = BREAKPOINT_STEP;

//                dsp::init_point_xyz(&sSource.z, 0.0f, 0.0f, 1.0f);
//                dsp::init_vector_dxyz(&sSource.v, 0.0f, 0.0f, -1.0f); // 12" speaker source

                dsp::init_point_xyz(&sSource.z, -1.0f, 0.0f, 0.0f);
                dsp::init_vector_dxyz(&sSource.v, 0.0f, 0.0f, 1.0f);
                dsp::init_point_xyz(&sCapture.z, 1.0f, 0.0f, 0.0f);
                dsp::init_vector_dxyz(&sCapture.v, 0.0f, 0.0f, 1.0f); // 2" microphone diaphragm
            }

            virtual ~Renderer()
            {
            }

        public:
            virtual void on_key_press(const XKeyEvent &ev, KeySym key)
            {
                switch (key)
                {
                    case XK_F1: case XK_F7:
                    {
                        float incr  = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        ray3d_t *r  = (key == XK_F1) ? &sSource : &sCapture;
                        r->z.x     += incr;
                        update_view();
                        break;
                    }

                    case XK_F2: case XK_F8:
                    {
                        float incr  = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        ray3d_t *r  = (key == XK_F1) ? &sSource : &sCapture;
                        r->z.y     += incr;
                        update_view();
                        break;
                    }

                    case XK_F3: case XK_F9:
                    {
                        float incr  = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        ray3d_t *r  = (key == XK_F1) ? &sSource : &sCapture;
                        r->z.z     += incr;
                        update_view();
                        break;
                    }

                    case XK_F4:
                    case XK_F10:
                    {
                        matrix3d_t m;
                        float incr = (ev.state & ShiftMask) ? M_PI/16.0f : -M_PI/16.0f;
                        ray3d_t *r  = (key == XK_F4) ? &sSource : &sCapture;
                        dsp::init_matrix3d_rotate_x(&m, incr);
                        dsp::apply_matrix3d_mv1(&r->v, &m);
                        update_view();
                        break;
                    }

                    case XK_F5:
                    case XK_F11:
                    {
                        matrix3d_t m;
                        float incr = (ev.state & ShiftMask) ? M_PI/16.0f : -M_PI/16.0f;
                        ray3d_t *r  = (key == XK_F5) ? &sSource : &sCapture;
                        dsp::init_matrix3d_rotate_y(&m, incr);
                        dsp::apply_matrix3d_mv1(&r->v, &m);
                        update_view();
                        break;
                    }

                    case XK_F6:
                    case XK_F12:
                    {
                        matrix3d_t m;
                        float incr = (ev.state & ShiftMask) ? M_PI/16.0f : -M_PI/16.0f;
                        ray3d_t *r  = (key == XK_F6) ? &sSource : &sCapture;
                        dsp::init_matrix3d_rotate_z(&m, incr);
                        dsp::apply_matrix3d_mv1(&r->v, &m);
                        update_view();
                        break;
                    }

                    case XK_End:
                        nTrace++;
                        lsp_trace("Set trace breakpoint to %d", int(nTrace));
                        update_view();
                        break;
                    case XK_Home:
                        if (nTrace >= 0)
                        {
                            nTrace--;
                            lsp_trace("Set trace breakpoint to %d", int(nTrace));
                            update_view();
                        }
                        break;

                    case 'f':
                        bDrawFront = ! bDrawFront;
                        update_view();
                        break;

                    case 'b':
                    {
                        bBoundBoxes = ! bBoundBoxes;
                        update_view();
                        break;
                    }

                    case 'm':
                    {
                        bDrawMatched = ! bDrawMatched;
                        update_view();
                        break;
                    }

                    case 'i':
                    {
                        bDrawIgnored = ! bDrawIgnored;
                        update_view();
                        break;
                    }

                    case 'd':
                    {
                        bDrawDebug = ! bDrawDebug;
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
            status_t perform_trace(RayTrace3D *trace, rt_debug_t *shared)
            {
                status_t res    = trace->init();
                if (res != STATUS_OK)
                    return res;

                trace->set_debug_context(shared);
                trace->set_sample_rate(DEFAULT_SAMPLE_RATE);

                res     = trace->set_scene(pScene, true);
                if (res != STATUS_OK)
                    return res;

                rt_source_settings_t src;
                dsp::calc_matrix3d_transform_r1(&src.pos, &sSource);
//                src.type        = RT_AS_ICOSPHERE;
                src.type        = RT_AS_TRIANGLE;
                src.size        = 0.3048f;  // 12" speaker source
                src.height      = 0.3048f;
                src.angle       = 50.0f;
                src.curvature   = 0.0f;
                src.amplitude   = 1.0f;

                res     = trace->add_source(&src);
                if (res != STATUS_OK)
                    return res;

                rt_capture_settings_t cap;
                dsp::calc_matrix3d_transform_r1(&cap.pos, &sCapture);
                cap.type        = RT_AC_OMNI;
                cap.radius      = 0.0508f;

                res     = trace->add_capture(&cap);
                if (res != STATUS_OK)
                    return res;

                return trace->process(1, 100.0f);
            }

            virtual void view_changed()
            {
                v_segment3d_t s;
                v_vertex3d_t v[3];
                status_t res = STATUS_OK;

                if (!pScene->validate())
                    return;

                // List of ignored and matched triangles
                rt_debug_t global;
                global.breakpoint   = nTrace;
                global.step         = 0;

                // Perform raytrace
                RayTrace3D trace;
                res     = perform_trace(&trace, &global);
                if (res == STATUS_BREAK_POINT)
                {
                    pView->swap(&global.trace);
                    res         = STATUS_OK;
                }
                trace.destroy(false);
//                RTObjectFactory f;
//                Object3D *obj;
//                obj   = f.buildIcosphere(2);
//                if (obj != NULL)
//                {
//                    for (size_t i=0, m=obj->num_triangles(); i < m; ++i)
//                        pView->add_triangle_3c(obj->triangle(i), &C_RED, &C_GREEN, &C_BLUE);
//                }

                if (!bDrawDebug)
                    pView->clear_all();

                if (!pScene->validate())
                    return;

                // Render bounding boxes of the scene
                if (bBoundBoxes)
                {
                    s.c[0] = C3D_ORANGE;
                    s.c[1] = C3D_ORANGE;
                    for (size_t i=0, n=pScene->num_objects(); i<n; ++i)
                    {
                        Object3D *o = pScene->object(i);
                        matrix3d_t *m = o->matrix();
                        bound_box3d_t *pmbox = o->bound_box();
                        bound_box3d_t bbox;

                        for (size_t i=0; i<8; ++i)
                            dsp::apply_matrix3d_mp2(&bbox.p[i], &pmbox->p[i], m);

                        for (size_t i=0; i<4; ++i)
                        {
                            s.p[0] = bbox.p[i];
                            s.p[1] = bbox.p[(i+1)%4];
                            pView->add_segment(&s);
                            s.p[0] = bbox.p[i];
                            s.p[1] = bbox.p[i+4];
                            pView->add_segment(&s);
                            s.p[0] = bbox.p[i+4];
                            s.p[1] = bbox.p[(i+1)%4 + 4];
                            pView->add_segment(&s);
                        }
                    }
                }

                // Build final scene from matched and ignored items
                if (bDrawIgnored)
                {
                    for (size_t i=0, m=global.ignored.size(); i < m; ++i)
                        pView->add_triangle_1c(global.ignored.at(i), &C3D_GRAY);
                }

                if (bDrawMatched)
                {
                    for (size_t i=0, m=global.matched.size(); i < m; ++i)
                    {
                        v_triangle3d_t *t = global.matched.at(i);
                        v[0].p     = t->p[0];
                        v[0].n     = t->n[0];
                        v[0].c     = C3D_RED;

                        v[1].p     = t->p[1];
                        v[1].n     = t->n[1];
                        v[1].c     = C3D_GREEN;

                        v[2].p     = t->p[2];
                        v[2].n     = t->n[2];
                        v[2].c     = C3D_BLUE;

                        pView->add_triangle(v);
                    }
                }

                global.ignored.flush();
                global.matched.flush();
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

