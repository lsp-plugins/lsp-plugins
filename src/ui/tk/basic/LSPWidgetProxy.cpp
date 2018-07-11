/*
 * LSPWidgetProxy.cpp
 *
 *  Created on: 21 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPWidgetProxy::metadata = { "LSPWidgetProxy", &LSPWidgetContainer::metadata };
        
        LSPWidgetProxy::LSPWidgetProxy(LSPDisplay *dpy): LSPWidgetContainer(dpy)
        {
            pWidget         = NULL;
            pClass          = &metadata;
            nLevel          = 0;
        }
        
        LSPWidgetProxy::~LSPWidgetProxy()
        {
            pWidget         = NULL;
        }

        void LSPWidgetProxy::destroy()
        {
            if (pWidget != NULL)
            {
                unlink_widget(pWidget);
                pWidget = NULL;
            }
        }

        void LSPWidgetProxy::query_draw(size_t flags)
        {
            LSPWidgetContainer::query_draw(flags);

            if (!(nLevel++))
            {
                if (pWidget != NULL)
                    pWidget->query_draw(flags);
            }
            nLevel--;
        }

        void LSPWidgetProxy::commit_redraw()
        {
            LSPWidgetContainer::commit_redraw();

            if (!(nLevel++))
            {
                if (pWidget != NULL)
                    pWidget->commit_redraw();
            }
            nLevel--;
        }

        status_t LSPWidgetProxy::add(LSPWidget *child)
        {
            if (pWidget != NULL)
                return STATUS_ALREADY_EXISTS;

            child->set_parent(this);
            pWidget = child;

            return STATUS_OK;
        }

        status_t LSPWidgetProxy::remove(LSPWidget *child)
        {
            if (pWidget != child)
                return STATUS_NOT_FOUND;

            if (pWidget != NULL)
            {
                unlink_widget(pWidget);
                pWidget  = NULL;
            }

            return STATUS_OK;
        }

        void LSPWidgetProxy::render(ISurface *s, bool force)
        {
            if (pWidget != NULL)
                pWidget->render(s, force);
        }

        void LSPWidgetProxy::draw(ISurface *s)
        {
            if (pWidget != NULL)
                pWidget->draw(s);
        }

        void LSPWidgetProxy::realize(const realize_t *r)
        {
            if (pWidget != NULL)
            {
                pWidget->realize(r);
                LSPWidgetContainer::realize(r);
            }
        }

        void LSPWidgetProxy::size_request(size_request_t *r)
        {
            if (pWidget != NULL)
                pWidget->size_request(r);
        }

        bool LSPWidgetProxy::hide()
        {
            if (pWidget != NULL)
                pWidget->hide();
            return LSPWidgetContainer::hide();
        }

        bool LSPWidgetProxy::show()
        {
            if (pWidget != NULL)
                pWidget->show();
            return LSPWidgetContainer::show();
        }

        void LSPWidgetProxy::set_visible(bool visible)
        {
            if (pWidget != NULL)
                pWidget->set_visible(visible);
            LSPWidgetContainer::set_visible(visible);
        }

        status_t LSPWidgetProxy::handle_event(const ws_event_t *e)
        {
            return (pWidget != NULL) ? pWidget->handle_event(e) : STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
