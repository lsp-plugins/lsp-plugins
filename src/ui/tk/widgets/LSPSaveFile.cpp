/*
 * LSPSaveFile.cpp
 *
 *  Created on: 20 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPSaveFile::metadata = { "LSPSaveFile", &LSPWidget::metadata };

        typedef struct state_descr_t
        {
            const char *text;
            size_t color_id;
        } state_descr_t;

        LSPSaveFile::LSPSaveFile(LSPDisplay *dpy):
            LSPWidget(dpy),
            sFont(this),
            sDialog(dpy)
        {
            nState      = SFS_SELECT;
            fProgress   = 0;
            nButtons    = 0;
            nBtnState   = 0;
            pDisk       = NULL;
            nSize       = -1;

            for (size_t i=0; i<SFS_TOTAL; ++i)
                vStates[i].pColor   = NULL;

            pClass      = &metadata;
        }

        LSPSaveFile::~LSPSaveFile()
        {
            if (pDisk != NULL)
            {
                pDisk->destroy();
                delete pDisk;
                pDisk = NULL;
            }
        }

        status_t LSPSaveFile::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPSaveFile *_this = widget_ptrcast<LSPSaveFile>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPSaveFile::slot_on_activate(LSPWidget *sender, void *ptr, void *data)
        {
            LSPSaveFile *_this = widget_ptrcast<LSPSaveFile>(ptr);
            return (_this != NULL) ? _this->on_activate() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPSaveFile::slot_on_dialog_close(LSPWidget *sender, void *ptr, void *data)
        {
            // Cast widget
            LSPSaveFile *_this = widget_ptrcast<LSPSaveFile>(ptr);
            if (_this == NULL)
                return STATUS_BAD_STATE;

            // Remember the last path used
            _this->sDialog.get_path(&_this->sPath);
            return _this->sSlots.execute(LSPSLOT_CLOSE, _this, data);
        }

        status_t LSPSaveFile::slot_on_close(LSPWidget *sender, void *ptr, void *data)
        {
            LSPSaveFile *_this = widget_ptrcast<LSPSaveFile>(ptr);
            return (_this != NULL) ? _this->on_close() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPSaveFile::slot_on_file_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPSaveFile *_this = widget_ptrcast<LSPSaveFile>(ptr);
            return (_this != NULL) ? _this->sSlots.execute(LSPSLOT_SUBMIT, sender) : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPSaveFile::init()
        {
            static const state_descr_t initial[SFS_TOTAL] =
            {
                { " Save ", C_BUTTON_FACE },
                { " Saving ", C_YELLOW },
                { " Saved ", C_GREEN },
                { " Error ", C_RED }
            };

            LSP_STATUS_ASSERT(LSPWidget::init());

            for (size_t i=0; i<SFS_TOTAL; ++i)
            {
                const state_descr_t *sd = &initial[i];

                vStates[i].pColor = new LSPColor(this);
                if (vStates[i].pColor == NULL)
                    return STATUS_NO_MEM;
                init_color(color_t(sd->color_id), vStates[i].pColor);
                vStates[i].sText.set_utf8(sd->text);
            }

            sFont.init();
            sFont.set_size(10);

            // Create dialog
            LSP_STATUS_ASSERT(sDialog.init());
            sDialog.set_mode(FDM_SAVE_FILE);
            sDialog.title()->set("titles.save_to_file");
            sDialog.action_title()->set("actions.save");
            sDialog.set_use_confirm(true);
            sDialog.confirm()->set("messages.file.confirm_overwrite");
            {
                LSPFileFilterItem ffi;
                ffi.pattern()->set("*");
                ffi.title()->set("files.all");
                ffi.set_extension("");
                sDialog.filter()->add(&ffi);
            }
            sDialog.bind_action(slot_on_file_submit, self());

            sDialog.slots()->bind(LSPSLOT_HIDE, slot_on_dialog_close, self());

            // Add slots
            ui_handler_id_t id = 0;
            id = sSlots.add(LSPSLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_ACTIVATE, slot_on_activate, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_CLOSE, slot_on_close, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void LSPSaveFile::destroy()
        {
            sDialog.destroy();

            for (size_t i=0; i<SFS_TOTAL; ++i)
                if (vStates[i].pColor != NULL)
                {
                    delete vStates[i].pColor;
                    vStates[i].pColor = NULL;
                }

            LSPWidget::destroy();
        }

        LSPColor *LSPSaveFile::state_color(size_t i)
        {
            if ((i < 0) || (i >= SFS_TOTAL))
                return NULL;
            return vStates[i].pColor;
        }

        const char *LSPSaveFile::state_text(size_t i) const
        {
            if ((i < 0) || (i >= SFS_TOTAL))
                return NULL;
            return vStates[i].sText.get_native();
        }

        status_t LSPSaveFile::get_state_text(size_t i, LSPString *dst)
        {
            if ((i < 0) || (i >= SFS_TOTAL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;
            return (dst->set(&vStates[i].sText)) ? STATUS_OK : STATUS_NO_MEM;
        }

        const char *LSPSaveFile::file_name() const
        {
            return sDialog.selected_file();
        }

        status_t LSPSaveFile::get_file_name(LSPString *dst)
        {
            return sDialog.get_selected_file(dst);
        }

        status_t LSPSaveFile::set_state(save_file_state_t state)
        {
            if (nState == state)
                return STATUS_OK;
            nState  = state;
            query_draw();
            return STATUS_OK;
        }

        status_t LSPSaveFile::set_state_text(size_t i, const char *s)
        {
            if ((i < 0) || (i >= SFS_TOTAL))
                return STATUS_BAD_ARGUMENTS;
            if (!vStates[i].sText.set_native(s))
                return STATUS_NO_MEM;
            query_resize();
            return STATUS_OK;
        }

        status_t LSPSaveFile::set_state_text(size_t i, const LSPString *s)
        {
            if ((i < 0) || (i >= SFS_TOTAL))
                return STATUS_BAD_ARGUMENTS;
            if (!vStates[i].sText.set(s))
                return STATUS_NO_MEM;
            query_resize();
            return STATUS_OK;
        }

        status_t LSPSaveFile::set_progress(float value)
        {
            if (value < 0.0f)
                value = 0.0f;
            else if (value > 100.0f)
                value = 100.0f;

            if (value == fProgress)
                return value;
            fProgress   = value;
            if (nState == SFS_SAVING)
                query_draw();

            return STATUS_OK;
        }

        status_t LSPSaveFile::set_path(const LSPString *path)
        {
            if (!sPath.set(path))
                return STATUS_NO_MEM;
            return (sDialog.visible()) ? sDialog.set_path(&sPath) : STATUS_OK;
        }

        status_t LSPSaveFile::set_path(const char *path)
        {
            if (!sPath.set_native(path))
                return STATUS_NO_MEM;
            return (sDialog.visible()) ? sDialog.set_path(&sPath) : STATUS_OK;
        }

        void LSPSaveFile::set_size(ssize_t size)
        {
            if (nSize == size)
                return;
            nSize   = size;
            query_resize();
        }

        ISurface *LSPSaveFile::render_disk(ISurface *s, ssize_t w, const Color &c, const Color &bg)
        {
#define N 9
            static const float xx[] = { 0.5, 7, 8, 8, 7.5, 0.5, 0, 0, 0.5 };
            static const float yy[] = { 0, 0, 1, 7.5, 8, 8, 7.5, 0.5, 0 };
            float xa[N], ya[N];

            // Check surface
            if (pDisk != NULL)
            {
                if ((w != ssize_t(pDisk->width())) || (w != ssize_t(pDisk->height())))
                {
                    pDisk->destroy();
                    delete pDisk;
                    pDisk = NULL;
                }
            }

            // Create new surface if needed
            if (pDisk == NULL)
            {
                if (s == NULL)
                    return NULL;
                pDisk          = s->create(w, w);
                if (pDisk == NULL)
                    return NULL;
            }

            float aa    = pDisk->set_antialiasing(true);
            IGradient *gr = NULL;

            // Determine button parameters
            float b_rad  = sqrtf(sSize.nWidth*sSize.nWidth + sSize.nHeight*sSize.nHeight);
            size_t bsize = (sSize.nWidth < sSize.nHeight) ? sSize.nWidth : sSize.nHeight;
            ssize_t b_w  = sSize.nWidth >> 1;
            ssize_t b_h  = sSize.nHeight >> 1;
            ssize_t b_r  = bsize >> 1;          // Button radius
            ssize_t b_rr = 2 + (bsize >> 4);    // Button rounding radius

            // Change size if pressed
            ssize_t b_l = b_rr;
            if (nBtnState & S_PRESSED)
            {
                b_l ++;
                b_r --;
                b_w --;
                b_h --;
                b_rr --;
            }
            else
                b_l ++;

            float lightness = c.lightness();
            for (ssize_t i=0; (i++)<b_l; )
            {
                float bright = lightness * sqrtf(i * i) / b_l;

                if (nBtnState & S_PRESSED)
                    gr = s->radial_gradient(sSize.nWidth + b_l, - b_l, b_rad * 0.25f, sSize.nWidth + b_l, - b_l, b_rad * 2.0f);
                else
                    gr = s->radial_gradient(b_l, sSize.nHeight - b_l, b_rad * 0.25f, b_l, sSize.nHeight - b_l, b_rad * 2.0f);

                Color cl(c);
                cl.lightness(bright);
                gr->add_color(0.0f, cl);
                cl.darken(0.9f);
                gr->add_color(1.0f, cl);

                float k     = (w - i*2) * 0.125f;
                for (size_t j=0; j<N; ++j)
                {
                    xa[j] = i + xx[j] * k;
                    ya[j] = i + yy[j] * k;
                }

                pDisk->fill_poly(xa, ya, N, gr);
                delete gr; // Delete gradient!

                if ((--b_r) < 0)
                    b_r = 0;
                if ((--b_w) < 0)
                    b_w = 0;
                if ((--b_h) < 0)
                    b_h = 0;
            }

            // Clear canvas
            float k     = (w - b_l*2) * 0.125f;

            pDisk->wire_rect(b_l + k + 0.5f, b_l + 0.5f, 5.5*k, 3.5*k - 0.5f, 1, bg);
            pDisk->fill_rect(b_l + k*2.5f, b_l, 4.0*k, 3.5*k, bg);
            pDisk->fill_rect(b_l + 4.5*k, b_l + 0.5*k, k, 2.5*k, c);
            pDisk->fill_rect(b_l + 0.5*k, b_l + 4.0*k, 7.0*k, 3.5*k, bg);
            for (size_t i=0; i<N; ++i)
            {
                xa[i] = b_l + xx[i] * k;
                ya[i] = b_l + yy[i] * k;
            }
            pDisk->wire_poly(xa, ya, N, 1, bg);

            // Output text
            LSPString *txt = &vStates[nState].sText;
            font_parameters_t fp;
            text_parameters_t tp;
            sFont.get_parameters(pDisk, &fp);
            sFont.get_text_parameters(pDisk, &tp, txt);

            float fx = b_l + 4.0 * k - (tp.Width * 0.5f) - tp.XBearing;
            float fy = b_l + 5.75 * k - (fp.Height * 0.5f) + fp.Ascent;

            sFont.draw(pDisk, fx, fy, c, txt);

            pDisk->set_antialiasing(aa);

            return pDisk;
#undef N
        }

        void LSPSaveFile::draw(ISurface *s)
        {
            // Prepare palette
            Color color(vStates[nState].pColor->color());
            Color bg_color(sBgColor);
            color.scale_lightness(brightness());

            // Clear
            s->clear(bg_color);

            // Render disk
            ISurface *d = render_disk(s, sSize.nWidth, color, bg_color);
            if (d != NULL)
                s->draw(d, 0, 0);

            if (nState == SFS_SAVING)
            {
                size_t pw = (fProgress * sSize.nWidth * 0.01f);
                if (pw > 0)
                {
                    color.copy(vStates[SFS_SAVED].pColor->color());
                    color.scale_lightness(brightness());

                    ISurface *d = render_disk(s, sSize.nWidth, color, bg_color);
                    if (d != NULL)
                        s->draw_clipped(d, 0, 0, 0, 0, pw, sSize.nWidth);
                }
            }
        }

        void LSPSaveFile::size_request(size_request_t *r)
        {
            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return;

            // Estimate maximum text size
            font_parameters_t fp;
            text_parameters_t tp, tp2;
            sFont.get_parameters(s, &fp);

            for (size_t i=0; i<SFS_TOTAL; ++i)
            {
                if (i == 0)
                    sFont.get_text_parameters(s, &tp, &vStates[i].sText);
                else
                {
                    sFont.get_text_parameters(s, &tp2, &vStates[i].sText);
                    if (tp2.Width > tp.Width)
                        tp = tp2;
                }
            }
            s->destroy();
            delete s;

            // Padding border
            fp.Height      += 4;

            if ((fp.Height*2) > tp.Width)
                tp.Width    = fp.Height *2;

            r->nMinWidth        = ((tp.Width * 8) / 7) + 14;
            if ((nSize > 0) && (r->nMinWidth < nSize))
                r->nMinWidth        = nSize;

            r->nMinHeight       = r->nMinWidth;
            r->nMaxWidth        = r->nMinWidth;
            r->nMaxHeight       = r->nMinHeight;
        }

        status_t LSPSaveFile::on_mouse_down(const ws_event_t *e)
        {
            take_focus();

            bool mover          = inside(e->nLeft, e->nTop);
            nButtons           |= (1 << e->nCode);
            size_t state        = nBtnState;

            if (nState != SFS_SAVING)
            {
                // Update state according to mouse position and mouse button state
                if ((nButtons == (1 << MCB_LEFT)) && (mover))
                    nBtnState  |= S_PRESSED;
                else
                    nBtnState  &= ~S_PRESSED;
            }
            else
                nBtnState   &= ~S_PRESSED;

            // Query draw if state changed
            if (state != nBtnState)
                query_draw();

            return STATUS_OK;
        }

        status_t LSPSaveFile::on_mouse_up(const ws_event_t *e)
        {
            bool mover          = inside(e->nLeft, e->nTop);

            // Update state according to mouse position and mouse button state
            size_t state        = nBtnState;
            size_t bstate       = nButtons;
            nButtons           &= ~(1 << e->nCode);
            if ((nButtons == (1 << MCB_LEFT)) && (mover))
                nBtnState  |= S_PRESSED;
            else
                nBtnState  &= ~S_PRESSED;
            if ((bstate == (1 << MCB_LEFT)) && (e->nCode == MCB_LEFT) && (mover) && (nState != SFS_SAVING))
            {
                status_t result = sSlots.execute(LSPSLOT_ACTIVATE, NULL);
                if (result == STATUS_OK)
                {
                    sDialog.set_path(&sPath);
                    sDialog.show(this);
                }
            }

            // Query draw if state changed
            if (state != nBtnState)
                query_draw();

            return STATUS_OK;
        }

        status_t LSPSaveFile::on_mouse_move(const ws_event_t *e)
        {
            bool mover          = inside(e->nLeft, e->nTop);

            // Update state according to mouse position and mouse button state
            size_t state        = nBtnState;
            if (nState != SFS_SAVING)
            {
                if ((nButtons == (1 << MCB_LEFT)) && (mover))
                    nBtnState  |= S_PRESSED;
                else
                    nBtnState  &= ~S_PRESSED;
            }
            else
                nBtnState   &= ~S_PRESSED;

            // Query draw if state changed
            if (state != nBtnState)
                query_draw();

            return STATUS_OK;
        }

        status_t LSPSaveFile::on_submit()
        {
            return STATUS_OK;
        }

        status_t LSPSaveFile::on_activate()
        {
            return STATUS_OK;
        }

        status_t LSPSaveFile::on_close()
        {
            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */
