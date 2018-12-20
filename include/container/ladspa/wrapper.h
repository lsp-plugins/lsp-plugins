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
            size_t                  nLatencyID;     // ID of Latency port
            LADSPA_Data            *pLatency;       // Latency pointer
            bool                    bUpdateSettings;// Settings update

            position_t              sPosition;
            position_t              sNewPosition;

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
                pPlugin         = plugin;
                pExecutor       = NULL;
                nLatencyID      = 0;
                pLatency        = NULL;
                bUpdateSettings = true;

                position_t::init(&sPosition);
                position_t::init(&sNewPosition);
            }

            ~LADSPAWrapper()
            {
                pPlugin         = NULL;
            }

        public:
            void init(unsigned long sr)
            {
                const plugin_metadata_t *m = pPlugin->get_metadata();

                // Bind ports
                lsp_trace("Binding ports");

                size_t n_ports = 0;
                for (const port_t *port = m->ports; (port->id != NULL) && (port->name != NULL); ++port, ++n_ports)
                {
                    lsp_trace("processing port id=%s", port->id);
                    bool out = IS_OUT_PORT(port);
                    switch (port->role)
                    {
                        case R_AUDIO:
                        {
                            LADSPAPort *lp  = new LADSPAAudioPort(port);
                            add_port(lp);
                            lsp_trace("added as audio port");
                            break;
                        }
                        case R_CONTROL:
                        case R_METER:
                        {
                            LADSPAPort *lp = NULL;
                            if (out)
                                lp = new LADSPAOutputPort(port);
                            else
                                lp = new LADSPAInputPort(port);

                            add_port(lp);
                            break;
                        }
                        case R_PORT_SET: // TODO: implement recursive port creation?
                        case R_MESH: // Not supported by LADSPA, make it stub
                        case R_FBUFFER:
                        case R_UI_SYNC:
                        case R_MIDI:
                        case R_PATH:
                        default:
                            pPlugin->add_port(new LADSPAPort(port));
                            lsp_trace("added as stub port");
                            break;
                    }
                }

                // Store the latency ID port
                nLatencyID  = n_ports;

                // Store sample rate
                sPosition.sampleRate    = sr;
                sNewPosition.sampleRate = sr;

                // Initialize plugin
                lsp_trace("Initializing plugin");
                pPlugin->init(this);
                pPlugin->set_sample_rate(sr);
                bUpdateSettings = true;
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
                sPosition.frame     = 0;
                sNewPosition.frame  = 0;
                pPlugin->activate();
            }

            inline void deactivate()
            {
                pPlugin->deactivate();
            }

            inline void connect(size_t id, void *data)
            {
                if (id == nLatencyID)
                {
                    pLatency        = reinterpret_cast<LADSPA_Data *>(data);
                    return;
                }

                // Bind internal port
                LADSPAPort *p      = vPorts[id];
                if (p != NULL)
                    p->bind(data);
            }

            inline void run(size_t samples)
            {
                // Emulate the behaviour of position
                if (pPlugin->set_position(&sNewPosition))
                    bUpdateSettings = true;
                sPosition = sNewPosition;
//                lsp_trace("frame = %ld, tick = %f", long(sPosition.frame), float(sPosition.tick));

                // Process external ports for changes
                size_t n_ports  = vPorts.size();
                for (size_t i=0; i<n_ports; ++i)
                {
                    // Get port
                    LADSPAPort *port = vPorts.at(i);
                    if (port == NULL)
                        continue;

                    // Pre-process data in port
                    if (port->pre_process(samples))
                    {
                        lsp_trace("port changed: %s", port->metadata()->id);
                        bUpdateSettings = true;
                    }
                }

                // Check that input parameters have changed
                if (bUpdateSettings)
                {
                    lsp_trace("updating settings");
                    pPlugin->update_settings();
                    bUpdateSettings     = false;
                }

                // Call the main processing unit
                pPlugin->process(samples);

                // Process external ports for changes
                for (size_t i=0; i<n_ports; ++i)
                {
                    LADSPAPort *port = vPorts.at(i);
                    if (port != NULL)
                        port->post_process(samples);
                }

                // Write latency
                if (pLatency != NULL)
                    *pLatency       = pPlugin->get_latency();

                // Move the position
                size_t spb          = sNewPosition.sampleRate / sNewPosition.beatsPerMinute; // samples per beat
                sNewPosition.frame += samples;
                sNewPosition.tick   = ((sNewPosition.frame % spb) * sNewPosition.ticksPerBeat) / spb;
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

            virtual const position_t *position()
            {
                return &sPosition;
            }

    };
}

#endif /* CONTAINER_LADSPA_WRAPPER_H_ */
