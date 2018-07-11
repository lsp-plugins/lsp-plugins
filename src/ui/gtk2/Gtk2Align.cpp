/*
 * Gtk2Window.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    Gtk2Align::Gtk2Align(plugin_ui *ui): Gtk2Container(ui, W_ALIGN)
    {
        pWidget         = gtk_alignment_new(0.0, 0.0, 0.0, 0.0);
        nBorder         = 2;
        nPadLeft        = 0;
        nPadTop         = 0;
        nPadRight       = 0;
        nPadBottom      = 0;
        nVertPos        = 0.5;
        nHorPos         = 0.5;
        nVertScale      = 0.0;
        nHorScale       = 0.0;
    }

    Gtk2Align::~Gtk2Align()
    {
    }

    void Gtk2Align::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_BORDER:
                PARSE_INT(value, nBorder = size_t(__));
                break;
            case A_PAD_LEFT:
                PARSE_INT(value, nPadLeft = size_t(__));
                break;
            case A_PAD_TOP:
                PARSE_INT(value, nPadTop = size_t(__));
                break;
            case A_PAD_RIGHT:
                PARSE_INT(value, nPadRight = size_t(__));
                break;
            case A_PAD_BOTTOM:
                PARSE_INT(value, nPadBottom = size_t(__));
                break;
            case A_VPOS:
                PARSE_FLOAT(value, nVertPos = __);
                break;
            case A_HPOS:
                PARSE_FLOAT(value, nHorPos = __);
                break;
            case A_VSCALE:
                PARSE_FLOAT(value, nVertScale = __);
                break;
            case A_HSCALE:
                PARSE_FLOAT(value, nHorScale = __);
                break;
            default:
                Gtk2Container::set(att, value);
                break;
        }
    }

    void Gtk2Align::end()
    {
        gtk_alignment_set(GTK_ALIGNMENT(pWidget), nVertPos, nHorPos, nVertScale, nHorScale);
        gtk_alignment_set_padding(GTK_ALIGNMENT(pWidget), nBorder + nPadTop, nBorder + nPadBottom, nBorder + nPadLeft, nBorder + nPadRight);

        Gtk2Container::end();
    }

} /* namespace lsp */
