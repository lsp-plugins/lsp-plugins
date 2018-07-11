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
    class LADSPAWrapper
    {
        private:
            plugin_t               *pPlugin;
            cvector<LADSPAPort>     vPorts;

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
            }

            ~LADSPAWrapper()
            {
                lsp_trace("destroy");
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
                    }
                }

                // Initialize plugin
                lsp_trace("Initializing plugin");
                pPlugin->init();
                pPlugin->set_sample_rate(sr);
            }

            void destroy()
            {
                // Clear all ports
                vPorts.clear();

                // Delete plugin
                if (pPlugin != NULL)
                {
                    pPlugin->destroy();
                    delete pPlugin;
                    pPlugin     = NULL;
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
                // Run plugin
                pPlugin->run(samples);
            }

    };
}

#endif /* CONTAINER_LADSPA_WRAPPER_H_ */
