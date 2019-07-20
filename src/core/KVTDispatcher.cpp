/*
 * KVTDispatcher.cpp
 *
 *  Created on: 13 июн. 2019 г.
 *      Author: sadko
 */

#include <core/KVTDispatcher.h>
#include <dsp/atomic.h>

namespace lsp
{
    KVTDispatcher::KVTDispatcher(KVTStorage *kvt, ipc::Mutex *mutex)
    {
        pRx         = osc_buffer_t::create(OSC_BUFFER_MAX);
        pTx         = osc_buffer_t::create(OSC_BUFFER_MAX);
        pKVT        = kvt;
        pKVTMutex   = mutex;
        pPacket     = reinterpret_cast<uint8_t *>(::malloc(OSC_PACKET_MAX));
        nClients    = 0;
        nTxRequest  = 0;
    }

    KVTDispatcher::~KVTDispatcher()
    {
        if (pRx != NULL)
        {
            osc_buffer_t::destroy(pRx);
            pRx     = NULL;
        }
        if (pTx != NULL)
        {
            osc_buffer_t::destroy(pTx);
            pTx     = NULL;
        }
        if (pPacket != NULL)
        {
            ::free(pPacket);
            pPacket = NULL;
        }
    }

    size_t  KVTDispatcher::receive_changes()
    {
        size_t size, changes = 0;

        while (true)
        {
            // Fetch the packet
            status_t res    = pRx->fetch(pPacket, &size, OSC_PACKET_MAX);

            switch (res)
            {
                case STATUS_OK:
                {
                    lsp_trace("Received OSC message (%d bytes)", int(size));
                    osc::dump_packet(pPacket, size);

                    // Analyze parsing result
                    if ((res = parse_message(pKVT, pPacket, size, KVT_RX)) != STATUS_OK)
                    {
                        // Skipped message?
                        if (res != STATUS_SKIP)
                            return changes;
                    }
                    ++changes;
                    break;
                }

                case STATUS_OVERFLOW:
                    ++changes;
                    lsp_warn("Received too big OSC packet, skipping");
                    pRx->skip();
                    break;

                case STATUS_NO_DATA:
                    return changes;

                default:
                    lsp_warn("Received error while deserializing KVT changes: %d", int(res));
                    return changes;

            }
        }
    }

    size_t  KVTDispatcher::transmit_changes()
    {
        status_t res;
        size_t changes = 0;

        KVTIterator *iter = pKVT->enum_tx_pending();
        if (iter == NULL)
            return changes;

        const kvt_param_t *p;
        const char *kvt_name;
        size_t size;

        while (iter->next() == STATUS_OK)
        {
            // Do not transfer private properties
            if (iter->is_private())
                continue;

            // Fetch next change
            res     = iter->get(&p);
            if (res == STATUS_NOT_FOUND)
                continue;
            else if (res != STATUS_OK)
                break;

            kvt_name = iter->name();
            if (kvt_name == NULL)
                continue;;

            // Try to serialize changes
            res = build_message(kvt_name, p, pPacket, &size, OSC_PACKET_MAX);
            if (res != STATUS_OK)
            {
                iter->commit(KVT_TX);
                continue;
            }

            lsp_trace("Transmitting OSC message (%d bytes)", int(size));
            osc::dump_packet(pPacket, size);

            // Submit to queue
            res = pTx->submit(pPacket, size);

            switch (res)
            {
                case STATUS_OK:
                    iter->commit(KVT_TX);
                    break;

                case STATUS_TOO_BIG: // Packet too big
                    lsp_warn("Too large packet for parameter %s: %d bytes, skipping", kvt_name, int(size));
                    iter->commit(KVT_TX);
                    break;

                case STATUS_OVERFLOW: // Not enough space to store the packet
                    return changes;

                default:
                    return changes;
            }
        }

        return changes;
    }

    status_t KVTDispatcher::run()
    {
        size_t changes;

        while (!cancelled())
        {
            changes     = 0;

            // Lock KVT storage and perform transfer
            pKVTMutex->lock();
            if (nClients > 0)
            {
                if (nTxRequest > 0)
                {
                    lsp_trace("Setting all KVT parameters as required for transfer");
                    pKVT->touch_all(KVT_TX);
                    atomic_add(&nTxRequest, -1);
                }

                changes    += receive_changes();    // Perform receive first
                changes    += transmit_changes();   // Perform transmit then
            }
            else
            {
                pTx->clear();
                pRx->clear();
            }
            pKVT->gc();                         // Perform garbage collection
            pKVTMutex->unlock();

            if (changes <= 0)
                Thread::sleep(100);     // No changes? Sleep for a while
        }

        return STATUS_OK;
    }


    status_t KVTDispatcher::submit(const void *data, size_t size)
    {
        return pRx->submit(data, size);
    }

    status_t KVTDispatcher::submit(const osc::packet_t *packet)
    {
        return pRx->submit(packet);
    }

    status_t KVTDispatcher::fetch(void *data, size_t *size, size_t limit)
    {
        return pTx->fetch(data, size, limit);
    }

    status_t KVTDispatcher::fetch(osc::packet_t *packet, size_t limit)
    {
        return pTx->fetch(packet, limit);
    }

    status_t KVTDispatcher::skip()
    {
        return pTx->skip();
    }

    void KVTDispatcher::connect_client()
    {
        atomic_add(&nClients, 1);
        atomic_add(&nTxRequest, 1);
    }

    void KVTDispatcher::disconnect_client()
    {
        if (atomic_add(&nClients, -1) == 0)
            nTxRequest  = 0;
    }

    status_t KVTDispatcher::parse_message(KVTStorage *kvt, const void *data, size_t size, size_t flags)
    {
        osc::parser_t parser;
        osc::parse_frame_t root, message;
        osc::parse_token_t token;

        const char *address;
        kvt_param_t p;
        status_t res;

        if ((res = osc::parse_begin(&root, &parser, data, size)) != STATUS_OK)
        {
            lsp_trace("Failed parse_begin()");
            return res;
        }

        if ((res = osc::parse_begin_message(&message, &root, &address)) != STATUS_OK)
        {
            lsp_trace("Failed parse_begin_message()");
            osc::parse_end(&root);
            osc::parse_destroy(&parser);
            return res;
        }

        if (::strstr(address, "/KVT/") != address) // Non-KVT destination?
        {
            lsp_trace("Prefix does not match /KVT/");
            osc::parse_end(&root);
            osc::parse_destroy(&parser);
            return STATUS_SKIP;
        }

        address += ::strlen("/KVT");
        lsp_trace("Parameter name = %s", address);

        if ((res = osc::parse_token(&message, &token)) != STATUS_OK)
        {
            lsp_trace("Could not fetch token");
            osc::parse_end(&message);
            osc::parse_end(&root);
            osc::parse_destroy(&parser);
            return res;
        }
        lsp_trace("Token type = %d", int(token));

        // Parse value
        switch (token)
        {
            case osc::PT_FLOAT32:
            {
                p.type      = KVT_FLOAT32;
                res         = osc::parse_float32(&message, &p.f32);
                break;
            }
            case osc::PT_INT32:
            {
                p.type      = KVT_INT32;
                res         = osc::parse_int32(&message, &p.i32);
                break;
            }
            case osc::PT_OSC_STRING:
            case osc::PT_NULL:
            {
                p.type      = KVT_STRING;
                osc::parse_string(&message, &p.str);
                if (res != STATUS_OK)
                    break;

                // Try to look up for blob
                if ((res = osc::parse_token(&message, &token)) != STATUS_OK)
                    break;
                if (token == osc::PT_OSC_BLOB)
                {
                    p.blob.ctype    = p.str;
                    res             = osc::parse_blob(&message, &p.blob.data, &p.blob.size);
                }
                break;
            }
            case osc::PT_INT64:
            {
                p.type      = KVT_INT64;
                res         = osc::parse_int64(&message, &p.i64);
                break;
            }
            case osc::PT_DOUBLE64:
            {
                p.type      = KVT_FLOAT64;
                res         = osc::parse_double64(&message, &p.f64);
                break;
            }
            case osc::PT_OSC_BLOB:
            {
                p.type      = KVT_BLOB;
                res         = osc::parse_blob(&message, &p.blob.data, &p.blob.size);
                if (res != STATUS_OK)
                    break;

                // Try to parse content type
                if ((res = osc::parse_token(&message, &token)) != STATUS_OK)
                    break;
                if (token == osc::PT_OSC_STRING)
                    res = osc::parse_string(&message, &p.blob.ctype);
                else if (token == osc::PT_NULL)
                {
                    res = osc::parse_skip(&message);
                    p.blob.ctype    = NULL;
                }
                break;
            }
            default:
                lsp_trace("Unknown token");
                res     = STATUS_BAD_TYPE;
                break;
        }

        // Require end of message, and only then update KVT parameter value
        if (res == STATUS_OK)
        {
            kvt_dump_parameter("Parsed parameter: %s = ", &p, address);
            if ((res = osc::parse_token(&message, &token)) == STATUS_OK)
            {
                if (token == osc::PT_EOR)
                {
                    lsp_trace("Message has been fully read, submitting to KVT");

                    // Put the change to the KVT storage with RX/TX flags set
                    // We can freely use the address pointer while the pPacket is valid
                    res = kvt->put(address, &p, flags);
                }
            }
        }

        // Finalize the message parser
        osc::parse_end(&message);
        osc::parse_end(&root);
        osc::parse_destroy(&parser);

        return res;
    }

    status_t KVTDispatcher::parse_message(KVTStorage *kvt, const osc::packet_t *packet, size_t flags)
    {
        return parse_message(kvt, packet->data, packet->size, flags);
    }


    status_t KVTDispatcher::build_message(const char *kvt_name, const kvt_param_t *p, void *data, size_t *size, size_t limit)
    {
        status_t res;
        osc::forge_t forge;
        osc::forge_frame_t root, message;
        osc::packet_t packet;

        // Serialize message
        res = osc::forge_begin_fixed(&root, &forge, data, limit);
        if (res != STATUS_OK)
            return res;

        res = osc::forge_begin_message(&message, &root, "/KVT", kvt_name);
        if (res != STATUS_OK)
        {
            osc::forge_end(&root);
            osc::forge_close(&packet, &forge);
            osc::forge_destroy(&forge);
            return res;
        }

        switch (p->type)
        {
            case KVT_INT32:
                res = osc::forge_int32(&message, p->i32);
                break;
            case KVT_UINT32:
                res = osc::forge_int32(&message, p->u32);
                break;
            case KVT_INT64:
                res = osc::forge_int64(&message, p->i64);
                break;
            case KVT_UINT64:
                res = osc::forge_int64(&message, p->u64);
                break;
            case KVT_FLOAT32:
                res = osc::forge_float32(&message, p->f32);
                break;
            case KVT_FLOAT64:
                res = osc::forge_double64(&message, p->f64);
                break;
            case KVT_STRING:
                res = osc::forge_string(&message, p->str);
                break;
            case KVT_BLOB:
                res = osc::forge_string(&message, p->blob.ctype);
                if (res == STATUS_OK)
                    res = osc::forge_blob(&message, p->blob.data, p->blob.size);
                break;
            default:
                res = STATUS_BAD_TYPE;
                break;
        }

        osc::forge_end(&message);
        osc::forge_end(&root);
        osc::forge_close(&packet, &forge);
        osc::forge_destroy(&forge);

        *size   = packet.size;
        return res;
    }

    status_t KVTDispatcher::build_message(const char *param_name, const kvt_param_t *param, osc::packet_t *packet, size_t limit)
    {
        return build_message(param_name, param, &packet->data, &packet->size, limit);
    }
}
