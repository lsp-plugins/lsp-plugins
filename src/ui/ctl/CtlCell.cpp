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
        CtlCell::CtlCell(CtlRegistry *src, LSPCell *cell): CtlWidget(src, cell)
        {
        }

        CtlCell::~CtlCell()
        {
        }

        void CtlCell::set(widget_attribute_t att, const char *value)
        {
            LSPCell *cell = (pWidget != NULL) ? static_cast<LSPCell *>(pWidget) : NULL;

            switch (att)
            {
                case A_ROWS:
                    if (cell != NULL)
                        PARSE_INT(value, cell->set_rowspan(__));
                    break;
                case A_COLS:
                    if (cell != NULL)
                        PARSE_INT(value, cell->set_colspan(__));
                    break;

                default:
                    CtlWidget::set(att, value);
                    break;
            }
        }

        status_t CtlCell::add(LSPWidget *child)
        {
            if (pWidget == NULL)
                return STATUS_BAD_STATE;

            LSPCell *cell   = static_cast<LSPCell *>(pWidget);
            return cell->add(child);
        }
    } /* namespace ctl */
} /* namespace lsp */
