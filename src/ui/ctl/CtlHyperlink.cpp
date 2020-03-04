/*
 * CtlHyperlink.cpp
 *
 *  Created on: 23 окт. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlHyperlink::metadata = { "CtlHyperlink", &CtlWidget::metadata };

        CtlHyperlink::CtlHyperlink(CtlRegistry *src, LSPHyperlink *widget, ctl_label_type_t type): CtlWidget(src, widget)
        {
            pClass          = &metadata;
        }

        CtlHyperlink::~CtlHyperlink()
        {
        }

        void CtlHyperlink::init()
        {
            CtlWidget::init();

            if (pWidget == NULL)
                return;

            LSPHyperlink *hlink = widget_cast<LSPHyperlink>(pWidget);
            if (hlink == NULL)
                return;

            // Initialize color controllers
            sColor.init_hsl(pRegistry, hlink, hlink->font()->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sHoverColor.init_hsl(pRegistry, hlink, hlink->hover(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
        }

        void CtlHyperlink::set(widget_attribute_t att, const char *value)
        {
            LSPHyperlink *hlink = widget_cast<LSPHyperlink>(pWidget);

            switch (att)
            {
                case A_TEXT:
                    if (hlink != NULL)
                        hlink->text()->set_raw(value);
                    break;
                case A_URL:
                    if (hlink != NULL)
                        hlink->set_url(value);
                    break;
                case A_FONT_SIZE:
                    if (hlink != NULL)
                        PARSE_FLOAT(value, hlink->font()->set_size(__));
                    break;
                case A_VALIGN:
                    if (hlink != NULL)
                        PARSE_FLOAT(value, hlink->set_valign(__));
                    break;
                case A_HALIGN:
                    if (hlink != NULL)
                        PARSE_FLOAT(value, hlink->set_halign(__));
                    break;
                case A_BORDER:
                    PARSE_INT(value, hlink->set_border(__));
                    break;
                default:
                {
                    sColor.set(att, value);
                    sHoverColor.set(att, value);
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

    } /* namespace ctl */
} /* namespace lsp */
