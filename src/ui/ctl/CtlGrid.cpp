/*
 * CtlGrid.cpp
 *
 *  Created on: 5 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        CtlGrid::CtlGrid(CtlRegistry *src, LSPGrid *widget, ssize_t orientation): CtlWidget(src, widget)
        {
            nOrientation    = orientation;
        }

        CtlGrid::~CtlGrid()
        {
        }

        void CtlGrid::set(widget_attribute_t att, const char *value)
        {
            LSPGrid *grid   = (pWidget != NULL) ? static_cast<LSPGrid *>(pWidget) : NULL;

            switch (att)
            {
                case A_ROWS:
                    if (grid != NULL)
                        PARSE_INT(value, grid->set_rows(__));
                    break;
                case A_COLS:
                    if (grid != NULL)
                        PARSE_INT(value, grid->set_columns(__));
                    break;
//                case A_BORDER:
//                    PARSE_INT(value, grid->set_border(__));
//                    break;
                case A_VSPACING:
                    PARSE_INT(value, grid->set_vspacing(__));
                    break;
                case A_HSPACING:
                    PARSE_INT(value, grid->set_hspacing(__));
                    break;
                case A_SPACING:
                    PARSE_INT(value, grid->set_spacing(__, __));
                    break;
                case A_HORIZONTAL:
                    if ((grid != NULL) && (nOrientation < 0))
                        PARSE_BOOL(value, grid->set_horizontal(__));
                    break;
                case A_VERTICAL:
                case A_TRANSPOSE:
                    if ((grid != NULL) && (nOrientation < 0))
                        PARSE_BOOL(value, grid->set_vertical(__));
                    break;
                default:
                    CtlWidget::set(att, value);
                    break;
            }
        }

        status_t CtlGrid::add(LSPWidget *child)
        {
            if (pWidget == NULL)
                return STATUS_BAD_STATE;

            LSPGrid *grid       = static_cast<LSPGrid *>(pWidget);
            return grid->add(child);
        }
    } /* namespace ctl */
} /* namespace lsp */
