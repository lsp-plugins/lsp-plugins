/*
 * Gtk2Led.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2LED_H_
#define _UI_GTK2_GTK2LED_H_

namespace lsp
{
    class Gtk2Led: public Gtk2CustomWidget
    {
        private:
            Color           sBgColor;
            size_t          nSize;
            IUIPort        *pPort;
            float           fValue;
            float           fKey;

            ColorHolder     sColor;

        protected:
            bool            key_matched();

        public:
            Gtk2Led(plugin_ui *ui);
            virtual ~Gtk2Led();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void render();

            virtual void resize(size_t &w, size_t &h);

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2LED_H_ */
