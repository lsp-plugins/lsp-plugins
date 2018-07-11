/*
 * wrapper.h
 *
 *  Created on: 12 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_WRAPPER_H_
#define CONTAINER_LV2_WRAPPER_H_

namespace lsp
{
    class LV2Wrapper
    {
        private:
            plugin_t           *pPlugin;
            LV2Extensions      *pExt;
            LV2AtomTransport   *pTransport;
            LV2Port            *pLatency;
            cvector<LV2Port>    vPorts;

        protected:
            inline void add_port(LV2Port *p)
            {
                lsp_trace("wrapping port id=%s, index=%d", p->metadata()->id, int(vPorts.size()));
                vPorts.add(p);
            }

        public:
            inline explicit LV2Wrapper(plugin_t *plugin, LV2Extensions *ext)
            {
                pPlugin     = plugin;
                pExt        = ext;
                pTransport  = NULL;
                pLatency    = NULL;
            }

            ~LV2Wrapper()
            {
                pPlugin     = NULL;
                pExt        = NULL;
                pTransport  = NULL;
                pLatency    = NULL;
            }

        public:
            void init(float srate)
            {
                // Get plugin metadata
                const plugin_metadata_t *m  = pPlugin->get_metadata();

                // Check atom support
                if (pExt->atom_supported())
                {
                    lsp_trace("Creating atom transport");
                    pTransport      = new LV2AtomTransport(lv2_atom_ports, pExt, pPlugin);
                }

                // Bind ports
                lsp_trace("Binding ports");
                for (const port_t *port = m->ports; (port->id != NULL) && (port->name != NULL); ++port)
                {
                    bool out = (port->flags & F_OUT);

                    switch (port->role)
                    {
                        case R_UI_SYNC:
                            pPlugin->add_port(new LV2Port(port, pExt));
                            break;
                        case R_MESH:
                            if (pTransport != NULL)
                                pPlugin->add_port(new LV2MeshPort(port, pTransport));
                            else
                                pPlugin->add_port(new LV2Port(port, pExt));
                            break;

                        case R_AUDIO:
                        {
                            LV2Port *lvp = new LV2AudioPort(port, pExt);
                            pPlugin->add_port(lvp);
                            add_port(lvp);
                            break;
                        }

                        case R_CONTROL:
                        case R_METER:
                        default:
                        {
                            LV2Port *lvp = NULL;
                            if (out)
                                lvp = new LV2OutputPort(port, pExt);
                            else
                                lvp = new LV2InputPort(port, pExt);

                            pPlugin->add_port(lvp);
                            add_port(lvp);
                            break;
                        }
                    }
                }

                // Add state ports (if supported)
                if (pTransport != NULL)
                {
                    lsp_trace("binding LV2Transport");
                    add_port(pTransport->in());
                    add_port(pTransport->out());
                }

                // Add latency port
                {
                    const port_t *port = &lv2_latency_port;
                    if ((port->id != NULL) && (port->name != NULL))
                    {
                        lsp_trace("binding Latency Port");
                        pLatency    = new LV2LatencyPort(port, pExt, pPlugin);
                        add_port(pLatency);
                    }
                }

                // Initialize plugin
                lsp_trace("Initializing plugin");
                pPlugin->init();
                pPlugin->set_sample_rate(srate);
            }

            void destroy()
            {
                // Drop plugin
                if (pPlugin != NULL)
                {
                    pPlugin->destroy();
                    delete pPlugin;

                    pPlugin     = NULL;
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

            inline void activate()
            {
                pPlugin->activate();
            }

            inline void deactivate()
            {
                pPlugin->deactivate();
            }

            inline void connect(size_t id, void *data)
            {
//                lsp_trace("id=%d, data=%p", int(id), data);
                LV2Port *p      = vPorts[id];
                if (p != NULL)
                    p->bind(data);
            }

            inline void run(size_t samples)
            {
                // Pre-process special ports
                pTransport->in()->pre_process();
                pTransport->out()->pre_process();
                pLatency->pre_process();

                // Run plugin
                pPlugin->run(samples);

                // Post-process special ports
                pTransport->in()->post_process();
                pTransport->out()->post_process();
                pLatency->post_process();
            }
    };
}

#endif /* CONTAINER_LV2_WRAPPER_H_ */
