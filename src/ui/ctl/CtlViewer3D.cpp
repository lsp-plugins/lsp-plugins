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

        status_t CtlViewer3D::redraw_area(timestamp_t ts, void *arg)
        {
            if (arg == NULL)
                return STATUS_OK;
            CtlViewer3D *_this = static_cast<CtlViewer3D *>(arg);

            if ((_this->pWidget != NULL) && (_this->pWidget->visible()))
                _this->pWidget->query_draw();
            return STATUS_OK;
        }

        void CtlViewer3D::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPArea3D *r3d    = widget_cast<LSPArea3D>(pWidget);

            // Initialize color controllers
            sColor.init_hsl2(pRegistry, r3d, r3d->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sBgColor.init_basic2(pRegistry, r3d, r3d->bg_color(), A_BG_COLOR);
            sPadding.init(r3d->padding());

            sTimer.bind(r3d->display());
            sTimer.set_handler(redraw_area, this);
            sTimer.launch(-1, 250); // Schedule at 4 hz rate
        }

        void CtlViewer3D::set(widget_attribute_t att, const char *value)
        {
            LSPArea3D *r3d  = (pWidget != NULL) ? widget_cast<LSPArea3D>(pWidget) : NULL;

            switch (att)
            {
                case A_WIDTH:
                    if (r3d != NULL)
                        PARSE_INT(value, r3d->set_min_width(__));
                    break;
                case A_HEIGHT:
                    if (r3d != NULL)
                        PARSE_INT(value, r3d->set_min_height(__));
                    break;
                case A_BORDER:
                    if (r3d != NULL)
                        PARSE_INT(value, r3d->set_border(__));
                    break;
                case A_SPACING:
                    if (r3d != NULL)
                        PARSE_INT(value, r3d->set_radius(__));
                    break;
                default:
                {
                    bool set = sColor.set(att, value);
                    set |= sBgColor.set(att, value);
                    set |= sPadding.set(att, value);

                    if (!set)
                        CtlWidget::set(att, value);
                    break;
                }
            }
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
