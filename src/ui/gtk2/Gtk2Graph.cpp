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
        nGrabbingID     = -1;
        pCanvas         = new Gtk2Canvas(this);
        pGlass          = NULL;

        #ifdef LSP_TRACE
        sClock.tv_sec   = 0;
        sClock.tv_nsec  = 0;
        nFrames         = 0;
        #endif /* LSP_TRACE */

        // Update event mask
        lsp_trace("gtk events=0x%x", int(gtk_widget_get_events(pWidget)));
        gtk_widget_add_events(pWidget,
                    GDK_POINTER_MOTION_MASK |
                    GDK_BUTTON_MOTION_MASK |
                    GDK_SCROLL_MASK |
                    GDK_BUTTON_PRESS_MASK |
                    GDK_BUTTON_RELEASE_MASK |
                    GDK_ENTER_NOTIFY_MASK |
                    GDK_LEAVE_NOTIFY_MASK
                  );
        lsp_trace("gtk events=0x%x", int(gtk_widget_get_events(pWidget)));
    }

    Gtk2Graph::~Gtk2Graph()
    {
        if (pCanvas != NULL)
        {
            delete pCanvas;
            pCanvas     = NULL;
        }
        if (pGlass != NULL)
        {
            cairo_surface_destroy(pGlass);
            pGlass      = NULL;
        }
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

    void Gtk2Graph::draw(cairo_t *cr)
    {
        #ifdef LSP_TRACE
        if (sClock.tv_sec == 0)
            clock_gettime(CLOCK_REALTIME, &sClock);
        #endif /* LSP_TRACE */

        // Get resource
        cairo_pattern_t *cp = NULL;

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

            if (i < nRadius)
            {
                cp = cairo_pattern_create_radial (bw + 1, nHeight - bw - 1, bw, 1, nHeight - bw - 1, pr * 1.5);
                cairo_pattern_add_color_stop_rgb(cp, 0.0, c.red(), c.green(), c.blue());
                cairo_pattern_add_color_stop_rgb(cp, 1.0, sColor.red(), sColor.green(), sColor.blue());

                cairo_set_source(cr, cp);
            }
            else
                cairo_set_source_rgb(cr, sColor.red(), sColor.green(), sColor.blue());

            cairo_arc(cr, bw + 0.5, bw + 0.5, bw - i, M_PI, 1.5 * M_PI);
            cairo_arc(cr, nWidth - bw - 0.5, bw + 0.5, bw - i, 1.5 * M_PI, 2.0 * M_PI);
            cairo_arc(cr, nWidth - bw - 0.5, nHeight - bw - 0.5, bw - i, 0.0, 0.5 * M_PI);
            cairo_arc(cr, bw + 0.5, nHeight - bw - 0.5, bw - i, 0.5 * M_PI, M_PI);
            cairo_close_path(cr);

            if (i < nRadius)
            {
                cairo_stroke(cr);
                cairo_pattern_destroy(cp);
            }
            else
                cairo_fill(cr);
        }

        // Draw glass // TEMPORARY
        size_t bs = bw * M_SQRT2 * 0.5;
        size_t gw = nWidth  - (bs << 1);
        size_t gh = nHeight - (bs << 1);

        // Calculate center
        Gtk2Canvas *cv = pCanvas;
        cv->resize(gw, gh, nPadding);
        if (cv->valid())
        {
            cv->set_color(sColor);
            cv->clear();

            size_t n_objects = vObjects.size();
            for (size_t i=0; i<n_objects; ++i)
                vObjects.at(i)->draw(cv);

            cv->draw(cr, bs, bs);
        }

        // Draw glass effect
        if (pGlass != NULL)
        {
            size_t width    = cairo_image_surface_get_width(pGlass);
            size_t height   = cairo_image_surface_get_height(pGlass);

            if ((nWidth != width) || (nHeight != height))
            {
                cairo_surface_destroy(pGlass);
                pGlass          = NULL;
            }
        }

        if (pGlass == NULL)
        {
            // Gradient effect is too expensive, draw it as little as possible
            pGlass      = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, nWidth, nHeight);
            if (pGlass != NULL)
            {
                cp = cairo_pattern_create_radial (nWidth, 0, bw << 1, nWidth, 0, pr);
                cairo_pattern_add_color_stop_rgba(cp, 0.0, 1.0, 1.0, 1.0, 0.15);
                cairo_pattern_add_color_stop_rgba(cp, 1.0, 1.0, 1.0, 1.0, 0.0);

                cairo_t *gcr    = cairo_create(pGlass);
                cairo_set_source(gcr, cp);
                cairo_arc(gcr, bw, bw, bw - nRadius, M_PI, 1.5 * M_PI);
                cairo_arc(gcr, nWidth - bw, bw, bw - nRadius, 1.5 * M_PI, 2.0 * M_PI);
                cairo_arc(gcr, nWidth - bw, nHeight - bw, bw - nRadius, 0.0, 0.5 * M_PI);
                cairo_arc(gcr, bw, nHeight - bw, bw - nRadius, 0.5 * M_PI, M_PI);
                cairo_close_path(gcr);
                cairo_fill(gcr);
                cairo_destroy(gcr);

                cairo_pattern_destroy(cp);
            }
        }

        if (pGlass != NULL)
        {
            cairo_set_source_surface (cr, pGlass, 0, 0);
            cairo_paint(cr);
        }
        // End of Glass effect

        #ifdef LSP_TRACE
        nFrames ++;
        struct timespec stime;
        clock_gettime(CLOCK_REALTIME, &stime);
        if ((stime.tv_sec - sClock.tv_sec) >= 5)
        {
            double dt = double(stime.tv_sec - sClock.tv_sec) + double(stime.tv_nsec - sClock.tv_nsec)*1e-9;
            lsp_trace("seconds = %.2f, FPS = %.2f", dt, nFrames / dt);

            nFrames = 0;
            sClock = stime;
        }
        #endif /* LSP_TRACE */
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
            case W_DOT:
                return static_cast<IGraphObject *>(widget);
            default:
                return NULL;
        }
    }

    IGraphCanvas *Gtk2Graph::canvas()
    {
        if (pCanvas != NULL)
            return (pCanvas->valid()) ? pCanvas : NULL;
        return NULL;
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

    void Gtk2Graph::button_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
//        lsp_trace("x=%d, y=%d, state=0x%x, button=%d", int(x), int(y), int(state), int(button));
        if (!translate_coords(x, y))
            return;

        bool redraw = false;
        size_t deliver = 0;
        size_t grabbing_id = nGrabbingID;
        size_t n_objects = vObjects.size();

        // Deliver to grabbing widget first
        if (nGrabbingID > 0)
        {
            size_t flags    = vObjects.at(nGrabbingID)->button_press(x, y, gtk2_decode_mcf(state), gtk2_decode_mcb(button));
            if (flags & EVF_HANDLED)
                redraw          = true;
            if (flags & EVF_GRAB)
                deliver         = n_objects; // Skip delivery to others
            else
                nGrabbingID     = -1;
            if (flags & EVF_STOP)
                deliver         = n_objects;
        }

        // Deliver to others (if possible)
        for (size_t i=deliver; i<n_objects; ++i)
        {
            if (i == grabbing_id)
                continue;
            size_t flags    = vObjects.at(i)->button_press(x, y, gtk2_decode_mcf(state), gtk2_decode_mcb(button));
            if (flags & EVF_HANDLED)
                redraw      = true;
            if (flags & EVF_GRAB)
                nGrabbingID = i;
            if (flags & EVF_STOP)
                break;
        }

        if (redraw)
            markRedraw();
    }

    void Gtk2Graph::button_release(ssize_t x, ssize_t y, size_t state, size_t button)
    {
//        lsp_trace("x=%d, y=%d, state=0x%x, button=%d", int(x), int(y), int(state), int(button));
        if (!translate_coords(x, y))
            return;

        bool redraw = false;
        size_t deliver = 0;
        size_t grabbing_id = nGrabbingID;
        size_t n_objects = vObjects.size();

        // Deliver to grabbing widget first
        if (nGrabbingID > 0)
        {
            size_t flags    = vObjects.at(nGrabbingID)->button_release(x, y, gtk2_decode_mcf(state), gtk2_decode_mcb(button));
            if (flags & EVF_HANDLED)
                redraw          = true;
            if (flags & EVF_GRAB)
                deliver         = n_objects; // Skip delivery to others
            else
                nGrabbingID     = -1;
            if (flags & EVF_STOP)
                deliver         = n_objects;
        }

        // Deliver to others (if possible)
        for (size_t i=deliver; i<n_objects; ++i)
        {
            if (i == grabbing_id)
                continue;
            size_t flags    = vObjects.at(i)->button_release(x, y, gtk2_decode_mcf(state), gtk2_decode_mcb(button));
            if (flags & EVF_HANDLED)
                redraw      = true;
            if (flags & EVF_GRAB)
                nGrabbingID = i;
            if (flags & EVF_STOP)
                break;
        }

        if (redraw)
            markRedraw();
    }

    void Gtk2Graph::motion(ssize_t x, ssize_t y, size_t state)
    {
//        lsp_trace("x=%d, y=%d, state=0x%x", int(x), int(y), int(state));
        if (!translate_coords(x, y))
            return;

        bool redraw = false;
        size_t deliver = 0;
        size_t grabbing_id = nGrabbingID;
        size_t n_objects = vObjects.size();

        // Deliver to grabbing widget first
        if (nGrabbingID > 0)
        {
            size_t flags    = vObjects.at(nGrabbingID)->motion(x, y, gtk2_decode_mcf(state));
            if (flags & EVF_HANDLED)
                redraw          = true;
            if (flags & EVF_GRAB)
                deliver         = n_objects; // Skip delivery to others
            else
                nGrabbingID     = -1;
            if (flags & EVF_STOP)
                deliver         = n_objects;
        }

        // Deliver to others (if possible)
        for (size_t i=deliver; i<n_objects; ++i)
        {
            if (i == grabbing_id)
                continue;
            size_t flags    = vObjects.at(i)->motion(x, y, gtk2_decode_mcf(state));
            if (flags & EVF_HANDLED)
                redraw      = true;
            if (flags & EVF_GRAB)
                nGrabbingID = i;
            if (flags & EVF_STOP)
                break;
        }

        if (redraw)
            markRedraw();
    }

    void Gtk2Graph::scroll(ssize_t x, ssize_t y, size_t state, size_t direction)
    {
        if (!translate_coords(x, y))
            return;

        bool redraw = false;
        size_t deliver = 0;
        size_t grabbing_id = nGrabbingID;
        size_t n_objects = vObjects.size();

        // Deliver to grabbing widget first
        if (nGrabbingID > 0)
        {
            size_t flags    = vObjects.at(nGrabbingID)->scroll(x, y, gtk2_decode_mcf(state), gtk2_decode_mcd(direction));
            if (flags & EVF_HANDLED)
                redraw          = true;
            if (flags & EVF_GRAB)
                deliver         = n_objects; // Skip delivery to others
            else
                nGrabbingID     = -1;
            if (flags & EVF_STOP)
                deliver         = n_objects;
        }

        // Deliver to others (if possible)
        for (size_t i=deliver; i<n_objects; ++i)
        {
            if (i == grabbing_id)
                continue;
            size_t flags    = vObjects.at(i)->scroll(x, y, gtk2_decode_mcf(state), gtk2_decode_mcd(direction));
            if (flags & EVF_HANDLED)
                redraw      = true;
            if (flags & EVF_GRAB)
                nGrabbingID = i;
            if (flags & EVF_STOP)
                break;
        }

        if (redraw)
            markRedraw();
    }

    bool Gtk2Graph::translate_coords(ssize_t &x, ssize_t &y)
    {
        ssize_t bs = nBorder * M_SQRT2 * 0.5;
        ssize_t gw = nWidth  - (bs << 1);
        ssize_t gh = nHeight - (bs << 1);

        x       = x - bs - (gw>>1);
        y       = bs + (gh>>1) - y;

        return true;
    }


} /* namespace lsp */
