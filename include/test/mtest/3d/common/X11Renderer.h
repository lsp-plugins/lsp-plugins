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

        public:
            X11Renderer();
            virtual ~X11Renderer();

        public:
            virtual status_t init();
            virtual status_t run();
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
