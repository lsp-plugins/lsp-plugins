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
            public:
                static const ctl_class_t metadata;

            protected:
                const ctl_class_t    *pClass;
                CtlRegistry    *pRegistry;
                LSPWidget      *pWidget;
                CtlColor        sBgColor;

                CtlExpression   sVisibility;
                CtlExpression   sBright;
                char           *pVisibilityID;
                ssize_t         nVisible;
                ssize_t         nVisibilityKey;
                bool            bVisibilitySet;
                bool            bVisibilityKeySet;

                ssize_t         nMinWidth;
                ssize_t         nMinHeight;

            protected:
                void            init_color(color_t value, Color *color);
                void            init_color(color_t value, LSPColor *color);
                void            set_lc_attr(widget_attribute_t att, LSPLocalString *s, const char *name, const char *value);

            public:
                explicit CtlWidget(CtlRegistry *src, LSPWidget *widget);
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
                virtual LSPWidget  *widget();

                /** Set attribute to widget controller
                 *
                 * @param name attribute name
                 * @param value attribute value
                 */
                virtual void set(const char *name, const char *value);

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
                virtual status_t add(CtlWidget *child);

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

                /**
                 * Resolve widget by it's unique identifier
                 * @param uid unique widget identifier
                 * @return pointer to resolved widget or NULL
                 */
                virtual LSPWidget *resolve(const char *uid);

            //---------------------------------------------------------------------------------
            // Metadata, casting and type information
            public:
                /** Get widget class
                 *
                 * @return actual widget class metadata
                 */
                inline const ctl_class_t *get_class() const { return pClass; }

                /** Check wheter the widget is instance of some class
                 *
                 * @param wclass widget class
                 * @return true if widget is instance of some class
                 */
                bool instance_of(const ctl_class_t *wclass) const;

                inline bool instance_of(const ctl_class_t &wclass) const { return instance_of(&wclass); }

                /** Another way to check if widget is instance of some class
                 *
                 * @return true if widget is instance of some class
                 */
                template <class CtlTarget>
                    inline bool instance_of() const { return instance_of(&CtlTarget::metadata); };

                /** Cast widget to another type
                 *
                 * @return pointer to widget or NULL if cast failed
                 */
                template <class CtlTarget>
                    inline CtlTarget *cast() { return instance_of(&CtlTarget::metadata) ? static_cast<CtlTarget *>(this) : NULL; }

                /** Cast widget to another type
                 *
                 * @return pointer to widget or NULL if cast failed
                 */
                template <class CtlTarget>
                    inline const CtlTarget *cast() const { return instance_of(&CtlTarget::metadata) ? static_cast<const CtlTarget *>(this) : NULL; }

                /** Get pointer to self as pointer to LSPWidget class
                 *
                 * @return pointer to self
                 */
                inline CtlWidget *self()              { return this;  }
        };

        template <class CtlTarget>
            inline CtlTarget *ctl_cast(CtlWidget *src)
            {
                return ((src != NULL) && (src->instance_of(&CtlTarget::metadata))) ? static_cast<CtlTarget *>(src) : NULL;
            }

        template <class CtlTarget>
            inline const CtlTarget *ctl_cast(const CtlWidget *src)
            {
                return ((src != NULL) && (src->instance_of(&CtlTarget::metadata))) ? static_cast<const CtlTarget *>(src) : NULL;
            }

        template <class CtlTarget>
            inline CtlTarget *ctl_ptrcast(void *src)
            {
                LSPWidget *w = (src != NULL) ? static_cast<CtlTarget *>(src) : NULL;
                return ((w != NULL) && (w->instance_of(&CtlTarget::metadata))) ? static_cast<CtlTarget *>(w) : NULL;
            }

        template <class CtlTarget>
            inline const CtlTarget *ctl_ptrcast(const void *src)
            {
                const LSPWidget *w = (src != NULL) ? static_cast<const CtlTarget *>(src) : NULL;
                return ((w != NULL) && (w->instance_of(&CtlTarget::metadata))) ? static_cast<const CtlTarget *>(w) : NULL;
            }

    }
} /* namespace lsp */

#endif /* UI_CTL_CTLWIDGET_H_ */
