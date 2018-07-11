/*
 * Gtk2Window.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    Gtk2WidgetProxy::Gtk2WidgetProxy(plugin_ui *ui): Gtk2Widget(ui)
    {
        pGtk2Widget = NULL;
    }

    Gtk2WidgetProxy::~Gtk2WidgetProxy()
    {
    }

    void Gtk2WidgetProxy::add(IWidget *widget)
    {
        Gtk2Widget *g_widget = dynamic_cast<Gtk2Widget *>(widget);
        if (g_widget != NULL)
            pGtk2Widget = g_widget;
    }

    GtkWidget *Gtk2WidgetProxy::widget()
    {
        return (pGtk2Widget != NULL) ? pGtk2Widget->widget() : NULL;
    }
} /* namespace lsp */
