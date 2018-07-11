/*
 * Gtk2Window.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef UI_GTK2_GTK2WIDGETPROXY_H_
#define UI_GTK2_GTK2WIDGETPROXY_H_

namespace lsp
{
    class Gtk2WidgetProxy : public Gtk2Widget
    {
        protected:
            Gtk2Widget *pGtk2Widget;

        public:
            Gtk2WidgetProxy(plugin_ui *ui, widget_t w_class);
            virtual ~Gtk2WidgetProxy();

            virtual GtkWidget *widget();

            virtual void add(IWidget *widget);

            virtual bool expand() const;

            virtual bool fill() const;
    };

} /* namespace lsp */

#endif /* UI_GTK2_GTK2WIDGETPROXY_H_ */
