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
                CtlColor        sColor;
                CtlColor        sBgColor;
                CtlPadding      sPadding;
                LSPTimer        sTimer;

            protected:
                static status_t slot_on_draw3d(LSPWidget *sender, void *ptr, void *data);
                static status_t redraw_area(timestamp_t ts, void *arg);

            public:
                explicit CtlViewer3D(CtlRegistry *src, LSPArea3D *widget);
                virtual ~CtlViewer3D();

            public:
                virtual void init();

                virtual status_t    on_draw3d(IR3DBackend *r3d);

                virtual void set(widget_attribute_t att, const char *value);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLVIEWER3D_H_ */
