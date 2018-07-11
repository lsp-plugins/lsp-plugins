/*
 * Gtk2Graph.cpp
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

#include <string.h>
#include <math.h>


namespace lsp
{
    Gtk2Graph::Gtk2Graph(plugin_ui *ui): Gtk2CustomWidget(ui, W_GRAPH), IGraph()
    {
        sColor.set(pUI->theme(), C_GLASS);
        sBgColor.set(pUI->theme(), C_BACKGROUND);
        nSetWidth       = 0;
        nSetHeight      = 0;
        nBorder         = 12;
        nRadius         = 4;
        fVPos           = 0.5;
        fHPos           = 0.5;
        nPadding        = 2;
    }

    Gtk2Graph::~Gtk2Graph()
    {
    }

    void Gtk2Graph::resize(size_t &w, size_t &h)
    {
        size_t ww = nSetWidth + (nBorder << 1);
        size_t hh = nSetHeight + (nBorder << 1);
        if (w < ww)
            w = ww;
        if (h < hh)
            h = hh;
    }

    void Gtk2Graph::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_WIDTH:
                PARSE_INT(value, nSetWidth = size_t(__));
                break;
            case A_PADDING:
                PARSE_INT(value, nPadding = size_t(__));
                break;
            case A_HEIGHT:
                PARSE_INT(value, nSetHeight = size_t(__));
                break;
            case A_BORDER:
                PARSE_INT(value, nBorder = size_t(__));
                break;
            case A_SPACING:
                PARSE_INT(value, nRadius = size_t(__));
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
            case A_VPOS:
                PARSE_FLOAT(value, fVPos = __);
                break;
            case A_HPOS:
                PARSE_FLOAT(value, fHPos = __);
                break;
            default:
                Gtk2CustomWidget::set(att, value);
                break;
        }
    }

    void Gtk2Graph::notify(IUIPort *port)
    {
        Gtk2CustomWidget::notify(port);
    }

    void Gtk2Graph::render()
    {
        // Get resource
        cairo_pattern_t *cp;
        cairo_t *cr = gdk_cairo_create(pWidget->window);
        cairo_save(cr);

        // Draw background
        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_rectangle(cr, 0, 0, nWidth, nHeight);
        cairo_fill(cr);

        // Draw border
        size_t bw = nBorder;
        size_t pr = sqrtf(nWidth*nWidth + nHeight*nHeight);// >> 1;
        cairo_set_line_width(cr, 1);
        for (size_t i=0; i <= nRadius; ++i)
        {
            float bright = float(nRadius - i) / nRadius; // TODO
            Color c(1.0, 1.0, 1.0);
            c.blend(sColor, bright);

            cp = cairo_pattern_create_radial (bw + 1, nHeight - bw - 1, bw, 1, nHeight - bw - 1, pr * 1.5);
            cairo_pattern_add_color_stop_rgb(cp, 0.0, c.red(), c.green(), c.blue());
            cairo_pattern_add_color_stop_rgb(cp, 1.0, sColor.red(), sColor.green(), sColor.blue());

            cairo_set_source(cr, cp);

            cairo_arc(cr, bw, bw, bw - i, M_PI, 1.5 * M_PI);
            cairo_arc(cr, nWidth - bw, bw, bw - i, 1.5 * M_PI, 2.0 * M_PI);
            cairo_arc(cr, nWidth - bw, nHeight - bw, bw - i, 0.0, 0.5 * M_PI);
            cairo_arc(cr, bw, nHeight - bw, bw - i, 0.5 * M_PI, M_PI);
            cairo_close_path(cr);

            cairo_fill(cr);
            cairo_pattern_destroy(cp);
        }

        // Draw glass // TEMPORARY
        size_t bs = bw * M_SQRT2 * 0.5;
        size_t gw = nWidth  - (bs << 1);
        size_t gh = nHeight - (bs << 1);

        // Calculate center
        Gtk2Canvas cv(this, gw, gh, nPadding);
        cv.set_color(sColor);
        cv.clear();

        for (size_t i=0; i<nObjects; ++i)
            vObjects[i]->draw(&cv);

        cv.draw(cr, bs, bs);

        // Draw glass effect
        cp = cairo_pattern_create_radial (nWidth, 0, bw << 1, nWidth, 0, pr);
        cairo_pattern_add_color_stop_rgba(cp, 0.0, 1.0, 1.0, 1.0, 0.15);
        cairo_pattern_add_color_stop_rgba(cp, 1.0, 1.0, 1.0, 1.0, 0.0);

        cairo_set_source(cr, cp);

        cairo_arc(cr, bw, bw, bw - nRadius, M_PI, 1.5 * M_PI);
        cairo_arc(cr, nWidth - bw, bw, bw - nRadius, 1.5 * M_PI, 2.0 * M_PI);
        cairo_arc(cr, nWidth - bw, nHeight - bw, bw - nRadius, 0.0, 0.5 * M_PI);
        cairo_arc(cr, bw, nHeight - bw, bw - nRadius, 0.5 * M_PI, M_PI);
        cairo_close_path(cr);

        cairo_fill(cr);
        cairo_pattern_destroy(cp);

        // Release resource
        cairo_restore(cr);
        cairo_destroy(cr);
    }

    IGraphObject *Gtk2Graph::getGraphObject(IWidget *widget)
    {
        switch (widget->getClass())
        {
            case W_AXIS:
            case W_MARKER:
            case W_MESH:
            case W_CENTER:
            case W_TEXT:
                return static_cast<IGraphObject *>(widget);
            default:
                return NULL;
        }
    }

    void Gtk2Graph::add(IWidget *widget)
    {
        // Try to cast to graphics object
        IGraphObject *obj = getGraphObject(widget);
        if (obj != NULL)
            IGraph::addItem(obj);
        else
            return Gtk2CustomWidget::add(widget);
    }

    void Gtk2Graph::markRedraw()
    {
        Gtk2CustomWidget::markRedraw();
    }
} /* namespace lsp */
