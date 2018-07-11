/*
 * wrapper.h
 *
 *  Created on: 12 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LADSPA_WRAPPER_H_
#define CONTAINER_LADSPA_WRAPPER_H_

namespace lsp
{
    class LADSPAWrapper: public IWrapper
    {
        private:
            cvector<LADSPAPort>     vPorts;
            plugin_t               *pPlugin;
            IExecutor              *pExecutor;      // Executor service

        protected:
            inline void add_port(LADSPAPort *p)
            {
                lsp_trace("wrapping port id=%s, index=%d", p->metadata()->id, int(vPorts.size()));
                pPlugin->add_port(p);
                vPorts.add(p);
            }

        public:
            LADSPAWrapper(plugin_t *plugin)
            {
                pPlugin     = plugin;
                pExecutor   = NULL;
            }

            ~LADSPAWrapper()
            {
                pPlugin     = NULL;
            }

        public:
            void init(unsigned long sr)
            {
                const plugin_metadata_t *m = pPlugin->get_metadata();

                // Bind ports
                lsp_trace("Binding ports");

                for (const port_t *port = m->ports; (port->id != NULL) && (port->name != NULL); ++port)
                {
                    lsp_trace("processing port id=%s", port->id);
                    bool out = port->flags & F_OUT;
                    switch (port->role)
                    {
                        case R_AUDIO:
                        {
                            LADSPAPort *lp  = new LADSPAAudioPort(port);
                            add_port(lp);
                            lsp_trace("added as audio port");
                            break;
                        }
                        case R_MESH: // Not supported by LADSPA, make it stub
                        case R_UI_SYNC:
                            pPlugin->add_port(new LADSPAPort(port));
                            lsp_trace("added as stub port");
                            break;
                        case R_CONTROL:
                        case R_METER:
                        default:
                        {
                            LADSPAPort *lp = NULL;
                            if (out)
                                lp = new LADSPAOutputPort(port);
                            else
                                lp = new LADSPAInputPort(port);

                            add_port(lp);
                            break;
                        }
                        case R_PORT_SET: // TODO: implement recursive port creation
                            break;
                    }
                }

                // Initialize plugin
                lsp_trace("Initializing plugin");
                pPlugin->init(this);
                pPlugin->set_sample_rate(sr);
            }

            void destroy()
            {
                // Clear all ports
                for (size_t i=0; i < vPorts.size(); ++i)
                {
                    lsp_trace("destroy port id=%s", vPorts[i]->metadata()->id);
                    delete vPorts[i];
                }
                vPorts.clear();

                // Delete plugin
                if (pPlugin != NULL)
                {
                    pPlugin->destroy();
                    delete pPlugin;
                    pPlugin     = NULL;
                }

                // Destroy executor
                if (pExecutor != NULL)
                {
                    pExecutor->shutdown();
                    delete pExecutor;
                    pExecutor   = NULL;
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
                LADSPAPort *p      = vPorts[id];
                if (p != NULL)
                    p->bind(data);
            }

            inline void run(size_t samples)
            {
                bool update     = false;

                // Process external ports for changes
                for (size_t i=0; i<vPorts.size(); ++i)
                {
                    // Get port
                    LADSPAPort *port = vPorts[i];
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
                for (size_t i=0; i<vPorts.size(); ++i)
                {
                    LADSPAPort *port = vPorts[i];
                    if (port != NULL)
                        port->post_process(samples);
                }
            }

            virtual IExecutor *get_executor()
            {
                if (pExecutor == NULL)
                {
                    lsp_trace("Creating native executor service");
                    pExecutor       = new NativeExecutor();
                }
                return pExecutor;
            }

    };
}

#endif /* CONTAINER_LADSPA_WRAPPER_H_ */
