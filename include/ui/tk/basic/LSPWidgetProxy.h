/*
 * LSPWidgetProxy.h
 *
 *  Created on: 21 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPWIDGETPROXY_H_
#define UI_TK_LSPWIDGETPROXY_H_

namespace lsp
{
    namespace tk
    {
        class LSPWidgetProxy: public LSPWidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                LSPWidget      *pWidget;
                size_t          nLevel;

            public:
                explicit LSPWidgetProxy(LSPDisplay *dpy);
                virtual ~LSPWidgetProxy();

                virtual void destroy();

            public:
                virtual void    query_draw(size_t flags);

                virtual void    commit_redraw();

                /** Add child to widget container
                 *
                 * @param child child widget to add
                 * @return status of operation
                 */
                virtual status_t    add(LSPWidget *child);

                /** Remove child from widget container
                 *
                 * @param child child widget to remove
                 * @return status of operation
                 */
                virtual status_t    remove(LSPWidget *child);

                /** Render widget to the external surface
                 *
                 * @param surface surface to perform rendering
                 * @param force force child rendering
                 */
                virtual void render(ISurface *s, bool force);

                /** Draw widget on the internal surface
                 *
                 * @param surface surface to perform drawing
                 */
                virtual void draw(ISurface *s);

                /** Realize widget
                 *
                 * @param r widget realization parameters
                 */
                virtual void realize(const realize_t *r);

                /** Request for size
                 *
                 * @param r minimum and maximum dimensions of the widget
                 */
                virtual void size_request(size_request_t *r);

                /** Hide widget
                 *
                 */
                virtual bool hide();

                /** Show widget
                 *
                 */
                virtual bool show();

                /** Set widget visibility
                 *
                 * @param visible widget visibility
                 */
                virtual void set_visible(bool visible);

                /** Handle UI event from the display
                 *
                 * @param e UI event
                 * @return status of operation
                 */
                virtual status_t handle_event(const ws_event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPWIDGETPROXY_H_ */
