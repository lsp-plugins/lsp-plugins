/*
 * LSPCapture3D.cpp
 *
 *  Created on: 12 мая 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPCapture3D::metadata = { "LSPCapture3D", &LSPObject3D::metadata };

        LSPCapture3D::LSPCapture3D(LSPDisplay *dpy):
            LSPObject3D(dpy),
            sColor(this),
            sAxisColor(this)
        {
            pClass          = &metadata;
            fRadius         = 1.0f;
        }
        
        LSPCapture3D::~LSPCapture3D()
        {
        }

        void LSPCapture3D::set_position(const point3d_t *pos)
        {
            sPosition = *pos;
            query_draw(REDRAW_SURFACE);
        }

        void LSPCapture3D::set_direction(const vector3d_t *dir)
        {
            sDirection = *dir;
            query_draw(REDRAW_SURFACE);
        }

        void LSPCapture3D::set_radius(float radius)
        {
            if (fRadius == radius)
                return;
            fRadius     = radius;
            query_draw(REDRAW_SURFACE);
        }

        void LSPCapture3D::render(IR3DBackend *r3d)
        {
        }
    
    } /* namespace tk */
} /* namespace lsp */
