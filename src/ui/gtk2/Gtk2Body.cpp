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
    Gtk2Body::Gtk2Body(plugin_ui *ui): Gtk2Container(ui, W_BODY)
    {
        pWidget     = gtk_event_box_new();
//        gtk_widget_add_events(pWidget,
//                    GDK_POINTER_MOTION_MASK |
//                    GDK_BUTTON_MOTION_MASK |
//                    GDK_SCROLL_MASK |
//                    GDK_BUTTON_PRESS_MASK |
//                    GDK_BUTTON_RELEASE_MASK |
//                    GDK_ENTER_NOTIFY_MASK |
//                    GDK_LEAVE_NOTIFY_MASK
//                  );
    }

    Gtk2Body::~Gtk2Body()
    {
    }

} /* namespace lsp */
