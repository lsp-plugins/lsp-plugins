/*
 * KVTDispatcher.cpp
 *
 *  Created on: 13 июн. 2019 г.
 *      Author: sadko
 */

#include <core/KVTDispatcher.h>

namespace lsp
{
    KVTDispathcer::KVTDispathcer(KVTStorage *kvt, ipc::Mutex *mutex)
    {
        pRx         = osc_buffer_t::create(OSC_BUFFER_MAX);
        pTx         = osc_buffer_t::create(OSC_BUFFER_MAX);
        pKVT        = kvt;
        pKVTMutex   = mutex;
        pPacket     = reinterpret_cast<uint8_t *>(::malloc(OSC_PACKET_MAX));
    }

    KVTDispathcer::~KVTDispathcer()
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

    size_t  KVTDispathcer::receive_changes()
    {
        size_t size, changes = 0;
        const char *address;

        osc::parser_t parser;
        osc::parser_frame_t root, message;
        osc::parse_token_t token;
        kvt_param_t p;

        while (true)
        {
            // Fetch the packet
            status_t res    = pRx->fetch(pPacket, &size, OSC_PACKET_MAX);

            switch (res)
            {
                case STATUS_OK:
                {
                    if ((res = osc::parse_begin(&root, &parser, pPacket, size)) != STATUS_OK)
                        return changes;

                    if ((res = osc::parse_begin_message(&message, &root, &address)) != STATUS_OK)
                    {
                        osc::parse_end(&root);
                        osc::parse_destroy(&parser);
                        return changes;
                    }

                    if (::strstr(address, "/KVT/") != address) // Non-KVT destination?
                    {
                        pRx->skip();
                        break;
                    }
                    else
                        address += ::strlen("/KVT");

                    ++changes;

                    if ((res = osc::parse_token(&message, &token)) != STATUS_OK)
                    {
                        osc::parse_end(&message);
                        osc::parse_end(&root);
                        osc::parse_destroy(&parser);
                        return changes;
                    }

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
                        }
                        default:
                            res     = STATUS_BAD_TYPE;
                            break;
                    }

                    // Require end of message, and only then update KVT parameter value
                    if (res == STATUS_OK)
                    {
                        if ((res = osc::parse_token(&message, &token)) == STATUS_OK)
                        {
                            if (token == osc::PT_EOR)
                            {
                                // Put the change to the KVT storage with RX flags set
                                // We can freely use the address pointer while the pPacket is valid
                                res = pKVT->put(address, &p, KVT_RX);
                                if (res == STATUS_OK)
                                    kvt_dump_parameter("RX kvt param (UI->DSP): %s = ", &p, address);
                            }
                        }
                    }

                    // Finalize the message parser
                    osc::parse_end(&message);
                    osc::parse_end(&root);
                    osc::parse_destroy(&parser);

                    // Analyze parsing result
                    if (res != STATUS_OK)
                        return changes;
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

    size_t  KVTDispathcer::transmit_changes()
    {
        size_t changes = 0;

        KVTIterator *iter = pKVT->enum_tx_pending();
        if (iter == NULL)
            return changes;

        osc::forge_t forge;
        osc::forge_frame_t root, message;
        osc::packet_t packet;

        const kvt_param_t *p;
        const char *kvt_name;
        status_t res;

        while (iter->next() == STATUS_OK)
        {
            // Fetch next change
            res = iter->get(&p);
            kvt_name = iter->name();
            if (res != STATUS_OK)
                break;

            // Serialize message
            res = osc::forge_begin_fixed(&root, &forge, pPacket, OSC_PACKET_MAX);
            if (res != STATUS_OK)
                break;

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

            // Success?
            if (res != STATUS_OK)
            {
                if (res == STATUS_BAD_TYPE)
                {
                    iter->commit(KVT_TX);
                    continue;
                }
                else
                    return changes;
            }

            // Submit to queue
            res = pTx->submit(&packet);

            switch (res)
            {
                case STATUS_OK:
                    iter->commit(KVT_TX);
                    break;

                case STATUS_TOO_BIG: // Packet too big
                    lsp_warn("Too large packet for parameter %s: %d bytes, skipping", kvt_name, int(packet.size));
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

    status_t KVTDispathcer::run()
    {
        size_t changes;

        while (!cancelled())
        {
            changes     = 0;

            // Lock KVT storage and perform transfer
            pKVTMutex->lock();
            changes    += receive_changes();    // Perform receive first
            changes    += transmit_changes();   // Perform transmit then
            pKVT->gc();                         // Perform garbage collection
            pKVTMutex->unlock();

            if (changes <= 0)
                Thread::sleep(100);     // No changes? Sleep for a while
        }

        return STATUS_OK;
    }


}
