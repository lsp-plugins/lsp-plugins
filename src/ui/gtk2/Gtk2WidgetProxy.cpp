/*
 * Gtk2Window.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    Gtk2WidgetProxy::Gtk2WidgetProxy(plugin_ui *ui, widget_t w_class): Gtk2Widget(ui, w_class)
    {
        pGtk2Widget = NULL;
    }

    Gtk2WidgetProxy::~Gtk2WidgetProxy()
    {
    }

    void Gtk2WidgetProxy::add(IWidget *widget)
    {
        pGtk2Widget = static_cast<Gtk2Widget *>(widget);
    }

    GtkWidget *Gtk2WidgetProxy::widget()
    {
        return (pGtk2Widget != NULL) ? pGtk2Widget->widget() : NULL;
    }
} /* namespace lsp */
