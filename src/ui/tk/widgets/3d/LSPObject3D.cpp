/*
 * LSPMesh3D.cpp
 *
 *  Created on: 12 мая 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPObject3D::metadata = { "LSPObject3D", &LSPWidget::metadata };
        
        LSPObject3D::LSPObject3D(LSPDisplay *dpy):
            LSPWidget(dpy)
        {
            pClass          = &metadata;
        }
        
        LSPObject3D::~LSPObject3D()
        {
        }

        void LSPObject3D::render(IR3DBackend *r3d)
        {
        }

        void LSPObject3D::set_view_point(const point3d_t *pov)
        {
        }

        LSPArea3D *LSPObject3D::area3d()
        {
            LSPComplexWidget *parent = pParent;

            while (parent != NULL)
            {
                LSPArea3D *area3d = widget_cast<LSPArea3D>(parent);
                if (area3d != NULL)
                    return area3d;
                parent  = parent->parent();
            }
            return NULL;
        }
    
    } /* namespace tk */
} /* namespace lsp */
