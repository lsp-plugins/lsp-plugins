/*
 * LSPEdit.h
 *
 *  Created on: 29 авг. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPEDIT_H_
#define UI_TK_WIDGETS_LSPEDIT_H_

#include <core/io/OutMemoryStream.h>

namespace lsp
{
    namespace tk
    {
        class LSPEdit: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                class TextSelection: public LSPTextSelection
                {
                    protected:
                        LSPEdit     *pEdit;

                    protected:
                        virtual ssize_t limit(ssize_t value);
                        virtual void on_change();

                    public:
                        explicit TextSelection(LSPEdit *widget);
                        virtual ~TextSelection();
                };

                class TextCursor: public LSPTextCursor
                {
                    protected:
                        LSPEdit     *pEdit;

                    protected:
                        virtual ssize_t limit(ssize_t value);
                        virtual void on_change();
                        virtual void on_blink();

                    public:
                        explicit TextCursor(LSPEdit *widget);
                        virtual ~TextCursor();
                };

                class KeyboardInput: public LSPKeyboardHandler
                {
                    protected:
                        LSPEdit     *pEdit;

                    public:
                        explicit KeyboardInput(LSPEdit *widget);
                        virtual ~KeyboardInput();

                    public:
                        virtual status_t on_key_press(const ws_event_t *e);
                };

                class DataSink: public IDataSink
                {
                    protected:
                        LSPEdit            *pEdit;
                        io::OutMemoryStream sOS;
                        char               *pMime;

                    public:
                        explicit DataSink(LSPEdit *widget);
                        virtual ~DataSink();

                    public:
                        void unbind();

                    public:
                        virtual ssize_t     open(const char * const *mime_types);
                        virtual status_t    write(const void *buf, size_t count);
                        virtual status_t    close(status_t code);
                };

            protected:
                LSPString       sText;
                TextSelection   sSelection;
                TextCursor      sCursor;
                LSPFont         sFont;
                KeyboardInput   sInput;
                LSPColor        sColor;
                LSPColor        sSelColor;
                ssize_t         sTextPos;
                ssize_t         nMinWidth;
                size_t          nMBState;
                ssize_t         nScrDirection;
                LSPTimer        sScroll;
                LSPMenu         sStdPopup;
                LSPMenuItem    *vStdItems[3];
                LSPMenu        *pPopup;
                DataSink       *pDataSink;

            protected:
                static status_t timer_handler(timestamp_t time, void *arg);
                ssize_t         mouse_to_cursor_pos(ssize_t x, ssize_t y);
                void            run_scroll(ssize_t dir);
                void            update_scroll();
                void            update_clipboard(size_t bufid);
                void            request_clipboard(size_t bufid);
                static status_t clipboard_handler(void *arg, status_t s, io::IInStream *is);
                status_t        paste_data(io::IInStream *is);
                status_t        cut_data(size_t bufid);
                status_t        copy_data(size_t bufid);
                status_t        paste_data(size_t bufid);
                void            paste_clipboard(const LSPString *data);

                static status_t slot_on_change(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_popup_cut_action(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_popup_copy_action(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_popup_paste_action(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit LSPEdit(LSPDisplay *dpy);
                virtual ~LSPEdit();

                virtual status_t init();
                virtual void destroy();

            public:
                inline LSPTextSelection   *selection()         { return &sSelection;   }
                inline LSPTextCursor      *cursor()            { return &sCursor;      }
                inline LSPFont            *font()              { return &sFont;        }
                inline const char         *text() const        { return sText.get_native(); }
                inline status_t            get_text(LSPString *dst) const { return dst->set(&sText) ? STATUS_OK : STATUS_NO_MEM; };
                inline const ssize_t       min_width() const   { return nMinWidth;     }
                inline LSPColor           *sel_color()         { return &sSelColor;    }
                inline LSPColor           *color()             { return &sColor;       }
                inline LSPMenu            *get_popup()         { return pPopup;        }

            public:
                status_t            set_text(const char *text);
                status_t            set_text(const LSPString *text);
                void                set_min_width(ssize_t width);
                inline void         set_popup(LSPMenu *popup)   { pPopup = popup; }

            public:
                virtual void size_request(size_request_t *r);

                virtual void draw(ISurface *s);

                virtual status_t on_change();

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);

                virtual status_t on_mouse_dbl_click(const ws_event_t *e);

                virtual status_t on_mouse_tri_click(const ws_event_t *e);

                virtual status_t on_focus_in(const ws_event_t *e);

                virtual status_t on_focus_out(const ws_event_t *e);

                virtual status_t on_key_down(const ws_event_t *e);

                virtual status_t on_key_up(const ws_event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPEDIT_H_ */
