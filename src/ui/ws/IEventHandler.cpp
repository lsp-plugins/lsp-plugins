/*
 * IEventHandler.cpp
 *
 *  Created on: 16 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ws/ws.h>

namespace lsp
{
    namespace ws
    {
        IEventHandler::IEventHandler()
        {
        }

        IEventHandler::~IEventHandler()
        {
        }

        status_t IEventHandler::handle_event(const ws_event_t *ev)
        {
            return STATUS_OK;
        }
    } /* namespace ws */
} /* namespace lsp */
