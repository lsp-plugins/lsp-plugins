/*
 * CtlWidget.h
 *
 *  Created on: 15 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLWIDGET_H_
#define UI_CTL_CTLWIDGET_H_

namespace lsp
{
    namespace ctl
    {
        class CtlWidget: public CtlPortListener
        {
            protected:
                CtlRegistry    *pRegistry;
                LSPWidget      *pWidget;

                CtlExpression   sVisibility;
                char           *pVisibilityID;
                ssize_t         nVisible;
                ssize_t         nVisibilityKey;
                bool            bVisibilitySet;
                bool            bVisibilityKeySet;

                ssize_t         nMinWidth;
                ssize_t         nMinHeight;

            protected:
                void            init_color(color_t value, Color *color);

            public:
                CtlWidget(CtlRegistry *src, LSPWidget *widget);
                virtual ~CtlWidget();

                /** Destroy widget controller
                 *
                 */
                virtual void destroy();

            public:
                /** Get widget
                 *
                 * @return widget
                 */
                inline LSPWidget   *widget() { return pWidget; };

            public:
                /** Set attribute to widget controller
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

                /** Add child widget
                 *
                 * @param child child widget to add
                 */
                virtual status_t add(LSPWidget *child);

                /** Initialize widget
                 *
                 */
                virtual void init();

                /** Begin internal part of controller
                 *
                 */
                virtual void begin();

                /** End internal part of controller
                 *
                 */
                virtual void end();

                /** Notify controller about one of port bindings has changed
                 *
                 * @param port port triggered change
                 */
                virtual void notify(CtlPort *port);
        };

    }
} /* namespace lsp */

#endif /* UI_CTL_CTLWIDGET_H_ */
