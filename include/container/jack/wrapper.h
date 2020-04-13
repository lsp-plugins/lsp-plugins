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
#include <core/ICanvas.h>
#include <core/ipc/NativeExecutor.h>
#include <core/ipc/Mutex.h>
#include <container/CairoCanvas.h>

#include <ui/ui.h>
#include <ui/IUIWrapper.h>

#include <data/cvector.h>

#define JACK_INLINE_DISPLAY_SIZE        128

namespace lsp
{
    class JACKPort;
    class JACKUIPort;
    class JACKDataPort;
    class JACKPositionPort;

    class JACKWrapper: public IWrapper, public IUIWrapper
    {
        protected:
            enum state_t
            {
                S_CREATED,
                S_INITIALIZED,
                S_CONNECTED,
                S_CONN_LOST,
                S_DISCONNECTED
            };

        private:
            plugin_t               *pPlugin;
            plugin_ui              *pUI;
            ipc::IExecutor         *pExecutor;
            jack_client_t          *pClient;
            atomic_t                nQueryDraw;
            atomic_t                nQueryDrawLast;
            CairoCanvas            *pCanvas;
            bool                    bUpdateSettings;
            state_t                 nState;
            size_t                  nCounter;
            ssize_t                 nLatency;

            position_t              sPosition;

            cvector<JACKPort>       vPorts;
            cvector<JACKDataPort>   vDataPorts;
            cvector<JACKUIPort>     vUIPorts;
            cvector<JACKUIPort>     vSyncPorts;
            cvector<port_t>         vGenMetadata;   // Generated metadata

            KVTStorage              sKVT;
            ipc::Mutex              sKVTMutex;

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
                nState          = S_CREATED;
                nCounter        = 0;
                nLatency        = 0;

                position_t::init(&sPosition);
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
            static int sync_buffer_size(jack_nframes_t nframes, void *arg);
            static int jack_sync(jack_transport_state_t state, jack_position_t *pos, void *arg);
            static int latency_callback(jack_latency_callback_mode_t mode, void *arg);
            static void shutdown(void *arg);

            int run(size_t samples);
            int sync_position(jack_transport_state_t state, const jack_position_t *pos);
            int latency_callback(jack_latency_callback_mode_t mode);

            void create_port(const port_t *port, const char *postfix);

        public:
            virtual ipc::IExecutor *get_executor();

        public:
            inline jack_client_t *client() { return pClient; };

            status_t init(int argc, const char **argv);
            status_t connect();
            status_t disconnect();

            void destroy();
            void show_ui();
            bool transfer_dsp_to_ui();

            inline bool initialized() const     { return nState != S_CREATED;       }
            inline bool connected() const       { return nState == S_CONNECTED;     }
            inline bool disconnected() const    { return nState == S_DISCONNECTED;  }
            inline bool connection_lost() const { return nState == S_CONN_LOST;     }

            // Inline display interface
            canvas_data_t *render_inline_display(size_t width, size_t height);
            virtual void query_display_draw()
            {
                nQueryDraw++;
            }

            virtual const position_t *position()
            {
                return &sPosition;
            }

            virtual ICanvas *create_canvas(ICanvas *&cv, size_t width, size_t height);

            inline bool test_display_draw()
            {
                atomic_t last       = nQueryDraw;
                bool result         = last != nQueryDrawLast;
                nQueryDrawLast      = last;
                return result;
            }

            /**
             * Lock KVT storage
             * @return pointer to locked storage or NULL
             */
            virtual KVTStorage *kvt_lock();

            /**
             * Try to lock KVT storage and return pointer to the storage on success
             * @return pointer to KVT storage or NULL
             */
            virtual KVTStorage *kvt_trylock();

            /**
             * Release the KVT storage
             * @return true on success
             */
            virtual bool kvt_release();
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
        dsp::context_t ctx;
        int result;

        // Call the plugin for processing
        dsp::start(&ctx);
        JACKWrapper *_this  = reinterpret_cast<JACKWrapper *>(arg);
        result              = _this->run(nframes);
        dsp::finish(&ctx);

        return result;
    }

    int JACKWrapper::sync_buffer_size(jack_nframes_t nframes, void *arg)
    {
        JACKWrapper *_this  = reinterpret_cast<JACKWrapper *>(arg);

        for (size_t i=0, n=_this->vDataPorts.size(); i<n; ++i)
        {
            JACKDataPort *p = _this->vDataPorts.at(i);
            if (p != NULL)
                p->set_buffer_size(nframes);
        }

        return 0;
    }

    int JACKWrapper::jack_sync(jack_transport_state_t state, jack_position_t *pos, void *arg)
    {
        dsp::context_t ctx;
        int result;

        // Call the plugin for processing
        dsp::start(&ctx);
        JACKWrapper *_this  = reinterpret_cast<JACKWrapper *>(arg);
        result              = _this->sync_position(state, pos);
        dsp::finish(&ctx);

        return result;
    }

    int JACKWrapper::latency_callback(jack_latency_callback_mode_t mode, void *arg)
    {
        JACKWrapper *_this  = reinterpret_cast<JACKWrapper *>(arg);
        return _this->latency_callback(mode);
    }

    int JACKWrapper::latency_callback(jack_latency_callback_mode_t mode)
    {
        if (mode == JackCaptureLatency)
        {
            ssize_t latency = pPlugin->get_latency();

            for (size_t i=0, n=vDataPorts.size(); i < n; ++i)
            {
                JACKDataPort *dp = vDataPorts.at(i);
                if (dp == NULL)
                    continue;
                dp->report_latency(latency);
            }
        }

        return 0;
    }

    int JACKWrapper::sync_position(jack_transport_state_t state, const jack_position_t *pos)
    {
        position_t npos = sPosition;

        // Update settings
        npos.speed          = (state == JackTransportRolling) ? 1.0f : 0.0f;
        npos.frame          = pos->frame;

        if (pos->valid & JackPositionBBT)
        {
            npos.numerator      = pos->beats_per_bar;
            npos.denominator    = pos->beat_type;
            npos.beatsPerMinute = pos->beats_per_minute;
            npos.tick           = pos->tick;
            npos.ticksPerBeat   = pos->ticks_per_beat;
        }

//        lsp_trace("numerator = %.3f, denominator = %.3f, bpm = %.3f, tick = %.3f, tpb = %.3f",
//                float(npos.numerator), float(npos.denominator), float(npos.beatsPerMinute),
//                float(npos.tick), float(npos.ticksPerBeat));

        // Call plugin for position update
        if (pPlugin->set_position(&npos))
            bUpdateSettings = true;
        if (pUI != NULL)
            pUI->position_updated(&npos);

        // Update current position
        sPosition = npos;

        return 0;
    }

    int JACKWrapper::run(size_t samples)
    {
        // Prepare ports
        size_t n_ports      = vPorts.size();
        JACKPort **v_ports  = vPorts.get_array();
        for (size_t i=0; i<n_ports; ++i)
        {
            // Get port
            JACKPort *port = v_ports[i];
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

        // Report latency if changed
        ssize_t latency = pPlugin->get_latency();
        if (latency != nLatency)
        {
            jack_recompute_total_latencies(pClient);
            nLatency = latency;
        }

        // Post-process ALL ports
        for (size_t i=0; i<n_ports; ++i)
        {
            JACKPort *port = v_ports[i];
            if (port != NULL)
                port->post_process(samples);
        }
        return 0;
    }

    void JACKWrapper::shutdown(void *arg)
    {
        // Reset the client state
        JACKWrapper *_this  = reinterpret_cast<JACKWrapper *>(arg);
        _this->nState       = S_CONN_LOST;
        lsp_warn("JACK NOTIFICATION: shutdown");
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

            case R_FBUFFER:
                jp      = new JACKFrameBufferPort(port, this);
                jup     = new JACKUIFrameBufferPort(jp);
                if (IS_OUT_PORT(port))
                    vSyncPorts.add(jup);
                break;

            case R_MIDI:
            case R_AUDIO:
            {
                JACKDataPort *jdp = new JACKDataPort(port, this);
                vDataPorts.add(jdp);
                jp      = jdp;
                break;
            }

            case R_OSC:
                jp      = new JACKOscPort(port, this);
                if (IS_OUT_PORT(port))
                {
                    jup     = new JACKUIOscPortIn(jp);
                    vSyncPorts.add(jup);
                }
                else
                    jup     = new JACKUIOscPortOut(jp);
                break;

            case R_PATH:
                jp      = new JACKPathPort(port, this);
                jup     = new JACKUIPathPort(jp);
                break;

            case R_CONTROL:
            case R_BYPASS:
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

                JACKPort **vp = vPorts.get_array();
                for (size_t i=0, n=vPorts.size(); i<n; ++i)
                {
                    if (!::strcmp(src_id, vp[i]->metadata()->id))
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

    status_t JACKWrapper::init(int argc, const char **argv)
    {
        // Create ports
        for (const port_t *meta = pPlugin->get_metadata()->ports ; meta->id != NULL; ++meta)
            create_port(meta, NULL);

        // Initialize plugin and UI
        if (pPlugin != NULL)
            pPlugin->init(this);
        if (pUI != NULL)
        {
            status_t res = pUI->init(this, argc, argv);
            if (res == STATUS_OK)
                res     = pUI->build();
            if (res != STATUS_OK)
            {
                if (res == STATUS_NO_DEVICE)
                    lsp_error("Could not initialize graphical subsystem (display)");
                return res;
            }
        }

        // Update state, mark initialized
        nState      = S_INITIALIZED;

        return STATUS_OK;
    }

    status_t JACKWrapper::connect()
    {
        // Ensure that client identifier is not longer than jack_client_name_size()
        size_t max_client_size = jack_client_name_size();
        char *client_name = static_cast<char *>(alloca(max_client_size));
        strncpy(client_name, pPlugin->get_metadata()->lv2_uid, max_client_size);
        client_name[max_client_size-1] = '\0';

        // Check connection state
        switch (nState)
        {
            case S_CREATED:
                lsp_error("connect() on uninitialized JACK wrapper");
                return STATUS_BAD_STATE;
            case S_CONNECTED:
                return STATUS_OK;

            case S_INITIALIZED:
            case S_DISCONNECTED:
                // OK, valid states
                break;

            case S_CONN_LOST:
                lsp_error("connect() from CONNECTION_LOST state, need to perform disconnect() first");
                return STATUS_BAD_STATE;

            default:
                lsp_error("connect() from invalid state");
                return STATUS_BAD_STATE;
        }

        // Get JACK client
        jack_status_t jack_status;
        pClient     = jack_client_open(client_name, JackNoStartServer, &jack_status);
        if (pClient == NULL)
        {
            lsp_warn("Could not connect to JACK (status=0x%08x)", int(jack_status));
            nState = S_DISCONNECTED;
            return STATUS_DISCONNECTED;
        }

        // Set-up shutdown handler
        jack_on_shutdown(pClient, shutdown, this);

        // Determine size of buffer
        size_t buf_size             = jack_get_buffer_size(pClient);
        if (jack_set_buffer_size_callback(pClient, sync_buffer_size, this))
        {
            lsp_error("Could not setup buffer size callback");
            nState = S_CONN_LOST;
            return STATUS_DISCONNECTED;
        }

        // Connect data ports
        for (size_t i=0, n=vDataPorts.size(); i<n; ++i)
        {
            JACKDataPort *dp = vDataPorts.at(i);
            if (dp != NULL)
            {
                dp->connect();
                dp->set_buffer_size(buf_size);
            }
        }

        // Set plugin sample rate and call for settings update
        jack_nframes_t sr           = jack_get_sample_rate(pClient);
        pPlugin->set_sample_rate(sr);
        sPosition.sampleRate        = sr;
        bUpdateSettings             = true;

        // Now we ready for processing
        if (pPlugin != NULL)
            pPlugin->activate();
        if (pUI != NULL)
            pPlugin->activate_ui();

        // Add processing callback
        if (jack_set_process_callback(pClient, process, this))
        {
            lsp_error("Could not initialize JACK client");
            nState = S_CONN_LOST;
            return STATUS_DISCONNECTED;
        }

        // Setup position synchronization callback
        if (jack_set_sync_callback(pClient, jack_sync, this))
        {
            lsp_error("Could not bind position sync callback");
            nState = S_CONN_LOST;
            return STATUS_DISCONNECTED;
        }

        // Set sync timeout for handler
        if (jack_set_sync_timeout(pClient, 100000)) // 100 msec timeout
        {
            lsp_error("Could not setup sync timeout");
            nState = S_CONN_LOST;
            return STATUS_DISCONNECTED;
        }

        // Update the caption of the UI
        if (pUI != NULL)
        {
            char buf[PATH_MAX];
            const plugin_metadata_t *meta = pPlugin->get_metadata();
            sprintf(buf, "%s %s - %s (Client ID: %s)", LSP_ACRONYM, meta->description, meta->name, jack_get_client_name(pClient));
            pUI->set_title(buf);
        }

        // Sync all ports
        size_t n_ui_ports   = vUIPorts.size();
        for (size_t i=0; i<n_ui_ports; ++i)
        {
            CtlPort *p = vUIPorts.at(i);
            if (p != NULL)
                p->notify_all();
        }

        // Activate JACK client
        if (jack_activate(pClient))
        {
            lsp_error("Could not activate JACK client");
            nState  = S_CONN_LOST;
            return STATUS_DISCONNECTED;
        }

        nState = S_CONNECTED;
        return STATUS_OK;
    }

    status_t JACKWrapper::disconnect()
    {
        // Check connection state
        switch (nState)
        {
            case S_CREATED:
            case S_DISCONNECTED:
            case S_INITIALIZED:
                // OK, valid state
                return STATUS_OK;

            case S_CONNECTED:
            case S_CONN_LOST:
                // OK, perform disconnect
                break;

            default:
                lsp_error("disconnect() from invalid state");
                return STATUS_BAD_STATE;
        }

        // Try to deactivate application
        if (pClient != NULL)
            jack_deactivate(pClient);

        // Deactivate plugin
        if ((pUI != NULL) && (pPlugin->ui_active()))
            pPlugin->deactivate_ui();
        if (pPlugin != NULL)
            pPlugin->deactivate();

        // Try to disconnect all data ports
        for (size_t i=0, n=vDataPorts.size(); i<n; ++i)
        {
            JACKDataPort *dp = vDataPorts.at(i);
            dp->disconnect();
        }

        // Destroy jack client
        if (pClient != NULL)
            jack_client_close(pClient);

        nState      = S_DISCONNECTED;
        pClient     = NULL;

        return STATUS_OK;
    }

    void JACKWrapper::destroy()
    {
        // Disconnect
        disconnect();

        // Destroy UI ports
        for (size_t i=0; i<vUIPorts.size(); ++i)
        {
            lsp_trace("destroy ui port id=%s", vUIPorts[i]->metadata()->id);
            delete vUIPorts[i];
        }
        vUIPorts.clear();

        // Destroy ports
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            lsp_trace("destroy port id=%s", vPorts[i]->metadata()->id);
            vPorts[i]->destroy();
            delete vPorts[i];
        }
        vPorts.clear();

        // Cleanup generated metadata
        for (size_t i=0; i<vGenMetadata.size(); ++i)
        {
            lsp_trace("destroy generated port metadata %p", vGenMetadata[i]);
            drop_port_metadata(vGenMetadata[i]);
        }

        // Clear all other port containers
        vDataPorts.clear();
        vSyncPorts.clear();

        // Forget plugin and UI
        pUI     = NULL;
        pPlugin = NULL;

        // Drop canvas
        if (pCanvas != NULL)
        {
            pCanvas->destroy();
            delete pCanvas;
            pCanvas     = NULL;
        }

        // Destroy executor service
        if (pExecutor != NULL)
        {
            pExecutor->shutdown();
            delete pExecutor;
            pExecutor   = NULL;
        }
    }

    bool JACKWrapper::transfer_dsp_to_ui()
    {
        // Validate state
        dsp::context_t ctx;
        dsp::start(&ctx);

        // Transfer the values of the ports to the UI
        size_t sync = vSyncPorts.size();
        for (size_t i=0; i<sync; ++i)
        {
            JACKUIPort *jup     = vSyncPorts.at(i);
            do {
                if (jup->sync())
                    jup->notify_all();
            } while (jup->sync_again());
        }
        if (pUI != NULL)
        {
            pUI->sync_meta_ports();

            if (sKVTMutex.try_lock())
            {
                // Synchronize DSP -> UI transfer
                size_t sync;
                const char *kvt_name;
                const kvt_param_t *kvt_value;

                do
                {
                    sync = 0;

                    KVTIterator *it = sKVT.enum_tx_pending();
                    while (it->next() == STATUS_OK)
                    {
                        kvt_name = it->name();
                        if (kvt_name == NULL)
                            break;
                        status_t res = it->get(&kvt_value);
                        if (res != STATUS_OK)
                            break;
                        if ((res = it->commit(KVT_TX)) != STATUS_OK)
                            break;

                        kvt_dump_parameter("TX kvt param (DSP->UI): %s = ", kvt_value, kvt_name);
                        pUI->kvt_write(&sKVT, kvt_name, kvt_value);
                        ++sync;
                    }
                } while (sync > 0);

                // Synchronize UI -> DSP transfer
                #ifdef LSP_DEBUG
                {
                    KVTIterator *it = sKVT.enum_rx_pending();
                    while (it->next() == STATUS_OK)
                    {
                        kvt_name = it->name();
                        if (kvt_name == NULL)
                            break;
                        status_t res = it->get(&kvt_value);
                        if (res != STATUS_OK)
                            break;
                        if ((res = it->commit(KVT_RX)) != STATUS_OK)
                            break;

                        kvt_dump_parameter("RX kvt param (UI->DSP): %s = ", kvt_value, kvt_name);
                    }
                }
                #else
                    sKVT.commit_all(KVT_RX);    // Just clear all RX queue for non-debug version
                #endif

                // Call garbage collection and release KVT storage
                sKVT.gc();
                sKVTMutex.unlock();
            }
        }

        // Limit refresh rate of window icon and refresh icon
        if (nCounter++ >= 5)
        {
            // Reset counter first
            nCounter = 0;
            LSPWindow *root = (pUI != NULL) ? pUI->root_window() : NULL;

            if (root != NULL)
            {
                // Check if inline display is present
                canvas_data_t *data = NULL;
                if (test_display_draw())
                    data = render_inline_display(JACK_INLINE_DISPLAY_SIZE, JACK_INLINE_DISPLAY_SIZE);

                // Check that returned data is valid
                if ((data != NULL) && (data->pData != NULL) && (data->nWidth > 0) && (data->nHeight > 0))
                {
                    size_t row_size = data->nWidth * sizeof(uint32_t);

                    if (data->nStride > row_size)
                    {
                        // Compress image data if stride is greater than row size
                        uint8_t *dst = data->pData;
                        for (size_t row = 0; row < data->nHeight; ++row)
                        {
                            uint8_t *p  = &data->pData[row * data->nStride];
                            memmove(dst, p, row_size);
                        }
                    }

                    root->set_icon(data->pData, data->nWidth, data->nHeight);
                }
            } // root != NULL
        }

        dsp::finish(&ctx);

        return true;
    }

    ipc::IExecutor *JACKWrapper::get_executor()
    {
        lsp_trace("executor = %p", reinterpret_cast<void *>(pExecutor));
        if (pExecutor != NULL)
            return pExecutor;

        lsp_trace("Creating native executor service");
        ipc::NativeExecutor *exec = new ipc::NativeExecutor();
        if (exec == NULL)
            return NULL;
        if (exec->start() != STATUS_OK)
        {
            delete exec;
            return NULL;
        }
        return pExecutor = exec;
    }

    canvas_data_t *JACKWrapper::render_inline_display(size_t width, size_t height)
    {
        // Check for Inline display support
        const plugin_metadata_t *meta = pPlugin->get_metadata();
        if ((meta == NULL) || (!(meta->extensions & E_INLINE_DISPLAY)))
            return NULL;

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

    ICanvas *JACKWrapper::create_canvas(ICanvas *&cv, size_t width, size_t height)
    {
        if ((cv != NULL) && (cv->width() == width) && (cv->height() == height))
            return cv;

        ICanvas *ncv = new CairoCanvas();
        if (ncv == NULL)
            return NULL;
        if (!ncv->init(width, height))
        {
            delete ncv;
            return NULL;
        }

        if (cv != NULL)
        {
            cv->destroy();
            delete cv;
        }

        return cv = ncv;
    }

    KVTStorage *JACKWrapper::kvt_lock()
    {
        return (sKVTMutex.lock()) ? &sKVT : NULL;
    }

    KVTStorage *JACKWrapper::kvt_trylock()
    {
        return (sKVTMutex.try_lock()) ? &sKVT : NULL;
    }

    bool JACKWrapper::kvt_release()
    {
        return sKVTMutex.unlock();
    }

    void JACKWrapper::show_ui()
    {
        for (size_t i=0, n=vUIPorts.size(); i<n; ++i)
        {
            JACKUIPort *port = vUIPorts.at(i);
            if (port != NULL)
                port->notify_all();
        }

        size_request_t r;
        LSPWindow *wnd      = pUI->root_window();
        LSPDisplay *dpy     = wnd->display();

        // Limit window size
        wnd->size_request(&r);

        // Set location and size of window
        ssize_t w, h;
        if (dpy->screen_size(wnd->screen(), &w, &h) == STATUS_OK)
        {
            w = (w - r.nMinWidth) >> 1;
            h = (h - r.nMinHeight) >> 1;
            wnd->set_geometry(w, h, r.nMinWidth, r.nMinHeight);
        }
        else
            wnd->resize(r.nMinWidth, r.nMinHeight);

        pUI->show();
    }
}

#endif /* CONTAINER_JACK_WRAPPER_H_ */
