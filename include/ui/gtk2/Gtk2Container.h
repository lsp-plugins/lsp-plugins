/*
 * Gtk2Container.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef UI_GTK2_GTK2CONTAINER_H_
#define UI_GTK2_GTK2CONTAINER_H_

namespace lsp
{
    class Gtk2Container : public Gtk2Widget
    {
        protected:
            Color       sBgColor;

        public:
            Gtk2Container(plugin_ui *ui, widget_t w_class);
            virtual ~Gtk2Container();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void end();

    };

} /* namespace lsp */

#endif /* UI_GTK2_GTK2CONTAINER_H_ */
