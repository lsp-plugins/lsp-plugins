/*
 * LSPSlotSet.h
 *
 *  Created on: 12 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPSLOTSET_H_
#define UI_TK_LSPSLOTSET_H_

namespace lsp
{
    namespace tk
    {
        class LSPSlotSet
        {
            protected:
                typedef struct item_t
                {
                    ui_slot_t       nType;
                    LSPSlot        *pSlot;
                } item_t;

            protected:
                cstorage<item_t>    vSlots;

            public:
                explicit LSPSlotSet();
                ~LSPSlotSet();

            public:
                /** Get slot by identifier
                 *
                 * @param id slot identifier
                 * @return slot or NULL if not present
                 */
                LSPSlot            *slot(ui_slot_t id);

                /** Add slot
                 *
                 * @param id slot identifier
                 * @return added slot or NULL if no memory
                 */
                LSPSlot            *add(ui_slot_t id);

                /** Add slot and bind
                 *
                 * @param id slot identifier
                 * @param handler slot handler
                 * @param arg argument
                 * @param enabled enable flag
                 * @return status of operation
                 */
                ui_handler_id_t     add(ui_slot_t id, ui_event_handler_t handler, void *arg = NULL, bool enabled = true);

                /** Destroy previously allocated structures
                 *
                 */
                void                destroy();

                /** Bind slot handler to slot
                 *
                 * @param id slot identifier
                 * @param handler slot handler
                 * @param arg slot argument
                 * @param enabled enable flag
                 * @return status of operation
                 */
                ui_handler_id_t     bind(ui_slot_t id, ui_event_handler_t handler, void *arg = NULL, bool enabled = true);

                /** Intercept slot
                 *
                 * @param id slot identifier
                 * @param handler interceptor handler
                 * @param arg slot argument
                 * @param enabled enable flag
                 * @return status of operation
                 */
                ui_handler_id_t     intercept(ui_slot_t id, ui_event_handler_t handler, void *arg = NULL, bool enabled = true);

                /** Unbind slot handler
                 *
                 * @param id slot identifier
                 * @param handler slot handler identifier
                 * @return status of operation
                 */
                status_t            unbind(ui_slot_t id, ui_handler_id_t handler);

                /** Unbind slot handler
                 *
                 * @param id slot identifier
                 * @param handler slot handler routine
                 * @param arg slot argument
                 * @return status of operation
                 */
                ui_handler_id_t     unbind(ui_slot_t id, ui_event_handler_t handler, void *arg = NULL);

                /** Unbind all handlers for the slot
                 *
                 * @param id slot identifier
                 * @return number of handlers removed from bindings
                 */
                size_t              unbind_all(ui_slot_t id);

                /** Disable event handler in the slot
                 *
                 * @param id slot identifier
                 * @param handler handler identifier
                 * @return status of operation
                 */
                status_t            disable(ui_slot_t id, ui_handler_id_t handler);

                /** Disable all event handlers
                 *
                 * @param id slot identifier
                 * @return number of non-disabled handlers that were disabled
                 */
                ssize_t             disable_all(ui_slot_t id);

                /** Enable event handler in the slot
                 *
                 * @param id handler identifier
                 * @return status of operation
                 */
                status_t            enable(ui_slot_t id, ui_handler_id_t handler);

                /** Enable all event handlers for slot
                 *
                 * @param id slot identifier
                 * @return number of non-enabled handlers that were disabled
                 */
                size_t              enable_all(ui_slot_t id);

                /** Execute slot handlers
                 *
                 * @param sender the widget that initiates an event
                 * @param id slot identifier
                 * @param data data to process
                 * @return status of operation
                 */
                status_t            execute(ui_slot_t id, LSPWidget *sender, void *data = NULL);

        };

    }
} /* namespace lsp */

#endif /* UI_TK_LSPSLOTSET_H_ */
