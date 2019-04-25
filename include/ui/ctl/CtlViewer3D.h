/*
 * CtlViewer3D.h
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLVIEWER3D_H_
#define UI_CTL_CTLVIEWER3D_H_

namespace lsp
{
    namespace ctl
    {
        /**
         * 3D Model viewer
         */
        class CtlViewer3D: public CtlWidget
        {
            protected:
                static status_t slot_on_draw3d(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit CtlViewer3D(CtlRegistry *src, LSPArea3D *widget);
                virtual ~CtlViewer3D();

            public:
                status_t    on_draw3d(IR3DBackend *r3d);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLVIEWER3D_H_ */
