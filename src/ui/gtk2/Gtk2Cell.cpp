/*
 * Gtk2Window.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    Gtk2Cell::Gtk2Cell(plugin_ui *ui): Gtk2WidgetProxy(ui, W_CELL)
    {
        nRowSpan    = 1;
        nColSpan    = 1;
    }

    Gtk2Cell::~Gtk2Cell()
    {
    }

    void Gtk2Cell::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ROWS:
                PARSE_INT(value, nRowSpan = size_t(__));
                break;
            case A_COLS:
                PARSE_INT(value, nColSpan = size_t(__));
                break;

            default:
                if (pGtk2Widget != NULL)
                    pGtk2Widget->set(att, value);
                Gtk2Widget::set(att, value);
                break;
        }
    }

} /* namespace lsp */
