/*
 * ui.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_UI_H_
#define _UI_GTK2_UI_H_

// Include common interface
#include <ui/ui.h>

// GTK library include
#include <gtk/gtk.h>

// Helper macros
#define GTK_PARSE_STRING(var, code) \
    { \
        gchar * __ = g_strdup(var); \
        if (__) \
        { \
            { code; } \
            g_free(__); \
        } \
    }

#define GTK_PANGO_STRING(var, code, format...) \
    { \
        gchar * __ = g_markup_printf_escaped(format); \
        if (__) \
        { \
            { code; } \
            g_free(__); \
        } \
    }

// Include GTK2 widgets
#include <ui/gtk2/Gtk2WidgetFactory.h>
#include <ui/gtk2/Gtk2Widget.h>
#include <ui/gtk2/Gtk2CustomWidget.h>
#include <ui/gtk2/Gtk2WidgetProxy.h>

#include <ui/gtk2/Gtk2Container.h>
#include <ui/gtk2/Gtk2Window.h>
#include <ui/gtk2/Gtk2Align.h>
#include <ui/gtk2/Gtk2Box.h>
#include <ui/gtk2/Gtk2Grid.h>
#include <ui/gtk2/Gtk2Cell.h>
#include <ui/gtk2/Gtk2Body.h>

#include <ui/gtk2/Gtk2Group.h>
#include <ui/gtk2/Gtk2ComboBox.h>
#include <ui/gtk2/Gtk2MountStud.h>
#include <ui/gtk2/Gtk2Button.h>
#include <ui/gtk2/Gtk2Switch.h>
#include <ui/gtk2/Gtk2Label.h>
#include <ui/gtk2/Gtk2Indicator.h>
#include <ui/gtk2/Gtk2Led.h>
#include <ui/gtk2/Gtk2Knob.h>
#include <ui/gtk2/Gtk2Graph.h>
#include <ui/gtk2/Gtk2Canvas.h>


#endif /* _UI_GTK2_UI_H_ */
