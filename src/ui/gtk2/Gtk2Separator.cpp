/*
 * Gtk2Separator.cpp
 *
 *  Created on: 29 марта 2016 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>
#include <ui/gtk2/override.h>

namespace lsp
{
    OVERRIDE_GTK2_CONTROL(GTK_TYPE_HSEPARATOR, GtkHSeparatorClass, GtkHSeparator, Gtk2HSeparator)
    OVERRIDE_GTK2_CONTROL(GTK_TYPE_HSEPARATOR, GtkHSeparatorClass, GtkHSeparator, Gtk2VSeparator)

    //-------------------------------------------------------------------------
    Gtk2Separator::Gtk2Separator(plugin_ui *ui, widget_t w_class): Gtk2Widget(ui, w_class)
    {
        nBorder     = 2;
        nWidth      = 1;
        sColor.set(pUI->theme(), C_LABEL_TEXT);
        sBgColor.set(pUI->theme(), C_BACKGROUND);
    }

    Gtk2Separator::~Gtk2Separator()
    {
    }

    void Gtk2Separator::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
//            case A_BORDER:
//                PARSE_INT(value, nBorder = size_t(__));
//                break;
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
            default:
                Gtk2Widget::set(att, value);
                break;
        }
    }

    void Gtk2Separator::end()
    {
    }

    void Gtk2Separator::render()
    {
        // Get resource
        cairo_t *cr = gdk_cairo_create(pWidget->window);
        cairo_save(cr);

        ssize_t cx = pWidget->allocation.x + nBorder;
        ssize_t cy = pWidget->allocation.y + nBorder;
        ssize_t sx = pWidget->allocation.width - (nBorder << 1);
        ssize_t sy = pWidget->allocation.height - (nBorder << 1);

        // Draw background
        cairo_set_source_rgb(cr, sColor.red(), sColor.green(), sColor.blue());
        cairo_rectangle(cr, cx, cy, sx, sy);
        cairo_fill(cr);

        // Destroy cairo
        cairo_restore(cr);
        cairo_destroy(cr);
    }

    void Gtk2Separator::resize(ssize_t &w, ssize_t &h)
    {
        ssize_t d       = (nBorder << 1) + nWidth;
        if (enClass == W_VSEP)
        {
            if (w < d)
                w = d;
        }
        else
        {
            if (h < d)
                h = d;
        }
    }

    void Gtk2Separator::destroy()
    {
        pWidget     = NULL;
    }

    //-------------------------------------------------------------------------
    Gtk2HSeparator::Gtk2HSeparator(plugin_ui *ui): Gtk2Separator(ui, W_HSEP)
    {
        pWidget     = Gtk2HSeparatorImpl_new(this);
    }

    Gtk2HSeparator::~Gtk2HSeparator()
    {
        if (pWidget != NULL)
        {
            Gtk2HSeparatorImpl_delete(pWidget);
            pWidget = NULL;
        }
    }

    //-------------------------------------------------------------------------
    Gtk2VSeparator::Gtk2VSeparator(plugin_ui *ui): Gtk2Separator(ui, W_VSEP)
    {
        pWidget     = Gtk2VSeparatorImpl_new(this);
    }

    Gtk2VSeparator::~Gtk2VSeparator()
    {
        if (pWidget != NULL)
        {
            Gtk2VSeparatorImpl_delete(pWidget);
            pWidget = NULL;
        }
    }

} /* namespace lsp */
