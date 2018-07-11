/*
 * object.h
 *
 *  Created on: 08 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTANIER_VST_OBJECT_H_
#define CONTAINER_VST_OBJECT_H_

#include <container/vst/defs.h>

namespace lsp
{
    class VSTAudioPort;
    class VSTParameterPort;
    class VSTPort;
    class VSTUIPort;

    class VSTWrapper
    {
        private:
            plugin_t                   *pPlugin;
            AEffect                    *pEffect;
            plugin_ui                  *pUI;
            const char                 *pName;
            char                       *pBundlePath;
            GtkWidget                  *pUIWidget;
            GtkWidget                  *pWidget;
            GdkWindow                  *pParent;
            guint                       pTimer;
            ERect                       sRect;
            audioMasterCallback         pMaster;

            cvector<VSTAudioPort>       vInputs;    // List of input audio ports
            cvector<VSTAudioPort>       vOutputs;   // List of output audio ports
            cvector<VSTParameterPort>   vParams;    // List of controllable parameters
            cvector<VSTPort>            vPorts;     // List of all created VST ports
            cvector<VSTUIPort>          vUIPorts;   // List of all created UI ports

        private:
            static gboolean transport_synchronize(gpointer arg);
            void transfer_dsp_to_ui();
            void create_transport_thread();
            void drop_transport_thread();
            void add_ui_port(VSTUIPort *p);

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
                pName           = name;
                pBundlePath     = strdup(bundle_path);
                pUIWidget       = NULL;
                pWidget         = NULL;
                pParent         = NULL;
                pTimer          = 0;
                pMaster         = callback;
                sRect.top       = 0;
                sRect.left      = 0;
                sRect.bottom    = 0;
                sRect.right     = 0;
            }

            ~VSTWrapper()
            {
                pPlugin         = NULL;
                pEffect         = NULL;
                pUI             = NULL;
                pName           = NULL;
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

            inline void set_sample_rate(float sr)
            {
                pPlugin->set_sample_rate(sr);
            }

            void set_parameter(size_t index, float value);

            inline void mains_changed(VstIntPtr value)
            {
                if (value)
                    pPlugin->activate();
                else
                    pPlugin->deactivate();
            }

            bool show_ui(void *wnd, IWidgetFactory *wf);
            void init_ui();
            void hide_ui();
            void iterate_ui();
            void destroy_ui();
            inline ERect *get_ui_rect()                 { return &sRect;                        };
    };
}

// Here Port description should be included
#include <container/vst/ports.h>
#include <container/vst/ui_ports.h>

namespace lsp
{
    void VSTWrapper::init()
    {
        AEffect *e                      = pEffect;
        const plugin_metadata_t *m      = pPlugin->get_metadata();

        // Bind ports
        lsp_trace("Binding ports");
        for (const port_t *port = m->ports; (port->id != NULL) && (port->name != NULL); ++port)
        {
            bool out = (port->flags & F_OUT);
            lsp_trace("Binding port %s", port->id);

            switch (port->role)
            {
                case R_UI_SYNC:
                    lsp_error("R_UI_SYNC found in port list");
                    break;
                case R_MESH:
                {
                    VSTMeshPort *vp = new VSTMeshPort(port, e, pMaster);
                    pPlugin->add_port(vp);
                    vPorts.add(vp);
                    break;
                }

                case R_AUDIO:
                {
                    VSTAudioPort *vp = new VSTAudioPort(port, e, pMaster);
                    pPlugin->add_port(vp);
                    if (out)
                        vOutputs.add(vp);
                    else
                        vInputs.add(vp);
                    vPorts.add(vp);
                    break;
                }

                case R_CONTROL:
                case R_METER:
                    // VST specifies only INPUT parameters, output should be read in different way
                    if (out)
                    {
                        VSTMeterPort *vp = new VSTMeterPort(port, e, pMaster);
                        pPlugin->add_port(vp);
                        vPorts.add(vp);
                    }
                    else
                    {
                        VSTParameterPort *vp = new VSTParameterPort(port, e, pMaster);
                        pPlugin->add_port(vp);
                        vPorts.add(vp);
                        if (!out)
                            vParams.add(vp);
                    }
                    break;

                default:
                    break;
            }
        }

        // Update instance parameters
        e->numInputs                    = vInputs.size();
        e->numOutputs                   = vOutputs.size();
        e->numParams                    = vParams.size();

        // Generate IDs for parameter ports
        for (ssize_t id=0; id < e->numParams; ++id)
            vParams[id]->setID(id);
    }

    void VSTWrapper::destroy()
    {
        // First destroy the UI
        destroy_ui();

        // Destrop plugin
        lsp_trace("destroying plugin");
        if (pPlugin != NULL)
        {
            pPlugin->destroy();
            delete pPlugin;

            pPlugin = NULL;
        }

        // Destroy ports
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            lsp_trace("destroy port id=%s", vPorts[i]->metadata()->id);
            delete vPorts[i];
        }

        // Clear all port lists
        vInputs.clear();
        vOutputs.clear();
        vParams.clear();
        vPorts.clear();

        if (pBundlePath != NULL)
        {
            free(pBundlePath);
            pBundlePath     = NULL;
        }

        pMaster     = NULL;
        pEffect     = NULL;
        pName       = NULL;

        lsp_trace("destroy complete");
    }

    void VSTWrapper::run(float** inputs, float** outputs, size_t samples)
    {
        // Bind audio ports
        for (size_t i=0; i < vInputs.size(); ++i)
        {
            VSTAudioPort *p = vInputs[i];
            if (p != NULL)
                p->bind(inputs[i]);
        }
        for (size_t i=0; i < vOutputs.size(); ++i)
        {
            VSTAudioPort *p = vOutputs[i];
            if (p != NULL)
                p->bind(outputs[i]);
        }

        // Process external ports for changes
        bool update = false;

        for (size_t i=0; i<vPorts.size(); ++i)
        {
            // Get port
            VSTPort *port = vPorts[i];
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

        // Process external ports for changes
        for (size_t i=0; i<vPorts.size(); ++i)
        {
            VSTPort *port = vPorts[i];
            if (port != NULL)
                port->post_process(samples);
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
            // Create UI pointer
            lsp_trace("create ui");
            pUI                         = new plugin_ui(pName, m, wf);
            if (pUI == NULL)
                return false;

            // Initialize UI
            lsp_trace("init ui");
            init_ui();

            // Build UI
            lsp_trace("build ui");
            pUI->build();

            // GTK Init
            static bool gtk_initialized = false;
            if (!gtk_initialized)
            {
                lsp_trace("init gtk");
                int argc = 0;
                gtk_init_check(&argc, NULL);
                gtk_initialized     = true;
            }

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

    void VSTWrapper::add_ui_port(VSTUIPort *p)
    {
        lsp_trace("wrapping ui port id=%s", p->metadata()->id);
        lsp_trace("  external id=%d", int(vUIPorts.size()));
        vUIPorts.add(p);
        lsp_trace("  internal id=%d", int(pUI->ports_count()));
        pUI->add_port(p);
    }

    void VSTWrapper::init_ui()
    {
        // Call for initialization function
        pUI->init();

        lsp_trace("Binding UI ports");
        for (size_t port_id=0, ports_count = pPlugin->ports_count(); port_id < ports_count; ++port_id)
        {
            IPort *ip           = pPlugin->port(port_id);
            if (ip == NULL)
                continue;
            VSTPort *vip        = static_cast<VSTPort *>(ip);

            const port_t *port  = vip->metadata();
            if (port == NULL)
                continue;

            switch (port->role)
            {
                case R_UI_SYNC:
                case R_AUDIO:
                    // Nothing
                    break;

                case R_MESH:
                    add_ui_port(new VSTUIMeshPort(port, vip));
                    break;

                case R_CONTROL:
                    add_ui_port(new VSTUIParameterPort(port, static_cast<VSTParameterPort *>(vip)));
                    break;

                case R_METER:
                    add_ui_port(new VSTUIMeterPort(port, vip));
                    break;

                default:
                    break;
            }
        }
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

        gdk_display_sync(gdk_display_get_default());

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

        // Destroy ports
        for (size_t i=0; i<vUIPorts.size(); ++i)
        {
            lsp_trace("destroy ui port id=%s", vUIPorts[i]->metadata()->id);
            delete vUIPorts[i];
        }
        vUIPorts.clear();
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

            vup->sync();
        } // for port_id
    }

    void VSTWrapper::set_parameter(size_t index, float value)
    {

    }

    void VSTWrapper::create_transport_thread()
    {
        drop_transport_thread();
        pTimer = g_timeout_add (1000 / 25, transport_synchronize, this);
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
}

#endif /* CONTAINER_VST_OBJECT_H_ */
