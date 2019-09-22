/*
 * CtlCenter.cpp
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlCenter::metadata = { "CtlCenter", &CtlWidget::metadata };

        CtlCenter::CtlCenter(CtlRegistry *src, LSPCenter *cnt): CtlWidget(src, cnt)
        {
            pClass          = &metadata;
        }

        CtlCenter::~CtlCenter()
        {
        }

        void CtlCenter::set(widget_attribute_t att, const char *value)
        {
            LSPCenter *cnt = (pWidget != NULL) ? static_cast<LSPCenter *>(pWidget) : NULL;

            switch (att)
            {
                case A_VPOS:
                    if (cnt != NULL)
                        PARSE_FLOAT(value, cnt->set_canvas_top(__));
                    break;
                case A_HPOS:
                    if (cnt != NULL)
                        PARSE_FLOAT(value, cnt->set_canvas_left(__));
                    break;
                case A_SIZE:
                    if (cnt != NULL)
                        PARSE_FLOAT(value, cnt->set_radius(__));
                    break;
                default:
                {
                    bool set = sColor.set(att, value);
                    if (!set)
                        CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlCenter::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPCenter *cnt  = static_cast<LSPCenter *>(pWidget);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, cnt, cnt->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
        }
    } /* namespace ctl */
} /* namespace lsp */
