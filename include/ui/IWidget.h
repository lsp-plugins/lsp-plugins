/*
 * IWidget.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_IWIDGET_H_
#define _UI_IWIDGET_H_

namespace lsp
{
    class IWidget
    {
        protected:
            plugin_ui  *pUI;
            widget_t    enClass;

        public:
            IWidget(plugin_ui *ui, widget_t w_class);

            virtual ~IWidget();

        public:
            inline widget_t getClass()  { return enClass;   };
            inline plugin_ui *getUI()   { return pUI;       };

        public:
            /** Set attribute to widget
             *
             * @param name attribute name
             * @param value attribute value
             */
            void set(const char *name, const char *value);

            /** Set attribute to widget
             *
             * @param att attribute identifier
             * @param value attribute value
             */
            virtual void set(widget_attribute_t att, const char *value);

            /** Begin element body
             *
             */
            virtual void begin();

            /** End element body (complete initialization)
             *
             */
            virtual void end();

            /** Add child widget to current widget
             *
             * @param widget child widget to add
             */
            virtual void add(IWidget *widget);

            /** Notify port change
             *
             * @param port port that has been changed
             * @param buffer port data
             * @param count number of elements
             */
            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* _UI_IWIDGET_H_ */
