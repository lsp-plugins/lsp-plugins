/*
 * Gtk2Widget.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    Gtk2Widget::Gtk2Widget(plugin_ui *ui): IWidget(ui)
    {
        pWidget     = NULL;
        nWFlags     = 0;
        nAdded      = 0;
    }

    Gtk2Widget::~Gtk2Widget()
    {
        if (pWidget != NULL)
            pWidget = NULL;
    }

    GtkWidget *Gtk2Widget::widget()
    {
        return pWidget;
    };

    void Gtk2Widget::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_EXPAND:
                PARSE_BOOL(value,
                    if (__)
                        nWFlags     |= F_EXPAND;
                    else
                        nWFlags     &= ~F_EXPAND;
                );
                break;
            case A_FILL:
                PARSE_BOOL(value,
                    if (__)
                        nWFlags     |= F_FILL;
                    else
                        nWFlags     &= ~F_FILL;
                );
                break;
            default:
                IWidget::set(att, value);
                break;
        }
    }

    void Gtk2Widget::add(IWidget *widget)
    {
        Gtk2Widget *g_widget = dynamic_cast<Gtk2Widget *>(widget);
        if (g_widget != NULL)
        {
            if (GTK_IS_CONTAINER(pWidget))
            {
                gtk_container_add (GTK_CONTAINER (pWidget), g_widget->widget());
                nAdded++;
            }
            else
                lsp_error("Could not cast widget to container");
        }
    }
} /* namespace lsp */
