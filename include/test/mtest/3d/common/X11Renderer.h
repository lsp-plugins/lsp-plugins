/*
 * Renderer.h
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#ifndef INCLUDE_TEST_MTEST_3D_COMMON_X11RENDERER_H_
#define INCLUDE_TEST_MTEST_3D_COMMON_X11RENDERER_H_

#include <core/types.h>
#include <core/status.h>
#include <core/3d/Scene3D.h>
#include <test/mtest/3d/common/View3D.h>

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

    class X11Renderer
    {
        private:
            Display                 *dpy;
            Window                  win;
            GLXContext              glc;
            bool                    stopped;
            size_t                  nBMask;
            ssize_t                 nMouseX, nMouseY;

        protected:
            bool                    bWireframe;
            bool                    bRotate;
            bool                    bLight;
            bool                    bInvert;
            bool                    bCullFace;
            bool                    bDrawRays;
            bool                    bDrawSegments;
            bool                    bDrawTriangles;
            bool                    bDrawPoints;
            bool                    bDrawNormals;
            bool                    bDrawCapture;
            bool                    bDrawSource;

            float                   fAngleX;
            float                   fAngleY;
            float                   fAngleZ;
            float                   fScale;

            float                   fAngleDX;
            float                   fAngleDY;
            float                   fAngleDZ;
            float                   fDeltaScale;
            View3D                 *pView;
//            Scene3D                *pScene;

        public:
            X11Renderer(View3D *view);
            virtual ~X11Renderer();

        protected:
            static bool is_supported(const char *set, const char *ext);
            static void perspectiveGL(double fovY, double aspect, double zNear, double zFar);

        public:
            virtual status_t init();
            virtual status_t run();
            virtual void render(size_t width, size_t height);
            void stop();
            virtual void destroy();

        public:
            virtual void on_key_press(const XKeyEvent &ev, KeySym key);
            virtual void on_mouse_down(const XButtonEvent &ev);
            virtual void on_mouse_up(const XButtonEvent &ev);
            virtual void on_mouse_move(const XMotionEvent &ev);
    };

} /* namespace mtest */

#endif /* INCLUDE_TEST_MTEST_3D_COMMON_X11RENDERER_H_ */
