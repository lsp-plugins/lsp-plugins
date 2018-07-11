/*
 * IEventHandler.h
 *
 *  Created on: 16 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_WS_IEVENTHANDLER_H_
#define UI_WS_IEVENTHANDLER_H_

namespace lsp
{
    namespace ws
    {
        /** Windowing system's event handler
         *
         */
        class IEventHandler
        {
            public:
                IEventHandler();
                virtual ~IEventHandler();

            public:
                virtual status_t handle_event(const ws_event_t *ev);
        };
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_WS_IEVENTHANDLER_H_ */
