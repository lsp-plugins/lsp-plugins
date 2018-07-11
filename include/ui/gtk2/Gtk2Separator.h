/*
 * Gtk2Separator.h
 *
 *  Created on: 29 марта 2016 г.
 *      Author: sadko
 */

#ifndef UI_GTK2SEPARATOR_H_
#define UI_GTK2SEPARATOR_H_

namespace lsp
{
    class Gtk2Separator: public Gtk2Widget
    {
        protected:
            ColorHolder     sColor;
            ColorHolder     sBgColor;
            size_t          nBorder;
            size_t          nWidth;

        public:
            Gtk2Separator(plugin_ui *ui, widget_t w_class);
            virtual ~Gtk2Separator();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void render();

            virtual void end();

            virtual void resize(ssize_t &w, ssize_t &h);

            virtual void destroy();
    };

    class Gtk2VSeparator: public Gtk2Separator
    {
        public:
            Gtk2VSeparator(plugin_ui *ui);
            virtual ~Gtk2VSeparator();
    };

    class Gtk2HSeparator: public Gtk2Separator
    {
        public:
            Gtk2HSeparator(plugin_ui *ui);
            virtual ~Gtk2HSeparator();
    };

} /* namespace lsp */

#endif /* UI_GTK2SEPARATOR_H_ */
