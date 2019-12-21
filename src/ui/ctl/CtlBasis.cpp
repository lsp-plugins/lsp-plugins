/*
 * CtlBasis.cpp
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlBasis::metadata = { "CtlBasis", &CtlWidget::metadata };

        CtlBasis::CtlBasis(CtlRegistry *src, LSPBasis *base): CtlWidget(src, base)
        {
            pClass          = &metadata;
        }

        CtlBasis::~CtlBasis()
        {
        }

        void CtlBasis::set(widget_attribute_t att, const char *value)
        {
            LSPBasis *base = (pWidget != NULL) ? static_cast<LSPBasis *>(pWidget) : NULL;

            switch (att)
            {
                case A_ID:
                    if (base != NULL)
                        PARSE_INT(value, base->set_id(__));
                    break;
                default:
                    CtlWidget::set(att, value);
                    break;
            }
        }
    } /* namespace ctl */
} /* namespace lsp */
