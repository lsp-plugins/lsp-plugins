/*
 * kvt_dispatcher.h
 *
 *  Created on: 13 июн. 2019 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_KVT_DISPATCHER_H_
#define CONTAINER_LV2_KVT_DISPATCHER_H_

#include <core/ipc/Thread.h>
#include <core/ipc/Mutex.h>
#include <core/port_data.h>
#include <core/KVTStorage.h>

namespace lsp
{
    class KVTDispatcher: public ipc::Thread
    {
        protected:
            osc_buffer_t   *pRx;
            osc_buffer_t   *pTx;
            KVTStorage     *pKVT;
            ipc::Mutex     *pKVTMutex;
            uint8_t        *pPacket;

        protected:
            size_t      receive_changes();
            size_t      transmit_changes();

        public:
            explicit KVTDispatcher(KVTStorage *kvt, ipc::Mutex *mutex);
            virtual ~KVTDispatcher();

        public:
            virtual status_t run();
    };
}


#endif /* CONTAINER_LV2_KVT_DISPATCHER_H_ */
