/*
 * Gtk2WidgetFactory.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2WIDGETFACTORY_HPP_
#define _UI_GTK2_GTK2WIDGETFACTORY_HPP_

namespace lsp
{
    class Gtk2Widget;

    class Gtk2WidgetFactory: public IWidgetFactory
    {
        private:
            Gtk2Widget     *pWidget;

        public:
            Gtk2WidgetFactory(const char *path);
            virtual ~Gtk2WidgetFactory();

        public:
            virtual IWidget    *createWidget(plugin_ui *ui, widget_t w_class);

            virtual void       *root_widget();
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2WIDGETFACTORY_HPP_ */
