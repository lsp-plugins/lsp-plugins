/*
 * wrapper.h
 *
 *  Created on: 12 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2UI_WRAPPER_H_
#define CONTAINER_LV2UI_WRAPPER_H_

namespace lsp
{
    class LV2UIWrapper
    {
        private:
            plugin_ui              *pUI;
            LV2Extensions          *pExt;
            LV2UIAtomTransport     *pTransport;
            cvector<LV2UIPort>      vExtPorts;
            cvector<LV2UIPort>      vAllPorts;

        protected:
            inline void add_port(LV2UIPort *p, bool external, bool ui)
            {
                lsp_trace("wrapping ui port id=%s", p->metadata()->id);
                vAllPorts.add(p);

                if (external)
                {
                    lsp_trace("  external_id=%d", int(vExtPorts.size()));
                    p->set_id(vExtPorts.size());
                    vExtPorts.add(p);
                }
                if (ui)
                {
                    lsp_trace("  internal_id=%d", int(pUI->ports_count()));
                    pUI->add_port(p);
                }

                if (pTransport != NULL)
                {
                    const port_t *meta = p->metadata();
                    if ((meta != NULL) && (meta->flags & F_OUT))
                    {
                        switch (meta->role)
                        {
                            case R_METER:
                            case R_CONTROL:
                                pTransport->add_control_port(p);
                                lsp_trace("  added as control port");
                                break;
                            default:
                                break;
                        }
                    }
                }
            }

        public:
            inline explicit LV2UIWrapper(plugin_ui *ui, LV2Extensions *ext)
            {
                pUI         = ui;
                pExt        = ext;
                pTransport  = NULL;
            }

            ~LV2UIWrapper()
            {
                pUI         = NULL;
                pExt        = NULL;
                pTransport  = NULL;
            }

        public:
            void init()
            {
                // Get plugin metadata
                const plugin_metadata_t *m  = pUI->metadata();

                if (pExt->atom_supported())
                {
                    lsp_trace("Creating atom transport lv2_atom_ports=%p, pExt=%p, pUI=%p",
                        lv2_atom_ports, pExt, pUI);
                    pTransport              = new LV2UIAtomTransport(lv2_atom_ports, pExt, pUI);
                }

                // Initialize plugin
                lsp_trace("Initializing UI");
                pUI->init();

                // Perform all port bindings
                for (const port_t *port = m->ports; (port->id != NULL) && (port->name != NULL); ++port)
                {
                    lsp_trace("bind port id=%s, role = %d", port->id, int(port->role));

                    switch (port->role)
                    {
                        case R_AUDIO: // Stub port for audio
                            add_port(new LV2UIPort(port, pExt), true, false);
                            break;
                        case R_METER:
                            add_port(new LV2UIPeakPort(port, pExt), true, true);
                            break;
                        case R_CONTROL:
                            add_port(new LV2UIFloatPort(port, pExt), true, true);
                            break;
                        case R_UI_SYNC:
                            continue;
                        case R_MESH:
                            if (pTransport != NULL)
                                add_port(new LV2UIMeshPort(port, pTransport), false, true);
                            else // Stub port
                                add_port(new LV2UIPort(port, pExt), false, true);
                            continue;
                        default:
                            add_port(new LV2UIFloatPort(port, pExt), true, true);
                            break;
                    }
                }

                // Build plugin
                pUI->build();

                // Add Atom communication ports
                if (pTransport != NULL)
                {
                    lsp_trace("binding LV2UITransport");
                    add_port(pTransport->out(), true, false);
                    add_port(pTransport->in(), true, false);
                }

                // Add stub for latency reporting
                {
                    const port_t *port = &lv2_latency_port;
                    if ((port->id != NULL) && (port->name != NULL))
                        add_port(new LV2UIFloatPort(port, pExt), true, false);
                }

                // And last: query state
                if (pTransport != NULL)
                    pTransport->query_state();

                // Return UI
                lsp_trace("Return handle");
            }

            void destroy()
            {
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
                vAllPorts.clear();
                vExtPorts.clear();

                // Destroy transport
                if (pTransport != NULL)
                {
                    delete pTransport;
                    pTransport  = NULL;
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
                LV2UIPort *p = vExtPorts[id];
                if (p != NULL)
                    p->notify(buf, format, size);
            }
    };
}

#endif /* CONTAINER_LV2UI_WRAPPER_H_ */
