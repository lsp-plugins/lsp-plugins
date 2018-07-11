/*
 * Gtk2MountStud.cpp
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>
#include <core/debug.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

namespace lsp
{
    Gtk2MountStud::Gtk2MountStud(plugin_ui *ui): Gtk2CustomWidget(ui, W_MSTUD)
    {
        sBgColor.set(pUI->theme(), C_BACKGROUND);
        sColor.set(pUI->theme(), C_LOGO_FACE);
        sTextColor.set(pUI->theme(), C_LOGO_TEXT);

        bLeft       = false;
        sText       = NULL;
        nSize       = 32;
    }

    Gtk2MountStud::~Gtk2MountStud()
    {
    }

    void Gtk2MountStud::draw_screw(cairo_t *cr, size_t x, size_t y, float angle)
    {
        cairo_pattern_t *cp;

        size_t h_s = 8;
        size_t h_rr = 3;

        // Draw hole
        cairo_set_line_width(cr, 1.0);
        Gtk2Color hole(sBgColor); //pUI->theme(), C_HOLE);
        float hlb = hole.lightness() + 0.5;
        float hld = 0;

        for (size_t i=0; i<=h_rr; ++i)
        {
            float bright = (hlb - hld) * (h_rr - i) / h_rr + hld;
//            hole.lighten(0.1);
            hole.lightness(bright);

            cp = cairo_pattern_create_radial(x - h_s, y + h_s, h_s >> 2, x - h_s, y + h_s, h_s << 1);
            cairo_pattern_add_color_stop_rgb(cp, 0.0, hole.red(), hole.green(), hole.blue());
            cairo_pattern_add_color_stop_rgb(cp, 1.0, 0.5 * hole.red(), 0.5 *  hole.green(), 0.5 * hole.blue());
            cairo_set_source(cr, cp);

//            cairo_set_source_rgb(cr, hole.red(), hole.green(), hole.blue());
            cairo_arc(cr, x + (h_s * 0.75), y, h_s - i, 1.5 * M_PI, 2.5 * M_PI);
            cairo_arc(cr, x - (h_s * 0.75), y, h_s - i, 0.5 * M_PI, 1.5 * M_PI);
            cairo_close_path(cr);
//            cairo_stroke(cr);
            cairo_fill(cr);

            cairo_pattern_destroy(cp);
        }

        // Draw mounting stud body
        Gtk2Color stud(0, 0, 0);
        size_t f_rr     = h_s - 3;
        float slb       = stud.lightness();
        float sle       = slb + 0.2;

        for (size_t i=0; i<=f_rr; ++i)
        {
            Gtk2Color c(0, 0, 0);
            c.blend(stud, float(f_rr - i) / f_rr);
            float bright = (sle - slb) * sinf(M_PI * i / f_rr) + slb;

            c.lightness(bright * i / f_rr);
            cp = cairo_pattern_create_radial(x + (h_s >> 1), y - (h_s >> 1), 0, x + (h_s >> 1), y - (h_s >> 1), h_s * 1.5);
            cairo_pattern_add_color_stop_rgb(cp, 0.0, 1.0, 1.0, 1.0);
            cairo_pattern_add_color_stop_rgb(cp, 1.0, 0.5 * c.red(), 0.5 * c.green(), 0.5 * c.blue());
            cairo_set_source(cr, cp);

            cairo_arc(cr, x, y, h_s + 2 - i, 0, 2.0 * M_PI);
            cairo_fill(cr);

            cairo_pattern_destroy(cp);
        }

        // Draw cross
        size_t c_rr     = 3;
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

        for (size_t i=0; i < c_rr; ++i)
        {
            float a_cos = (h_s - i) * cosf(angle), a_sin = (h_s - i) * sinf(angle);
            float bright = float(i) / c_rr;
            Gtk2Color c(1, 1, 1);
            c.blend(0.5, 0.5, 0.5, bright);

            cairo_set_line_width(cr, c_rr - i);

            cp = cairo_pattern_create_radial(x - (h_s >> 1), y + (h_s >> 1), 0, x - (h_s >> 1), y + (h_s >> 1), h_s * 1.5);
            cairo_pattern_add_color_stop_rgb(cp, 0.0, c.red(), c.green(), c.blue());
            cairo_pattern_add_color_stop_rgb(cp, 1.0, 0, 0, 0);
            cairo_set_source(cr, cp);

            cairo_move_to(cr, x + a_cos, y + a_sin);
            cairo_line_to(cr, x - a_cos, y - a_sin);
            cairo_stroke(cr);
            cairo_move_to(cr, x - a_sin, y + a_cos);
            cairo_line_to(cr, x + a_sin, y - a_cos);
            cairo_stroke(cr);

            cairo_pattern_destroy(cp);
        }
    }

    void Gtk2MountStud::render()
    {
        cairo_text_extents_t te;
        cairo_pattern_t *cp;

        // Get resource
        cairo_t *cr = gdk_cairo_create(pWidget->window);
        cairo_save(cr);

        // Draw background
        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_rectangle(cr, 0, 0, nWidth, nHeight);
        cairo_fill(cr);

//        // Draw left and right
//        cairo_set_source_rgb(cr, 1, 1, 1);
//        cairo_rectangle(cr, 0, 0, nWidth -1 , nHeight -1);
//        cairo_stroke(cr);

        // Draw screws
        float angle = (bLeft) ? 0 : 1;
//        size_t pos = (bLeft) ? SCREW_SIZE + 2 : nWidth - SCREW_SIZE - 2;
//        size_t base = (bLeft) ? 2 : nWidth - 2;
        size_t screw = (bLeft) ? SCREW_SIZE >> 1 : nWidth - (SCREW_SIZE >> 1);
//        ssize_t c_dir = (bLeft) ? 1 : -1;

        draw_screw(cr, screw, STUD_H * 0.75, M_PI * (angle + 1) / 8 + M_PI / 16);
        draw_screw(cr, screw, nHeight - STUD_H * 0.75, M_PI * (angle + 3) / 8 + M_PI / 16);

//        // Draw envelope
//        size_t e_rr  = 3;
//        for (size_t i=0; i<=e_rr; ++i)
//        {
//            float bright = float(i*i) / float(e_rr * e_rr);
//            Color c(0.75, 0.75, 0.75);
//            c.blend(sBgColor, bright);
//
//            cairo_set_source_rgb(cr, c.red(), c.green(), c.blue());
//            cairo_move_to(cr, pos, 0);
//            cairo_line_to(cr, pos, STUD_H);
//            cairo_curve_to(cr, pos, STUD_H + CURVE * 2, base, STUD_H, base, STUD_H + CURVE * 2);
//            cairo_line_to(cr, base, nHeight - STUD_H - CURVE * 2);
//            cairo_curve_to(cr, base, nHeight - STUD_H, pos, nHeight - STUD_H - CURVE * 2, pos, nHeight - STUD_H);
//            cairo_line_to(cr, pos, nHeight);
//            cairo_stroke(cr);
//
//            pos  += c_dir;
//            base += c_dir;
//        }

        // Draw logo with text
        size_t l_r  = 5; // logo radius

        if (sText != NULL)
        {
            cairo_select_font_face(cr, "Arial",
                  CAIRO_FONT_SLANT_NORMAL,
                  CAIRO_FONT_WEIGHT_BOLD);
            cairo_set_font_size(cr, 16);

            cairo_text_extents(cr, "WWW0", &te);
            size_t min_w    = te.width;

            cairo_text_extents(cr, sText, &te);
            size_t lw       = te.width;
            size_t lh       = te.height;

            if (lw < min_w)
                lw = min_w;
            if (lh < nSize)
                lh = nSize;

            lw         += l_r << 1;
            lh         += l_r << 1;

            Gtk2Color logo(sColor);
            float logo_l    = logo.lightness();
            float l_rr      = 3;
            size_t l_x      = (bLeft) ? 8 : l_rr;
            size_t l_y      = (nHeight - lh) >> 1;

            for (size_t i=0; i<=l_rr; ++i)
            {
                float bright = logo_l * (i + 1) / (l_rr + 1);

                cp = cairo_pattern_create_radial(l_x + lw, l_y , lw >> 2, l_x + lw, l_y , lw);
                logo.lightness(bright * 1.5);
                cairo_pattern_add_color_stop_rgb(cp, 0.0, logo.red(), logo.green(), logo.blue());
                logo.lightness(bright);
                cairo_pattern_add_color_stop_rgb(cp, 1.0, logo.red(), logo.green(), logo.blue());
                cairo_set_source(cr, cp);

//                cairo_set_source_rgb(cr, logo.red(), logo.green(), logo.blue());
                cairo_arc(cr, l_x + l_r, l_y + l_r, l_r - i, M_PI, 1.5 * M_PI);
                cairo_arc(cr, l_x + lw - l_r, l_y + l_r, l_r - i, 1.5 * M_PI, 2.0 * M_PI);
                cairo_arc(cr, l_x + lw - l_r, l_y + lh - l_r, l_r - i, 0.0, 0.5 * M_PI);
                cairo_arc(cr, l_x + l_r, l_y + lh - l_r, l_r - i, 0.5 * M_PI, M_PI);
                cairo_close_path(cr);

                cairo_fill(cr);
            }

            // Now l_x, ly become center of logo
            l_x        += (lw >> 1);
            l_y        += (lh >> 1);

            // Output text
            cairo_set_source_rgb(cr, sTextColor.red(), sTextColor.green(), sTextColor.blue());
            cairo_select_font_face(cr, "Arial",
              CAIRO_FONT_SLANT_NORMAL,
              CAIRO_FONT_WEIGHT_BOLD);
            cairo_set_font_size(cr, 16);

            cairo_move_to(cr, l_x - (te.width + te.x_bearing) * 0.5, l_y - te.height * 0.5 - te.y_bearing);
            cairo_show_text (cr, sText);
        }

        // Release resource
        cairo_restore(cr);
        cairo_destroy(cr);
    }

    void Gtk2MountStud::resize(size_t &w, size_t &h)
    {
        cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 1, 1);
        cairo_t *cr = cairo_create(surface);

        size_t min_h = (STUD_H + CURVE * 2);
        size_t min_w = (8 + 3) * 2;

        cairo_text_extents_t te;
        cairo_select_font_face(cr, "Arial",
              CAIRO_FONT_SLANT_NORMAL,
              CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 16);

        cairo_text_extents(cr, "WWW0", &te);
        min_w          += te.width;
        min_h          += (te.height > nSize) ? te.height : nSize;

        if (w < min_w)
            w   = min_w;
        if (h < min_h)
            h = min_h;

        cairo_destroy(cr);
        cairo_surface_destroy(surface);
    }

    void Gtk2MountStud::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_TEXT:
                if (sText != NULL)
                {
                    free(sText);
                    sText = NULL;
                }
                if (strlen(value) > 0)
                    sText = strdup(value);
                break;
            case A_ANGLE:
                PARSE_INT(value, bLeft = (__ % 2));
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_TEXT_COLOR:
                sTextColor.set(pUI->theme(), value);
                break;
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
            default:
                Gtk2CustomWidget::set(att, value);
                break;
        }
    }

} /* namespace lsp */
