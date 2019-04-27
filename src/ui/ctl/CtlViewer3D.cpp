/*
 * CtlViewer3D.cpp
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <core/3d/common.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlViewer3D::CtlViewer3D(CtlRegistry *src, LSPArea3D *widget):
            CtlWidget(src, widget)
        {
            widget->slots()->bind(LSPSLOT_DRAW3D, slot_on_draw3d, this);
        }
        
        CtlViewer3D::~CtlViewer3D()
        {
        }

        status_t CtlViewer3D::slot_on_draw3d(LSPWidget *sender, void *ptr, void *data)
        {
            CtlViewer3D *_this      = static_cast<CtlViewer3D *>(ptr);
            if (data == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (_this != NULL) ? _this->on_draw3d(reinterpret_cast<IR3DBackend *>(data)) : STATUS_OK;
        }

        status_t CtlViewer3D::on_draw3d(IR3DBackend *r3d)
        {
            static const v_point3d_t points[] =
            {
                { { -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
                { { 1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
                { { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
            };

            r3d_buffer_t buf;
            buf.type    = R3D_PRIMITIVE_TRIANGLES;
            buf.count   = 1;
            buf.width   = 1.0f;

            buf.vertex.data     = &points[0].p;
            buf.vertex.stride   = sizeof(v_point3d_t);
            buf.normal.data     = NULL;
            buf.color.data      = &points[0].c;
            buf.color.stride    = sizeof(v_point3d_t);
            buf.index.data      = NULL;

            r3d->draw_primitives(&buf);

            return STATUS_OK;
        }
    
    } /* namespace ctl */
} /* namespace lsp */
