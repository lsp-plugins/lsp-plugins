/*
 * Gtk2Body.cpp
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>

namespace lsp
{
    Gtk2Body::Gtk2Body(plugin_ui *ui): Gtk2Container(ui)
    {
        pWidget     = gtk_event_box_new();
    }

    Gtk2Body::~Gtk2Body()
    {
    }

} /* namespace lsp */
