/*
 * reflections3d.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#if 0

#include <test/mtest.h>
#include <testing/mtest/3d/common/X11Renderer.h>
#include <core/files/Model3DFile.h>
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
#include <GL/glu.h>
#include <sys/poll.h>

//#define TEST_DEBUG

#ifndef TEST_DEBUG
//    #define BREAKPOINT_STEP     -1
//    #define BREAKPOINT_STEP     27
//    #define BREAKPOINT_STEP     83
    #define BREAKPOINT_STEP     215
//    #define BREAKPOINT_STEP     0

#else /* DEBUG */
    #define BREAKPOINT_STEP     0
#endif /* DEBUG */

MTEST_BEGIN("3d", boolean)

    class Renderer: public X11Renderer
    {
        private:
            Scene3D        *pScene;
            ssize_t         nTrace;
            bool            bDrawMatched;
            bool            bDrawIgnored;
            bool            bDrawDebug;

        public:
            explicit Renderer(Scene3D *scene, View3D *view): X11Renderer(view)
            {
                pScene = scene;
                bDrawMatched    = true;
                bDrawIgnored    = true;
                bDrawDebug      = true;
                bDrawNormals    = false;
                nTrace          = BREAKPOINT_STEP;

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
                    case XK_Up:
                        nTrace++;
                        lsp_trace("Set trace breakpoint to %d", int(nTrace));
                        update_view();
                        break;
                    case XK_Down:
                        if (nTrace >= 0)
                        {
                            nTrace--;
                            lsp_trace("Set trace breakpoint to %d", int(nTrace));
                            update_view();
                        }
                        break;

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
            status_t perform_boolean(rt_debug_t *shared)
            {
                if (pScene->num_objects() <= 0)
                    return STATUS_OK;

                rt_mesh_t mesh;
                mesh.set_debug_context(shared, &shared->trace);

                Object3D *obj = pScene->object(0);
                status_t res = mesh.init(obj, 0, obj->matrix());
                if (res != STATUS_OK)
                    return res;

                for (size_t i=1, n=pScene->num_objects(); i<n; ++i)
                {
                    obj = pScene->object(i);
                    res = mesh.subtract(obj, i, obj->matrix());
                    if (res != STATUS_OK)
                        return res;
                }

                return STATUS_OK;
            }

            status_t    update_view()
            {
                v_vertex3d_t v[3];
                status_t res = STATUS_OK;

                if (!pScene->validate())
                    return STATUS_BAD_STATE;

                // List of ignored and matched triangles
                rt_debug_t global;
                global.breakpoint   = nTrace;
                global.step         = 0;

                // Perform raytrace
                res     = perform_boolean(&global);
                if (res == STATUS_BREAKPOINT)
                {
                    pView->swap(&global.trace);
                    res         = STATUS_OK;
                }

                if (!bDrawDebug)
                    pView->clear_all();

                if (!pScene->validate())
                    return STATUS_BAD_STATE;

                // Build final scene from matched and ignored items
                if (bDrawIgnored)
                {
                    for (size_t i=0, m=global.ignored.size(); i < m; ++i)
                        pView->add_triangle_1c(global.ignored.at(i), &C_GRAY);
                }

                if (bDrawMatched)
                {
                    for (size_t i=0, m=global.matched.size(); i < m; ++i)
                    {
                        v_triangle3d_t *t = global.matched.at(i);
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
                }

                global.ignored.flush();
                global.matched.flush();

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

#endif
