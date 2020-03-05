/*
 * LSPEdit.cpp
 *
 *  Created on: 29 авг. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <wctype.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPEdit::metadata = { "LSPEdit", &LSPWidget::metadata };

        //-----------------------------------------------------------------------------
        // LSPEdit::TextSelection implementation
        LSPEdit::TextSelection::TextSelection(LSPEdit *widget): LSPTextSelection()
        {
            pEdit   = widget;
        }

        LSPEdit::TextSelection::~TextSelection()
        {
        }

        ssize_t LSPEdit::TextSelection::limit(ssize_t value)
        {
            ssize_t max = pEdit->sText.length();
            return (value > max) ? max : value;
        }

        void LSPEdit::TextSelection::on_change()
        {
            pEdit->query_draw();
        }

        //-----------------------------------------------------------------------------
        // LSPEdit::TextCursor implementation
        LSPEdit::TextCursor::TextCursor(LSPEdit *widget): LSPTextCursor(widget->display())
        {
            pEdit   = widget;
        }

        LSPEdit::TextCursor::~TextCursor()
        {
        }

        ssize_t LSPEdit::TextCursor::limit(ssize_t value)
        {
            ssize_t max = pEdit->sText.length();
            return (value < 0) ? 0 : (value > max) ? max : value;
        }

        void LSPEdit::TextCursor::on_change()
        {
            set_shining(true);
            pEdit->query_draw();
        }

        void LSPEdit::TextCursor::on_blink()
        {
            pEdit->query_draw();
        }

        //-----------------------------------------------------------------------------
        // LSPEdit::KeyboardInput implementation
        LSPEdit::KeyboardInput::KeyboardInput(LSPEdit *widget)
        {
            pEdit   = widget;
        }

        LSPEdit::KeyboardInput::~KeyboardInput()
        {
        }

        status_t LSPEdit::KeyboardInput::on_key_press(const ws_event_t *e)
        {
            LSPString s;
            s.set(lsp_wchar_t(e->nCode));
            lsp_trace("Key code pressed=%x, symbol received=%s", int(e->nCode), s.get_native());

            return STATUS_OK;
        }

        //-----------------------------------------------------------------------------
        // LSPEdit::DataSink implementation
        LSPEdit::DataSink::DataSink(LSPEdit *widget)
        {
            pEdit   = widget;
            pMime   = NULL;
        }

        LSPEdit::DataSink::~DataSink()
        {
            unbind();
        }

        void LSPEdit::DataSink::unbind()
        {
            if (pEdit != NULL)
            {
                if (pEdit->pDataSink == this)
                    pEdit->pDataSink = NULL;
                pEdit       = NULL;
            }

            sOS.drop();

            if (pMime != NULL)
            {
                ::free(pMime);
                pMime   = NULL;
            }
        }

        ssize_t LSPEdit::DataSink::open(const char * const *mime_types)
        {
            const char *mime = NULL;
            size_t i=0, idx = 0;
            for (const char * const *p = mime_types; *p != NULL; ++p, ++i)
            {
                lsp_trace("available mime type: %s", *p);
                if (!::strcasecmp(*p, "text/plain"))
                {
                    mime    = *p;
                    idx     = i;
                }
                else if (!::strcasecmp(*p, "utf8_string"))
                {
                    mime    = *p;
                    idx     = i;
                    break;
                }
            }
            if (mime == NULL)
                return -STATUS_UNSUPPORTED_FORMAT;
            pMime   = ::strdup(mime);
            lsp_trace("Selected mime type: %s, index=%d", pMime, int(idx));
            return (pMime != NULL) ? idx : -STATUS_NO_MEM;
        }

        status_t LSPEdit::DataSink::write(const void *buf, size_t count)
        {
            if (pEdit == NULL)
                return STATUS_CANCELLED;
            if (pMime == NULL)
                return STATUS_CLOSED;
            ssize_t written = sOS.write(buf, count);
            return (written >= ssize_t(count)) ? STATUS_OK : STATUS_UNKNOWN_ERR;
        }

        status_t LSPEdit::DataSink::close(status_t code)
        {
            lsp_trace("code: %x", int(code));
            if ((pMime == NULL) || (pEdit == NULL))
            {
                unbind();
                return STATUS_OK;
            }

            // Commit data
            if (code == STATUS_OK)
            {
                LSPString tmp;

                bool ok = false;
                if (!::strcasecmp(pMime, "utf8_string"))
                    ok  = tmp.set_utf8(reinterpret_cast<const char *>(sOS.data()), sOS.size());
                else
                    ok  = tmp.set_native(reinterpret_cast<const char *>(sOS.data()), sOS.size());

                // Successful set?
                if (ok)
                    pEdit->paste_clipboard(&tmp);
            }

            // Unbind
            unbind();

            return STATUS_OK;
        }

        //-----------------------------------------------------------------------------
        // LSPEdit implementation
        LSPEdit::LSPEdit(LSPDisplay *dpy):
            LSPWidget(dpy),
            sSelection(this),
            sCursor(this),
            sFont(dpy, this),
            sInput(this),
            sColor(this),
            sSelColor(this),
            sStdPopup(dpy)
        {
            sTextPos        = 0;
            nMinWidth       = -1;
            nMBState        = 0;
            nScrDirection   = 0;
            pPopup          = &sStdPopup;
            pDataSink       = NULL;
            pClass          = &metadata;

            vStdItems[0]    = NULL;
            vStdItems[1]    = NULL;
            vStdItems[2]    = NULL;

            sScroll.bind(pDisplay);
            sScroll.set_handler(timer_handler, this);
        }

        LSPEdit::~LSPEdit()
        {
        }

        status_t LSPEdit::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            if (pDisplay != NULL)
            {
                // Initialize keyboard input
                sInput.init(pDisplay);

                // Get theme
                LSPTheme *theme = pDisplay->theme();
                if (theme != NULL)
                {
                    sFont.init(theme->font());
                    init_color(C_BACKGROUND, sFont.color());
                }
            }

            init_color(C_LABEL_TEXT, &sColor);
            init_color(C_KNOB_SCALE, &sSelColor);

            // Initialize standard menu
            ui_handler_id_t id = 0;
            LSP_STATUS_ASSERT(sStdPopup.init());
            LSPMenuItem *mi = new LSPMenuItem(pDisplay);
            if (mi == NULL)
                return STATUS_NO_MEM;
            vStdItems[0] = mi;
            LSP_STATUS_ASSERT(mi->init());
            LSP_STATUS_ASSERT(sStdPopup.add(mi));
            LSP_STATUS_ASSERT(mi->text()->set("actions.edit.cut"));
            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_popup_cut_action, self());
            if (id < 0)
                return -id;

            mi = new LSPMenuItem(pDisplay);
            if (mi == NULL)
                return STATUS_NO_MEM;
            vStdItems[1] = mi;
            LSP_STATUS_ASSERT(mi->init());
            LSP_STATUS_ASSERT(sStdPopup.add(mi));
            LSP_STATUS_ASSERT(mi->text()->set("actions.edit.copy"));
            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_popup_copy_action, self());
            if (id < 0)
                return -id;

            mi = new LSPMenuItem(pDisplay);
            if (mi == NULL)
                return STATUS_NO_MEM;
            vStdItems[2] = mi;
            LSP_STATUS_ASSERT(mi->init());
            LSP_STATUS_ASSERT(sStdPopup.add(mi));
            LSP_STATUS_ASSERT(mi->text()->set("actions.edit.paste"));
            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_popup_paste_action, self());
            if (id < 0)
                return -id;

            // Bind slots
            id = sSlots.add(LSPSLOT_CHANGE, slot_on_change, self());

            // Initialize pointer
            set_cursor(MP_IBEAM);

            return (id >= 0) ? STATUS_OK : -id;
        }

        void LSPEdit::destroy()
        {
            for (size_t i=0; i<3; ++i)
                if (vStdItems[i] != NULL)
                {
                    vStdItems[i]->destroy();
                    delete vStdItems[i];
                    vStdItems[i] = NULL;
                }

            if (pDataSink != NULL)
            {
                pDataSink->unbind();
                pDataSink   = NULL;
            }

            LSPWidget::destroy();
        }

        status_t LSPEdit::timer_handler(timestamp_t time, void *arg)
        {
            LSPEdit *_this = static_cast<LSPEdit *>(arg);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;
            _this->update_scroll();
            return STATUS_OK;
        }

        void LSPEdit::run_scroll(ssize_t dir)
        {
            nScrDirection = dir;
            if (dir == 0)
                sScroll.cancel();
            else if (!sScroll.is_launched())
                sScroll.launch(0, 25);
        }

        void LSPEdit::update_scroll()
        {
            sCursor.move(nScrDirection);
            if (sSelection.valid())
                sSelection.set_last(sCursor.location());
            if ((sCursor.position() <= 0) || (sCursor.position() >= ssize_t(sText.length())))
                sScroll.cancel();
        }

        void LSPEdit::update_clipboard(size_t bufid)
        {
            if (sSelection.valid() && sSelection.non_empty())
            {
                LSPTextDataSource *src = new LSPTextDataSource();
                if (src == NULL)
                    return;
                src->acquire();

                // Set the selection
                ssize_t first, last;
                sSelection.read_range(&first, &last);
                status_t result = src->set_text(&sText, first, last);
                if (result == STATUS_OK)
                    pDisplay->set_clipboard(bufid, src);

                src->release();
            }
        }

        status_t LSPEdit::set_text(const char *text)
        {
            if (!sText.set_native(text))
                return STATUS_NO_MEM;

            query_draw();

            ssize_t len = sText.length();
            if (sCursor.location() > len)
                sCursor.set(len);
            if (sSelection.valid())
            {
                if (sSelection.first() > len)
                    sSelection.set_first(len);
                if (sSelection.last() > len)
                    sSelection.set_last(len);
                if (sSelection.length() <= 0)
                    sSelection.clear();
            }
            return STATUS_OK;
        }

        status_t LSPEdit::set_text(const LSPString *text)
        {
            if (!sText.set(text))
                return STATUS_NO_MEM;

            query_draw();

            ssize_t len = sText.length();
            if (sCursor.location() > len)
                sCursor.set(len);
            if (sSelection.valid())
            {
                if (sSelection.first() > len)
                    sSelection.set_first(len);
                if (sSelection.last() > len)
                    sSelection.set_last(len);
            }
            return STATUS_OK;
        }

        void LSPEdit::set_min_width(ssize_t width)
        {
            if (width == nMinWidth)
                return;
            nMinWidth       = width;
            query_resize();
        }

        void LSPEdit::size_request(size_request_t *r)
        {
            size_t pad      = 3;

            text_parameters_t tp;
            if (!sFont.estimate_text_parameters(&tp, "WW"))
                tp.Width = 0;

            r->nMinWidth    = pad * 2;
            if (nMinWidth > 0)
                r->nMinWidth    += (nMinWidth > tp.Width) ? nMinWidth : tp.Width;
            else
                r->nMinWidth    += tp.Width;
            r->nMinHeight   = sFont.height() + pad * 2;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = r->nMinHeight;
        }

        void LSPEdit::draw(ISurface *s)
        {
            font_parameters_t fp;
            text_parameters_t tp;
            ssize_t pad  = 3;

            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            Color fcol(sFont.raw_color());
            Color sel_col(sSelColor);

            color.scale_lightness(brightness());
            fcol.scale_lightness(brightness());
            sel_col.scale_lightness(brightness());

            // Draw background
            s->clear(bg_color);

            // Draw
            bool aa = s->set_antialiasing(true);
            s->fill_round_rect(0.5f, 0.5f, sSize.nWidth - 1, sSize.nHeight - 1, 4, SURFMASK_ALL_CORNER, color);

            s->set_antialiasing(aa);
            ssize_t fw = sSize.nWidth - pad *2;

            sFont.get_parameters(s, &fp);
            sFont.get_text_parameters(s, &tp, &sText, 0, sCursor.location());
            ssize_t tw    = /*tp.XBearing +*/ tp.XAdvance;

            if (sCursor.visible() && sCursor.replacing() && (sCursor.position() >= ssize_t(sText.length())))
            {
                sFont.get_text_parameters(s, &tp, "_");
                fw         -= tp.Width;
            }

            ssize_t xleft = sTextPos + tw;

            // Adjust cursor position
            if (sCursor.visible())
            {
                // Make cursor visible within the range of edit
                if (xleft < 0)
                {
                    sTextPos    = -tw;
                    xleft       = 0;
                }
                else if (xleft >= ssize_t(fw))
                {
                    sTextPos    = fw - tw;
                    xleft       = fw;
                }
                else if ((sTextPos < 0) && (sCursor.location() == ssize_t(sText.length())))
                {
                    sTextPos   += fw - xleft;
                    xleft       = fw;
                }
            }

            // Adjust text position
            if (sTextPos > 0)
            {
                xleft          -= sTextPos;
                sTextPos        = 0;
            }

            if ((sSelection.valid()) && (!sSelection.is_empty()))
            {
                ssize_t first   = sSelection.starting();
                ssize_t last    = sSelection.ending();

                ssize_t xpos    = sTextPos + pad;

                if (first > 0)
                {
                    sFont.get_text_parameters(s, &tp, &sText, 0, first);
                    sFont.draw(s, xpos, pad + (sSize.nHeight - pad * 2 - fp.Height)*0.5f + fp.Ascent, fcol, &sText, 0, first);
                    xpos           += /*tp.XBearing + */ tp.XAdvance;
                }
                sFont.get_text_parameters(s, &tp, &sText, first, last);
                s->fill_rect(xpos, pad, tp.XBearing + tp.XAdvance, sSize.nHeight - pad*2, sel_col);
                sFont.draw(s, xpos, pad + (sSize.nHeight - pad * 2 - fp.Height)*0.5f + fp.Ascent, color, &sText, first, last);
                xpos           += /*tp.XBearing + */ tp.XAdvance;

                if (last < ssize_t(sText.length()))
                {
                    sFont.get_text_parameters(s, &tp, &sText, last);
                    sFont.draw(s, xpos, pad + (sSize.nHeight - pad * 2 - fp.Height)*0.5f + fp.Ascent, fcol, &sText, last);
                    xpos           += /*tp.XBearing + */ tp.XAdvance;
                }
            }
            else
                sFont.draw(s, sTextPos + pad, pad + (sSize.nHeight - pad * 2 - fp.Height)*0.5f + fp.Ascent, fcol, &sText);

            // Draw cursor if required
            if (sCursor.visible() && sCursor.shining())
            {
                float cleft = xleft + pad ; // + tp.XAdvance + tp.XBearing;
                float ctop  = pad + (sSize.nHeight - pad * 2 - fp.Height)*0.5f;

                if (sCursor.inserting())
                {
                    if ((sSelection.valid()) && (!sSelection.is_empty()))
                        s->line(cleft + 0.5f, ctop, cleft, ctop + fp.Height, 1.0f, bg_color);
                    else
                        s->line(cleft + 0.5f, ctop, cleft, ctop + fp.Height, 1.0f, fcol);
                }
                else // replacing
                {
                    if (sCursor.position() >= ssize_t(sText.length()))
                    {
                        sFont.get_text_parameters(s, &tp, "_");
                        s->fill_rect(cleft, pad, tp.XAdvance, sSize.nHeight - pad * 2, bg_color);
                    }
                    else
                    {
                        sFont.get_text_parameters(s, &tp, &sText, sCursor.position(), sCursor.position() + 1);
                        ssize_t xw = (tp.XAdvance > tp.Width) ? tp.XAdvance : tp.Width + 1;
                        s->fill_rect(cleft + tp.XBearing - 1, pad, xw, sSize.nHeight - pad * 2, bg_color);
                        sFont.draw(s, cleft, ctop + fp.Ascent, color, &sText, sCursor.position(), sCursor.position() + 1);
                    }
                }
            }

            s->set_antialiasing(true);
            s->wire_round_rect(0.5f, 0.5f, sSize.nWidth - 1, sSize.nHeight - 1, 4, SURFMASK_ALL_CORNER, 1, color);

            s->set_antialiasing(aa);
        }

        status_t LSPEdit::on_change()
        {
            return STATUS_OK;
        }

        status_t LSPEdit::slot_on_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPEdit *_this = widget_ptrcast<LSPEdit>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPEdit::on_mouse_down(const ws_event_t *e)
        {
            size_t state = nMBState;
            nMBState    |= (1 << e->nCode);
            if (state == 0)
                take_focus();

            if ((e->nCode == MCB_LEFT) && (state == 0))
            {
                ssize_t first = mouse_to_cursor_pos(e->nLeft, e->nTop);
                if (first >= 0)
                {
                    sSelection.set(first);
                    sCursor.set_position(first);
                }
            }

            return STATUS_OK;
        }

        ssize_t LSPEdit::mouse_to_cursor_pos(ssize_t x, ssize_t y)
        {
            x -= sSize.nLeft;
            if ((x < 0) || (x >= sSize.nWidth))
                return -1;

            // Get surface
            if (pDisplay == NULL)
                return -1;
            ISurface *s = pDisplay->create_surface(1, 1);
            if (pDisplay == NULL)
                return -1;

            ssize_t left = 0, right = sText.length(), pad = 3;
            text_parameters_t tp;
            if (sFont.get_text_parameters(s, &tp, &sText))
            {
                if (x > (sTextPos + pad + tp.XAdvance))
                {
                    s->destroy();
                    delete s;
                    return right;
                }
            }

            while ((right - left) > 1)
            {
                ssize_t middle = (left + right) >> 1;
                if (!sFont.get_text_parameters(s, &tp, &sText, 0, middle))
                {
                    s->destroy();
                    delete s;
                    return -1;
                }

                ssize_t tx = sTextPos + pad + tp.XAdvance;
                if (tx > x)
                    right = middle;
                else if (tx < x)
                    left = middle;
                else // tx == x
                {
                    left    = middle;
                    break;
                }
            }

            // Destroy surface
            s->destroy();
            delete s;

            return left;
        }

        status_t LSPEdit::on_mouse_dbl_click(const ws_event_t *e)
        {
            if (e->nCode == MCB_LEFT)
            {
                ssize_t first = mouse_to_cursor_pos(e->nLeft, e->nTop);
                if (!iswalnum(sText.at(first)))
                    return STATUS_OK;

                ssize_t last = first, len = sText.length();
                while (first > 0)
                {
                    if (!iswalnum(sText.at(first-1)))
                        break;
                    first --;
                }

                while ((++last) < len)
                {
                    if (!iswalnum(sText.at(last)))
                        break;
                }

                sSelection.set(first, last);
                update_clipboard(CBUF_PRIMARY);
                sCursor.set(last);
            }
            return STATUS_OK;
        }

        status_t LSPEdit::on_mouse_tri_click(const ws_event_t *e)
        {
            if (e->nCode == MCB_LEFT)
            {
                sSelection.set(0, sText.length());
                update_clipboard(CBUF_PRIMARY);
            }
            return STATUS_OK;
        }

        status_t LSPEdit::on_mouse_up(const ws_event_t *e)
        {
            lsp_trace("mouse up");
            if ((nMBState == (1 << MCB_RIGHT)) && (e->nCode == MCB_RIGHT))
            {
                if (pPopup != NULL)
                    pPopup->show(this, e);
            }
            else if ((nMBState == (1 << MCB_LEFT)) && (e->nCode == MCB_LEFT))
            {
                update_clipboard(CBUF_PRIMARY);
                if (sSelection.length() <= 0)
                    sSelection.clear();
            }
            else if ((nMBState == (1 << MCB_MIDDLE)) && (e->nCode == MCB_MIDDLE))
            {
                ssize_t first = mouse_to_cursor_pos(e->nLeft, e->nTop);
                sSelection.set(first);
                sCursor.set(first);
                request_clipboard(CBUF_PRIMARY);
            }

            nMBState    &= ~(1 << e->nCode);
            return STATUS_OK;
        }

        status_t LSPEdit::on_mouse_move(const ws_event_t *e)
        {
            if (nMBState == (1 << MCB_LEFT))
            {
                if (e->nLeft < sSize.nLeft)
                    run_scroll(-1);
                else if (e->nLeft > (sSize.nLeft + sSize.nWidth))
                    run_scroll(+1);
                else
                {
                    run_scroll(0);

                    ssize_t last = mouse_to_cursor_pos(e->nLeft, e->nTop);
                    if (last >= 0)
                    {
                        sSelection.set_last(last);
                        sCursor.set_position(last);
                    }
                }
            }

            return STATUS_OK;
        }

        status_t LSPEdit::on_focus_in(const ws_event_t *e)
        {
            sCursor.show();
            return STATUS_OK;
        }

        status_t LSPEdit::on_focus_out(const ws_event_t *e)
        {
            sCursor.hide();
            return STATUS_OK;
        }

        status_t LSPEdit::clipboard_handler(void *arg, status_t s, io::IInStream *is)
        {
            LSPEdit *_this = widget_ptrcast<LSPEdit>(arg);
            return ((s == STATUS_OK) && (_this != NULL) && (is != NULL)) ? _this->paste_data(is) : STATUS_BAD_STATE;
        }

        status_t LSPEdit::paste_data(io::IInStream *is)
        {
            LSPString s;
            size_t avail = is->avail();
            char *buf = new char[avail + 1];
            if (buf == NULL)
                return STATUS_NO_MEM;

            char *dst = buf;
            while (avail > 0)
            {
                size_t count = is->read(dst, avail);
                if (count <= 0)
                {
                    delete [] buf;
                    return is->last_error();
                }
                dst    += count;
                avail  -= count;
            }
            *dst = '\0'; // To be sure that line ends properly

            if (!s.set_utf8(buf))
            {
                delete [] buf;
                return STATUS_NO_MEM;
            }

            size_t pos = sCursor.location();
            if (!sText.insert(pos, &s))
                return STATUS_NO_MEM;

            pos += s.length();
            sCursor.set_location(pos);
            sSelection.set(pos);
            return STATUS_OK;
        }

        void LSPEdit::paste_clipboard(const LSPString *s)
        {
            if (sSelection.valid() && sSelection.non_empty())
            {
                sText.remove(sSelection.starting(), sSelection.ending());
                sCursor.set_location(sSelection.starting());
                sSelection.clear();
            }

            size_t pos = sCursor.location();
            if (!sText.insert(pos, s))
                return;

            pos += s->length();
            sCursor.set_location(pos);
            sSelection.set(pos);
        }

        status_t LSPEdit::on_key_down(const ws_event_t *e)
        {
            LSPString s;
            s.set(lsp_wchar_t(e->nCode));
            lsp_trace("Key code pressed=%x, symbol received=%s, modifiers=%x", int(e->nCode), s.get_native(), int(e->nState));

            ws_code_t key = LSPKeyboardHandler::translate_keypad(e->nCode);

            if (is_character_key(key))
            {
                if (!(e->nState & (MCF_CONTROL | MCF_ALT)))
                {
                    if (sSelection.valid() && sSelection.length() > 0)
                    {
                        sText.remove(sSelection.starting(), sSelection.ending());
                        sCursor.set_location(sSelection.starting());
                        sSelection.clear();
                        update_clipboard(CBUF_PRIMARY);
                    }

                    ssize_t loc = sCursor.location();
                    if ((sCursor.replacing()) && (loc < ssize_t(sText.length())))
                        sText.set(loc, lsp_wchar_t(key));
                    else
                        sText.insert(sCursor.location(), lsp_wchar_t(key));

                    sCursor.move(1);
                    sSlots.execute(LSPSLOT_CHANGE, this);
                    return STATUS_OK;
                }
                else if (e->nState & MCF_CONTROL)
                {
                    switch (e->nCode)
                    {
                        case 'C': case 'c':
                            if (sSelection.valid() && sSelection.non_empty())
                                update_clipboard(CBUF_CLIPBOARD);
                            break;
                        case 'V': case 'v':
                            request_clipboard(CBUF_CLIPBOARD);
                            break;
                        case 'A': case 'a':
                            sSelection.set(0, sText.length());
                            update_clipboard(CBUF_PRIMARY);
                            break;
                        case 'X': case 'x':
                            cut_data(CBUF_CLIPBOARD);
                            break;
                    }
                    return STATUS_OK;
                }
            }

            if (e->nState & MCF_SHIFT)
            {
                if (sSelection.first() < 0)
                    sSelection.set_first(sCursor.location());
            }

            switch (key)
            {
                case WSK_HOME:
                    if (e->nState & MCF_SHIFT)
                        sSelection.set_last(0);
                    else
                        sSelection.clear();
                    sCursor.set_location(0);
                    break;
                case WSK_END:
                    if (e->nState & MCF_SHIFT)
                        sSelection.set_last(sText.length());
                    else
                        sSelection.clear();
                    sCursor.set_location(sText.length());
                    break;
                case WSK_LEFT:
                    sCursor.move(-1);
                    if (e->nState & MCF_SHIFT)
                        sSelection.set_last(sCursor.location());
                    else
                        sSelection.clear();
                    break;
                case WSK_RIGHT:
                    sCursor.move(1);
                    if (e->nState & MCF_SHIFT)
                        sSelection.set_last(sCursor.location());
                    else
                        sSelection.clear();
                    break;
                case WSK_BACKSPACE:
                {
                    if (sSelection.valid() && sSelection.length() > 0)
                    {
                        sText.remove(sSelection.starting(), sSelection.ending());
                        sCursor.set_location(sSelection.starting());
                        sSelection.clear();
                    }
                    else
                    {
                        ssize_t pos = sCursor.location();
                        if (pos <= 0)
                            break;
                        sText.remove(pos - 1, pos);
                        sCursor.set_location(pos-1);
                    }
                    query_draw();
                    sSlots.execute(LSPSLOT_CHANGE, this);
                    break;
                }
                case WSK_DELETE:
                {
                    if (sSelection.valid() && sSelection.length() > 0)
                    {
                        sText.remove(sSelection.starting(), sSelection.ending());
                        sCursor.set_location(sSelection.starting());
                        sSelection.clear();
                    }
                    else
                    {
                        ssize_t pos = sCursor.location();
                        if (pos >= ssize_t(sText.length()))
                            break;
                        sText.remove(pos, pos + 1);
                        sCursor.set_location(pos);
                    }
                    query_draw();
                    sSlots.execute(LSPSLOT_CHANGE, this);
                    break;
                }
                case WSK_INSERT:
                {
                    size_t flags = (e->nState & (MCF_CONTROL | MCF_SHIFT | MCF_ALT));
                    if (flags == MCF_SHIFT)
                        request_clipboard(CBUF_CLIPBOARD);
                    else if (flags == MCF_CONTROL)
                    {
                        if (sSelection.valid() && sSelection.non_empty())
                            update_clipboard(CBUF_CLIPBOARD);
                    }
                    else
                        sCursor.toggle_mode();
                    break;
                }
            }

            //            return sInput.handle_event(e);
            return STATUS_OK;
        }

        void LSPEdit::request_clipboard(size_t bufid)
        {
            // Unbind previous data sink
            if (pDataSink != NULL)
            {
                pDataSink->unbind();
                pDataSink = NULL;
            }

            // Create new data sink and run
            DataSink *sink  = new DataSink(this);
            if (sink == NULL)
                return;
            pDataSink       = sink;

            // Request clipboard contents in async mode
            pDisplay->get_clipboard(bufid, sink);

//            pDisplay->get_clipboard(bufid, sink);
//
//            if (sSelection.valid() && sSelection.non_empty())
//            {
//                sText.remove(sSelection.starting(), sSelection.ending());
//                sCursor.set_location(sSelection.starting());
//                sSelection.clear();
//            }
//            pDisplay->fetch_clipboard(bufid, "UTF8_STRING", clipboard_handler, self());
        }

        status_t LSPEdit::on_key_up(const ws_event_t *e)
        {
            lsp_trace("Key code released=%x, modifiers=%x", int(e->nCode), int(e->nState));
            ws_code_t key = LSPKeyboardHandler::translate_keypad(e->nCode);
            if (((key == WSK_SHIFT_L) || (key == WSK_SHIFT_R)) && (e->nState & MCF_SHIFT))
                update_clipboard(CBUF_PRIMARY);

            return STATUS_OK;
        }

        status_t LSPEdit::cut_data(size_t bufid)
        {
            if (sSelection.valid() && sSelection.non_empty())
            {
                update_clipboard(bufid);
                sText.remove(sSelection.starting(), sSelection.ending());
                sCursor.set_location(sSelection.starting());
                sSelection.clear();
            }
            return STATUS_OK;
        }

        status_t LSPEdit::copy_data(size_t bufid)
        {
            if (sSelection.valid() && sSelection.non_empty())
                update_clipboard(bufid);
            return STATUS_OK;
        }

        status_t LSPEdit::paste_data(size_t bufid)
        {
            request_clipboard(bufid);
            return STATUS_OK;
        }

        status_t LSPEdit::slot_popup_cut_action(LSPWidget *sender, void *ptr, void *data)
        {
            LSPEdit *_this = widget_ptrcast<LSPEdit>(ptr);
            return (_this != NULL) ? _this->cut_data(CBUF_CLIPBOARD) : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPEdit::slot_popup_copy_action(LSPWidget *sender, void *ptr, void *data)
        {
            LSPEdit *_this = widget_ptrcast<LSPEdit>(ptr);
            return (_this != NULL) ? _this->copy_data(CBUF_CLIPBOARD) : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPEdit::slot_popup_paste_action(LSPWidget *sender, void *ptr, void *data)
        {
            LSPEdit *_this = widget_ptrcast<LSPEdit>(ptr);
            return (_this != NULL) ? _this->paste_data(CBUF_CLIPBOARD) : STATUS_BAD_ARGUMENTS;
        }

    } /* namespace tk */
} /* namespace lsp */
