/*
 * CtlViewer3D.cpp
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

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
            // TODO
            return STATUS_OK;
        }
    
    } /* namespace ctl */
} /* namespace lsp */
