/*
 * IWidgetFactory.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_IWIDGETFACTORY_H_
#define _UI_IWIDGETFACTORY_H_

namespace lsp
{
    class IWidgetFactory
    {
        private:
            char       *pPath;

        public:
            IWidgetFactory(const char *path);
            virtual ~IWidgetFactory();

        public:
            /** Get installation/bundle path
             *
             * @return installation/bundle path
             */
            inline const char *path() const { return pPath; }

            /** Create widget
             *
             * @param ui plugin user interface
             * @param w_class widget class string
             * @return created widget
             */
            IWidget *createWidget(plugin_ui *ui, const char *w_class);

            /** Create widget
             *
             * @param widget widget class
             * @return created widget
             */
            virtual IWidget *createWidget(plugin_ui *ui, widget_t w_class);

            /** Get root widget
             *
             */
            virtual void       *root_widget();

    };

} /* namespace lsp */

#endif /* IWIDGETFACTORY_H_ */
