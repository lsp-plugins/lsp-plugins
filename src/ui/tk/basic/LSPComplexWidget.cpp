/*
 * LSPComplexWidget.cpp
 *
 *  Created on: 10 авг. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPComplexWidget::metadata = { "LSPComplexWidget", &LSPWidget::metadata };

        LSPComplexWidget::LSPComplexWidget(LSPDisplay *dpy): LSPWidget(dpy)
        {
            nMouse      = 0;
            nKey        = 0;
            pMouse      = NULL;
            pKey        = NULL;
            pClass      = &metadata;
        }

        LSPComplexWidget::~LSPComplexWidget()
        {
        }

        LSPWidget *LSPComplexWidget::acquire_mouse_handler(const ws_event_t *e)
        {
            // Check that we work in exclusive mode
            if ((nMouse != 0) && (pMouse != NULL))
                return pMouse;

            LSPWidget *child  = find_widget(e->nLeft, e->nTop);
            if (child == pMouse)
                return pMouse;

            // New child has been found
            if (pMouse != NULL)
            {
                ws_event_t ev = *e;
                ev.nType    = UIE_MOUSE_OUT;
                pMouse->handle_event(&ev);
                mark_pointed();
            }
            if (child != NULL)
            {
                ws_event_t ev = *e;
                ev.nType    = UIE_MOUSE_IN;
                child->handle_event(&ev);
            }

            return pMouse = child;
        }

        void LSPComplexWidget::release_mouse_handler(const ws_event_t *e)
        {
            if (pMouse == NULL)
            {
                nMouse = 0;
                return;
            }

            if (e->nState & MCF_BTN_MASK)
                return;

            // Check that cursor is out of the widget
            if (pMouse != find_widget(e->nLeft, e->nTop))
            {
                ws_event_t ev = *e;
                ev.nType    = UIE_MOUSE_OUT;
                pMouse->handle_event(&ev);
                pMouse      = NULL;
            }
        }

        LSPWidget *LSPComplexWidget::find_widget(ssize_t x, ssize_t y)
        {
            return NULL;
        }

        status_t LSPComplexWidget::handle_event_internal(const ws_event_t *e)
        {
            return LSPWidget::handle_event(e);
        }

        status_t LSPComplexWidget::handle_event(const ws_event_t *e)
        {
            switch (e->nType)
            {
                case UIE_KEY_UP:
                {
                    LSPWidget *child  = (pKey == NULL) ? find_widget(e->nLeft, e->nTop) : pKey;
                    if (child == NULL)
                        return handle_event_internal(e);

                    // Handle key release event and free if possible
                    child->handle_event(e);
                    if ((--nKey) <= 0)
                        pKey        = NULL;

                    break;
                }

                case UIE_KEY_DOWN:
                {
                    LSPWidget *child  = (pKey == NULL) ? find_widget(e->nLeft, e->nTop) : pKey;
                    if (child == NULL)
                        return handle_event_internal(e);

                    // Handle key press event
                    child->handle_event(e);
                    nKey++;
                    pKey        = child;

                    break;
                }

                case UIE_MOUSE_UP:
                {
                    LSPWidget *child = acquire_mouse_handler(e);
                    nMouse     &= ~(1 << e->nCode);
                    if (child == NULL)
                        return handle_event_internal(e);

                    // Handle mouse release event and free if possible
                    child->handle_event(e);
                    release_mouse_handler(e);
                    break;
                }

                case UIE_MOUSE_DOWN:
                {
                    LSPWidget *child = acquire_mouse_handler(e);
                    nMouse     |= 1 << e->nCode;
                    if (child == NULL)
                        return handle_event_internal(e);

                    // Handle mouse button press event
                    return child->handle_event(e);
                }

                case UIE_MOUSE_DBL_CLICK:
                case UIE_MOUSE_TRI_CLICK:
                case UIE_MOUSE_SCROLL:
                {
                    LSPWidget *child  = acquire_mouse_handler(e);
                    if (child == NULL)
                        handle_event_internal(e);
                    else
                        child->handle_event(e);
                    release_mouse_handler(e);
                    break;
                }

                case UIE_MOUSE_MOVE:
                {
                    LSPWidget *child  = acquire_mouse_handler(e);
                    nMouse      = e->nState & MCF_BTN_MASK;
                    if (child == NULL)
                        return handle_event_internal(e);

                    child->handle_event(e);
                    break;
                }

                case UIE_MOUSE_OUT:
                {
                    // Check that cursor is out of the widget
                    if ((nMouse == 0) && (pMouse != NULL))
                    {
                        ws_event_t ev = *e;
                        ev.nType    = UIE_MOUSE_OUT;
                        pMouse->handle_event(&ev);
                        pMouse      = NULL;
                    }
                    break;
                }

                case UIE_DRAG_REQUEST:
                {
                    LSPWidget *child = find_widget(e->nLeft, e->nTop);
                    if (child != NULL) // Pass event to the child
                        child->handle_event(e);
                    else
                    {
                        ws_event_t tmp = *e;
                        sSlots.execute(LSPSLOT_DRAG_REQUEST, this, &tmp);
                    }
                    break;
                }

                default:
                    return handle_event_internal(e);
            }

            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */
