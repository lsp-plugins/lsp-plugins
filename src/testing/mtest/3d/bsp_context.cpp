/*
 * reflections3d.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <testing/mtest/3d/common/X11Renderer.h>
#include <core/files/Model3DFile.h>
#include <core/3d/rt_context.h>
#include <core/3d/RayTrace3D.h>
#include <core/3d/bsp_context.h>

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
    #define BREAKPOINT_STEP     0
#endif /* DEBUG */

namespace lsp
{
    static const color3d_t *colors[] =
    {
        &C3D_RED,
        &C3D_GREEN,
        &C3D_BLUE,
        &C3D_CYAN,
        &C3D_MAGENTA,
        &C3D_YELLOW
    };
}

MTEST_BEGIN("3d", bsp_context)

    class Renderer: public X11Renderer
    {
        private:
            Scene3D        *pScene;
            ssize_t         nTrace;
            bool            bDrawDebug;

        public:
            explicit Renderer(Scene3D *scene, View3D *view): X11Renderer(view)
            {
                pScene          = scene;
                bDrawDebug      = true;
                nTrace          = BREAKPOINT_STEP;
            }

            virtual ~Renderer()
            {
            }

        public:
            virtual void view_changed()
            {
                status_t res    = STATUS_OK;

                if (!pScene->validate())
                    return;

                // List of ignored and matched triangles
                rt_debug_t global;
                global.breakpoint   = nTrace;
                global.step         = 0;

                // Add objects to BSP context
                bsp_context_t ctx;
                IF_RT_TRACE_Y(
                    ctx.set_debug_context(&global);
                );

                for (size_t i=0, n=pScene->num_objects(); i<n; ++i)
                {
                    Object3D *o = pScene->object(i);
                    if (!o->is_visible())
                        continue;

                    color3d_t c = *(colors[i % 6]);
                    c.a         = 0.5f; // Update alpha value

                    res = ctx.add_object(o, i, &c);
                    if (res != STATUS_OK)
                        return;
                }

                // Build tree
                res = ctx.build_tree();
                if (res == STATUS_OK)
                {
                    pView->clear_all();
                    res = ctx.build_mesh(pView->vertexes2(), &sPov);
                }

                if (res == STATUS_BREAK_POINT)
                {
                    IF_RT_TRACE_Y(
                        global.trace.swap(&ctx.trace);
                    );
                    pView->swap(&global.trace);
                    res = STATUS_OK;
                }
                else
                    return;

//                point3d_t p0, p1;
//                dsp::init_point_xyz(&p0, 0, 0, 0);
//                dsp::init_point_xyz(&p1, 1, 0, 0);
//                pView->add_segment(&p0, &p1, &C_RED);
//                dsp::init_point_xyz(&p1, 0, 1, 0);
//                pView->add_segment(&p0, &p1, &C_GREEN);
//                dsp::init_point_xyz(&p1, 0, 0, 1);
//                pView->add_segment(&p0, &p1, &C_BLUE);

                if (!bDrawDebug)
                    pView->clear_all();

                if (!pScene->validate())
                    return;

                global.ignored.flush();
                global.matched.flush();
            }

            virtual void on_key_press(const XKeyEvent &ev, KeySym key)
            {
                switch (key)
                {
                    case XK_Insert:
                        nTrace++;
                        lsp_trace("Set trace breakpoint to %d", int(nTrace));
                        update_view();
                        break;
                    case XK_Delete:
                        if (nTrace >= 0)
                        {
                            nTrace--;
                            lsp_trace("Set trace breakpoint to %d", int(nTrace));
                            update_view();
                        }
                        break;

                    case 'b':
                    {
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

