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

MTEST_BEGIN("3d", reflections)

    class Renderer: public X11Renderer
    {
        public:
            explicit Renderer(Scene3D *scene): X11Renderer(scene)
            {
            }

            virtual ~Renderer()
            {
            }
    };

    bool on_key_press(/*view_t *s,*/ KeySym key)
    {
        lsp_trace("Keycode = %x", int(key));

        /*
        switch (key)
        {
            case XK_Escape:
                return true;
            case ' ':
                s->bRotate = !s->bRotate;
                break;
            case 'w':
                s->bWireframe = !s->bWireframe;
                break;
            case 'l':
                s->bLight = !s->bLight;
                break;
            case 'c':
                s->bCullFace = !s->bCullFace;
                break;
            case 'i':
                s->bInvert = !s->bInvert;
                break;
            case 'r':
                s->bDrawRays    = ! s->bDrawRays;
                break;
            case 't':
                s->bDrawTriangles   = ! s->bDrawTriangles;
                break;
            case 'p':
                s->bDrawPoints  = !s->bDrawPoints;
                break;
            case 's':
                s->bDrawSegments  = !s->bDrawSegments;
                break;
            case 'g':
                s->bDrawSource  = !s->bDrawSource;
                break;
            case 'n':
                s->bDrawNormals = !s->bDrawNormals;
                break;
            case 'd':
                s->bDrawCapture = !s->bDrawCapture;
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            {
                Object3D *obj = s->pScene->get_object(key - '0');
                if (obj != NULL)
                    obj->set_visible(!obj->is_visible());
                break;
            }
        }*/

        return false;
    }

    MTEST_MAIN
    {
        const char *scene_file = (argc < 1) ? "res/test/3d/cross.obj" : argv[0];

        // Load scene
        Scene3D s;
        status_t res = Model3DFile::load(&s, scene_file, true);
        MTEST_ASSERT_MSG(res == STATUS_OK, "Error loading scene from file %s", scene_file);

        // Initialize renderer
        Renderer r(&s);
        MTEST_ASSERT_MSG(r.init() == STATUS_OK, "Error initializing renderer");
        r.run();
        r.destroy();

        // Destroy scene
        s.destroy();
    }

MTEST_END



