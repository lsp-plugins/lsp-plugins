/*
 * wrapper.h
 *
 *  Created on: 11 мая 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_JACK_WRAPPER_H_
#define CONTAINER_JACK_WRAPPER_H_

#include <core/types.h>
#include <core/debug.h>
#include <core/alloc.h>
#include <core/IWrapper.h>
#include <core/IPort.h>
#include <core/NativeExecutor.h>
#include <core/ICanvas.h>
#include <container/CairoCanvas.h>

#include <ui/ui.h>
#include <ui/IUIWrapper.h>

#include <data/cvector.h>

namespace lsp
{
    class JACKPort;
    class JACKUIPort;

    class JACKWrapper: public IWrapper, public IUIWrapper
    {
        private:
            plugin_t           *pPlugin;
            plugin_ui          *pUI;
            IExecutor          *pExecutor;
            jack_client_t      *pClient;
            atomic_t            nQueryDraw;
            atomic_t            nQueryDrawLast;
            CairoCanvas        *pCanvas;
            bool                bUpdateSettings;

            cvector<JACKPort>   vPorts;
            cvector<JACKUIPort> vUIPorts;
            cvector<JACKUIPort> vSyncPorts;
            cvector<port_t>     vGenMetadata;   // Generated metadata

        public:
            JACKWrapper(plugin_t *plugin, plugin_ui *ui)
            {
                pPlugin         = plugin;
                pUI             = ui;
                pExecutor       = NULL;
                pClient         = NULL;
                nQueryDraw      = 0;
                nQueryDrawLast  = 0;
                pCanvas         = NULL;
                bUpdateSettings = true;
            }

            virtual ~JACKWrapper()
            {
                pPlugin         = NULL;
                pUI             = NULL;
                pExecutor       = NULL;
                pClient         = NULL;
                nQueryDraw      = 0;
                nQueryDrawLast  = 0;
                pCanvas         = NULL;
            }

        protected:
            static int process(jack_nframes_t nframes, void *arg);
            static void shutdown(void *arg);
            int run(size_t samples);

            void create_port(const port_t *port, const char *postfix);
            void create_ports(const port_t *meta);

        public:
            virtual IExecutor *get_executor();

        public:
            inline const char *client_id();
            inline jack_client_t *client() { return pClient; };

            int init();
            void destroy();
            bool transfer_dsp_to_ui();

            // Inline display interface
            canvas_data_t *render_inline_display(size_t width, size_t height);
            virtual void query_display_draw()
            {
                nQueryDraw++;
            }

            inline bool test_display_draw()
            {
                atomic_t last       = nQueryDraw;
                bool result         = last != nQueryDrawLast;
                nQueryDrawLast      = last;
                return result;
            }
    };
}

#include <container/jack/defs.h>
#include <container/jack/types.h>
#include <container/jack/ports.h>
#include <container/jack/ui_ports.h>

namespace lsp
{
    int JACKWrapper::process(jack_nframes_t nframes, void *arg)
    {
        dsp_context_t ctx;
        int result;

        // Call the plugin for processing
        dsp::start(&ctx);
        JACKWrapper *_this  = reinterpret_cast<JACKWrapper *>(arg);
        result              = _this->run(nframes);
        dsp::finish(&ctx);

        return result;
    }

    int JACKWrapper::run(size_t samples)
    {
        size_t n_ports  = vPorts.size();

        for (size_t i=0; i<n_ports; ++i)
        {
            // Get port
            JACKPort *port = vPorts.at(i);
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
            bUpdateSettings = false;
        }

        // Call the main processing unit
        pPlugin->process(samples);

        // Report latency
        // TODO (maybe)

        // Post-process ALL ports
        for (size_t i=0; i<n_ports; ++i)
        {
            JACKPort *port = vPorts.at(i);
            if (port != NULL)
                port->post_process(samples);
        }
        return 0;
    }

    void JACKWrapper::shutdown(void *arg)
    {
        // Reset the client state
        JACKWrapper *_this  = reinterpret_cast<JACKWrapper *>(arg);
        _this->pClient      = NULL;
    }

    inline const char *JACKWrapper::client_id()
    {
        return (pClient != NULL) ? jack_get_client_name(pClient) : NULL;
    }

    void JACKWrapper::create_port(const port_t *port, const char *postfix)
    {
        JACKPort *jp    = NULL;
        JACKUIPort *jup = NULL;

        switch (port->role)
        {
            case R_MESH:
                jp      = new JACKMeshPort(port, this);
                jup     = new JACKUIMeshPort(jp);
                if (IS_OUT_PORT(port))
                    vSyncPorts.add(jup);
                break;

            case R_MIDI:
            case R_AUDIO:
                jp      = new JACKDataPort(port, this);
                break;

            case R_PATH:
                jp      = new JACKPathPort(port, this);
                jup     = new JACKUIPathPort(jp);
                break;

            case R_CONTROL:
                jp      = new JACKControlPort(port, this);
                jup     = new JACKUIControlPort(jp);
                break;

            case R_METER:
                jp      = new JACKMeterPort(port, this);
                jup     = new JACKUIMeterPort(jp);
                vSyncPorts.add(jup);
                break;

            case R_PORT_SET:
            {
                char postfix_buf[LSP_MAX_PARAM_ID_BYTES];
                JACKPortGroup       *pg      = new JACKPortGroup(port, this);
                pg->init();
                vPorts.add(pg);
                pPlugin->add_port(pg);

                JACKUIPortGroup     *upg     = new JACKUIPortGroup(pg);
                vUIPorts.add(upg);
                pUI->add_port(upg);

                for (size_t row=0; row<pg->rows(); ++row)
                {
                    // Generate postfix
                    snprintf(postfix_buf, sizeof(postfix_buf)-1, "%s_%d", (postfix != NULL) ? postfix : "", int(row));

                    // Clone port metadata
                    port_t *cm          = clone_port_metadata(port->members, postfix_buf);
                    if (cm != NULL)
                    {
                        vGenMetadata.add(cm);

                        for (; cm->id != NULL; ++cm)
                        {
                            if (IS_GROWING_PORT(cm))
                                cm->start    = cm->min + ((cm->max - cm->min) * row) / float(pg->rows());
                            else if (IS_LOWERING_PORT(cm))
                                cm->start    = cm->max - ((cm->max - cm->min) * row) / float(pg->rows());

                            create_port(cm, postfix_buf);
//                            if ((p != NULL) && (p->metadata()->role != R_PORT_SET))
//                                pPlugin->add_port(p);
                        }
                    }
                }

                break;
            }

            default:
                break;
        }

        if (jp != NULL)
        {
            jp->init();
            #ifdef LSP_DEBUG
                const char *src_id = jp->metadata()->id;

                for (size_t i=0; i<vPorts.size(); ++i)
                {
                    if (!strcmp(src_id, vPorts.at(i)->metadata()->id))
                    {
                        lsp_error("ERROR: port %s already defined", src_id);
                    }
                }
            #endif /* LSP_DEBUG */

            vPorts.add(jp);
            pPlugin->add_port(jp);
        }
        if (jup != NULL)
        {
            vUIPorts.add(jup);
            pUI->add_port(jup);
        }
    }

    void JACKWrapper::create_ports(const port_t *meta)
    {
        // Create ports
        for ( ; meta->id != NULL; ++meta)
            create_port(meta, NULL);
    }

    int JACKWrapper::init()
    {
        // Get JACK client
        jack_status_t jack_status;
        pClient     = jack_client_open(pPlugin->get_metadata()->lv2_uid, JackNoStartServer, &jack_status);
        if (pClient == NULL)
        {
            lsp_error("Could not connect to JACK (status=0x%08x)", int(jack_status));
            return STATUS_DISCONNECTED;
        }

        // Set-up shutdown handler
        jack_on_shutdown (pClient, shutdown, this);

        // Get sample rate
        jack_nframes_t sr   = jack_get_sample_rate (pClient);

        // Bind ports
        create_ports(pPlugin->get_metadata()->ports);

        // Initialize plugin and UI
        pPlugin->init(this);
        pUI->init(this);

        // Build plugin UI
        pUI->build();

        // Sync all ports
        size_t n_ui_ports   = vUIPorts.size();
        for (size_t i=0; i<n_ui_ports; ++i)
        {
            IUIPort *p = vUIPorts.at(i);
            if (p != NULL)
                p->notifyAll();
        }

        // Add processing callback
        if (jack_set_process_callback(pClient, process, this) != 0)
        {
            lsp_error("Could not initialize JACK client");
            destroy();
            return STATUS_DISCONNECTED;
        }

        // Set plugin sample rate and call for settings update
        pPlugin->set_sample_rate(sr);
        bUpdateSettings = true;

        // Now we ready for processing
        pPlugin->activate();
        pPlugin->activate_ui();
        if (jack_activate (pClient))
        {
            lsp_error("Could not activate JACK client");
            return STATUS_DISCONNECTED;
        }

        return STATUS_OK;
    }

    void JACKWrapper::destroy()
    {
        // Destroy jack client
        if (pClient != NULL)
        {
            jack_deactivate(pClient);
            jack_client_close(pClient);
            pClient     = NULL;
        }

        // Destroy UI ports
        for (size_t i=0; i<vUIPorts.size(); ++i)
        {
            lsp_trace("destroy ui port id=%s", vUIPorts[i]->metadata()->id);
            delete vUIPorts[i];
        }

        // Destroy ports
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            lsp_trace("destroy port id=%s", vPorts[i]->metadata()->id);
            vPorts[i]->destroy();
            delete vPorts[i];
        }

        // Forget plugin (will be destroyed by caller)
        if (pPlugin != NULL)
        {
            if (pPlugin->ui_active())
                pPlugin->deactivate_ui();
            if (pPlugin->active())
                pPlugin->deactivate();

            pPlugin = NULL;
        }

        // Forget plugin UI
        pUI     = NULL;

        // Destroy executor
        if (pExecutor != NULL)
        {
            pExecutor->shutdown();
            pExecutor   = NULL;
        }
    }

    bool JACKWrapper::transfer_dsp_to_ui()
    {
        if (pClient == NULL)
            return false;

        // Transfer the values of the ports to the UI
        size_t sync = vSyncPorts.size();
        for (size_t i=0; i<sync; ++i)
        {
            JACKUIPort *jup     = vSyncPorts.at(i);
            if (jup->sync())
                jup->notifyAll();
        }

        return true;
    }

    IExecutor *JACKWrapper::get_executor()
    {
        lsp_trace("executor = %p", reinterpret_cast<void *>(pExecutor));
        if (pExecutor != NULL)
            return pExecutor;

        lsp_trace("Creating native executor service");
        pExecutor       = new NativeExecutor();
        return pExecutor;
    }

    canvas_data_t *JACKWrapper::render_inline_display(size_t width, size_t height)
    {
        // Lazy initialization
//        lsp_trace("pCanvas = %p", pCanvas);
        if (pCanvas == NULL)
        {
            pCanvas     =   new CairoCanvas();
            if (pCanvas == NULL)
                return NULL;
//            lsp_trace("pCanvas = %p", pCanvas);
        }

        // Resize canvas, lock for update
        if (!pCanvas->init(width, height))
            return NULL;

        // Call plugin for rendering
        if (!pPlugin->inline_display(pCanvas, width, height))
        {
            // Unlock canvas if possible
            pCanvas->get_data();
//            lsp_trace("failed pPlugin->inline_display");
            return NULL;
        }

        return pCanvas->get_data();
    }
}

#endif /* CONTAINER_JACK_WRAPPER_H_ */
