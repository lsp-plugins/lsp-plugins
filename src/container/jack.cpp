#include <gtk/gtk.h>

#include <jack/jack.h>
#include <jack/transport.h>
#include <jack/midiport.h>

#include <core/types.h>
#include <core/lib.h>
#include <core/debug.h>
#include <core/status.h>
#include <core/dsp.h>
#include <core/NativeExecutor.h>

#include <plugins/plugins.h>
#include <metadata/plugins.h>

#include <container/const.h>
#include <container/jack/wrapper.h>

#if defined(LSP_UI_GTK2)
    #include <ui/gtk2/ui.h>
    #define LSP_PACKAGE gtk2
    #define LSP_WIDGET_FACTORY Gtk2WidgetFactory
#elif defined(LSP_UI_GTK3)
    #include <ui/gtk3/ui.h>
    #define LSP_PACKAGE gtk3
    #define LSP_WIDGET_FACTORY Gtk3WidgetFactory
#endif /* LSP_UI_GTK3 */

namespace lsp
{
#if defined(LSP_UI_GTK2)
    typedef struct gtk_wrapper_t
    {
        JACKWrapper    *pWrapper;
        GtkWidget      *pWindow;
        bool            bNotified;
    } gtk_wrapper_t;

    gboolean jack_ui_synchronize(gpointer arg)
    {
        gtk_wrapper_t *wrapper = reinterpret_cast<gtk_wrapper_t *>(arg);
        if (!wrapper->pWrapper->transfer_dsp_to_ui())
        {
            // Need to notify
            if (!wrapper->bNotified)
            {
                // Display the message to the user
                GtkWidget *msg = gtk_message_dialog_new (GTK_WINDOW(wrapper->pWindow), GtkDialogFlags(0), GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
                    "JACK backend has been shutdown. Further sound processing is not possible. "
                    "Please consider to save the configuration of the plugin before shutting it down."
                );
                if (msg != NULL)
                {
                    gtk_dialog_run(GTK_DIALOG(msg));
                    gtk_widget_destroy(msg);
                }

                // Mark the dialog already shown
                wrapper->bNotified = true;
            }
        }

        return TRUE;
    }

    static void jack_ui_window_destroy( GtkWidget *widget, gpointer data)
    {
        gtk_main_quit();
    }

    static int execute_ui(const plugin_metadata_t *meta, JACKWrapper *w, void *root)
    {
        char plugin_name[1024];
        snprintf(plugin_name, sizeof(plugin_name), LSP_ACRONYM " %s - %s [JACK] (Client ID: %s)", meta->name, meta->description, w->client_id());

        // Create GTK window
        GtkWidget *widget   = reinterpret_cast<GtkWidget *>(root);
        GtkWidget *window   = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        if (window == NULL)
            return STATUS_NO_MEM;

        // Create wrapper
        gtk_wrapper_t wrapper;
        wrapper.pWrapper    = w;
        wrapper.pWindow     = window;
        wrapper.bNotified   = false;

        // Create synchronization timer
        guint timer         = g_timeout_add (1000 / MESH_REFRESH_RATE, jack_ui_synchronize, &wrapper);
        lsp_trace("added transport=%ld", long(timer));

        // Initialize window parameters
        gtk_window_set_title(GTK_WINDOW(window), plugin_name);
        gtk_window_set_default_size (GTK_WINDOW (window), 500, 300);
        gtk_container_set_border_width (GTK_CONTAINER (window), 0);
        gtk_container_add(GTK_CONTAINER(window), widget);
        g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (jack_ui_window_destroy), NULL);

        // Show the window
        gtk_widget_show_all(window);

        // Now we can enter gtk_main()
        gtk_main();

        // Destroy the timer
        if (timer > 0)
            g_source_remove(timer);

        return STATUS_OK;
    }
#endif /* LSP_UI_GTK2 */

    static int jack_plugin_main(plugin_t *plugin, int argc, const char **argv)
    {
        static bool gtk_initialized     = false;
        int status                      = STATUS_OK;

        // Get metadata
        const plugin_metadata_t *meta   = (plugin != NULL) ? plugin->get_metadata() : NULL;

        if (meta != NULL)
        {
            // Initialize GTK
            if (!gtk_initialized)
            {
                gtk_init (&argc, const_cast<char ***>(&argv));
                gtk_initialized = true;
            }

            // Initialize DSP
            dsp::init();

            // Create widget factory and UI
            LSP_WIDGET_FACTORY *factory = new LSP_WIDGET_FACTORY("./res");
            if (factory != NULL)
            {
                plugin_ui ui(meta, factory);

                // Create wrapper
                JACKWrapper w(plugin, &ui);

                // Initialize
                status                  = w.init();
                if (status == STATUS_OK)
                {
                    // Do UI interaction
                    execute_ui(meta, &w, factory->root_widget());
                }
                else
                    lsp_error("Error initializing Jack wrapper");

                // Destroy objects
                ui.destroy();
                w.destroy();
                // Factory will be automatically destroyed by the UI
            }
            else
                status  = STATUS_NO_MEM;
        }
        else
        {
            lsp_error("Plugin has no metadata");
            status  = STATUS_NO_MEM;
        }

        // Destroy plugin
        plugin->destroy();
        return status;
    }

}

extern "C"
{
    extern int JACK_MAIN_FUNCTION(const char *plugin_id, int argc, const char **argv)
    {
        using namespace lsp;

        // Call corresponding plugin for execute
        #define MOD_GTK2(plugin)    \
            if (!strcmp(plugin::metadata.lv2_uid, plugin_id)) \
            { \
                plugin p; \
                return - jack_plugin_main(&p, argc, argv); \
            }

        #include <metadata/modules.h>

        // Output error
        lsp_error("Unknown plugin id=%s", plugin_id);
        return -STATUS_INVALID_UID;
    }
}
