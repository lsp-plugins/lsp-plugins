/*
 * Gtk2Window.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>
#include <ui/gtk2/override.h>

namespace lsp
{
    Gtk2Box::Gtk2Box(plugin_ui *ui, bool horizontal): Gtk2Container(ui, (horizontal) ? W_HBOX : W_VBOX)
    {
        nSize           = -1;
        pWidget         = (horizontal) ? gtk_hbox_new(FALSE, 0) : gtk_vbox_new(FALSE, 0);
        nBorder         = 0;
        nSpacing        = 0;
        bProportional   = false;
    }

    Gtk2Box::~Gtk2Box()
    {
    }

    void Gtk2Box::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_SIZE:
                PARSE_INT(value, nSize = size_t(__));
                break;
            case A_BORDER:
                PARSE_INT(value, nBorder = size_t(__));
                break;
            case A_SPACING:
                PARSE_INT(value, nSpacing = size_t(__));
                break;
            case A_PROPORTIONAL:
                PARSE_BOOL(value, bProportional = __);
                break;
            default:
                Gtk2Container::set(att, value);
                break;
        }
    }

    void Gtk2Box::end()
    {
        gtk_container_set_border_width(GTK_CONTAINER(pWidget), nBorder);
        gtk_box_set_spacing(GTK_BOX(pWidget), nSpacing);
        gtk_box_set_homogeneous(GTK_BOX(pWidget), (bProportional) ? TRUE : FALSE);

        if (nSize > 0)
        {
            while (nAdded < size_t(nSize))
            {
                gtk_container_add(GTK_CONTAINER (pWidget), gtk_label_new(NULL));
                nAdded++;
            }
        }

        Gtk2Container::end();
    }

    void Gtk2Box::add(IWidget *widget)
    {
        Gtk2Widget *g_widget = Gtk2Widget::cast(widget);
        if (g_widget == NULL)
            return;

        gtk_box_pack_start(GTK_BOX(pWidget), g_widget->widget(),
                g_widget->expand() ? TRUE : FALSE,
                g_widget->fill() ? TRUE : FALSE,
                0);
        nAdded++;
    }


} /* namespace lsp */
