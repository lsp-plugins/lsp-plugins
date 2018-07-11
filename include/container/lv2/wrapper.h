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
            cvector<LV2Port>    vExtPorts;
            cvector<LV2Port>    vIntPorts;

        protected:
            void add_port(LV2Port *p, bool external, bool plugin)
            {
                lsp_trace("adding port id=%s, index=%d", p->metadata()->id, int (vIntPorts.size()));

                if (plugin)
                    pPlugin->add_port(p);

                vIntPorts.add(p);

                if (external)
                {
                    lsp_trace("  external index=%d", int(vExtPorts.size()));
                    vExtPorts.add(p);
                }
            }

        public:
            inline explicit LV2Wrapper(plugin_t *plugin, LV2Extensions *ext)
            {
                pPlugin     = plugin;
                pExt        = ext;
                pTransport  = NULL;
            }

            ~LV2Wrapper()
            {
                pPlugin     = NULL;
                pExt        = NULL;
                pTransport  = NULL;
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
                            add_port(new LV2Port(port, pExt), false, true);
                            break;
                        case R_MESH:
                            if (pTransport != NULL)
                                add_port(new LV2MeshPort(port, pTransport), false, true);
                            else
                                add_port(new LV2Port(port, pExt), false, true);
                            break;

                        case R_AUDIO:
                            add_port(new LV2AudioPort(port, pExt), true, true);
                            break;

                        case R_CONTROL:
                        case R_METER:
                        default:
                            if (out)
                                add_port(new LV2OutputPort(port, pExt), true, true);
                            else
                                add_port(new LV2InputPort(port, pExt), true, true);
                            break;
                    }
                }

                // Add state ports (if supported)
                if (pTransport != NULL)
                {
                    lsp_trace("binding LV2Transport");
                    add_port(pTransport->in(), true, false);
                    add_port(pTransport->out(), true, false);
                }

                // Add latency port
                {
                    const port_t *port = &lv2_latency_port;
                    if ((port->id != NULL) && (port->name != NULL))
                    {
                        lsp_trace("binding Latency Port");
                        add_port(new LV2LatencyPort(port, pExt, pPlugin), true, false);
                    }
                }

                // Initialize plugin
                lsp_trace("Initializing plugin");
                pPlugin->init();
                pPlugin->set_sample_rate(srate);
                if (pTransport != NULL)
                    pTransport->init();
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
                for (size_t i=0; i < vIntPorts.size(); ++i)
                {
                    lsp_trace("destroy port id=%s", vIntPorts[i]->metadata()->id);
                    delete vIntPorts[i];
                }
                vIntPorts.clear();
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
                LV2Port *p      = vExtPorts[id];
                if (p != NULL)
                    p->bind(data);
            }

            inline void run(size_t samples)
            {
                bool update     = false;

                // Process external ports for changes
                for (size_t i=0; i<vIntPorts.size(); ++i)
                {
                    // Get port
                    LV2Port *port = vIntPorts[i];
                    if (port == NULL)
                        continue;

                    // Pre-process data in port
                    if (port->pre_process(samples))
                    {
                        lsp_trace("port changed: %s", port->metadata()->id);
                        update = true;
                    }
                }

                // Check that input parameters have changed
                if (update)
                {
                    lsp_trace("updating settings");
                    pPlugin->update_settings();
                }

                // Call the main processing unit
                pPlugin->process(samples);

                // Process external ports for changes
                for (size_t i=0; i<vIntPorts.size(); ++i)
                {
                    LV2Port *port = vIntPorts[i];
                    if (port != NULL)
                        port->post_process(samples);
                }
            }
    };
}

#endif /* CONTAINER_LV2_WRAPPER_H_ */
