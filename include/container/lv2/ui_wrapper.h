/*
 * wrapper.h
 *
 *  Created on: 12 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_UI_WRAPPER_H_
#define CONTAINER_LV2_UI_WRAPPER_H_

namespace lsp
{
    class LV2UIWrapper: public IUIWrapper
    {
        private:
            cvector<LV2UIPort>      vExtPorts;
            cvector<LV2UIPort>      vMeshPorts;
            cvector<LV2UIPort>      vFrameBufferPorts;
            cvector<LV2UIPort>      vUIPorts;
            cvector<LV2UIPort>      vAllPorts;  // List of all created ports, for garbage collection
            cvector<LV2UIPort>      vOscInPorts;
            cvector<LV2UIPort>      vOscOutPorts;
            cvector<port_t>         vGenMetadata;   // Generated metadata

            position_t              sPosition;

            plugin_ui              *pUI;
            LV2Extensions          *pExt;
            size_t                  nLatencyID; // ID of latency port
            LV2UIPort              *pLatency;
            bool                    bConnected;
            KVTStorage              sKVT;
            ipc::Mutex              sKVTMutex;
            uint8_t                *pOscBuffer;     // OSC packet data

        protected:
            LV2UIPort *create_port(const port_t *p, const char *postfix);
            void create_ports(const port_t *port);

            void receive_atom(const LV2_Atom_Object * atom);
            static LV2UIPort *find_by_urid(cvector<LV2UIPort> &v, LV2_URID urid);
            void sort_by_urid(cvector<LV2UIPort> &v);

            static status_t slot_ui_hide(LSPWidget *sender, void *ptr, void *data);
            static status_t slot_ui_show(LSPWidget *sender, void *ptr, void *data);

        public:
            inline explicit LV2UIWrapper(plugin_ui *ui, LV2Extensions *ext)
            {
                pUI         = ui;
                pExt        = ext;
                nLatencyID  = 0;
                pLatency    = NULL;
                bConnected  = false;
                pOscBuffer  = NULL;

                position_t::init(&sPosition);
            }

            virtual ~LV2UIWrapper()
            {
                pUI         = NULL;
                pExt        = NULL;
                nLatencyID  = 0;
                pLatency    = NULL;
                bConnected  = false;

            }

        public:
            void init()
            {
                // Get plugin metadata
                const plugin_metadata_t *m  = pUI->metadata();

                // Create OSC packet buffer
                pOscBuffer      = reinterpret_cast<uint8_t *>(::malloc(OSC_PACKET_MAX + sizeof(LV2_Atom)));

                // Perform all port bindings
                create_ports(m->ports);

                // Sort plugin ports
                sort_by_urid(vUIPorts);
                sort_by_urid(vMeshPorts);
                sort_by_urid(vFrameBufferPorts);

                // Create atom transport
                if (pExt->atom_supported())
                    pExt->ui_create_atom_transport(vExtPorts.size(), lv2_all_port_sizes(m->ports, true, false));

                // Add stub for latency reporting
                {
                    const port_t *port = &lv2_latency_port;
                    if ((port->id != NULL) && (port->name != NULL))
                    {
                        pLatency = new LV2UIFloatPort(port, pExt, NULL);
                        vAllPorts.add(pLatency);
                        pUI->add_port(pLatency);
                        nLatencyID  = vExtPorts.size();
                        if (pExt->atom_supported())
                            nLatencyID  += 2;           // Skip AtomIn, AtomOut
                    }
                }

                // Initialize plugin
                lsp_trace("Initializing UI");
                status_t res = pUI->init(this, 0, NULL);
                if (res == STATUS_OK)
                    res = pUI->build();

                // Initialize size of root window
                size_request_t sr;
                LSPWindow *root = pUI->root_window();
                if (root == NULL)
                    return;

                root->slots()->bind(LSPSLOT_SHOW, slot_ui_show, this);
                root->slots()->bind(LSPSLOT_HIDE, slot_ui_hide, this);

                // Sync state of UI ports with the UI
                for (size_t i=0, n=vUIPorts.size(); i<n; ++i)
                {
                    LV2UIPort *p = vUIPorts.at(i);
                    if (p != NULL)
                        p->notify_all();
                }

                // Resize UI and show
                root->size_request(&sr);
                root->resize(sr.nMinWidth, sr.nMinHeight);
                pExt->resize_ui(sr.nMinWidth, sr.nMinHeight);

                pUI->show();
            }

            int resize_ui(ssize_t width, ssize_t height)
            {
                LSPWindow *root = (pUI != NULL) ? pUI->root_window() : NULL;
                if (root == NULL)
                    return 0;

                // Resize UI and show
                lsp_trace("width=%d, height=%d", int(width), int(height));
                size_request_t sr;
                root->size_request(&sr);

                // Apply size constraints
                if ((sr.nMaxWidth >= 0) && (width > sr.nMaxWidth))
                    width = sr.nMaxWidth;
                if ((sr.nMaxHeight >= 0) && (height > sr.nMaxHeight))
                    height = sr.nMaxHeight;

                if ((sr.nMinWidth >= 0) && (width < sr.nMinWidth))
                    width = sr.nMinWidth;
                if ((sr.nMinHeight >= 0) && (height < sr.nMinHeight))
                    height = sr.nMinHeight;

                // Perform resize
                root->resize(width, height);
                return 0;
            }

            void ui_activated()
            {
                if (bConnected)
                    return;

                lsp_trace("UI has been activated");
                if (pExt != NULL)
                {
                    LV2Wrapper *w = pExt->wrapper();
                    if (w != NULL)
                    {
                        lsp_trace("Connecting directly to plugin");
                        w->connect_direct_ui();
                    }
                    else
                        pExt->ui_connect_to_plugin();
                    bConnected = true;
                }
            }

            virtual KVTStorage *kvt_lock();

            virtual KVTStorage *kvt_trylock();

            virtual bool kvt_release();

            void parse_raw_osc_event(osc::parse_frame_t *frame);

            void ui_deactivated()
            {
                if (!bConnected)
                    return;

                lsp_trace("UI has been deactivated");
                if (pExt != NULL)
                {
                    LV2Wrapper *w = pExt->wrapper();
                    if (w != NULL)
                    {
                        lsp_trace("Disconnecting directly from plugin");
                        w->disconnect_direct_ui();
                    }
                    else
                        pExt->ui_disconnect_from_plugin();
                    bConnected = false;
                }
            }

            void destroy()
            {
                // Disconnect UI
                ui_deactivated();

                // Drop plugin UI
                if (pUI != NULL)
                {
                    pUI->destroy();
                    delete pUI;

                    pUI         = NULL;
                }

                // Cleanup ports
                for (size_t i=0; i<vAllPorts.size(); ++i)
                {
                    lsp_trace("destroy ui port %s", vAllPorts[i]->metadata()->id);
                    delete vAllPorts[i];
                }
                pLatency        = NULL;

                // Cleanup generated metadata
                for (size_t i=0; i<vGenMetadata.size(); ++i)
                {
                    lsp_trace("destroy generated port metadata %p", vGenMetadata[i]);
                    drop_port_metadata(vGenMetadata[i]);
                }

                vAllPorts.clear();
                vExtPorts.clear();
                vMeshPorts.clear();
                vFrameBufferPorts.clear();

                if (pOscBuffer != NULL)
                {
                    ::free(pOscBuffer);
                    pOscBuffer = NULL;
                }

                // Drop extensions
                if (pExt != NULL)
                {
                    delete pExt;
                    pExt        = NULL;
                }
            }

            void notify(size_t id, size_t size, size_t format, const void *buf)
            {
                if (id < vExtPorts.size())
                {
                    LV2UIPort *p = vExtPorts[id];
    //                lsp_trace("id=%d, size=%d, format=%d, buf=%p, port_id=%s", int(id), int(size), int(format), buf, p->metadata()->id);
                    if (p != NULL)
                    {
//                        lsp_trace("notify id=%d, size=%d, format=%d, buf=%p value=%f",
//                            int(id), int(size), int(format), buf, *(reinterpret_cast<const float *>(buf)));

                        p->notify(buf, format, size);
                        p->notify_all();
                    }
                }
                else if ((pExt->nAtomIn >= 0) && (id == size_t(pExt->nAtomIn)))
                {
                    if (format != pExt->uridEventTransfer)
                        return;

                    // Check that event is an object
                    const LV2_Atom* atom = reinterpret_cast<const LV2_Atom*>(buf);
//                    lsp_trace("atom.type = %d (%s)", int(atom->type), pExt->unmap_urid(atom->type));

                    if ((atom->type == pExt->uridObject) || (atom->type == pExt->uridBlank))
                        receive_atom(reinterpret_cast<const LV2_Atom_Object *>(atom));
                    else if (atom->type == pExt->uridOscRawPacket)
                    {
                        osc::parser_t parser;
                        osc::parser_frame_t root;
                        status_t res = osc::parse_begin(&root, &parser, &atom[1], atom->size);
                        if (res == STATUS_OK)
                        {
                            parse_raw_osc_event(&root);
                            osc::parse_end(&root);
                            osc::parse_destroy(&parser);
                        }
                    }
                }
                else if (id == nLatencyID)
                {
                    if (pLatency != NULL)
                        pLatency->notify(buf, format, size);
                }
            }

            void send_kvt_state()
            {
                KVTIterator *iter = sKVT.enum_rx_pending();
                if (iter == NULL)
                    return;

                const kvt_param_t *p;
                const char *kvt_name;
                size_t size;
                status_t res;

                while (iter->next() == STATUS_OK)
                {
                    // Fetch next change
                    res = iter->get(&p);
                    kvt_name = iter->name();
                    if ((res != STATUS_OK) || (kvt_name == NULL))
                        break;

                    // Try to serialize changes
                    res = KVTDispatcher::build_message(kvt_name, p, &pOscBuffer[sizeof(LV2_Atom)], &size, OSC_PACKET_MAX);
                    if (res == STATUS_OK)
                    {
                        KVTDispatcher *d = (pExt->wrapper() != NULL) ? pExt->wrapper()->kvt_dispatcher() : NULL;

                        // Forge raw OSC message as an atom message
                        if (d != NULL)
                        {
                            lsp_trace("Submitting OSC message");
                            osc::dump_packet(&pOscBuffer[sizeof(LV2_Atom)], size);
                            d->submit(&pOscBuffer[sizeof(LV2_Atom)], size); // Submit directly to the KVT dispatcher
                        }
                        else
                        {
                            lsp_trace("Sending OSC message");
                            osc::dump_packet(&pOscBuffer[sizeof(LV2_Atom)], size);

                            // Transmit message via atom interface
                            LV2_Atom *atom  = reinterpret_cast<LV2_Atom *>(pOscBuffer);
                            atom->size      = size;
                            atom->type      = pExt->uridOscRawPacket;
                            size            = (size + sizeof(LV2_Atom) + sizeof(uint64_t) - 1) & ~(sizeof(uint64_t) - 1); // padding

                            // Submit message to the atom output port
                            pExt->write_data(pExt->nAtomOut, size, pExt->uridEventTransfer, pOscBuffer);
                        }
                    }

                    // Commit transfer
                    iter->commit(KVT_RX);
                }
            }

            void receive_kvt_state()
            {
                LV2Wrapper *w = pExt->wrapper();
                if (w == NULL)
                    return;

                // Obtain the dispatcher
                KVTDispatcher *d = (pExt->wrapper() != NULL) ? pExt->wrapper()->kvt_dispatcher() : NULL;
                if (d == NULL)
                    return;
                if (d->tx_size() <= 0) // Is there data for transfer?
                    return;

                KVTStorage *skvt = w->kvt_trylock();
                if (skvt == NULL)
                    return;

                size_t size;
                if (sKVTMutex.lock())
                {
                    status_t res;

                    do
                    {
                        // Try to fetch record from buffer
                        res = d->fetch(pOscBuffer, &size, OSC_PACKET_MAX);

                        switch (res)
                        {
                            case STATUS_OK:
                            {
                                lsp_trace("Fetched OSC packet of %d bytes", int(size));
                                osc::dump_packet(pOscBuffer, size);
                                KVTDispatcher::parse_message(&sKVT, pOscBuffer, size, KVT_TX);
                                break;
                            }

                            case STATUS_NO_DATA: // No more data to transmit
                                break;

                            case STATUS_OVERFLOW:
                            {
                                lsp_warn("Too large OSC packet in the buffer, skipping");
                                d->skip();
                                break;
                            }

                            default:
                            {
                                lsp_warn("OSC packet parsing error %d, skipping", int(res));
                                d->skip();
                                break;
                            }
                        }
                    } while (res != STATUS_NO_DATA);

                    sKVTMutex.unlock();
                }
                w->kvt_release();
            }

            void sync_kvt_state()
            {
                // Synchronize DSP -> UI transfer
                size_t sync;
                const char *kvt_name;
                const kvt_param_t *kvt_value;

                do
                {
                    sync = 0;

                    KVTIterator *it = sKVT.enum_tx_pending();
                    while (it->next() == STATUS_OK)
                    {
                        kvt_name = it->name();
                        if (kvt_name == NULL)
                            break;
                        status_t res = it->get(&kvt_value);
                        if (res != STATUS_OK)
                            break;
                        if ((res = it->commit(KVT_TX)) != STATUS_OK)
                            break;

                        kvt_dump_parameter("TX kvt param (DSP->UI): %s = ", kvt_value, kvt_name);
                        pUI->kvt_write(&sKVT, kvt_name, kvt_value);
                        ++sync;
                    }
                } while (sync > 0);

                // Synchronize UI -> DSP transfer
                #ifdef LSP_DEBUG
                {
                    KVTIterator *it = sKVT.enum_rx_pending();
                    while (it->next() == STATUS_OK)
                    {
                        kvt_name = it->name();
                        if (kvt_name == NULL)
                            break;
                        status_t res = it->get(&kvt_value);
                        if (res != STATUS_OK)
                            break;
                        if ((res = it->commit(KVT_RX)) != STATUS_OK)
                            break;

                        kvt_dump_parameter("RX kvt param (UI->DSP): %s = ", kvt_value, kvt_name);
                    }
                }
                #else
                    sKVT.commit_all(KVT_RX);    // Just clear all RX queue for non-debug version
                #endif
            }

            int idle()
            {
                if (pUI == NULL)
                    return -1;

                dsp::context_t ctx;
                dsp::start(&ctx);

                // Synchronize port states avoiding LV2 Atom transport
                LV2Wrapper *w = pExt->wrapper();
                if (w != NULL)
                {
                    for (size_t i=0, n=vAllPorts.size(); i<n; ++i)
                    {
                        LV2UIPort *p = vAllPorts.at(i);
                        if (p == NULL)
                            continue;
                        if (p->sync())
                            p->notify_all();
                    }

                    // Check that sample rate has changed
                    position_t pos      = *(w->position());
                    pUI->position_updated(&pos);
                    sPosition           = pos;
                }

                // Transmit KVT state
                if (sKVTMutex.try_lock())
                {
                    receive_kvt_state();
                    send_kvt_state();
                    sync_kvt_state();
                    sKVT.gc();
                    sKVTMutex.unlock();
                }

                // Call UI to process events
                pUI->sync_meta_ports();
                pUI->main_iteration();
                dsp::finish(&ctx);

                return 0;
            }
    };

    status_t LV2UIWrapper::slot_ui_show(LSPWidget *sender, void *ptr, void *data)
    {
        LV2UIWrapper *_this = static_cast<LV2UIWrapper *>(ptr);
        _this->ui_activated();
        return STATUS_OK;
    }

    status_t LV2UIWrapper::slot_ui_hide(LSPWidget *sender, void *ptr, void *data)
    {
        LV2UIWrapper *_this = static_cast<LV2UIWrapper *>(ptr);
        _this->ui_deactivated();
        return STATUS_OK;
    }

    LV2UIPort *LV2UIWrapper::create_port(const port_t *p, const char *postfix)
    {
        LV2UIPort *result = NULL;
        LV2Wrapper *w = pExt->wrapper();

        switch (p->role)
        {
            case R_MIDI: // Skip all MIDI ports
                break;
            case R_AUDIO: // Stub ports
                result = new LV2UIPort(p, pExt);
                break;
            case R_CONTROL:
                result = new LV2UIFloatPort(p, pExt, (w != NULL) ? w->get_port(p->id) : NULL);
                break;
            case R_BYPASS:
                result = new LV2UIBypassPort(p, pExt, (w != NULL) ? w->get_port(p->id) : NULL);
                break;
            case R_METER:
                result = new LV2UIPeakPort(p, pExt, (w != NULL) ? w->get_port(p->id) : NULL);
                break;
            case R_PATH:
                if (pExt->atom_supported())
                    result = new LV2UIPathPort(p, pExt, (w != NULL) ? w->get_port(p->id) : NULL);
                else
                    result = new LV2UIPort(p, pExt); // Stub port
                break;
            case R_MESH:
                if (pExt->atom_supported())
                {
                    result = new LV2UIMeshPort(p, pExt, (w != NULL) ? w->get_port(p->id) : NULL);
                    vMeshPorts.add(result);
                }
                else // Stub port
                    result = new LV2UIPort(p, pExt);
                break;
            case R_FBUFFER:
                if (pExt->atom_supported())
                {
                    result = new LV2UIFrameBufferPort(p, pExt, (w != NULL) ? w->get_port(p->id) : NULL);
                    vFrameBufferPorts.add(result);
                }
                else // Stub port
                    result = new LV2UIPort(p, pExt);
                break;
            case R_PORT_SET:
            {
                char postfix_buf[LSP_MAX_PARAM_ID_BYTES];
                LV2UIPortGroup   *pg    = new LV2UIPortGroup(p, pExt, (w != NULL) ? w->get_port(p->id) : NULL);
                pUI->add_port(pg);
                vUIPorts.add(pg);

                for (size_t row=0; row<pg->rows(); ++row)
                {
                    // Generate postfix
                    snprintf(postfix_buf, sizeof(postfix_buf)-1, "%s_%d", (postfix != NULL) ? postfix : "", int(row));

                    // Clone port metadata
                    port_t *cm          = clone_port_metadata(p->members, postfix_buf);
                    if (cm == NULL)
                        continue;

                    vGenMetadata.add(cm);

                    // Create nested ports
                    for (; cm->id != NULL; ++cm)
                    {
                        if (IS_GROWING_PORT(cm))
                            cm->start    = cm->min + ((cm->max - cm->min) * row) / float(pg->rows());
                        else if (IS_LOWERING_PORT(cm))
                            cm->start    = cm->max - ((cm->max - cm->min) * row) / float(pg->rows());

                        // Create port
                        LV2UIPort *p        = create_port(cm, postfix_buf);
                        if ((p != NULL) && (p->metadata()->role != R_PORT_SET))
                        {
                            vUIPorts.add(p);
                            pUI->add_port(p);
                        }
                    }

                }

                result = pg;
                break;
            }

            default:
                break;
        }

        if (result != NULL)
            vAllPorts.add(result);

        return result;
    }

    void LV2UIWrapper::create_ports(const port_t *port)
    {
        for ( ; port->id != NULL; ++port)
        {
            LV2UIPort *p        = create_port(port, NULL);
            if (p == NULL)
                continue;

            // Add port to the lists
            switch (port->role)
            {
                case R_PORT_SET:
                case R_MIDI:
                    break;

                case R_PATH:
                case R_MESH:
                case R_FBUFFER:
                    pUI->add_port(p);
                    vUIPorts.add(p);
                    break;

                case R_AUDIO:
                case R_METER:
                case R_CONTROL:
                case R_BYPASS:
                {
                    pUI->add_port(p);
                    vUIPorts.add(p);

                    size_t port_id      = vExtPorts.size();
                    p->set_id(port_id);
                    vExtPorts.add(p);
                    lsp_trace("Added external port id=%s, external_id=%d", p->metadata()->id, int(port_id));
                    break;
                }

                default:
                    break;
            }
        }
    }

    void LV2UIWrapper::sort_by_urid(cvector<LV2UIPort> &v)
    {
        ssize_t items = v.size();
        if (items < 2)
            return;
        for (ssize_t i=0; i<(items-1); ++i)
            for (ssize_t j=i+1; j<items; ++j)
                if (v.at(i)->get_urid() > v.at(j)->get_urid())
                    v.swap(i, j);
    }

    LV2UIPort *LV2UIWrapper::find_by_urid(cvector<LV2UIPort> &v, LV2_URID urid)
    {
        // Try to find the corresponding port
        ssize_t first = 0, last = v.size() - 1;
        while (first <= last)
        {
            size_t center   = (first + last) >> 1;
            LV2UIPort *p    = v[center];
            if (urid == p->get_urid())
                return p;
            else if (urid < p->get_urid())
                last    = center - 1;
            else
                first   = center + 1;
        }
        return NULL;
    }

    void LV2UIWrapper::receive_atom(const LV2_Atom_Object * obj)
    {
        if ((obj->body.id == pExt->uridState) && (obj->body.otype == pExt->uridStateChange)) // State change
        {
            lsp_trace("Received STATE_CHANGE primitive");
            for (
                LV2_Atom_Property_Body *body = lv2_atom_object_begin(&obj->body) ;
                !lv2_atom_object_is_end(&obj->body, obj->atom.size, body) ;
                body = lv2_atom_object_next(body)
            )
            {
//                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                // Try to find the corresponding port
                LV2UIPort *p = find_by_urid(vUIPorts, body->key);
                if ((p != NULL) && (p->get_type_urid() == body->value.type))
                {
                    p->deserialize(&body->value);
                    p->notify_all();
                }
                else
                    lsp_warn("Port id=%d (%s) not found or has bad type", int(body->key), pExt->unmap_urid(body->key));
            }
        }
        else if (obj->body.otype == pExt->uridPatchSet)
        {
            lsp_trace("received PATCH_SET request");

            // Parse atom body
            LV2_Atom_Property_Body *body    = lv2_atom_object_begin(&obj->body);
            const LV2_Atom_URID    *key     = NULL;
            const LV2_Atom         *value   = NULL;

            while (!lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
            {
                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                if ((body->key == pExt->uridPatchProperty) && (body->value.type == pExt->uridAtomUrid))
                {
                    key     = reinterpret_cast<const LV2_Atom_URID *>(&body->value);
                    lsp_trace("body->value.body (%d) = %s", int(key->body), pExt->unmap_urid(key->body));
                }
                else if (body->key == pExt->uridPatchValue)
                    value   = &body->value;

                if ((key != NULL) && (value != NULL))
                {
                    LV2UIPort *p    = find_by_urid(vUIPorts, key->body);
                    if ((p != NULL) && (value->type == p->get_type_urid()))
                    {
                        p->deserialize(value);
                        p->notify_all();
                    }

                    key     = NULL;
                    value   = NULL;
                }

                body = lv2_atom_object_next(body);
            }
        }
        else if (obj->body.otype == pExt->uridTimePosition)
        {
//            lsp_trace("Received timePosition");
            position_t pos      = sPosition;

            pos.ticksPerBeat    = DEFAULT_TICKS_PER_BEAT;

//            lsp_trace("triggered timePosition event");
            for (
                LV2_Atom_Property_Body *body = lv2_atom_object_begin(&obj->body) ;
                !lv2_atom_object_is_end(&obj->body, obj->atom.size, body) ;
                body = lv2_atom_object_next(body)
            )
            {
//                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
//                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                if ((body->key == pExt->uridTimeFrame) && (body->value.type == pExt->forge.Long))
                    pos.frame           = (reinterpret_cast<LV2_Atom_Long *>(&body->value))->body;
                else if ((body->key == pExt->uridTimeSpeed) && (body->value.type == pExt->forge.Float))
                    pos.speed           = (reinterpret_cast<LV2_Atom_Float *>(&body->value))->body;
                else if ((body->key == pExt->uridTimeBeatsPerMinute) && (body->value.type == pExt->forge.Float))
                    pos.beatsPerMinute  = (reinterpret_cast<LV2_Atom_Float *>(&body->value))->body;
                else if ((body->key == pExt->uridTimeBeatUnit) && (body->value.type == pExt->forge.Int))
                    pos.denominator     = (reinterpret_cast<LV2_Atom_Int *>(&body->value))->body;
                else if ((body->key == pExt->uridTimeBeatsPerBar) && (body->value.type == pExt->forge.Float))
                    pos.numerator       = (reinterpret_cast<LV2_Atom_Float *>(&body->value))->body;
                else if ((body->key == pExt->uridTimeBarBeat) && (body->value.type == pExt->forge.Float))
                    pos.tick            = (reinterpret_cast<LV2_Atom_Float *>(&body->value))->body * pos.ticksPerBeat;
                else if ((body->key == pExt->uridTimeFrameRate) && (body->value.type == pExt->forge.Float))
                    pos.sampleRate      = (reinterpret_cast<LV2_Atom_Float *>(&body->value))->body;
            }

            // Call plugin callback and update position
            if (pUI != NULL)
                pUI->position_updated(&pos);
            sPosition = pos;
        }
        else if (obj->body.otype == pExt->uridMeshType)
        {
            // Try to find the corresponding mesh port
            LV2UIPort *p    = find_by_urid(vMeshPorts, obj->body.id);
            if (p != NULL)
            {
                p->deserialize(obj);
                p->notify_all();
            }
        }
        else if (obj->body.otype == pExt->uridFrameBufferType)
        {
            // Try to find the corresponding mesh port
            LV2UIPort *p    = find_by_urid(vFrameBufferPorts, obj->body.id);
            if (p != NULL)
            {
                p->deserialize(obj);
                p->notify_all();
            }
        }
        else
        {
            lsp_trace("obj->body.otype = %d (%s)", int(obj->body.otype), pExt->unmap_urid(obj->body.otype));
            lsp_trace("obj->body.id = %d (%s)", int(obj->body.id), pExt->unmap_urid(obj->body.id));
        }
    }

    KVTStorage *LV2UIWrapper::kvt_lock()
    {
        return (sKVTMutex.lock()) ? &sKVT : NULL;
    }

    KVTStorage *LV2UIWrapper::kvt_trylock()
    {
        return (sKVTMutex.try_lock()) ? &sKVT : NULL;
    }

    bool LV2UIWrapper::kvt_release()
    {
        return sKVTMutex.unlock();
    }

    void LV2UIWrapper::parse_raw_osc_event(osc::parse_frame_t *frame)
    {
        osc::parse_token_t token;
        status_t res = osc::parse_token(frame, &token);
        if (res != STATUS_OK)
            return;

        if (token == osc::PT_BUNDLE)
        {
            osc::parse_frame_t child;
            uint64_t time_tag;
            status_t res = osc::parse_begin_bundle(&child, frame, &time_tag);
            if (res != STATUS_OK)
                return;
            parse_raw_osc_event(&child); // Perform recursive call
            osc::parse_end(&child);
        }
        else if (token == osc::PT_MESSAGE)
        {
            const void *msg_start;
            size_t msg_size;
            const char *msg_addr;

            // Perform address lookup and routing
            status_t res = osc::parse_raw_message(frame, &msg_start, &msg_size, &msg_addr);
            if (res != STATUS_OK)
                return;

            lsp_trace("Received OSC message, address=%s, size=%d", msg_addr, int(msg_size));
            osc::dump_packet(msg_start, msg_size);

            // Try to parse KVT message first
            res = KVTDispatcher::parse_message(&sKVT, msg_start, msg_size, KVT_TX);
            if (res != STATUS_SKIP)
                return;

            // Not a KVT message, submit to OSC ports (if present)
            for (size_t i=0, n=vOscInPorts.size(); i<n; ++i)
            {
                LV2UIPort *p = vOscInPorts.at(i);
                if (p == NULL)
                    continue;

                // Submit message to the buffer
                osc_buffer_t *buf = p->get_buffer<osc_buffer_t>();
                if (buf != NULL)
                    buf->submit(msg_start, msg_size);
            }
        }
    }
}

#endif /* CONTAINER_LV2_UI_WRAPPER_H_ */
