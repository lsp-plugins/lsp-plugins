/*
 * Gtk2Widget.cpp
 *
 *  Created on, 20 окт. 2015 г.
 *      Author, sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    Gtk2Widget::Gtk2Widget(plugin_ui *ui, widget_t w_class): IWidget(ui, w_class)
    {
        pWidget         = NULL;
        nWFlags         = 0;
        nAdded          = 0;
        atomic_init(lkWRedraw);
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
        if (GTK_IS_CONTAINER(pWidget))
        {
            Gtk2Widget *g_widget = Gtk2Widget::cast(widget);
            if (g_widget != NULL)
            {
                GtkWidget *w = g_widget->widget();
                gtk_container_add (GTK_CONTAINER (pWidget), w);
                nAdded++;
            }
        }
        else
            lsp_error("Could not cast widget to container");
    }

    void Gtk2Widget::update_gtk2_visibility()
    {
        if ((pWidget != NULL) && (GTK_IS_WIDGET(pWidget)))
        {
            gtk_widget_set_no_show_all (pWidget, !bVisible);
            if (bVisible)
                gtk_widget_show_all(GTK_WIDGET(pWidget));
            else
                gtk_widget_hide_all(GTK_WIDGET(pWidget));
            gtk_widget_set_visible(GTK_WIDGET(pWidget), bVisible);

            if (bVisible)
                gtk_widget_queue_draw(pWidget);
        }
    }

    void Gtk2Widget::hide()
    {
        IWidget::hide();
        update_gtk2_visibility();
    }

    void Gtk2Widget::show()
    {
        IWidget::show();
        update_gtk2_visibility();
    }

    void Gtk2Widget::draw()
    {
        // Call GTK subsystem for widget redraw
        if (pWidget != NULL)
            gtk_widget_queue_draw(pWidget);
    }

    void Gtk2Widget::allowRedraw()
    {
        atomic_unlock(lkWRedraw);
    }

    void Gtk2Widget::markRedraw()
    {
        if (pUI == NULL)
            return;

//        lsp_trace("wflags = %x", int(nWFlags));
        if (!atomic_lock(lkWRedraw))
            return;

        // Queue redraw event on the UI
        if (!pUI->queue_redraw(this))
            atomic_unlock(lkWRedraw);
    }

    Gtk2Widget *Gtk2Widget::cast(IWidget *widget)
    {
        switch (widget->getClass())
        {
            case W_PLUGIN:
            case W_MSTUD:
            case W_BUTTON:
            case W_VBOX:
            case W_HBOX:
            case W_GRID:
            case W_ROW:
            case W_CELL:
            case W_LABEL:
            case W_PARAM:
            case W_VALUE:
            case W_INDICATOR:
            case W_LED:
            case W_KNOB:
            case W_GROUP:
            case W_ALIGN:
            case W_SWITCH:
            case W_GRAPH:
            case W_BODY:
            case W_COMBO:
            case W_TEXT:
            case W_FILE:
            case W_HSEP:
            case W_VSEP:
            case W_METER:
                return static_cast<Gtk2Widget *>(widget);

            case W_AXIS:
            case W_MARKER:
            case W_MESH:
            case W_BASIS:
            case W_CENTER:
            case W_PORT:
                return NULL;

            default:
                return NULL;
        }
    }

} /* namespace lsp */
