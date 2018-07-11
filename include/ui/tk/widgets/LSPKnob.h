/*
 * LSPKnob.h
 *
 *  Created on: 10 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPKNOB_H_
#define UI_TK_LSPKNOB_H_

namespace lsp
{
    namespace tk
    {
        class LSPKnob: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum state_t
                {
                    S_NONE,
                    S_MOVING,
                    S_CLICK
                };

                Color               sColor;
                Color               sBgColor;
                Color               sScaleColor;

                size_t              nSize;
                float               fBalance;
                size_t              nButtons;

                float               fDflValue;
                float               fValue;
                float               fStep;
                float               fTinyStep;
                float               fMin;
                float               fMax;

                ssize_t             nLastY;
                size_t              nState;

            protected:
                size_t          check_mouse_over(ssize_t x, ssize_t y);
                float           get_normalized_value();
                void            set_normalized_value(float value);
                void            update_value(float delta);
                void            on_click(ssize_t x, ssize_t y);

                float           limit_value(float value);

            public:
                explicit LSPKnob(LSPDisplay *dpy);
                virtual ~LSPKnob();

                virtual status_t init();

            public:
                inline Color           *color() { return &sColor; }

                inline Color           *bg_color() { return &sBgColor; }

                inline Color           *scale_color() { return &sScaleColor; }

                inline size_t           size() const { return nSize; }

                inline float            balance() const { return fBalance; }

                inline float            value() const { return fValue; }

                inline float            default_value() const { return fDflValue; }

                inline float            step() const { return fStep; }

                inline float            tiny_step() const { return fTinyStep; }

                inline float            min_value() const { return fMin; }

                inline float            max_value() const { return fMax; }

            public:
                void                    set_size(size_t value);

                void                    set_balance(float value);

                void                    set_value(float value);

                void                    set_default_value(float value);

                void                    set_step(float value);

                void                    set_tiny_step(float value);

                void                    set_min_value(float value);

                void                    set_max_value(float value);

            public:
                virtual void size_request(size_request_t *r);

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);

                virtual status_t on_mouse_scroll(const ws_event_t *e);

                virtual status_t on_mouse_dbl_click(const ws_event_t *e);

                virtual void draw(ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPKNOB_H_ */
