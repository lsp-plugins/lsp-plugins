/*
 * LSPSlot.cpp
 *
 *  Created on: 12 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

#define ID_GEN_MASK         0x7fffff

namespace lsp
{
    namespace tk
    {
        LSPSlot::LSPSlot()
        {
            pRoot       = NULL;
            nID         = 0;
        }

        LSPSlot::~LSPSlot()
        {
            unbind_all();
        }

        inline LSPSlot::handler_item_t *LSPSlot::find_item(ui_handler_id_t id)
        {
            handler_item_t *ptr = pRoot;
    
            while (ptr != NULL)
            {
                if (ptr->nID == id)
                    break;
                ptr     = ptr->pNext;
            }
            return ptr;
        }
    
        ui_handler_id_t LSPSlot::bind(ui_event_handler_t handler, void *arg, bool enabled)
        {
            return bind(handler, false, arg, enabled);
        }

        ui_handler_id_t LSPSlot::intercept(ui_event_handler_t handler, void *arg, bool enabled)
        {
            return bind(handler, true, arg, enabled);
        }

        ui_handler_id_t LSPSlot::bind(ui_event_handler_t handler, bool intercept, void *arg, bool enabled)
        {
            // Check data
            if (handler == NULL)
                return - STATUS_BAD_ARGUMENTS;

            // Now try to allocate new data
            handler_item_t *item        = new handler_item_t;
            if (item == NULL)
                return - STATUS_NO_MEM;

            // Generate handler identifier
            do
            {
                item->nID   = nID;
                nID         = (nID + 1) & ID_GEN_MASK;
            } while (find_item(item->nID) != NULL);

            // Initialize item and bind it
            size_t mask         = (intercept) ? BIND_DFL : BIND_DFL | BIND_INTERCEPT;
            item->nFlags        = (enabled) ? mask | BIND_ENABLED : mask;
            item->pHandler      = handler;
            item->pPtr          = arg;
            item->pNext         = pRoot;
            pRoot               = item;

            return item->nID;
        }

        status_t LSPSlot::unbind(ui_handler_id_t id)
        {
            // Check data
            if (id < 0)
                return STATUS_BAD_ARGUMENTS;

            handler_item_t *ptr     = pRoot;
            handler_item_t *prev    = NULL;

            while (ptr != NULL)
            {
                if (ptr->nID == id)
                {
                    // Unbind handler and remove it
                    if (prev == NULL)
                        pRoot           = ptr->pNext;
                    else
                        prev->pNext     = ptr->pNext;
                    delete ptr;
                    return STATUS_OK;
                }
                prev    = ptr;
                ptr     = ptr->pNext;
            }
            return STATUS_NOT_FOUND;
        }

        ui_handler_id_t LSPSlot::unbind(ui_event_handler_t handler, void *arg)
        {
            // Check data
            if (handler == NULL)
                return - STATUS_BAD_ARGUMENTS;

            handler_item_t *ptr     = pRoot;
            handler_item_t *prev    = NULL;

            while (ptr != NULL)
            {
                if ((ptr->pHandler == handler) && (ptr->pPtr == arg))
                {
                    // Unbind handler and remove it
                    ui_handler_id_t id  = ptr->nID;
                    if (prev == NULL)
                        pRoot           = ptr->pNext;
                    else
                        prev->pNext     = ptr->pNext;
                    delete ptr;
                    return id;
                }
                prev    = ptr;
                ptr     = ptr->pNext;
            }
            return - STATUS_NOT_FOUND;
        }

        size_t LSPSlot::unbind_all()
        {
            handler_item_t *ptr     = pRoot;
            handler_item_t *next    = NULL;
            size_t removed          = 0;

            while (ptr != NULL)
            {
                next        = ptr->pNext;
                delete ptr;
                ptr         = next;
                removed     ++;
            }

            return removed;
        }

        status_t LSPSlot::disable(ui_handler_id_t id)
        {
            // Check data
            if (id < 0)
                return STATUS_BAD_ARGUMENTS;

            handler_item_t *ptr     = find_item(id);
            if (ptr == NULL)
                return STATUS_NOT_FOUND;

            ptr->nFlags            &= ~BIND_ENABLED;
            return STATUS_OK;
        }

        size_t LSPSlot::disable_all()
        {
            return disable_all(true, true);
        }

        size_t LSPSlot::disable_all_interceptors()
        {
            return disable_all(false, true);
        }

        size_t LSPSlot::disable_all_bindings()
        {
            return disable_all(true, false);
        }

        size_t LSPSlot::disable_all(bool handler, bool interceptor)
        {
            if ((!handler) && (!interceptor))
                return 0;

            handler_item_t *ptr     = pRoot;
            size_t disabled         = 0;
            size_t mask             = (handler && interceptor) ? BIND_ENABLED : BIND_ENABLED | BIND_INTERCEPT;
            size_t check            = ((!handler) && interceptor) ? BIND_INTERCEPT | BIND_ENABLED : BIND_ENABLED;

            while (ptr != NULL)
            {
                if ((ptr->nFlags & mask) == check)
                {
                    ptr->nFlags    &= ~BIND_ENABLED;
                    disabled        ++;
                }
                ptr     = ptr->pNext;
            }

            return disabled;
        }

        status_t LSPSlot::enable(ui_handler_id_t id)
        {
            // Check data
            if (id < 0)
                return STATUS_BAD_ARGUMENTS;

            handler_item_t *ptr     = find_item(id);
            if (ptr == NULL)
                return STATUS_NOT_FOUND;

            ptr->nFlags            |= BIND_ENABLED;
            return STATUS_OK;
        }

        size_t LSPSlot::enable_all(bool handler, bool interceptor)
        {
            handler_item_t *ptr     = pRoot;
            size_t enabled          = 0;
            size_t mask             = (handler && interceptor) ? BIND_ENABLED : BIND_ENABLED | BIND_INTERCEPT;
            size_t check            = ((!handler) && interceptor) ? BIND_INTERCEPT : 0;

            while (ptr != NULL)
            {
                if ((ptr->nFlags & mask) == check)
                {
                    ptr->nFlags    |= BIND_ENABLED;
                    enabled         ++;
                }
                ptr     = ptr->pNext;
            }

            return enabled;
        }

        size_t LSPSlot::enable_all()
        {
            return enable_all(true, true);
        }

        size_t LSPSlot::enable_all_bindings()
        {
            return enable_all(true, false);
        }

        size_t LSPSlot::enable_all_interceptors()
        {
            return enable_all(false, true);
        }

        status_t LSPSlot::execute(LSPWidget *sender, void *data)
        {
            // First iteration, iterate all interceptors
            for (handler_item_t *ptr = pRoot; ptr != NULL; ptr = ptr->pNext)
            {
                // Execute handler in the chain
                if ((ptr->nFlags & (BIND_ENABLED | BIND_INTERCEPT)) == (BIND_ENABLED | BIND_INTERCEPT))
                {
                    status_t result      = ptr->pHandler(sender, ptr->pPtr, data);
                    if (result != STATUS_OK)
                        return STATUS_OK;
                }
            }

            // Second iteration, iterate all handlers
            for (handler_item_t *ptr = pRoot; ptr != NULL; ptr = ptr->pNext)
            {
                // Execute handler in the chain
                if ((ptr->nFlags & (BIND_ENABLED | BIND_INTERCEPT)) == BIND_ENABLED)
                {
                    status_t result      = ptr->pHandler(sender, ptr->pPtr, data);
                    if (result != STATUS_OK)
                        return result;
                }
            }

            return STATUS_OK;
        }

    }
} /* namespace lsp */
