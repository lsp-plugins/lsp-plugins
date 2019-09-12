/*
 * CtlCell.cpp
 *
 *  Created on: 17 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlCell::metadata = { "CtlCell", &CtlWidget::metadata };

        CtlCell::CtlCell(CtlRegistry *src): CtlWidget(src, NULL)
        {
            pClass          = &metadata;
            pChild          = NULL;
            nRows           = 1;
            nCols           = 1;
        }

        CtlCell::~CtlCell()
        {
            for (size_t i=0, n=vParams.size(); i<n; ++i)
            {
                param_t *p = vParams.at(i);
                if (p != NULL)
                    ::free(p);
            }
            vParams.flush();
        }

        LSPWidget *CtlCell::widget()
        {
            return (pChild != NULL) ? pChild->widget() : pWidget;
        }

        void CtlCell::set(widget_attribute_t att, const char *value)
        {
            switch (att)
            {
                case A_ROWS:
                    PARSE_INT(value, nRows = __ );
                    break;
                case A_COLS:
                    PARSE_INT(value, nCols = __ );
                    break;

                default:
                {
                    size_t ssize    = (::strlen(value) + 1) * sizeof(char);
                    size_t to_alloc = ALIGN_SIZE(sizeof(param_t) + ssize, DEFAULT_ALIGN);
                    param_t *p      = reinterpret_cast<param_t *>(::malloc(to_alloc));
                    if (p == NULL)
                        return;

                    if (vParams.add(p))
                    {
                        p->attribute    = att;
                        ::memcpy(p->value, value, ssize);
                    }
                    else
                        ::free(p);

                    break;
                }
            }
        }

        status_t CtlCell::add(CtlWidget *child)
        {
            pChild  = child;

            // Apply settings to the child
            if (child != NULL)
            {
                for (size_t i=0, n=vParams.size(); i<n; ++i)
                {
                    param_t *p = vParams.at(i);
                    if (p != NULL)
                        child->set(p->attribute, p->value);
                }
            }

            return STATUS_OK;
        }


    } /* namespace ctl */
} /* namespace lsp */
