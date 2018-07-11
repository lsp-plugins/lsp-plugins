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
            LV2UIPort              *pLatency;
            cvector<LV2UIPort>      vPorts;

        protected:
            inline void add_port(LV2UIPort *p)
            {
                lsp_trace("wrapping port id=%s, index=%d", p->metadata()->id, int(vPorts.size()));
                p->set_id(vPorts.size());
                vPorts.add(p);
            }

        public:
            inline explicit LV2UIWrapper(plugin_ui *ui, LV2Extensions *ext)
            {
                pUI         = ui;
                pExt        = ext;
                pTransport  = NULL;
                pLatency    = NULL;
            }

            ~LV2UIWrapper()
            {
                pUI         = NULL;
                pExt        = NULL;
                pTransport  = NULL;
                pLatency    = NULL;
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
                        {
                            LV2UIPort *lvp = new LV2UIPort(port, pExt);
                            pUI->add_port(lvp, true);
                            add_port(lvp);
                            break;
                        }
                        case R_METER:
                        {
                            LV2UIPort *lvp = new LV2UIPeakPort(port, pExt);
                            pUI->add_port(lvp, true);
                            add_port(lvp);
                            break;
                        }
                        case R_CONTROL:
                        {
                            LV2UIPort *lvp = new LV2UIFloatPort(port, pExt);
                            pUI->add_port(lvp, true);
                            add_port(lvp);
                            break;
                        }
                        case R_UI_SYNC:
                            continue;
                        case R_MESH:
                            if (pTransport != NULL)
                                pUI->add_port(new LV2UIMeshPort(port, pTransport), false);
                            else // Stub port
                                pUI->add_port(new LV2UIPort(port, pExt), false);
                            continue;
                        default:
                        {
                            LV2UIPort *lvp = new LV2UIFloatPort(port, pExt);
                            pUI->add_port(lvp, true);
                            add_port(lvp);
                            break;
                        }
                    }
                }

                // Build plugin
                pUI->build();

                // Add Atom communication ports
                if (pTransport != NULL)
                {
                    lsp_trace("binding LV2UITransport");
                    add_port(pTransport->out());
                    add_port(pTransport->in());
                }

                // Add stub for latency reporting
                {
                    const port_t *port = &lv2_latency_port;
                    if ((port->id != NULL) && (port->name != NULL))
                    {
                        pLatency    = new LV2UIFloatPort(port, pExt);
                        add_port(pLatency);
                    }
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
                vPorts.clear();

                // Destroy transport
                if (pTransport != NULL)
                {
                    delete pTransport;
                    pTransport  = NULL;
                }

                // Destroy latency port
                if (pLatency != NULL)
                {
                    delete pLatency;
                    pLatency    = NULL;
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
                LV2UIPort *p = vPorts[id];
                if (p != NULL)
                    p->notify(buf, format, size);
            }
    };
}

#endif /* CONTAINER_LV2UI_WRAPPER_H_ */
