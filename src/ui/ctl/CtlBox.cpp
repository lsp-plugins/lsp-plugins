/*
 * CtlBox.cpp
 *
 *  Created on: 28 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlBox::metadata = { "CtlBox", &CtlWidget::metadata };

        CtlBox::CtlBox(CtlRegistry *src, LSPBox *widget, ssize_t orientation): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            nOrientation    = orientation;
        }
        
        CtlBox::~CtlBox()
        {
        }

        void CtlBox::set(widget_attribute_t att, const char *value)
        {
            LSPBox *box     = widget_cast<LSPBox>(pWidget);

            switch (att)
            {
                case A_HORIZONTAL:
                    if ((box != NULL) && (nOrientation < 0))
                        PARSE_BOOL(value, box->set_horizontal(__));
                    break;
                case A_VERTICAL:
                    if ((box != NULL) && (nOrientation < 0))
                        PARSE_BOOL(value, box->set_vertical(__));
                    break;
                case A_SPACING:
                    if (box != NULL)
                        PARSE_INT(value, box->set_spacing(__));
                    break;
                default:
                    CtlWidget::set(att, value);
                    break;
            }
        }

        status_t CtlBox::add(CtlWidget *child)
        {
            LSPBox *box     = widget_cast<LSPBox>(pWidget);
            return (box != NULL) ? box->add(child->widget()) : STATUS_BAD_STATE;
        }

    } /* namespace ctl */
} /* namespace lsp */
