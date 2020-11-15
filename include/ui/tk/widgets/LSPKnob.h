/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 10 июл. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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

                LSPColor            sColor;
                LSPColor            sScaleColor;
                LSPColor            sHoleColor;
                LSPColor            sTipColor;

                size_t              nSize;
                float               fBalance;
                size_t              nButtons;

                float               fValue;
                float               fStep;
                float               fTinyStep;
                float               fMin;
                float               fMax;
                bool                bCycling;

                ssize_t             nLastY;
                size_t              nState;

            protected:
                size_t          check_mouse_over(ssize_t x, ssize_t y);
                float           get_normalized_value(float value);
                void            set_normalized_value(float value);
                void            update_value(float delta);
                void            on_click(ssize_t x, ssize_t y);

                float           limit_value(float value);

            public:
                explicit LSPKnob(LSPDisplay *dpy);
                virtual ~LSPKnob();

                virtual status_t init();

            public:
                inline LSPColor        *color() { return &sColor; }
                inline LSPColor        *scale_color() { return &sScaleColor; }
                inline LSPColor        *tip_color() { return &sTipColor; }
                inline size_t           size() const { return nSize; }
                inline float            balance() const { return fBalance; }
                inline float            value() const { return fValue; }
                inline float            step() const { return fStep; }
                inline float            tiny_step() const { return fTinyStep; }
                inline float            min_value() const { return fMin; }
                inline float            max_value() const { return fMax; }
                inline bool             cyclic() const { return bCycling; }

            public:
                void                    set_size(size_t value);
                void                    set_balance(float value);
                void                    set_value(float value);
                void                    set_step(float value);
                void                    set_tiny_step(float value);
                void                    set_min_value(float value);
                void                    set_max_value(float value);
                void                    set_cycling(bool cycling);

            public:
                virtual void size_request(size_request_t *r);

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);

                virtual status_t on_mouse_scroll(const ws_event_t *e);

                virtual void draw(ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPKNOB_H_ */
