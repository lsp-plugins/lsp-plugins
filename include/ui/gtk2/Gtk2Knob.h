/*
 * Gtk2Knob.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2KNOB_H_
#define _UI_GTK2_GTK2KNOB_H_

namespace lsp
{
    class Gtk2Knob: public Gtk2CustomWidget
    {
        protected:
            enum state_t
            {
                S_NONE,
                S_MOVING,
                S_CLICK
            };

            ColorHolder         sColor;
            ColorHolder         sBgColor;
            ColorHolder         sScaleColor;

            size_t              nSize;
            float               nBalance;
            size_t              nButtons;

            float               fValue;
            float               fStep;
            float               fMin;
            float               fMax;

            ssize_t             nLastY;
            size_t              nState;
            IUIPort            *pPort;

        protected:
            size_t          check_mouse_over(ssize_t x, ssize_t y);
            float           calc_step(bool tolerance);
            float           get_normalized_value();
            void            apply_metadata_params(const port_t *p);
            void            update_value(float delta);
            void            set_normalized_value(float value);
            void            on_click(ssize_t x, ssize_t y);
            virtual void    draw(cairo_t *cr);

        public:
            Gtk2Knob(plugin_ui *ui);
            virtual ~Gtk2Knob();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void resize(size_t &w, size_t &h);

            virtual void button_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void button_double_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void button_release(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void motion(ssize_t x, ssize_t y, size_t state);

            virtual void scroll(ssize_t x, ssize_t y, size_t state, size_t direction);

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2KNOB_H_ */
