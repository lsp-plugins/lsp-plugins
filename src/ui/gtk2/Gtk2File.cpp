/*
 * Gtk2File.cpp
 *
 *  Created on: 24 мар. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>
#include <core/status.h>

#include <string.h>
#include <math.h>


namespace lsp
{
    Gtk2File::Gtk2File(plugin_ui *ui): Gtk2CustomWidget(ui, W_FILE)
    {
        sColor.set(pUI->theme(), C_GLASS);
        sBgColor.set(pUI->theme(), C_BACKGROUND);
        sLineColor.set(pUI->theme(), C_GRAPH_LINE);
        sLeftColor.set(pUI->theme(), C_RED);
        sRightColor.set(pUI->theme(), C_CYAN);
        sMiddleColor.set(pUI->theme(), C_GRAPH_MESH);
        sFadeColor.set(pUI->theme(), C_YELLOW);
        sTextColor.set(pUI->theme(), C_LABEL_TEXT);

        pFile           = NULL;
        pHeadCut        = NULL;
        pTailCut        = NULL;
        pFadeIn         = NULL;
        pFadeOut        = NULL;
        pLength         = NULL;
        pStatus         = NULL;
        pMesh           = NULL;

        nBtnWidth       = 128;
        nBtnHeight      = 49;
        nBMask          = 0;
        nBorder         = 12;
        nRadius         = 4;
        bPressed        = false;
    }

    Gtk2File::~Gtk2File()
    {
    }

    void Gtk2File::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                BIND_PORT(pUI, pFile, value);
                break;
            case A_HEAD_ID:
                BIND_PORT(pUI, pHeadCut, value);
                break;
            case A_TAIL_ID:
                BIND_PORT(pUI, pTailCut, value);
                break;
            case A_FADEIN_ID:
                BIND_PORT(pUI, pFadeIn, value);
                break;
            case A_FADEOUT_ID:
                BIND_PORT(pUI, pFadeOut, value);
                break;
            case A_LENGTH_ID:
                BIND_PORT(pUI, pLength, value);
                break;
            case A_STATUS_ID:
                BIND_PORT(pUI, pStatus, value);
                break;
            case A_MESH_ID:
                BIND_PORT(pUI, pMesh, value);
                break;
            case A_WIDTH:
                PARSE_INT(value, nBtnWidth = size_t(__));
                break;
            case A_HEIGHT:
                PARSE_INT(value, nBtnHeight = size_t(__));
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
                break;
            default:
                Gtk2CustomWidget::set(att, value);
                break;
        }
    }

    void Gtk2File::render_mesh(cairo_t *cr, mesh_t *mesh, size_t index, const Color &color, ssize_t x, ssize_t y, ssize_t w, ssize_t h)
    {
        const float *src = mesh->pvData[index];

//        cairo_set_source_rgb(cr, color.red(), color.green(), color.blue());
//        for (ssize_t i=0; i<w; ++i)
//        {
//            size_t idx  = (i * mesh->nItems) / w;
//            cairo_move_to(cr, x + i, y);
//            cairo_line_to(cr, x + i, y + h * src[idx]);
//            cairo_stroke(cr);
//        }

        cairo_set_source_rgba(cr, color.red(), color.green(), color.blue(), 0.5f);
        cairo_move_to(cr, x, y);
        for (ssize_t i=0; i<w; ++i)
        {
            size_t idx  = (i * mesh->nItems) / w;
            cairo_line_to(cr, x + i, y + h * src[idx]);
        }
        cairo_line_to(cr, x + w - 1, y);
        cairo_fill_preserve(cr);

        cairo_set_source_rgb(cr, color.red(), color.green(), color.blue());
        cairo_stroke(cr);
    }

    void Gtk2File::render_graph(cairo_surface_t *s, size_t w, size_t h)
    {
        cairo_text_extents_t extents;
        cairo_t *cr     = cairo_create(s);
        cairo_save(cr);

        cairo_set_source_rgb(cr, sColor.red(), sColor.green(), sColor.blue());
        cairo_rectangle(cr, 0, 0, w, h);
        cairo_fill(cr);

#ifdef LSP_HOST_SIMULATION
        // THIS IS TEST CODE
        mesh_t *mesh    = reinterpret_cast<mesh_t *>(alloca(sizeof(mesh_t) + sizeof(float *) * 2));
        mesh->nBuffers  = 2;
        mesh->nItems    = 320;
        mesh->pvData[0] = reinterpret_cast<float *>(alloca(mesh->nItems * sizeof(float)));
        mesh->pvData[1] = reinterpret_cast<float *>(alloca(mesh->nItems * sizeof(float)));

//        dsp::fill_zero(mesh->pvData[0], mesh->nItems);
//        dsp::fill_zero(mesh->pvData[1], mesh->nItems);
//
//        mesh->pvData[0][rand()%mesh->nItems]    = 0.5f;
//        mesh->pvData[1][rand()%mesh->nItems]    = 0.5f;
//
//        dsp::normalize(mesh->pvData[0], mesh->pvData[0], mesh->nItems);
//        dsp::normalize(mesh->pvData[1], mesh->pvData[1], mesh->nItems);
        for (size_t i=0; i<mesh->nItems; ++i)
        {
            mesh->pvData[0][i] = fabs(sinf((16 * M_PI * i) / mesh->nItems));
            mesh->pvData[1][i] = fabs(cosf((16 * M_PI * i) / mesh->nItems));
        }
        float length    = (pLength != NULL) ? pLength->metadata()->max : 1000.0f;
        size_t f_status = STATUS_OK;
//        mesh->nBuffers  = 1;
#else
        mesh_t *mesh    = (pMesh != NULL)   ? reinterpret_cast<mesh_t *>(pMesh->getBuffer()) : NULL;
        float length    = (pLength != NULL) ? pLength->getValue() : 0.0f;
        size_t f_status = (pStatus != NULL) ? pStatus->getValue() : STATUS_UNSPECIFIED;
#endif /* HOST_SIMULATION */

        size_t hc = (h >> 1);

//        lsp_trace("mesh=%p, mesh_buffers=%d, status=%d, length=%f", mesh, int(mesh->nBuffers), int(nStatus), length);

        if ((mesh != NULL) && (f_status == STATUS_OK))
        {
            // Draw file contents
            float head_cut  = (pHeadCut != NULL) ? pHeadCut->getValue() : 0.0f;
            float tail_cut  = (pTailCut != NULL) ? pTailCut->getValue() : 0.0f;
            float from_time = head_cut;
            float to_time   = length - tail_cut;

//            lsp_trace("length = %.3f, head_cut = %.3f, tail_cut = %.3f, from_time = %.3f, to_time = %.3f", length, head_cut, tail_cut, from_time, to_time);

            cairo_set_line_width(cr, 1);

            length       = to_time - from_time;

            if (length > 0.0f)
            {
                // File contents
                if (mesh->nItems >= 2)
                {
                    if (mesh->nBuffers == 1)
                    {
                        render_mesh(cr, mesh, 0, sMiddleColor, 0, hc, w, -hc);
                        render_mesh(cr, mesh, 0, sMiddleColor, 0, hc - 1, w, hc);
                    }
                    else if (mesh->nBuffers == 2)
                    {
                        render_mesh(cr, mesh, 0, sLeftColor, 0, hc, w, -hc);
                        render_mesh(cr, mesh, 1, sRightColor, 0, hc - 1, w, hc);
                    }
                }

                // Fade-in
                if (pFadeIn != NULL)
                {
                    float fade_in       = pFadeIn->getValue();
                    if (fade_in > 0.0f)
                    {
                        ssize_t fade_len    = w * (fade_in / length);

                        cairo_move_to(cr, 0, 0);
                        cairo_line_to(cr, fade_len, 0);
                        cairo_line_to(cr, 0, hc);
                        cairo_close_path(cr);
                        cairo_set_source_rgba(cr, sFadeColor.red(), sFadeColor.green(), sFadeColor.blue(), 0.5f);
                        cairo_fill_preserve(cr);
                        cairo_set_source_rgb(cr, sFadeColor.red(), sFadeColor.green(), sFadeColor.blue());
                        cairo_stroke(cr);

                        cairo_move_to(cr, 0, hc);
                        cairo_line_to(cr, fade_len, h);
                        cairo_line_to(cr, 0, h);
                        cairo_close_path(cr);
                        cairo_set_source_rgba(cr, sFadeColor.red(), sFadeColor.green(), sFadeColor.blue(), 0.5f);
                        cairo_fill_preserve(cr);
                        cairo_set_source_rgb(cr, sFadeColor.red(), sFadeColor.green(), sFadeColor.blue());
                        cairo_stroke(cr);
                    }
                }

                // Fade-out
                if (pFadeOut != NULL)
                {
                    float fade_out       = pFadeOut->getValue();
                    if (fade_out > 0.0f)
                    {
                        ssize_t fade_len    = w * (fade_out / length);

                        cairo_move_to(cr, w, 0);
                        cairo_line_to(cr, ssize_t(w) - fade_len, 0);
                        cairo_line_to(cr, w, hc);
                        cairo_close_path(cr);
                        cairo_set_source_rgba(cr, sFadeColor.red(), sFadeColor.green(), sFadeColor.blue(), 0.5f);
                        cairo_fill_preserve(cr);
                        cairo_set_source_rgb(cr, sFadeColor.red(), sFadeColor.green(), sFadeColor.blue());
                        cairo_stroke(cr);

                        cairo_move_to(cr, w, hc);
                        cairo_line_to(cr, ssize_t(w) - fade_len, h);
                        cairo_line_to(cr, w, h);
                        cairo_close_path(cr);
                        cairo_set_source_rgba(cr, sFadeColor.red(), sFadeColor.green(), sFadeColor.blue(), 0.5f);
                        cairo_fill_preserve(cr);
                        cairo_set_source_rgb(cr, sFadeColor.red(), sFadeColor.green(), sFadeColor.blue());
                        cairo_stroke(cr);
                    }
                }
            }

            // Draw horizontal axis
            cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
            cairo_set_source_rgb(cr, sLineColor.red(), sLineColor.green(), sLineColor.blue());
            cairo_move_to(cr, 0, hc);
            cairo_line_to(cr, w, hc);
            cairo_stroke(cr);
        }
        else
        {
            Color c;
            const char *prompt = NULL;
            switch (f_status)
            {
                case STATUS_OK:
                    c.set_rgb(0.0f, 1.0f, 0.0f);
                    break;
                case STATUS_UNSPECIFIED:
                    c.set_rgb(0.0f, 1.0f, 0.0f);
                    prompt  = "Click to load";
                    break;
                case STATUS_LOADING:
                    c.set_rgb(1.0f, 1.0f, 0.0f);
                    break;
                default:
                    c.set_rgb(1.0f, 0.0f, 0.0f);
                    break;
            }

            if (prompt == NULL)
                prompt  = get_status(f_status);

            if (prompt != NULL)
            {
                cairo_select_font_face(cr, "Sans",
                      CAIRO_FONT_SLANT_NORMAL,
                      CAIRO_FONT_WEIGHT_BOLD);
                cairo_set_font_size(cr, 16);
                cairo_text_extents(cr, prompt, &extents);

                float r_w = extents.x_advance - extents.x_bearing;
                float r_h = extents.y_advance - extents.y_bearing;

                cairo_set_source_rgb(cr, c.red(), c.green(), c.blue());
                cairo_move_to(cr, (w >> 1) - r_w * 0.5, (h >> 1) + r_h * 0.5);
                cairo_show_text (cr, prompt);
            }
        }

        if (pFile != NULL)
        {
            #ifdef LSP_HOST_SIMULATION
                const char *path = "/path/to/some/audio_file.wav";
            #else
                const char *path = reinterpret_cast<const char *>(pFile->getBuffer());
            #endif
            if (path != NULL)
            {
                // Remove the path, keep only file name
                const char *fname = strrchr(path, '/');
                const char *fname2 = strrchr(path, '\\');
                if ((fname == NULL) || (fname < fname2))
                    fname      = fname2;
                if (fname == NULL)
                    fname      = path;
                else
                    fname++;

                // Now we are ready to output text
                if ((fname != NULL) && (strlen(fname) > 0))
                {
                    cairo_select_font_face(cr, "Sans",
                          CAIRO_FONT_SLANT_NORMAL,
                          CAIRO_FONT_WEIGHT_BOLD);
                    cairo_set_font_size(cr, 10);
                    cairo_text_extents(cr, fname, &extents);

                    size_t bw = 4;
                    float r_w = extents.x_advance - extents.x_bearing + (bw << 1);
                    float r_h = extents.y_advance - extents.y_bearing + (bw << 1);

                    cairo_set_source_rgba(cr, sColor.red(), sColor.green(), sColor.blue(), 0.75f);
                    cairo_arc(cr, bw, h - r_h + bw, bw, M_PI, 1.5 * M_PI);
                    cairo_arc(cr, r_w - bw, h - r_h + bw, bw, 1.5 * M_PI, 2.0 * M_PI);
                    cairo_arc(cr, r_w - bw, h - bw, bw, 0.0, 0.5 * M_PI);
                    cairo_arc(cr, bw, h - bw, bw, 0.5 * M_PI, M_PI);
                    cairo_close_path(cr);
    //                cairo_rectangle(cr, 0, h - r_h - 4, r_w + 4, r_h + 4);
                    cairo_fill(cr);

                    cairo_set_source_rgb(cr, sTextColor.red(), sTextColor.green(), sTextColor.blue());
                    cairo_move_to(cr, bw, h - bw);
                    cairo_show_text (cr, fname);
                }
            }
        }

        cairo_restore(cr);
        cairo_destroy(cr);
    }

    void Gtk2File::render()
    {
//        lsp_trace("pressed = %d", int(pressed));

        // Get resource
//        cairo_pattern_t *cp;
        cairo_t *cr = gdk_cairo_create(pWidget->window);
        cairo_pattern_t *cp = NULL;
        cairo_save(cr);

        // Draw background
        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_rectangle(cr, 0, 0, nWidth, nHeight);
        cairo_fill(cr);

        // Determine left and top coordinates
        size_t rw   = nBtnWidth + 2 + (nBorder << 1);
        size_t rh   = nBtnHeight + 2 + (nBorder << 1);
        size_t r    = (bPressed) ? nRadius + 2 : nRadius;

        cairo_translate(cr, (nWidth - rw) >> 1, (nHeight - rh) >> 1);

        // Draw border
        size_t bw = nBorder;
        size_t pr = sqrtf(rw*rw + rh*rh);
        cairo_set_line_width(cr, 1);
        for (size_t i=0; i <= r; ++i)
        {
            float bright = float(r - i) / r; // TODO
            Color c(1.0, 1.0, 1.0);
            c.blend(sColor, bright);

            cp = cairo_pattern_create_radial (bw + 1, rh - bw - 1, bw, 1, rh - bw - 1, pr * 1.5);
            cairo_pattern_add_color_stop_rgb(cp, 0.0, c.red(), c.green(), c.blue());
            cairo_pattern_add_color_stop_rgb(cp, 1.0, sColor.red(), sColor.green(), sColor.blue());

            cairo_set_source(cr, cp);

            cairo_arc(cr, bw, bw, bw - i, M_PI, 1.5 * M_PI);
            cairo_arc(cr, rw - bw, bw, bw - i, 1.5 * M_PI, 2.0 * M_PI);
            cairo_arc(cr, rw - bw, rh - bw, bw - i, 0.0, 0.5 * M_PI);
            cairo_arc(cr, bw, rh - bw, bw - i, 0.5 * M_PI, M_PI);
            cairo_close_path(cr);

            cairo_fill(cr);
            cairo_pattern_destroy(cp);
        }

        // Draw main contents
        cairo_surface_t *cs     = cairo_image_surface_create(CAIRO_FORMAT_RGB24, nBtnWidth, nBtnHeight);
        render_graph(cs, nBtnWidth, nBtnHeight);

        if (!bPressed)
        {
            cairo_set_source_surface (cr, cs, nBorder, nBorder);
            cairo_paint(cr);
        }
        else
        {
            cairo_save(cr);
            cairo_translate(cr, nBorder + 1, nBorder + 1);

            cairo_scale(cr, float(nBtnWidth - 2) / float(nBtnWidth), float(nBtnHeight - 2) / float(nBtnHeight));
            cairo_set_source_surface(cr, cs, 0, 0);

            cairo_paint(cr);
            cairo_restore(cr);

            // Hide some gradient artifacts
            cairo_antialias_t aa = cairo_get_antialias(cr);
            cairo_set_source_rgb(cr, sColor.red(), sColor.green(), sColor.blue());
            cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
            cairo_rectangle(cr, nBorder + 1, nBorder + 1, nBtnWidth-1, nBtnHeight-1);
            cairo_stroke(cr);
            cairo_set_antialias(cr, aa);
        }

        cairo_surface_destroy(cs);

        // Draw glass effect
        cp = cairo_pattern_create_radial (rw, 0, bw << 1, rw, 0, pr);
        cairo_pattern_add_color_stop_rgba(cp, 0.0, 1.0, 1.0, 1.0, 0.15);
        cairo_pattern_add_color_stop_rgba(cp, 1.0, 1.0, 1.0, 1.0, 0.0);

        cairo_set_source(cr, cp);

        cairo_arc(cr, bw, bw, bw - r, M_PI, 1.5 * M_PI);
        cairo_arc(cr, rw - bw, bw, bw - r, 1.5 * M_PI, 2.0 * M_PI);
        cairo_arc(cr, rw - bw, rh - bw, bw - r, 0.0, 0.5 * M_PI);
        cairo_arc(cr, bw, rh - bw, bw - r, 0.5 * M_PI, M_PI);
        cairo_close_path(cr);

        cairo_fill(cr);
        cairo_pattern_destroy(cp);

        // Release resource
        cairo_restore(cr);
        cairo_destroy(cr);
    }

    void Gtk2File::resize(size_t &w, size_t &h)
    {
        size_t rw = nBtnWidth + 2 + (nBorder << 1);
        size_t rh = nBtnHeight + 2 + (nBorder << 1);
        if (w < rw)
            w = rw;
        if (h < rh)
            h = rh;
    }

    bool Gtk2File::check_mouse_over(ssize_t x, ssize_t y)
    {
        ssize_t left    = (ssize_t(nWidth - nBtnWidth) >> 1) - nRadius;
        ssize_t top     = (ssize_t(nHeight - nBtnHeight) >> 1) - nRadius;
        ssize_t right   = left + nBtnWidth;
        ssize_t bottom  = top + nBtnHeight;

        return ((x >= left) && (x <= right) && (y >= top) && (y <= bottom));
    }

    void Gtk2File::button_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
        nBMask         |= (1 << button);
        bPressed        = (nBMask == (1 << 1)) && (check_mouse_over(x, y));
        markRedraw();
    }

    void Gtk2File::button_release(ssize_t x, ssize_t y, size_t state, size_t button)
    {
        nBMask         &= ~(1 << button);

        if ((bPressed) && (nBMask == 0) && (button == 1))
            on_click();
        if (nBMask == 0)
            bPressed        = false;

        markRedraw();
    }

    void Gtk2File::motion(ssize_t x, ssize_t y, size_t state)
    {
        bool pressed    = bPressed;
        bPressed        = (nBMask == (1 << 1)) && (check_mouse_over(x, y));

        if (pressed != bPressed)
            markRedraw();
    }

    void Gtk2File::on_click()
    {
        // Create a dialog to select a sample file
        GtkWidget* dialog = gtk_file_chooser_dialog_new
        (
            "Load File",
            NULL,
            GTK_FILE_CHOOSER_ACTION_OPEN,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
            NULL
        );

        gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);

        // Run the dialog, and return if it is cancelled
        if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT)
        {
            gtk_widget_destroy(dialog);
            return;
        }

        /* Get the file path from the dialog. */
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        set_file(filename);

        /* Got what we need, destroy the dialog. */
        gtk_widget_destroy(dialog);
    }

    void Gtk2File::set_file(const char *fname)
    {
        if (pFile == NULL)
            return;

        // Write new path request
        pFile->write(fname, (fname != NULL) ? strlen(fname) : 0);
        pFile->notifyAll();
    }

    void Gtk2File::button_double_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
        if (button != 3)
            return;
        if (check_mouse_over(x, y))
            set_file(NULL);
    }

    void Gtk2File::notify(IUIPort *port)
    {
        Gtk2CustomWidget::notify(port);

        bool redraw = false;

        if (port == pFile)
            redraw = true;
        else if (port == pHeadCut)
            redraw = true;
        else if (port == pTailCut)
            redraw = true;
        else if (port == pFadeIn)
            redraw = true;
        else if (port == pFadeOut)
            redraw = true;
        else if (port == pLength)
            redraw = true;
        else if (port == pStatus)
            redraw = true;
        else if (port == pMesh)
            redraw = true;

        if (redraw)
            markRedraw();
    }

} /* namespace lsp */
