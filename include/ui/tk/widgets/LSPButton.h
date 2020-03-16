/*
 * LSPButton.h
 *
 *  Created on: 21 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPBUTTON_H_
#define UI_TK_LSPBUTTON_H_

namespace lsp
{
    namespace tk
    {
        class LSPButton: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum state_t
                {
                    S_PRESSED   = (1 << 0),
                    S_TOGGLED   = (1 << 1),
                    S_OUT       = (1 << 2),
                    S_LED       = (1 << 3),
                    S_TRIGGER   = (1 << 4),
                    S_TOGGLE    = (1 << 5),
                    S_DOWN      = (1 << 6),
                    S_EDITABLE  = (1 << 7),
                };

            protected:
                LSPColor            sColor;
                LSPFont             sFont;
                LSPLocalString      sTitle;

                size_t              nWidth;
                size_t              nHeight;
                size_t              nMinWidth;
                size_t              nMinHeight;
                size_t              nState;
                size_t              nBMask;
                size_t              nChanges;

            public:
                explicit LSPButton(LSPDisplay *dpy);
                virtual ~LSPButton();

                virtual status_t init();

            protected:
                bool            check_mouse_over(ssize_t x, ssize_t y);

                static status_t slot_on_change(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_submit(LSPWidget *sender, void *ptr, void *data);

            public:
                inline bool         is_trigger() const      { return nState & S_TRIGGER; }
                inline bool         is_toggle() const       { return nState & S_TOGGLE; }
                inline bool         is_normal() const       { return !(nState & (S_TOGGLE | S_TRIGGER)); }
                inline bool         is_down() const         { return nState & S_DOWN; }
                inline bool         is_led() const          { return nState & S_LED; }
                inline bool         is_editable() const     { return nState & S_EDITABLE; }
                inline LSPColor    *color()                 { return &sColor; }
                inline LSPColor    *bg_color()              { return &sBgColor; }
                inline LSPFont     *font()                  { return &sFont; }

                inline size_t       min_width() const       { return nMinWidth; }
                inline size_t       min_height() const      { return nMinHeight; }

                inline LSPLocalString *title()              { return &sTitle; }
                inline const LSPLocalString *title() const  { return &sTitle; }

            public:
                void            set_trigger();
                void            set_toggle();
                void            set_normal();
                void            set_editable(bool value = true);
                void            set_down(bool value = true);
                void            set_led(bool value = true);
                void            set_min_width(size_t value);
                void            set_min_height(size_t value);
                void            set_min_size(size_t width, size_t height);

            public:
                virtual void draw(ISurface *s);

                virtual void size_request(size_request_t *r);

                virtual void realize(const realize_t *r);

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);

                virtual status_t on_change();

                virtual status_t on_submit();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPBUTTON_H_ */
