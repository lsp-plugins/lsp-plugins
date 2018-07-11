/*
 * Gtk2ComboBox.cpp
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>
#include <ui/gtk2/override.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>

namespace lsp
{
    OVERRIDE_GTK2_CONTROL(GTK_TYPE_COMBO_BOX, GtkComboBoxClass, GtkComboBox, Gtk2ComboBox)

    Gtk2ComboBox::Gtk2ComboBox(plugin_ui *ui): Gtk2Widget(ui, W_COMBO)
    {
        pStore                      = gtk_list_store_new(1, G_TYPE_STRING);
        pWidget                     = Gtk2ComboBoxImpl_new(this, false);
        pRenderer                   = gtk_cell_renderer_text_new();
        hChangeHandler              = 0;
        fMin                        = 0;
        fMax                        = 0;
        fStep                       = 0;
        pPort                       = NULL;
        nWidth                      = 0;
        nHeight                     = 0;

        gtk_combo_box_set_model(GTK_COMBO_BOX(pWidget), GTK_TREE_MODEL(pStore));
        gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(pWidget), pRenderer, TRUE);
        gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(pWidget), pRenderer, "text", 0, NULL);

        sColor.set(pUI->theme(), C_LABEL_TEXT);
        sTextColor.set(pUI->theme(), C_BACKGROUND);
        sBgColor.set(pUI->theme(), C_BACKGROUND);
    }

    Gtk2ComboBox::~Gtk2ComboBox()
    {
        if (pWidget != NULL)
        {
            Gtk2ComboBoxImpl_delete(pWidget);
            pWidget = NULL;
        }
    }

    void Gtk2ComboBox::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                BIND_PORT(pUI, pPort, value);
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
            case A_TEXT_COLOR:
                sTextColor.set(pUI->theme(), value);
                break;
            case A_WIDTH:
                PARSE_INT(value, nWidth = __);
                break;
            case A_HEIGHT:
                PARSE_INT(value, nHeight = __);
                break;
            default:
                Gtk2Widget::set(att, value);
                break;
        }
    }

    void Gtk2ComboBox::apply_metadata_params(const port_t *p)
    {
        get_port_parameters(p, &fMin, &fMax, &fStep);

        if (p->unit == U_ENUM)
        {
            GtkTreeIter iter;

            size_t value    = pPort->getValue();
            size_t i        = 0;

            for (const char **item = p->items; (item != NULL) && (*item != NULL); ++item, ++i)
            {
                size_t key      = fMin + fStep * i;
                gtk_list_store_append(pStore ,&iter);
                gtk_list_store_set(pStore, &iter, 0, *item, -1);
                if (key == value)
                    gtk_combo_box_set_active_iter(GTK_COMBO_BOX(pWidget), &iter);
            }
        }
    }

    void Gtk2ComboBox::end()
    {
        if (pWidget == NULL)
            return;

        if (pPort != NULL)
            apply_metadata_params(pPort->metadata());

        hChangeHandler     = g_signal_connect(pWidget, "changed", G_CALLBACK(value_changed), gpointer(this));
        lsp_trace("Added change handler %x", int(hChangeHandler));
    }

    void Gtk2ComboBox::render()
    {
        // Get resource
        cairo_t *cr = gdk_cairo_create(pWidget->window);

        int x  = pWidget->allocation.x;
        int y  = pWidget->allocation.y;
        int sx = pWidget->allocation.width;
        int sy = pWidget->allocation.height;

        // Draw background
        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_rectangle(cr, x, y, sx, sy);
        cairo_fill(cr);

        // Draw Body
        cairo_set_source_rgb(cr, sColor.red(), sColor.green(), sColor.blue());
        cairo_move_to(cr, x, y);
        ssize_t r = 5;
        cairo_arc(cr, x + sx - 1 - r, y + r, r, M_PI * 1.5, M_PI * 2);
        cairo_arc(cr, x + sx - 1 - r, y + sy - 1- r, r, 0, M_PI * 0.5);
        cairo_arc(cr, x + r, y + sy - 1 - r, r, M_PI * 0.5, M_PI);
        cairo_close_path(cr);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_move_to(cr, x + sx - 14, y);
        cairo_line_to(cr, x + sx - 14, y + sy - 1);
        cairo_stroke(cr);

        // Draw button
        cairo_move_to(cr, x + sx - 7, y + r);
        cairo_line_to(cr, x + sx - r + 2, y + sy * 0.5);
        cairo_line_to(cr, x + sx - 7, y + sy - r - 1);
        cairo_line_to(cr, x + sx - 16 + r, y + sy * 0.5);
        cairo_close_path(cr);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, sColor.red(), sColor.green(), sColor.blue());
        cairo_move_to(cr, x + sx - r + 2, y + sy * 0.5);
        cairo_line_to(cr, x + sx - 16 + r, y + sy * 0.5);
        cairo_stroke(cr);

        // Output text
        cairo_set_source_rgb(cr, 0, 0, 0);
//        cairo_rectangle(cr, x + r - 2, y + r - 2, x + sx - 16 - r + 2, y + sy - r - 1 + 2);
        cairo_rectangle(cr, x + r - 2, y + r - 2, sx - 1 - 14 - (r - 2) * 2, sy - 1 - (r - 2) * 2);
//        cairo_fill(cr);
        cairo_clip(cr);

//        cairo_text_extents_t extents;
        GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX (pWidget));
        GtkTreeIter iter;
        gchar *text;
        if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (pWidget), &iter))
            gtk_tree_model_get (model, &iter, 0, &text, -1);
        else
            text = g_strdup("----------------");

        cairo_select_font_face(cr, "Sans",
              CAIRO_FONT_SLANT_NORMAL,
              CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 12);

        cairo_move_to (cr, x + r, y + (sy >> 1) + r);
        cairo_set_source_rgb(cr, sTextColor.red(), sTextColor.green(), sTextColor.blue());
        cairo_show_text (cr, text);
        g_free(text);

        cairo_destroy(cr);
    }

    void Gtk2ComboBox::resize(ssize_t &w, ssize_t &h)
    {
        cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
        cairo_t *cr = cairo_create(surface);

        ssize_t width = 16 + nWidth;
        ssize_t height = (5-2) * 2 + nHeight;

        cairo_text_extents_t extents;
        cairo_select_font_face(cr, "Sans",
              CAIRO_FONT_SLANT_NORMAL,
              CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 12);
        cairo_text_extents(cr, "Wpg", &extents);

        width += extents.width;
        height += extents.height;

        if ((w < width) || (nWidth > 0))
            w = width;
        if ((h < height) || (nHeight > 0))
            h = height;
//        w = 64;
//        h = 24;

        cairo_destroy(cr);
        cairo_surface_destroy(surface);
    }

    void Gtk2ComboBox::destroy()
    {
        lsp_trace("destroy");
        if ((pWidget != NULL) && (hChangeHandler > 0))
        {
            if (g_signal_handler_is_connected (pWidget, hChangeHandler))
            {
                lsp_trace("Removing change handler %x", int(hChangeHandler));
                g_signal_handler_disconnect(pWidget, hChangeHandler);
            }
        }
        pWidget     = NULL;
    }

    void Gtk2ComboBox::value_changed(GtkComboBox *widget, gpointer user_data)
    {
        lsp_trace("user_data = %p", user_data);
        Gtk2ComboBox *_this = reinterpret_cast<Gtk2ComboBox *>(user_data);
        if (_this == NULL)
            return;
        _this->changed();
    }

    void Gtk2ComboBox::changed()
    {
        if (pWidget == NULL)
        {
            lsp_trace("pWidget == NULL");
            return;
        }
        size_t index = gtk_combo_box_get_active(GTK_COMBO_BOX(pWidget));
        float value = fMin + fStep * index;
        lsp_trace("index = %d, value=%f", int(index), value);

        pPort->setValue(value);
        pPort->notifyAll();
    }

    void Gtk2ComboBox::notify(IUIPort *port)
    {
        Gtk2Widget::notify(port);

        if ((pPort == port) && (pWidget != NULL))
        {
            size_t index = (pPort->getValue() - fMin) / fStep;
//            lsp_trace("index = %d, value=%f", int(index), pPort->getValue());
            gtk_combo_box_set_active(GTK_COMBO_BOX(pWidget), index);
        }
    }

} /* namespace lsp */
