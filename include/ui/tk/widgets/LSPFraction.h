/*
 * LSPFraction.h
 *
 *  Created on: 29 мая 2018 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPFRACTION_H_
#define UI_TK_WIDGETS_LSPFRACTION_H_

namespace lsp
{
    namespace tk
    {
        class LSPFraction: public LSPComplexWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum mstate_t {
                    NUM_CLICK       = 1 << 0,
                    DENOM_CLICK     = 1 << 1,
                    NONE_CLICK      = 1 << 2
                };

            protected:
                LSPComboBox         sNumerator;
                LSPComboBox         sDenominator;
                LSPFont             sFont;
                LSPColor            sColor;
                realize_t           sNum;
                realize_t           sDenom;

                ssize_t             nMinWidth;
                ssize_t             nMinHeight;
                ssize_t             nTextBorder;
                float               fAngle;
                size_t              nMFlags;
                size_t              nMState;

            protected:
                void                do_destroy();
                status_t            on_list_change();
                status_t            on_list_submit();

                ssize_t             estimate_max_size(LSPComboBox *cb, ISurface *s);
                bool                check_mouse_over(const realize_t *r, ssize_t x, ssize_t y);

            protected:
                static status_t     slot_on_change(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_list_change(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_list_submit(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit LSPFraction(LSPDisplay *dpy);
                virtual ~LSPFraction();

                virtual status_t init();

                virtual void destroy();

            public:
                inline ssize_t      num_selected() const        { return sNumerator.selected(); };
                inline ssize_t      denom_selected() const      { return sDenominator.selected(); };
                inline LSPItemList *num_items()                 { return sNumerator.items();  }
                inline LSPItemList *denom_items()               { return sDenominator.items();  }
                inline ssize_t      min_width() const           { return nMinWidth;  }
                inline ssize_t      min_height() const          { return nMinHeight; }
                inline bool         num_opened() const          { return sNumerator.opened(); }
                inline bool         denom_opened() const        { return sDenominator.opened(); }
                inline bool         num_circular() const        { return sNumerator.circular();   }
                inline bool         denom_circular() const      { return sDenominator.circular();   }
                inline float        angle() const               { return fAngle; }
                inline ssize_t      text_border() const         { return nTextBorder; }
                inline bool         opened() const              { return sNumerator.opened() || sDenominator.opened(); }

                inline LSPFont     *font()                      { return &sFont; }
                inline LSPColor    *color()                     { return &sColor; }

            public:
                void set_min_width(ssize_t value);
                void set_min_height(ssize_t value);

                status_t set_num_selected(ssize_t value);
                status_t set_num_opened(bool open = true);
                status_t set_num_closed(bool closed = true);

                status_t set_denom_selected(ssize_t value);
                status_t set_denom_opened(bool open = true);
                status_t set_denom_closed(bool closed = true);

                status_t open_num();
                status_t toggle_num();
                status_t close_num();

                status_t open_denom();
                status_t toggle_denom();
                status_t close_denom();

                status_t close();

                status_t set_angle(float angle);

                status_t set_text_border(ssize_t border);

            public:
                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_scroll(const ws_event_t *e);

                virtual status_t on_change();

                virtual status_t on_submit();

                virtual void realize(const realize_t *r);

                virtual void size_request(size_request_t *r);

                virtual void draw(ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPFRACTION_H_ */
