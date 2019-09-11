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
            LSPCell *cell = widget_cast<LSPCell>(pWidget);

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
            LSPCell *cell   = widget_cast<LSPCell>(pWidget);
            if (cell == NULL)
                return STATUS_BAD_STATE;

            status_t res = cell->add(child);
            if (res != STATUS_OK)
                return res;

            // Apply cell changes to the child widget
            LSPWidget *dst = cell->unwrap();
            if (dst != NULL)
            {
                dst->padding()->set(cell->padding());
                dst->set_visible(cell->visible());
                dst->set_expand(cell->expand());
                dst->set_hfill(cell->hfill());
                dst->set_vfill(cell->vfill());
            }
            return STATUS_OK;
        }


    } /* namespace ctl */
} /* namespace lsp */
