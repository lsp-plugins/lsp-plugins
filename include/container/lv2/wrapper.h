/*
 * wrapper.h
 *
 *  Created on: 12 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_WRAPPER_H_
#define CONTAINER_LV2_WRAPPER_H_

#include <core/IWrapper.h>
#include <core/NativeExecutor.h>

namespace lsp
{
    class LV2Wrapper: public IWrapper
    {
        private:
            plugin_t           *pPlugin;
            LV2Extensions      *pExt;
            LV2AtomTransport   *pTransport; // Transport
            LV2Port            *pIn;        // Transport ports
            LV2Port            *pOut;       // Transport ports
            IExecutor          *pExecutor;  // Executor service
            cvector<LV2Port>    vExtPorts;
            cvector<LV2Port>    vIntPorts;

            enum add_flags
            {
                P_EXTERNAL      = 1 << 0,
                P_PLUGIN        = 1 << 1,
                P_INTERNAL      = 1 << 2
            };

        protected:
            void add_port(LV2Port *p, int flags)
            {
                lsp_trace("adding port id=%s", p->metadata()->id);

                if (flags & P_PLUGIN)
                {
                    lsp_trace("  plugin index=%d", int(pPlugin->ports_count()));
                    pPlugin->add_port(p);
                }

                if (flags & P_INTERNAL)
                {
                    lsp_trace("  internal index=%d", int(vIntPorts.size()));
                    vIntPorts.add(p);
                }

                if (flags & P_EXTERNAL)
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
                pIn         = NULL;
                pOut        = NULL;
                pExecutor   = NULL;
            }

            ~LV2Wrapper()
            {
                pPlugin     = NULL;
                pExt        = NULL;
                pTransport  = NULL;
                pIn         = NULL;
                pOut        = NULL;
                pExecutor   = NULL;
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
                            lsp_error("Error in metadata: R_UI_SYNC port is present in port list");
                            add_port(new LV2Port(port, pExt), P_EXTERNAL | P_INTERNAL);
                            break;
                        case R_MESH:
                            if (pTransport != NULL)
                                add_port(new LV2MeshPort(port, pTransport), P_INTERNAL | P_PLUGIN);
                            else
                                add_port(new LV2Port(port, pExt), P_INTERNAL | P_PLUGIN);
                            break;
                        case R_PATH:
                            if (pTransport != NULL)
                                add_port(new LV2PathPort(port, pTransport), P_INTERNAL | P_PLUGIN);
                            else
                                add_port(new LV2Port(port, pExt), P_INTERNAL | P_PLUGIN);
                            break;

                        case R_AUDIO:
                            add_port(new LV2AudioPort(port, pExt), P_EXTERNAL | P_INTERNAL | P_PLUGIN);
                            break;

                        case R_CONTROL:
                        case R_METER:
                        default:
                            if (out)
                                add_port(new LV2OutputPort(port, pExt), P_EXTERNAL | P_INTERNAL | P_PLUGIN);
                            else
                                add_port(new LV2InputPort(port, pExt), P_EXTERNAL | P_INTERNAL | P_PLUGIN);
                            break;
                    }
                }

                // Add state ports (if supported)
                if (pTransport != NULL)
                {
                    lsp_trace("binding LV2Transport");

                    pIn     = pTransport->in();
                    pOut    = pTransport->out();

                    add_port(pIn, P_EXTERNAL);
                    add_port(pOut, P_EXTERNAL);
                }

                // Add latency port
                {
                    const port_t *port = &lv2_latency_port;
                    if ((port->id != NULL) && (port->name != NULL))
                    {
                        lsp_trace("binding Latency Port");
                        add_port(new LV2LatencyPort(port, pExt, pPlugin), P_EXTERNAL | P_INTERNAL);
                    }
                }

                // Initialize plugin
                lsp_trace("Initializing plugin");
                pPlugin->init(this);
                pPlugin->set_sample_rate(srate);
                if (pTransport != NULL)
                    pTransport->init();
            }

            void destroy()
            {
                // Shutdown and delete executor if exists
                if (pExecutor != NULL)
                {
                    pExecutor->shutdown();
                    delete pExecutor;
                    pExecutor   = NULL;
                }

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

                // Forget transport ports
                pIn     = NULL;
                pOut    = NULL;

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

                // First pre-process transport ports
                if (pIn != NULL)
                    pIn->pre_process(samples);
                if (pOut != NULL)
                    pOut->pre_process(samples);

                // Pre-rocess regular ports
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

                // Post-process regular ports for changes
                for (size_t i=0; i<vIntPorts.size(); ++i)
                {
                    LV2Port *port = vIntPorts[i];
                    if (port != NULL)
                        port->post_process(samples);
                }

                // Post-process transport ports (if present)
                if (pIn != NULL)
                    pIn->post_process(samples);
                if (pOut != NULL)
                    pOut->post_process(samples);
            }

            inline void save_state(
                LV2_State_Store_Function   store,
                LV2_State_Handle           handle,
                uint32_t                   flags,
                const LV2_Feature *const * features)
            {
                pExt->init_state_context(store, NULL, handle, flags, features);

                size_t ports_count = vIntPorts.size();

                for (size_t i=0; i<ports_count; ++i)
                {
                    // Get port
                    LV2Port *lvp    = vIntPorts[i];
                    if (lvp == NULL)
                        continue;

                    // Save state of port
                    lvp->save();
                }

                pExt->reset_state_context();
            }

            inline void restore_state(
                LV2_State_Retrieve_Function retrieve,
                LV2_State_Handle            handle,
                uint32_t                    flags,
                const LV2_Feature *const *  features
            )
            {
                pExt->init_state_context(NULL, retrieve, handle, flags, features);

                size_t ports_count = vIntPorts.size();

                for (size_t i=0; i<ports_count; ++i)
                {
                    // Get port
                    LV2Port *lvp    = vIntPorts[i];
                    if (lvp == NULL)
                        continue;

                    // Restore state of port
                    lvp->restore();
                }

                pExt->reset_state_context();
            }

            inline void job_run(
                LV2_Worker_Respond_Handle   handle,
                LV2_Worker_Respond_Function respond,
                uint32_t                    size,
                const void*                 data
            )
            {
                LV2Executor *executor = static_cast<LV2Executor *>(pExecutor);
                executor->run_job(handle, respond, size, data);
            }

            inline void job_response(size_t size, const void *body)
            {
//                LV2Executor *executor = static_cast<LV2Executor *>(pExecutor);
            }

            inline void job_end()
            {
//                LV2Executor *executor = static_cast<LV2Executor *>(pExecutor);
            }

            virtual IExecutor *get_executor()
            {
                lsp_trace("executor = %p", reinterpret_cast<void *>(pExecutor));
                if (pExecutor != NULL)
                    return pExecutor;

                // Create executor service
                if (pExt->sched != NULL)
                {
                    lsp_trace("Creating LV2 executor service");
                    pExecutor       = new LV2Executor(pExt->sched);
                }
                else
                {
                    lsp_trace("Creating native executor service");
                    pExecutor       = new NativeExecutor();
                }
                return pExecutor;
            }
    };
}

#endif /* CONTAINER_LV2_WRAPPER_H_ */
