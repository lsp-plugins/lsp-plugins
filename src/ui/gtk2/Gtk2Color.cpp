/*
 * Gtk2Color.cpp
 *
 *  Created on: 26 окт. 2015 г.
 *      Author: sadko
 */

#include "ui/gtk2/ui.h"

namespace lsp
{
    void Gtk2Color::calc_gdk_color() const
    {
        if (nMask & M_GDK)
            return;

        sColor.red      =   R * 0xffff;
        sColor.green    =   G * 0xffff;
        sColor.blue     =   B * 0xffff;

        nMask          |= M_GDK;
    }

} /* namespace lsp */
