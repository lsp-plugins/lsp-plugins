/*
 * CtlAlign.cpp
 *
 *  Created on: 17 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        CtlAlign::CtlAlign(CtlRegistry *src, LSPAlign *widget): CtlWidget(src, widget)
        {
        }

        CtlAlign::~CtlAlign()
        {
        }

        void CtlAlign::init()
        {
            CtlWidget::init();

            if (pWidget == NULL)
                return;
        }

        void CtlAlign::set(widget_attribute_t att, const char *value)
        {
            LSPAlign *align     = widget_cast<LSPAlign>(pWidget);

            switch (att)
            {
                case A_VPOS:
                    if (align != NULL)
                        PARSE_FLOAT(value, align->set_vpos(__));
                    break;
                case A_HPOS:
                    if (align != NULL)
                        PARSE_FLOAT(value, align->set_hpos(__));
                    break;
                case A_VSCALE:
                    if (align != NULL)
                        PARSE_FLOAT(value, align->set_vscale(__));
                    break;
                case A_HSCALE:
                    if (align != NULL)
                        PARSE_FLOAT(value, align->set_hscale(__));
                    break;

                default:
                {
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        status_t CtlAlign::add(LSPWidget *child)
        {
            LSPAlign *align     = widget_cast<LSPAlign>(pWidget);
            if (align == NULL)
                return STATUS_BAD_STATE;
            return align->add(child);
        }
    } /* namespace ctl */
} /* namespace lsp */
