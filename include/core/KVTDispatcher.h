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
            osc_buffer_t       *pRx;
            osc_buffer_t       *pTx;
            KVTStorage         *pKVT;
            ipc::Mutex         *pKVTMutex;
            uint8_t            *pPacket;
            volatile atomic_t   nClients;
            volatile atomic_t   nTxRequest;

        protected:
            size_t      receive_changes();
            size_t      transmit_changes();

        public:
            explicit KVTDispatcher(KVTStorage *kvt, ipc::Mutex *mutex);
            virtual ~KVTDispatcher();

        public:
            virtual status_t run();

            status_t submit(const void *data, size_t size);
            status_t submit(const osc::packet_t *packet);

            status_t fetch(void *data, size_t *size, size_t limit);
            status_t fetch(osc::packet_t *packet, size_t limit);
            status_t skip();

            void     connect_client();
            void     disconnect_client();

            inline size_t rx_size() const { return pRx->size(); }
            inline size_t tx_size() const { return pTx->size(); }

            static status_t parse_message(KVTStorage *kvt, const void *data, size_t size, size_t flags);
            static status_t parse_message(KVTStorage *kvt, const osc::packet_t *packet, size_t flags);

            static status_t build_message(const char *param_name, const kvt_param_t *param, osc::packet_t *packet, size_t limit);
            static status_t build_message(const char *param_name, const kvt_param_t *param, void *data, size_t *size, size_t limit);
    };
}


#endif /* CONTAINER_LV2_KVT_DISPATCHER_H_ */
