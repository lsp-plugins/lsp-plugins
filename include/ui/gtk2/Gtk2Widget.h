/*
 * Gtk2Window.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef UI_GTK2_GTK2WIDGET_H_
#define UI_GTK2_GTK2WIDGET_H_

namespace lsp
{
    class Gtk2Widget : public IWidget
    {
        protected:
            enum wflags_t
            {
                F_EXPAND    = 1 << 0,
                F_FILL      = 1 << 0
            };

            size_t      nAdded;
            size_t      nWFlags;
            GtkWidget  *pWidget;

        public:
            Gtk2Widget(plugin_ui *ui, widget_t w_class);
            virtual ~Gtk2Widget();

            virtual GtkWidget *widget();

            virtual void add(IWidget *widget);

            virtual void set(widget_attribute_t att, const char *value);

            inline bool expand() const { return nWFlags & F_EXPAND; };

            inline bool fill() const { return nWFlags & F_FILL; };

    };

} /* namespace lsp */

#endif /* UI_GTK2_GTK2WIDGET_H_ */
