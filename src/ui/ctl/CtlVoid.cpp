/*
 * CtlVoid.cpp
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlVoid::metadata = { "CtlVoid", &CtlWidget::metadata };
        
        CtlVoid::CtlVoid(CtlRegistry *src, LSPVoid *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
        }
        
        CtlVoid::~CtlVoid()
        {
        }

        void CtlVoid::set(widget_attribute_t att, const char *value)
        {
            LSPVoid *v = widget_cast<LSPVoid>(pWidget);

            switch (att)
            {
                case A_MIN_WIDTH:
                    if (v != NULL)
                        PARSE_INT(value, v->constraints()->set_min_width(__));
                    break;
                case A_MIN_HEIGHT:
                    if (v != NULL)
                        PARSE_INT(value, v->constraints()->set_min_height(__));
                    break;
                case A_MAX_WIDTH:
                    if (v != NULL)
                        PARSE_INT(value, v->constraints()->set_max_width(__));
                    break;
                case A_MAX_HEIGHT:
                    if (v != NULL)
                        PARSE_INT(value, v->constraints()->set_max_height(__));
                    break;
                default:
                {
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

    } /* namespace ctl */
} /* namespace lsp */
