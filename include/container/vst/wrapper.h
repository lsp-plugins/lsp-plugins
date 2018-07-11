/*
 * wrapper.h
 *
 *  Created on: 08 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_VST_WRAPPER_H_
#define CONTAINER_VST_WRAPPER_H_

#include <container/vst/defs.h>
#include <core/NativeExecutor.h>

namespace lsp
{
    class VSTAudioPort;
    class VSTParameterPort;
    class VSTPort;
    class VSTUIPort;

    class VSTWrapper: public IWrapper, public IUIWrapper
    {
        private:
            plugin_t                   *pPlugin;
            AEffect                    *pEffect;
            plugin_ui                  *pUI;
            char                       *pBundlePath;
            GtkWidget                  *pUIWidget;
            GtkWidget                  *pWidget;
            GdkWindow                  *pParent;
            guint                       pTimer;
            ERect                       sRect;
            audioMasterCallback         pMaster;
            IExecutor                  *pExecutor;
            vst_state_buffer           *pState;

            cvector<VSTAudioPort>       vInputs;        // List of input audio ports
            cvector<VSTAudioPort>       vOutputs;       // List of output audio ports
            cvector<VSTParameterPort>   vParams;        // List of controllable parameters
            cvector<VSTPort>            vPorts;         // List of all created VST ports
            cvector<VSTPort>            vProxyPorts;    // List of all created VST proxy ports
            cvector<VSTUIPort>          vUIPorts;       // List of all created UI ports
            cvector<port_t>             vGenMetadata;   // Generated metadata

        private:
            static gboolean transport_synchronize(gpointer arg);
            void transfer_dsp_to_ui();
            void create_transport_thread();
            void drop_transport_thread();

            VSTPort *create_port(const port_t *port, const char *postfix);
            void create_ports(const port_t *meta);

        public:
            VSTWrapper(
                    AEffect *effect,
                    plugin_t *plugin,
                    const char *bundle_path,
                    const char *name,
                    audioMasterCallback callback
            )
            {
                pPlugin         = plugin;
                pEffect         = effect;
                pUI             = NULL;
                pBundlePath     = strdup(bundle_path);
                pUIWidget       = NULL;
                pWidget         = NULL;
                pParent         = NULL;
                pTimer          = 0;
                pMaster         = callback;
                pExecutor       = NULL;
                pState          = NULL;
                sRect.top       = 0;
                sRect.left      = 0;
                sRect.bottom    = 0;
                sRect.right     = 0;
            }

            virtual ~VSTWrapper()
            {
                pPlugin         = NULL;
                pEffect         = NULL;
                pUI             = NULL;
                pBundlePath     = NULL;
                pUIWidget       = NULL;
                pWidget         = NULL;
                pParent         = NULL;
                pTimer          = 0;
                pMaster         = NULL;
            }

        public:
            inline const plugin_metadata_t *get_metadata() const    {   return pPlugin->get_metadata();     };
            inline VSTParameterPort *get_parameter(size_t index)    {   return vParams[index];              };
            inline const char *get_bundle_path() const              {   return pBundlePath;                 };

            void init();
            void destroy();
            inline void open() { };
            void run(float** inputs, float** outputs, size_t samples);
            void run_legacy(float** inputs, float** outputs, size_t samples);
            void process_events(const VstEvents *e);

            inline void set_sample_rate(float sr)
            {
                pPlugin->set_sample_rate(sr);
            }

            inline void mains_changed(VstIntPtr value)
            {
                if (value)
                    pPlugin->activate();
                else
                    pPlugin->deactivate();
            }

            bool show_ui(void *wnd, IWidgetFactory *wf);
            void hide_ui();
            void iterate_ui();
            void destroy_ui();
            inline ERect *get_ui_rect()                 { return &sRect;                        };

            virtual IExecutor *get_executor()
            {
                lsp_trace("executor = %p", reinterpret_cast<void *>(pExecutor));
                if (pExecutor != NULL)
                    return pExecutor;

                lsp_trace("Creating native executor service");
                pExecutor       = new NativeExecutor();
                return pExecutor;
            }

            void init_state_chunk();
            size_t serialize_state(const void **dst);
            void deserialize_state(const void *data);
    };
}

// Here Port description should be included
#include <container/vst/ports.h>
#include <container/vst/ui_ports.h>

namespace lsp
{
    VSTPort *VSTWrapper::create_port(const port_t *port, const char *postfix)
    {
        VSTPort *vp = NULL;
        VSTUIPort *vup = NULL;

        switch (port->role)
        {
            case R_MESH:
                vp  = new VSTMeshPort(port, pEffect, pMaster);
                vup = new VSTUIMeshPort(port, vp);
                break;

            case R_MIDI:
                if (IS_OUT_PORT(port))
                    vp = new VSTMidiOutputPort(port, pEffect, pMaster);
                else
                {
                    pEffect->flags         |= effFlagsIsSynth;
                    vp = new VSTMidiInputPort(port, pEffect, pMaster);
                }
                break;

            case R_PATH:
                vp  = new VSTPathPort(port, pEffect, pMaster);
                vup = new VSTUIPathPort(port, vp);
                break;

            case R_AUDIO:
                vp = new VSTAudioPort(port, pEffect, pMaster);
                break;

            case R_CONTROL:
            case R_METER:
                // VST specifies only INPUT parameters, output should be read in different way
                if (IS_OUT_PORT(port))
                {
                    vp      = new VSTMeterPort(port, pEffect, pMaster);
                    vup     = new VSTUIMeterPort(port, vp);
                }
                else
                {
                    vp      = new VSTParameterPort(port, pEffect, pMaster);
                    vup     = new VSTUIParameterPort(port, static_cast<VSTParameterPort *>(vp));
                }
                break;

            case R_PORT_SET:
            {
                char postfix_buf[LSP_MAX_PARAM_ID_BYTES];
                VSTPortGroup       *pg      = new VSTPortGroup(port, pEffect, pMaster);
                pPlugin->add_port(pg);

                VSTUIPortGroup     *upg     = new VSTUIPortGroup(pg);
                vUIPorts.add(upg);

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

                            VSTPort *p = create_port(cm, postfix_buf);
                            if ((p != NULL) && (p->metadata()->role != R_PORT_SET))
                                pPlugin->add_port(p);
                        }
                    }
                }

                vp      = pg;
                break;
            }

            default:
                break;
        }

        if (vp != NULL)
            vPorts.add(vp);
        if (vup != NULL)
            vUIPorts.add(vup);

        return vp;
    }

    void VSTWrapper::create_ports(const port_t *meta)
    {
        for ( ; meta->id != NULL; ++meta)
        {
            VSTPort *vp = create_port(meta, NULL);
            if (vp == NULL)
                continue;

            switch (meta->role)
            {
                case R_PORT_SET:
                    break;

                case R_MESH:
                case R_MIDI:
                case R_PATH:
                    pPlugin->add_port(vp);
                    break;

                case R_AUDIO:
                    pPlugin->add_port(vp);
                    if (IS_OUT_PORT(meta))
                        vOutputs.add(static_cast<VSTAudioPort *>(vp));
                    else
                        vInputs.add(static_cast<VSTAudioPort *>(vp));
                    break;

                case R_CONTROL:
                case R_METER:
                    pPlugin->add_port(vp);
                    if (IS_IN_PORT(meta)) // VST specifies only INPUT parameters, output should be read in different way
                        vParams.add(static_cast<VSTParameterPort *>(vp));
                    break;

                default:
                    break;
            }
        }
    }

    void VSTWrapper::init()
    {
        AEffect *e                      = pEffect;
        const plugin_metadata_t *m      = pPlugin->get_metadata();

        // Bind ports
        lsp_trace("Binding ports");
        create_ports(m->ports);

        // Update instance parameters
        e->numInputs                    = vInputs.size();
        e->numOutputs                   = vOutputs.size();
        e->numParams                    = vParams.size();

        // Generate IDs for parameter ports
        for (ssize_t id=0; id < e->numParams; ++id)
            vParams[id]->setID(id);

        // Initialize state chunk
        init_state_chunk();

        // Initialize plugin
        pPlugin->init(this);
    }

    void VSTWrapper::destroy()
    {
        // First destroy the UI
        destroy_ui();

        // Shutdown and delete executor if exists
        if (pExecutor != NULL)
        {
            pExecutor->shutdown();
            delete pExecutor;
            pExecutor   = NULL;
        }

        // Destrop plugin
        lsp_trace("destroying plugin");
        if (pPlugin != NULL)
        {
            pPlugin->destroy();
            delete pPlugin;

            pPlugin = NULL;
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
            delete vPorts[i];
        }

        // Cleanup generated metadata
        for (size_t i=0; i<vGenMetadata.size(); ++i)
        {
            lsp_trace("destroy generated port metadata %p", vGenMetadata[i]);
            drop_port_metadata(vGenMetadata[i]);
        }

        // Clear all port lists
        vInputs.clear();
        vOutputs.clear();
        vParams.clear();
        vPorts.clear();
        vUIPorts.clear();

        if (pBundlePath != NULL)
        {
            free(pBundlePath);
            pBundlePath     = NULL;
        }

        if (pState != NULL)
        {
            delete [] reinterpret_cast<uint8_t *>(pState);
            pState          = NULL;
        }

        pMaster     = NULL;
        pEffect     = NULL;

        lsp_trace("destroy complete");
    }

    void VSTWrapper::run(float** inputs, float** outputs, size_t samples)
    {
        // Sync UI state
        if (pUI != NULL)
        {
            if (!pPlugin->ui_active())
                pPlugin->activate_ui();
        }
        else if (pPlugin->ui_active())
            pPlugin->deactivate_ui();

        // Bind audio ports
        size_t n_inputs = vInputs.size();
        for (size_t i=0; i < n_inputs; ++i)
        {
            VSTAudioPort *p = vInputs.at(i);
            if (p != NULL)
                p->bind(inputs[i]);
        }
        size_t n_outputs = vOutputs.size();
        for (size_t i=0; i < n_outputs; ++i)
        {
            VSTAudioPort *p = vOutputs.at(i);
            if (p != NULL)
                p->bind(outputs[i]);
        }

        // Process ALL ports for changes
        bool update = false;

        size_t n_ports = vPorts.size();
        for (size_t i=0; i<n_ports; ++i)
        {
            // Get port
            VSTPort *port = vPorts.at(i);
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

        // Report latency
        pEffect->initialDelay   = VstInt32(pPlugin->get_latency());

        // Post-process ALL ports
        for (size_t i=0; i<n_ports; ++i)
        {
            VSTPort *port = vPorts.at(i);
            if (port != NULL)
                port->post_process(samples);
        }
    }

    void VSTWrapper::process_events(const VstEvents *e)
    {
        // We need to deliver MIDI events to MIDI ports
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            VSTPort *p          = vPorts[i];
            const port_t *meta  = p->metadata();

            // Find MIDI port(s)
            if (!IS_IN_PORT(meta))
                continue;
            if (meta->role != R_MIDI)
                continue;

            // Call for event processing
            VSTMidiInputPort *mp    = static_cast<VSTMidiInputPort *>(p);
            mp->deserialize(e);
        }
    }

    void VSTWrapper::run_legacy(float** inputs, float** outputs, size_t samples)
    {
        run(inputs, outputs, samples);
    }

    bool VSTWrapper::show_ui(void *wnd, IWidgetFactory *wf)
    {
        lsp_trace("show ui");
        const plugin_metadata_t *m  = pPlugin->get_metadata();

        if (pUI == NULL)
        {
            // GTK Init
            static bool gtk_initialized = false;
            if (!gtk_initialized)
            {
                lsp_trace("init gtk");
                int argc = 0;
                gtk_init_check(&argc, NULL);
                gtk_initialized     = true;
            }

            // Create UI pointer
            lsp_trace("create ui");
            pUI                         = new plugin_ui(m, wf);
            if (pUI == NULL)
                return false;

            // Initialize UI
            lsp_trace("init ui");
            pUI->init(this);
            // Add pre-generated ports
            for (size_t i=0; i<vUIPorts.size(); ++i)
            {
                VSTUIPort  *vp      = vUIPorts[i];
                lsp_trace("Adding UI port id=%s", vp->metadata()->id);
                vp->resync();
                pUI->add_port(vp);
            }

            // Build UI
            lsp_trace("build ui");
            pUI->build();

            // Create window
            lsp_trace("create window");
            pWidget                     = gtk_window_new (GTK_WINDOW_TOPLEVEL);
            if (pWidget == NULL)
                return false;

            // Realize GTK window
            if (!gtk_widget_get_realized(pWidget))
            {
                gtk_widget_realize(pWidget);
                g_assert(gtk_widget_get_realized(pWidget));
            }

            // Configure GTK window
            lsp_trace("set attributes pWidget=%p", pWidget);
            gchar *title                = g_strdup_printf(LSP_ACRONYM " %s - %s [VST]", m->name, m->description);
            gtk_window_set_title(GTK_WINDOW(pWidget), title);
            gtk_window_set_default_size (GTK_WINDOW (pWidget), 64, 64);
            gtk_container_set_border_width (GTK_CONTAINER (pWidget), 0);
            g_free(title);

            // Get widget
            lsp_trace("create widget hierarchy root=%p", wf->root_widget());
            pUIWidget                   = reinterpret_cast<GtkWidget *>(wf->root_widget());
            gtk_container_add(GTK_CONTAINER(pWidget), pUIWidget);
//            g_object_ref(pWidget);

            // Reparent window
            gdk_display_sync(gdk_display_get_default());
            pParent                     = gdk_window_foreign_new(GdkNativeWindow(uintptr_t(wnd)));
            g_assert(pParent);
            gdk_window_reparent(gtk_widget_get_window(pWidget), pParent, 0, 0);
        }

        // Show window
        lsp_trace("create widget hierarchy pWidget=%p", pWidget);
        gtk_widget_show_all(pWidget);
        gdk_display_sync(gdk_display_get_default());

        {
            // Get size of widget
            GtkRequisition rq;
            gtk_widget_size_request(pUIWidget, &rq);
            lsp_trace("window width=%d, height=%d", int(rq.width), int(rq.height));

            // Change title
            gchar *title                = g_strdup_printf(LSP_ACRONYM " %s - %s [VST]", m->name, m->description);
            gdk_window_set_title(pParent, title);
            g_free(title);

            // Update geometry
            GdkGeometry g;
            g.min_width         = rq.width;
            g.min_height        = rq.height;
            g.max_width         = rq.width;
            g.max_height        = rq.height;
            g.base_width        = rq.width;
            g.base_height       = rq.height;
            g.width_inc         = 0;
            g.height_inc        = 0;

            gdk_window_resize(pParent, rq.width, rq.height);

            gdk_window_set_keep_above(pParent, TRUE);
            gdk_window_set_geometry_hints(
                pParent, &g, GdkWindowHints(GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE | GDK_HINT_RESIZE_INC | GDK_HINT_BASE_SIZE)
            );

            gdk_display_sync(gdk_display_get_default());

            // Update rect geometry
            sRect.top           = 0;
            sRect.left          = 0;
            sRect.bottom        = rq.height - 1;
            sRect.right         = rq.width - 1;
        }

        // Transfer state
        transfer_dsp_to_ui();

        // Create thread
        create_transport_thread();

        return true;
    }

    void VSTWrapper::destroy_ui()
    {
        lsp_trace("destroy ui");

        // Drop transport thread
        drop_transport_thread();

        // Hide window
        if (pWidget != NULL)
        {
            // Remove UI
            lsp_trace("Hiding (unmapping) window");
            GdkWindow *win = gtk_widget_get_window(pWidget);
            if (win != NULL)
                gdk_window_hide(win);

            // Destroy window
            gtk_widget_destroy(pWidget);
            pWidget     = NULL;
        }

        // Sync display (if possible)
        lsp_trace("Syncing display");
        GdkDisplay * display = gdk_display_get_default();
        if ((display != NULL) && (GDK_IS_DISPLAY(display)))
            gdk_display_sync(display);

        // Forget UI widget
        if (pUIWidget != NULL)
            pUIWidget = NULL;

        // Destroy parent
        if (pParent != NULL)
            pParent     = NULL;

        // Destroy UI
        if (pUI != NULL)
        {
            pUI->destroy();
            delete pUI;
            pUI         = NULL;
        }

        // Unbind all UI ports
        for (size_t i=0; i<vUIPorts.size(); ++i)
            vUIPorts[i]->unbindAll();
    }

    void VSTWrapper::iterate_ui()
    {
        // Check that UI is active
        if ((pUI == NULL) || (pWidget == NULL))
            return;

        // Update size of widget
        if (pUIWidget != NULL)
        {
            GtkAllocation       alloc;
            gtk_widget_get_allocation(pUIWidget, &alloc);

            sRect.top           = alloc.x;
            sRect.left          = alloc.y;
            sRect.bottom        = alloc.y + alloc.height - 1;
            sRect.right         = alloc.x + alloc.width - 1;
        }

        // Iterate GTK cycle(s)
        while (gtk_events_pending ())
        {
            // Call GTK iteration
            gtk_main_iteration();
        }
    }

    void VSTWrapper::hide_ui()
    {
        destroy_ui();
    }

    void VSTWrapper::transfer_dsp_to_ui()
    {
        // Get number of ports
        if (pUI == NULL)
            return;

        size_t ports_count  = vUIPorts.size();

        // DSP -> UI communication
        for (size_t i=0; i < ports_count; ++i)
        {
            // Get UI port
            VSTUIPort *vup          = vUIPorts[i];
            if (vup == NULL)
                continue;

            if (vup->sync())
                vup->notifyAll();
        } // for port_id
    }

    void VSTWrapper::create_transport_thread()
    {
        drop_transport_thread();
        pTimer = g_timeout_add (1000 / MESH_REFRESH_RATE, transport_synchronize, this);
        lsp_trace("added transport=%ld", long(pTimer));
    }

    void VSTWrapper::drop_transport_thread()
    {
        if (pTimer > 0)
        {
            lsp_trace("cancelling transport=%ld", long(pTimer));
            g_source_remove(pTimer);
            pTimer      = 0;
        }
    }

    gboolean VSTWrapper::transport_synchronize(gpointer arg)
    {
        VSTWrapper *_this = reinterpret_cast<VSTWrapper *>(arg);
        _this->transfer_dsp_to_ui();
        return TRUE;
    }

    void VSTWrapper::init_state_chunk()
    {
        // Calculate the overall maximum size of the chunk
        size_t chunk_size       = 0;
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            size_t p_size           = vPorts[i]->serial_size();
            if (p_size > 0)
            {
                chunk_size             += p_size;
                chunk_size             += LSP_MAX_PARAM_ID_BYTES;
            }
        }

        // Allocate chunk
        if (chunk_size <= 0)
            return;
        chunk_size                      = ALIGN_SIZE(chunk_size, DEFAULT_ALIGN);
        size_t alloc_size               = chunk_size + sizeof(vst_state_buffer);
        uint8_t *data                   = new uint8_t[alloc_size];
        if (data == NULL)
            return;

        // Update state
        pEffect->flags                 |= effFlagsProgramChunks;
        pState                          = reinterpret_cast<vst_state_buffer *>(data);

        // Initialize state with constant values
        pState->nDataSize               = chunk_size;

        memset(&pState->sHeader, 0x00, sizeof(fxBank));
        pState->sHeader.chunkMagic      = BE_DATA(VstInt32(VST_CHUNK_MAGIC));
        pState->sHeader.byteSize        = 0;
        pState->sHeader.fxMagic         = BE_DATA(VstInt32(VST_OPAQUE_BANK_MAGIC));
        pState->sHeader.version         = BE_DATA(1);
        pState->sHeader.fxID            = BE_DATA(VstInt32(pEffect->uniqueID));
        pState->sHeader.fxVersion       = BE_DATA(VstInt32(pEffect->version));
        pState->sHeader.numPrograms     = 0;

        pState->sState.nItems           = 0;
    }


    #ifdef LSP_TRACE
        static void dump_vst_bank(const fxBank *bank)
        {
            size_t ck_size              = BE_DATA(bank->byteSize) + 2 * sizeof(VstInt32);

            const uint8_t *ddump        = reinterpret_cast<const uint8_t *>(bank);
            lsp_trace("Chunk dump:");

            for (size_t offset=0; offset < ck_size; offset += 16)
            {
                // Print HEX dump
                lsp_printf("%08x: ", int(offset));
                for (size_t i=0; i<0x10; ++i)
                {
                    if ((offset + i) < ck_size)
                        lsp_printf("%02x ", int(ddump[i]));
                    else
                        lsp_printf("   ");
                }
                lsp_printf("   ");

                // Print character dump
                for (size_t i=0; i<0x10; ++i)
                {
                    if ((offset + i) < ck_size)
                    {
                        uint8_t c   = ddump[i];
                        if ((c < 0x20) || (c >= 0x80))
                            c           = '.';
                        lsp_printf("%c", c);
                    }
                    else
                        lsp_printf(" ");
                }
                lsp_printf("\n");

                // Move pointer
                ddump       += 0x10;
            }
        }
    #else
        #define dump_vst_bank(...)
    #endif /* LSP_TRACE */

    size_t VSTWrapper::serialize_state(const void **dst)
    {
        if (pState == NULL)
            return 0;

        uint8_t *ptr                    = pState->sState.vData;
        uint8_t *tail                   = reinterpret_cast<uint8_t *>(&pState->sState) + pState->nDataSize;
        size_t params                   = 0;

        for (size_t i=0; i<vPorts.size(); ++i)
        {
            // Get VST port
            VSTPort *vp             = vPorts[i];
            if (vp == NULL)
                continue;

            // Get metadata
            const port_t *p         = vp->metadata();
            if ((p == NULL) || (p->id == NULL) || (IS_OUT_PORT(p)))
                continue;

            // Check that port is serializable
            size_t p_size           = vp->serial_size();
            if (p_size <= 0)
                continue;

            lsp_trace("Serializing port id=%s", p->id);

            // Write ID of the port
            ssize_t delta           = vst_serialize_string(p->id, ptr, tail - ptr);
            if (delta < 0)
            {
                lsp_error("Error serializing port id=%s", p->id);
                return 0;
            }
            ptr                    += delta;

            // Serialize port
            delta                   = vp->serialize(ptr, tail - ptr);
            if (delta < 0)
            {
                lsp_error("Error serializing port id=%s", p->id);
                return 0;
            }
            ptr                    += delta;

            // Increment number of params
            params                  ++;
        }

        // Write the size of chunk
        pState->sState.nItems           = BE_DATA(uint32_t(params));
        pState->sHeader.byteSize        = BE_DATA(VstInt32(ptr - reinterpret_cast<uint8_t *>(&pState->sState) + VST_BANK_HDR_SIZE));
        size_t ck_size                  = ptr - reinterpret_cast<uint8_t *>(&pState->sHeader);

        dump_vst_bank(&pState->sHeader);

        // Return result
        *dst = &pState->sHeader;
        return ck_size;
    }

    void VSTWrapper::deserialize_state(const void *data)
    {
        const fxBank *bank          = reinterpret_cast<const fxBank *>(data);
        dump_vst_bank(bank);

        // Validate chunkMagic
        if (bank->chunkMagic != BE_DATA(VST_CHUNK_MAGIC))
        {
            lsp_trace("bank->chunkMagic (%08x) != BE_DATA(VST_CHUNK_MAGIC) (%08x)", int(bank->chunkMagic), int(BE_DATA(VST_CHUNK_MAGIC)));
            return;
        }

        // Get size of chunk
        size_t byte_size                = BE_DATA(VstInt32(bank->byteSize));
        if (byte_size < VST_STATE_BUFFER_SIZE)
        {
            lsp_trace("byte_size (%d) < VST_STATE_BUFFER_SIZE (%d)", int(byte_size), int(VST_STATE_BUFFER_SIZE));
            return;
        }

        // Validate fxMagic
        if (bank->fxMagic != BE_DATA(VST_OPAQUE_BANK_MAGIC))
        {
            lsp_trace("bank->fxMagic (%08x) != BE_DATA(VST_OPAQUE_BANK_MAGIC) (%08x)", int(bank->fxMagic), int(BE_DATA(VST_OPAQUE_BANK_MAGIC)));
            return;
        }

        // Validate fxID
        if (bank->fxID != BE_DATA(VstInt32(pEffect->uniqueID)))
        {
            lsp_trace("bank->fxID (%08x) != BE_DATA(VstInt32(pEffect->uniqueID)) (%08x)", int(bank->fxID), int(BE_DATA(VstInt32(pEffect->uniqueID))));
            return;
        }

        // Validate the version
        VstInt32 version  = BE_DATA(bank->version);
        if (version > pEffect->version)
        {
            lsp_error("Unsupported effect version (%d)", version);
            return;
        }

        // Validate the numParams
        if (bank->numPrograms != 0)
        {
            lsp_trace("bank->numPrograms (%d) != 0", int(bank->numPrograms));
            return;
        }

        // Ready to de-serialize
        const vst_state *state  = reinterpret_cast<const vst_state *>(bank + 1);
        size_t params           = BE_DATA(state->nItems);
        const uint8_t *ptr      = state->vData;
        const uint8_t *tail     = reinterpret_cast<const uint8_t *>(state) + byte_size - sizeof(vst_state);
        char param_id[LSP_MAX_PARAM_ID_BYTES];

        while ((params--) > 0)
        {
            // Deserialize port ID
            ssize_t delta           = vst_deserialize_string(param_id, LSP_MAX_PARAM_ID_BYTES, ptr, tail - ptr);
            if (delta <= 0)
            {
                lsp_error("Bank data corrupted");
                return;
            }
            ptr                    += delta;

            // Find port
            lsp_trace("Deserializing port id=%s", param_id);
            VSTPort *vp             = NULL;
            for (size_t i=0; i< vPorts.size(); ++i)
            {
                // Get VST port
                VSTPort *sp             = vPorts[i];
                if (sp == NULL)
                    continue;

                // Get port metadata
                const port_t *p         = sp->metadata();
                if ((p == NULL) || (p->id == NULL))
                    continue;

                // Check that ID of the port matches
                if (!strcmp(p->id, param_id))
                {
                    vp                      = sp;
                    break;
                }
            }

            if (vp == NULL)
            {
                lsp_error("Bank data corrupted: port id=%s not found", param_id);
                return;
            }

            // Deserialize port data
            delta                   = vp->deserialize(ptr, tail - ptr);
            if (delta <= 0)
            {
                lsp_error("bank data corrupted, could not deserialize port id=%s", param_id);
                return;
            }
            ptr                    += delta;
        }
    }
}

#endif /* CONTAINER_VST_WRAPPER_H_ */
