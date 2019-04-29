/*
 * Renderer.h
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#ifndef TEST_MTEST_3D_COMMON_X11RENDERER_H_
#define TEST_MTEST_3D_COMMON_X11RENDERER_H_

#include <core/types.h>
#include <core/status.h>
#include <core/3d/Scene3D.h>
#include <core/3d/View3D.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/poll.h>

#include <rendering/backend.h>
#include <core/ipc/Library.h>

namespace mtest
{
    using namespace lsp;

    class X11Renderer
    {
        protected:
            typedef struct pov_angles_t
            {
                float                   fYaw;
                float                   fPitch;
                float                   fRoll;
            } pov_angles_t;

        private:
            Display                 *dpy;
            Window                  win;
            Window                  glwnd;
            volatile bool           stopped;
            size_t                  nBMask;
            ssize_t                 nMouseX, nMouseY;
            ssize_t                 nWidth;
            ssize_t                 nHeight;
            bool                    bViewChanged;
            r3d_backend_t          *pBackend;
            ipc::Library            sLibrary;

        protected:
            // 3D rendering model
            pov_angles_t            sAngles;
            point3d_t               sPov;           // Point-of-view for the camera
            vector3d_t              sDir;           // Direction-of-view for the camera
            vector3d_t              sTop;           // Top-of-view for the camera
            vector3d_t              sSide;          // Side-of-view for the camera
//            matrix3d_t              sWorld;         // World matrix
            matrix3d_t              sDelta;         // Delta matrix
            matrix3d_t              sView;          // View (camera) matrix
            matrix3d_t              sProjection;    // Projection matrix

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

            View3D                 *pView;

        public:
            X11Renderer(View3D *view);
            virtual ~X11Renderer();

        protected:
            static bool is_supported(const char *set, const char *ext);
//            inline const matrix3d_t  *world() const { return &sWorld; };
            inline const matrix3d_t  *view() const          { return &sView; };
            inline const matrix3d_t  *projection() const    { return &sProjection; };
            virtual void view_changed();
            inline void update_view() { bViewChanged = true; }
            void move_camera(const vector3d_t *dir, float amount);
            void rotate_camera(ssize_t x, ssize_t y, bool commit);
            void draw_normals(v_vertex3d_t *vv, size_t nvertex);

        public:
            virtual status_t init();
            virtual status_t run();
            virtual void render();
            void stop();
            virtual void destroy();

        public:
            virtual void on_key_press(const XKeyEvent &ev, KeySym key);
            virtual void on_mouse_down(const XButtonEvent &ev);
            virtual void on_mouse_up(const XButtonEvent &ev);
            virtual void on_mouse_move(const XMotionEvent &ev);
    };

} /* namespace mtest */

#endif /* TEST_MTEST_3D_COMMON_X11RENDERER_H_ */
