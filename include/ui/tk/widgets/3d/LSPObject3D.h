/*
 * LSPObject3D.h
 *
 *  Created on: 12 мая 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPOBJECT3D_H_
#define UI_TK_WIDGETS_LSPOBJECT3D_H_

namespace lsp
{
    namespace tk
    {
        class LSPObject3D: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            public:
                explicit LSPObject3D(LSPDisplay *dpy);
                virtual ~LSPObject3D();

            public:
                LSPArea3D *area3d();

            public:
                virtual void set_view_point(const point3d_t *pov);

                virtual void render(IR3DBackend *r3d);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPOBJECT3D_H_ */
