/*
 * LSPFader.h
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPFADER_H_
#define UI_TK_WIDGETS_LSPFADER_H_

namespace lsp
{
    namespace tk
    {
        
        class LSPFader: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum flags_t
                {
                    F_IGNORE        = 1 << 0,
                    F_PRECISION     = 1 << 1,
                    F_MOVER         = 1 << 2
                };

            protected:
                float           fMin;
                float           fMax;
                float           fValue;
                float           fDefault;
                float           fStep;
                float           fTinyStep;
                ssize_t         nMinSize;
                size_t          nAngle;
                ssize_t         nLastV;
                size_t          nButtons;
                size_t          nBtnLength;
                size_t          nBtnWidth;
                size_t          nXFlags;
                float           fLastValue;
                float           fCurrValue;

                LSPColor        sColor;

            protected:
                float           limit_value(float value);
                float           get_normalized_value();
                bool            check_mouse_over(ssize_t x, ssize_t y);
                void            do_destroy();
                void            update();
                void            update_cursor_state(ssize_t x, ssize_t y, bool set);

                static status_t slot_on_change(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit LSPFader(LSPDisplay *dpy);
                virtual ~LSPFader();

                virtual status_t        init();
                virtual void            destroy();

            public:
                inline float            value() const { return fValue; }
                inline float            default_value() const { return fDefault; }
                inline float            step() const { return fStep; }
                inline float            tiny_step() const { return fTinyStep; }
                inline float            min_value() const { return fMin; }
                inline float            max_value() const { return fMax; }
                inline LSPColor        *color() { return &sColor; }
                inline size_t           angle() const { return nAngle; }
                inline size_t           button_length() const { return nBtnLength; }
                inline size_t           button_width() const { return nBtnWidth; }
                virtual mouse_pointer_t active_cursor() const;

            public:
                void                    set_value(float value);
                void                    set_default_value(float value);
                void                    set_step(float value);
                void                    set_tiny_step(float value);
                void                    set_min_value(float value);
                void                    set_max_value(float value);
                void                    set_min_size(ssize_t value);
                void                    set_angle(size_t value);
                void                    set_button_width(size_t value);
                void                    set_button_length(size_t value);

            public:
                virtual void size_request(size_request_t *r);

                virtual status_t on_change();

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_dbl_click(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);

                virtual status_t on_mouse_scroll(const ws_event_t *e);

                virtual void draw(ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPFADER_H_ */
