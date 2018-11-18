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
            cvector<port_t>         vGenMetadata;   // Generated metadata

            position_t              sPosition;

            plugin_ui              *pUI;
            LV2Extensions          *pExt;
            size_t                  nLatencyID; // ID of latency port
            LV2UIPort              *pLatency;
            bool                    bConnected;

        protected:
            LV2UIPort *create_port(const port_t *p, const char *postfix);
            void create_ports(const port_t *port);

            void receive_atom(const LV2_Atom_Object * atom);
            static LV2UIPort *find_by_urid(cvector<LV2UIPort> &v, LV2_URID urid);
            void sort_by_urid(cvector<LV2UIPort> &v);

            static status_t slot_ui_hide(LSPWidget *sender, void *ptr, void *data);
            static status_t slot_ui_show(LSPWidget *sender, void *ptr, void *data);
            static status_t slot_ui_resize(LSPWidget *sender, void *ptr, void *data);

        public:
            inline explicit LV2UIWrapper(plugin_ui *ui, LV2Extensions *ext)
            {
                pUI         = ui;
                pExt        = ext;
                nLatencyID  = 0;
                pLatency    = NULL;
                bConnected  = false;

                position_t::init(&sPosition);
            }

            ~LV2UIWrapper()
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
                pUI->init(this, 0, NULL);

                // Initialize size of root window
                size_request_t sr;
                LSPWindow *root = pUI->root_window();
                if (root == NULL)
                    return;

                root->slots()->bind(LSPSLOT_SHOW, slot_ui_show, this);
                root->slots()->bind(LSPSLOT_HIDE, slot_ui_hide, this);
                root->slots()->bind(LSPSLOT_RESIZE, slot_ui_resize, this);

                pUI->show();
                root->size_request(&sr);
                root->resize(sr.nMinWidth, sr.nMinHeight);
                realize_t r;
                r.nLeft     = 0;
                r.nTop      = 0;
                r.nWidth    = sr.nMinWidth;
                r.nHeight   = sr.nMinHeight;
                ui_resize(&r);
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

            void ui_resize(const realize_t *r)
            {
                lsp_trace("UI has been resized");
                if ((pUI == NULL) || (pExt == NULL))
                    return;

                pExt->resize_ui(r->nWidth, r->nHeight);
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
                        lsp_trace("notify id=%d, size=%d, format=%d, buf=%p value=%f",
                            int(id), int(size), int(format), buf, *(reinterpret_cast<const float *>(buf)));

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

                    if ((atom->type != pExt->uridObject) && (atom->type != pExt->uridBlank))
                        return;

                    receive_atom(reinterpret_cast<const LV2_Atom_Object *>(atom));
                }
                else if (id == nLatencyID)
                {
                    if (pLatency != NULL)
                        pLatency->notify(buf, format, size);
                }
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

    status_t LV2UIWrapper::slot_ui_resize(LSPWidget *sender, void *ptr, void *data)
    {
        LV2UIWrapper *_this = static_cast<LV2UIWrapper *>(ptr);
        _this->ui_resize(static_cast<realize_t *>(data));
        return STATUS_OK;
    }

    LV2UIPort *LV2UIWrapper::create_port(const port_t *p, const char *postfix)
    {
        LV2UIPort *result = NULL;
        LV2Wrapper *w = pExt->wrapper();

        switch (p->role)
        {
            case R_AUDIO: // Stub ports
            case R_MIDI:
                result = new LV2UIPort(p, pExt);
                break;
            case R_CONTROL:
                result = new LV2UIFloatPort(p, pExt, (w != NULL) ? w->get_port(p->id) : NULL);
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
                    break;

                case R_PATH:
                case R_MESH:
                case R_FBUFFER:
                    pUI->add_port(p);
                    vUIPorts.add(p);
                    break;

                case R_AUDIO:
                case R_MIDI:
                case R_METER:
                case R_CONTROL:
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
                lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
                lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

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
        else if ((obj->body.otype == pExt->uridPatchSet) && (obj->body.id == pExt->uridPatchMessage))
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
}

#endif /* CONTAINER_LV2_UI_WRAPPER_H_ */
