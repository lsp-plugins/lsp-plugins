/*
 * Gtk2Group.cpp
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>
#include <ui/gtk2/override.h>
#include <core/alloc.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>

namespace lsp
{
    OVERRIDE_GTK2_CONTROL(GTK_TYPE_FRAME, GtkFrameClass, GtkFrame, Gtk2Group)

    Gtk2Group::Gtk2Group(plugin_ui *ui): Gtk2Widget(ui, W_GROUP)
    {
        pWidget     = Gtk2GroupImpl_new(this);

        sText       = NULL;
        sColor.set(pUI->theme(), C_LABEL_TEXT);
        sTextColor.set(pUI->theme(), C_BACKGROUND);
        sBgColor.set(pUI->theme(), C_BACKGROUND);
        sRadius     = 10;
        nBorder     = 2;
    }

    Gtk2Group::~Gtk2Group()
    {
        if (pWidget != NULL)
        {
            Gtk2GroupImpl_delete(pWidget);
            pWidget = NULL;
        }
        if (sText != NULL)
        {
            lsp_free(sText);
            sText = NULL;
        }
    }

    void Gtk2Group::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_TEXT:
                if (sText != NULL)
                {
                    lsp_free(sText);
                    sText = NULL;
                }
                sText = lsp_strdup(value);
                GTK_PARSE_STRING(value, gtk_frame_set_label(GTK_FRAME(pWidget), " "));
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_BORDER:
                PARSE_INT(value, nBorder = size_t(__));
                break;
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
            case A_TEXT_COLOR:
                sTextColor.set(pUI->theme(), value);
                break;
            default:
                Gtk2Widget::set(att, value);
                break;
        }
    }

    void Gtk2Group::end()
    {
        size_t bw = round(sRadius * M_SQRT2 * 0.5) + nBorder + 1;
        gtk_container_set_border_width(GTK_CONTAINER(pWidget), bw);
    }

    void Gtk2Group::render()
    {
        // Get resource
        cairo_t *cr = gdk_cairo_create(pWidget->window);
        cairo_save(cr);

        ssize_t cx = pWidget->allocation.x;
        ssize_t cy = pWidget->allocation.y;
        ssize_t sx = pWidget->allocation.width - (nBorder << 1);
        ssize_t sy = pWidget->allocation.height - (nBorder << 1);
        size_t bw   = round(sRadius * M_SQRT2 * 0.5) + 1;

        // Draw background
        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_rectangle(cr, cx, cy, sx, sy);
        cairo_fill(cr);

        // Change coordinates
        cairo_translate(cr, cx + nBorder, cy + nBorder);
        cairo_set_line_width(cr, 2);

        // Draw frame
        cairo_set_source_rgb(cr, sColor.red(), sColor.green(), sColor.blue());
        cairo_move_to(cr, 1, 1);
        cairo_line_to(cr, sx - sRadius - 1, 1);
        cairo_arc(cr, sx - sRadius - 1, sRadius + 1, sRadius, 1.5 * M_PI, 2.0 * M_PI);
        cairo_line_to(cr, sx - 1, sy - sRadius - 1);
        cairo_arc(cr, sx - sRadius - 1, sy - sRadius - 1, sRadius, 0.0, 0.5 * M_PI);
        cairo_line_to(cr, sRadius + 1, sy - 1);
        cairo_arc(cr, sRadius + 1, sy - sRadius - 1, sRadius, 0.5 * M_PI, M_PI);
        cairo_close_path(cr);
        cairo_stroke(cr);

        // Draw text frame
        const char *text = sText;
        if ((text != NULL) && (strlen(text) > 0))
        {
            // Draw text border
            cairo_text_extents_t extents;
            cairo_set_source_rgb(cr, sColor.red(), sColor.green(), sColor.blue());

            cairo_select_font_face(cr, "Sans",
                  CAIRO_FONT_SLANT_NORMAL,
                  CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(cr, 12);
            cairo_text_extents(cr, text, &extents);

            float l = 1, t = 1, r = bw + extents.width + bw;

            cairo_text_extents(cr, "Wpg", &extents);

            float b = bw - extents.y_bearing;
            float v = b - t - sRadius;

            cairo_move_to(cr, l, t);
            cairo_line_to(cr, r, t);
            if (v > 0.0)
                cairo_line_to(cr, r, t + v);
            cairo_arc(cr, r - sRadius, b - sRadius, sRadius, 0.0, 0.5 * M_PI);
            cairo_line_to(cr, l, b);
            cairo_close_path(cr);
            cairo_fill(cr);

            // Show text
            cairo_move_to (cr, l + (bw >> 1), b - (bw >> 1));
            cairo_set_source_rgb(cr, sTextColor.red(), sTextColor.green(), sTextColor.blue());
            cairo_show_text (cr, text);
        }

        // Destroy cairo
        cairo_restore(cr);
        cairo_destroy(cr);
    }

    void Gtk2Group::resize(ssize_t &w, ssize_t &h)
    {
        cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
        cairo_t *cr = cairo_create(surface);
        size_t bw = round(sRadius * M_SQRT2 * 0.5) + 1;

        ssize_t width   = (bw + nBorder + 1) * 3;
        ssize_t height  = (bw + nBorder + 1) * 3;

        const char *text = sText;
        if ((text != NULL) && (strlen(text) > 0))
        {
            cairo_text_extents_t extents;
            cairo_select_font_face(cr, "Sans",
                  CAIRO_FONT_SLANT_NORMAL,
                  CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(cr, 12);
            cairo_text_extents(cr, text, &extents);

            width += extents.width;

            cairo_text_extents(cr, "Wpg", &extents);
            height += extents.height;
        }

        if (w < width)
            w = width;
        if (h < height)
            h = height;

        // Align to 8-pixel grid
        w   = ((w + 7) >> 3) << 3;
        h   = ((h + 7) >> 3) << 3;

        cairo_destroy(cr);
        cairo_surface_destroy(surface);
    }

    void Gtk2Group::destroy()
    {
        pWidget     = NULL;
    }

} /* namespace lsp */
