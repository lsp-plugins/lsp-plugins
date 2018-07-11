/*
 * Gtk2Button.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2BUTTON_H_
#define _UI_GTK2_GTK2BUTTON_H_

namespace lsp
{
    class Gtk2Button: public Gtk2CustomWidget
    {
        protected:
            enum state_t
            {
                S_PRESSED   = (1 << 0),
                S_TOGGLED   = (1 << 1),
            };

            Gtk2Color       sColor;
            Gtk2Color       sBgColor;

            IUIPort        *pPort;
            size_t          nSize;
            size_t          nState;
            size_t          nBMask;
            float           fValue;

        private:
            bool        check_mouse_over(ssize_t x, ssize_t y);
            float       next_value(bool down);
            void        set_value(float value);
            bool        is_trigger();

        public:
            Gtk2Button(plugin_ui *ui);
            virtual ~Gtk2Button();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void render();

            virtual void resize(size_t &w, size_t &h);

            virtual void button_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void button_release(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void motion(ssize_t x, ssize_t y, size_t state);

            virtual void on_click(bool down);

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2BUTTON_H_ */
