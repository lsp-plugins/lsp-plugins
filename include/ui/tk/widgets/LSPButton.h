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
                    S_DOWN      = (1 << 6)
                };

            protected:
                Color               sColor;
                Color               sBgColor;
                LSPString           sTitle;
                LSPWidgetFont       sFont;

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
                inline bool     is_trigger() const      { return nState & S_TRIGGER; }
                inline bool     is_toggle() const       { return nState & S_TOGGLE; }
                inline bool     is_normal() const       { return !(nState & (S_TOGGLE | S_TRIGGER)); }
                inline bool     is_down() const         { return nState & S_DOWN; }
                inline bool     is_led() const          { return nState & S_LED; }
                inline Color   *color()                 { return &sColor; }
                inline Color   *bg_color()              { return &sBgColor; }
                inline LSPFont *font()                  { return &sFont; }

                inline size_t   min_width() const       { return nMinWidth; }
                inline size_t   min_height() const      { return nMinHeight; }
                inline status_t get_title(LSPString *dst) const { return dst->set(&sTitle) ? STATUS_OK : STATUS_NO_MEM; };
                inline const char *title() const        { return sTitle.get_native(); }

            public:
                void            set_trigger();
                void            set_toggle();
                void            set_normal();
                void            set_color(const Color *c);
                void            set_bg_color(const Color *c);
                void            set_down(bool value = true);
                void            set_led(bool value = true);
                void            set_min_width(size_t value);
                void            set_min_height(size_t value);
                void            set_min_size(size_t width, size_t height);

                status_t        set_title(const char *title);
                status_t        set_title(const LSPString *title);

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
