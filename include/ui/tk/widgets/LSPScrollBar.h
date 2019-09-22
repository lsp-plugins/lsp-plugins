/*
 * LSPScrollBar.h
 *
 *  Created on: 3 авг. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPSCROLLBAR_H_
#define UI_TK_LSPSCROLLBAR_H_

namespace lsp
{
    namespace tk
    {
        class LSPScrollBar: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum flags_t
                {
                    F_ACTIVITY_BITS         = 5,

                    F_BTN_UP_ACTIVE         = 1 << 0,
                    F_BTN_DOWN_ACTIVE       = 1 << 1,
                    F_SLIDER_ACTIVE         = 1 << 2,
                    F_SPARE_UP_ACTIVE       = 1 << 3,
                    F_SPARE_DOWN_ACTIVE     = 1 << 4,

                    F_TRG_BTN_UP_ACTIVE     = F_BTN_UP_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_BTN_DOWN_ACTIVE   = F_BTN_DOWN_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_SLIDER_ACTIVE     = F_SLIDER_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_SPARE_UP_ACTIVE   = F_SPARE_UP_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_SPARE_DOWN_ACTIVE = F_SPARE_DOWN_ACTIVE << F_ACTIVITY_BITS,

                    F_FILL                  = 1 << (F_ACTIVITY_BITS << 1),
                    F_OUTSIDE               = 1 << ((F_ACTIVITY_BITS << 1) + 1),
                    F_PRECISION             = 1 << ((F_ACTIVITY_BITS << 1) + 2),

                    F_ACTIVITY_MASK         = F_BTN_UP_ACTIVE | F_BTN_DOWN_ACTIVE | F_SLIDER_ACTIVE | F_SPARE_UP_ACTIVE | F_SPARE_DOWN_ACTIVE,
                    F_TRG_ACTIVITY_MASK     = F_ACTIVITY_MASK << F_ACTIVITY_BITS,
                    F_ALL_ACTIVITY_MASK     = F_ACTIVITY_MASK | F_TRG_ACTIVITY_MASK
                };

            protected:
                float           fMin;
                float           fMax;
                float           fValue;
                float           fStep;
                float           fTinyStep;
                ssize_t         nSize;
                size_t          nFlags;
                size_t          nButtons;
                ssize_t         nLastV;
                float           fLastValue;
                float           fCurrValue;
                orientation_t   enOrientation;
                mouse_pointer_t enActiveCursor;

                LSPColor        sColor;
                LSPColor        sSelColor;
                LSPTimer        sTimer;

            protected:
                float           limit_value(float value);
                float           get_normalized_value();
                size_t          check_mouse_over(ssize_t x, ssize_t y);
                void            do_destroy();
                void            update();
                void            update_cursor_state(ssize_t x, ssize_t y, bool set);

                static status_t slot_on_change(LSPWidget *sender, void *ptr, void *data);
                static status_t timer_handler(timestamp_t time, void *arg);

            public:
                explicit LSPScrollBar(LSPDisplay *dpy, bool horizontal = false);
                virtual ~LSPScrollBar();

                virtual status_t        init();
                virtual void            destroy();

            public:
                inline float            value() const { return fValue; }
                inline float            step() const { return fStep; }
                inline float            tiny_step() const { return fTinyStep; }
                inline float            min_value() const { return fMin; }
                inline float            max_value() const { return fMax; }
                inline ssize_t          size() const { return nSize; }
                inline bool             horizontal() const { return enOrientation == O_HORIZONTAL; }
                inline bool             vertical() const { return enOrientation == O_VERTICAL; }
                inline orientation_t    orientation() const { return enOrientation; }
                inline bool             fill() const { return nFlags & F_FILL; }
                inline LSPColor        *color() { return &sColor; }
                inline LSPColor        *sel_color() { return &sSelColor; }
                virtual mouse_pointer_t active_cursor() const;

            public:
                void                    set_value(float value);
                void                    set_step(float value);
                void                    set_tiny_step(float value);
                void                    set_min_value(float value);
                void                    set_max_value(float value);
                void                    set_size(ssize_t value);
                void                    set_orientation(orientation_t value);
                inline void             set_horizontal(bool value = true)   { set_orientation((value) ? O_HORIZONTAL : O_VERTICAL); }
                inline void             set_vertical(bool value = true)     { set_orientation((value) ? O_VERTICAL : O_HORIZONTAL); }
                void                    set_fill(bool value = true);
                virtual status_t        set_cursor(mouse_pointer_t mp);

            public:
                virtual void size_request(size_request_t *r);

                virtual status_t on_change();

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);

                virtual status_t on_mouse_scroll(const ws_event_t *e);

                virtual void draw(ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPSCROLLBAR_H_ */
