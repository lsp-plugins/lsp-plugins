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
    class IWidget: public IUIPortListener
    {
        protected:
            plugin_ui  *pUI;
            widget_t    enClass;
            IUIPort    *pVisibility;
            ssize_t     nVisibilityKey;
            bool        bVisible;

        protected:
            void update_visibility();

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

            /** Hide widget
             *
             */
            virtual void hide();

            /** Show widget
             *
             */
            virtual void show();

        public:
            /** Get visibility key
             *
             * @return visibility key
             */
            inline ssize_t getVisibilityKey() const     { return nVisibilityKey;        }

            /** Set visibility key
             *
             * @param key visibility key
             */
            inline void setVisibilityKey(ssize_t key)   { nVisibilityKey    = key;      }

            /** Get visibility flag
             *
             * @return visibility flag
             */
            inline bool isVisible() const               { return bVisible; }
    };

} /* namespace lsp */

#endif /* _UI_IWIDGET_H_ */
