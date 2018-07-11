/*
 * LSPSlotSet.cpp
 *
 *  Created on: 12 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        LSPSlotSet::LSPSlotSet()
        {
        }

        LSPSlotSet::~LSPSlotSet()
        {
            destroy();
        }

        void LSPSlotSet::destroy()
        {
            size_t count        = vSlots.size();
            for (size_t i=0; i<count; ++i)
            {
                item_t *ptr     = vSlots.at(i);
                if (ptr->pSlot != NULL)
                {
                    delete ptr->pSlot;
                    ptr->pSlot      = NULL;
                }
            }
            vSlots.flush();
        }

        LSPSlot *LSPSlotSet::slot(ui_slot_t id)
        {
            ssize_t first   = 0, last = ssize_t(vSlots.size()) - 1;

            while (first <= last)
            {
                ssize_t idx     = (first + last) >> 1;
                item_t *ptr     = vSlots.at(idx);
                if (ptr->nType == id)
                    return ptr->pSlot;
                else if (ptr->nType < id)
                    first       = idx + 1;
                else
                    last        = idx - 1;
            }

            return NULL;
        }

        LSPSlot *LSPSlotSet::add(ui_slot_t id)
        {
            // Find position to insert slot
            ssize_t first   = 0, last = ssize_t(vSlots.size()) - 1, idx = 0;
            while (first <= last)
            {
                idx             = (first + last) >> 1;
                item_t *ptr     = vSlots.at(idx);
                if (ptr->nType == id)
                    return ptr->pSlot;
                else if (ptr->nType < id)
                    first       = ++idx;
                else
                    last        = --idx;
            }

            // Now allocate new slot
            LSPSlot *result      = new LSPSlot();
            if (result == NULL)
                return result;

            // Insert slot to the found position
            item_t *ptr         = vSlots.insert(first);
            if (ptr == NULL)
            {
                delete result;
                return NULL;
            }

            // Store slot
            ptr->nType          = id;
            ptr->pSlot          = result;

//            #ifdef LSP_TRACE
//            size_t n = vSlots.size();
//            for (size_t i=0; i<n; ++i)
//            {
//                ptr     = vSlots.at(i);
//                lsp_trace("slot idx=%d, id=%d", int(i), int (ptr->nType));
//            }
//            #endif /* LSP_TRACE */

            return result;
        }
    
        ui_handler_id_t LSPSlotSet::add(ui_slot_t id, ui_event_handler_t handler, void *arg, bool enabled)
        {
            // Check data
            if (handler == NULL)
                return - STATUS_BAD_ARGUMENTS;

            // Find position to insert slot
            ssize_t first   = 0, last = ssize_t(vSlots.size()) - 1, idx = 0;
            while (first <= last)
            {
                idx             = (first + last) >> 1;
                item_t *ptr     = vSlots.at(idx);
                if (ptr->nType == id)
                    return ptr->pSlot->bind(handler, arg, enabled);
                else if (ptr->nType < id)
                    first       = ++idx;
                else
                    last        = --idx;
            }

            // Now allocate new slot
            LSPSlot *result      = new LSPSlot();
            if (result == NULL)
                return - STATUS_NO_MEM;

            // Bind data to slot
            ui_handler_id_t hid     = result->bind(handler, arg, enabled);
            if (hid < 0)
            {
                delete result;
                return hid;
            }

            // Insert slot to the found position
            item_t *ptr         = vSlots.insert(first);
            if (ptr == NULL)
            {
                delete result;
                return - STATUS_NO_MEM;
            }

            // Store slot
            ptr->nType          = id;
            ptr->pSlot          = result;

            return hid;
        }

        ui_handler_id_t LSPSlotSet::bind(ui_slot_t id, ui_event_handler_t handler, void *arg, bool enabled)
        {
            LSPSlot *s       = slot(id);
            return (s == NULL) ? - STATUS_NOT_FOUND : s->bind(handler, arg, enabled);
        }

        ui_handler_id_t LSPSlotSet::intercept(ui_slot_t id, ui_event_handler_t handler, void *arg, bool enabled)
        {
            LSPSlot *s       = slot(id);
            return (s == NULL) ? - STATUS_NOT_FOUND : s->intercept(handler, arg, enabled);
        }

        status_t LSPSlotSet::unbind(ui_slot_t id, ui_handler_id_t handler)
        {
            LSPSlot *s       = slot(id);
            return (s == NULL) ? STATUS_NOT_FOUND : s->unbind(handler);
        }

        ui_handler_id_t LSPSlotSet::unbind(ui_slot_t id, ui_event_handler_t handler, void *arg)
        {
            LSPSlot *s       = slot(id);
            return (s == NULL) ? - STATUS_NOT_FOUND : s->unbind(handler, arg);
        }

        size_t LSPSlotSet::unbind_all(ui_slot_t id)
        {
            LSPSlot *s       = slot(id);
            return (s == NULL) ? 0 : s->unbind_all();
        }

        status_t LSPSlotSet::disable(ui_slot_t id, ui_handler_id_t handler)
        {
            LSPSlot *s       = slot(id);
            return (s == NULL) ? STATUS_NOT_FOUND : s->disable(handler);
        }

        ssize_t LSPSlotSet::disable_all(ui_slot_t id)
        {
            LSPSlot *s       = slot(id);
            return (s == NULL) ? 0 : s->disable_all();
        }

        status_t LSPSlotSet::enable(ui_slot_t id, ui_handler_id_t handler)
        {
            LSPSlot *s       = slot(id);
            return (s == NULL) ? STATUS_NOT_FOUND : s->enable(handler);
        }

        size_t LSPSlotSet::enable_all(ui_slot_t id)
        {
            LSPSlot *s       = slot(id);
            return (s == NULL) ? 0 : s->enable_all();
        }

        status_t LSPSlotSet::execute(ui_slot_t id, LSPWidget *sender, void *data)
        {
            LSPSlot *s       = slot(id);
            return (s == NULL) ? STATUS_NOT_FOUND : s->execute(sender, data);
        }
    }

} /* namespace lsp */
