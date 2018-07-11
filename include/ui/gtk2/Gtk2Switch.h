/*
 * Gtk2Switch.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2SWITCH_H_
#define _UI_GTK2_GTK2SWITCH_H_

namespace lsp
{
    class Gtk2Switch: public Gtk2CustomWidget
    {
        protected:
            enum state_t
            {
                S_PRESSED   = (1 << 0),
                S_TOGGLED   = (1 << 1),
                S_INVERT    = (1 << 2)
            };

            ColorHolder     sColor;
            ColorHolder     sTextColor;
            ColorHolder     sBorderColor;
            ColorHolder     sBgColor;

            IUIPort        *pPort;
            size_t          nSize;
            size_t          nBorder;
            float           nAspect;
            size_t          nState;
            size_t          nBMask;
            size_t          nAngle;

        private:
            bool        check_mouse_over(ssize_t x, ssize_t y);
            void        dimensions(ssize_t &w, ssize_t &h);

        protected:
            void        draw(cairo_t *cr);

        public:
            Gtk2Switch(plugin_ui *ui);
            virtual ~Gtk2Switch();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void resize(size_t &w, size_t &h);

            virtual void button_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void button_release(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void motion(ssize_t x, ssize_t y, size_t state);

            virtual void on_click(bool down);

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2SWITCH_H_ */
