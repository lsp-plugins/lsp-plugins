/*
 * Basis.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <ui/graphics.h>

#include <math.h>


namespace lsp
{
    Basis::Basis(plugin_ui *ui): IWidget(ui, W_BASIS)
    {
        nID         = -1;
    }

    Basis::~Basis()
    {
    }

    void Basis::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                PARSE_INT(value, nID = __);
                break;
            default:
                IWidget::set(att, value);
                break;
        }
    }
} /* namespace lsp */
