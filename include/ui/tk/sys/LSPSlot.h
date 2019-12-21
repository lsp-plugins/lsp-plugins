/*
 * LSPSlot.h
 *
 *  Created on: 12 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPSLOT_H_
#define UI_TK_LSPSLOT_H_

namespace lsp
{
    namespace tk
    {
        class LSPWidget;

        /** Event handler type
         *
         * @param sender the widget that initiated an event
         * @param ptr additional pointer passed as an argument to the bind()
         * @param data data structure to process (handle)
         * @return status of operation
         */
        typedef status_t (* ui_event_handler_t)(LSPWidget *sender, void *ptr, void *data);

        class LSPSlot
        {
            protected:
                enum bind_flags_t
                {
                    BIND_DFL            = 0,
                    BIND_ENABLED        = 1 << 0,
                    BIND_INTERCEPT      = 1 << 1
                };

                typedef struct handler_item_t
                {
                    ui_handler_id_t     nID;        // Identifier of handler
                    size_t              nFlags;     // Additional flags
                    ui_event_handler_t  pHandler;   // Handler
                    void               *pPtr;       // Additional argument to pass
                    handler_item_t     *pNext;      // Pointer to the next item
                } handler_item_t;

            protected:
                handler_item_t         *pRoot;      // Pointer to the first item
                ui_handler_id_t         nID;        // ID generator

            protected:
                inline handler_item_t  *find_item(ui_handler_id_t id);
                ui_handler_id_t bind(ui_event_handler_t handler, bool intercept, void *arg, bool enabled);
                size_t disable_all(bool handler, bool interceptor);
                size_t enable_all(bool handler, bool interceptor);

            public:
                explicit LSPSlot();
                ~LSPSlot();

            public:
                /** Bind slot
                 *
                 * @param handler event handler routine
                 * @param arg argument
                 * @param enabled binding is enabled
                 * @return identifier of handler or negative status code
                 */
                ui_handler_id_t bind(ui_event_handler_t handler, void *arg = NULL, bool enabled = true);

                /** Intercept slot
                 *
                 * @param handler interceptor handler routine
                 * @param arg argument
                 * @param enabled interceptor is enabled
                 * @return identifier of interceptor or negative status code
                 */
                ui_handler_id_t intercept(ui_event_handler_t handler, void *arg = NULL, bool enabled = true);

                /** Unbind handler or interceptor by identifier
                 *
                 * @param id handler identifier
                 * @return status of operation
                 */
                status_t unbind(ui_handler_id_t id);

                /** Unbind handler or interceptor by contents. Removes first occured binding
                 *
                 * @param handler event handler routine
                 * @param arg argument
                 * @return identifier of removed handler on success, negative status code on error
                 */
                ui_handler_id_t unbind(ui_event_handler_t handler, void *arg = NULL);

                /** Unbind all handlers and interceptors for the slot
                 *
                 * @return number of handlers removed from bindings
                 */
                size_t unbind_all();

                /** Disable event handler or interceptor for the slot
                 *
                 * @param id handler identifier
                 * @return status of operation
                 */
                status_t disable(ui_handler_id_t id);

                /** Disable all event handlers or interceptors
                 *
                 * @return number of non-disabled handlers that were disabled
                 */
                size_t disable_all();

                /** Disable all event handlers only
                 *
                 * @return number of non-disabled handlers that were disabled
                 */
                size_t disable_all_bindings();

                /** Disable all event interceptors only
                 *
                 * @return number of non-disabled handlers that were disabled
                 */
                size_t disable_all_interceptors();

                /** Enable event handler in the slot
                 *
                 * @param id handler identifier
                 * @return status of operation
                 */
                status_t enable(ui_handler_id_t id);

                /** Enable all event handlers and interceptors
                 *
                 * @return number of non-enabled handlers that were disabled
                 */
                size_t enable_all();

                /** Enable all interceptors
                 *
                 * @return number of non-enabled interceptors that were enabled
                 */
                size_t enable_all_interceptors();

                /** Enable all bindings
                 *
                 * @return number of non-enabled handlers that were enabled
                 */
                size_t enable_all_bindings();
    
                /** Execute slot handlers
                 *
                 * @param sender the object that initiated event
                 * @param data data to process
                 * @return status of operation
                 */
                status_t execute(LSPWidget *sender, void *data);
        };
    }
} /* namespace lsp */

#endif /* UI_TK_LSPSLOT_H_ */
