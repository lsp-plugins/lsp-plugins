//#include <gtk/gtk.h>

#include <jack/jack.h>
#include <jack/transport.h>
#include <jack/midiport.h>

#include <core/types.h>
#include <core/lib.h>
#include <core/debug.h>
#include <core/status.h>
#include <core/NativeExecutor.h>

#include <dsp/dsp.h>

#include <ui/ui_locale.h>

#include <plugins/plugins.h>
#include <metadata/plugins.h>

#include <container/const.h>
#include <container/jack/wrapper.h>

namespace lsp
{
    typedef struct jack_wrapper_t
    {
        size_t          nSync;
        JACKWrapper    *pWrapper;
        LSPWindow      *pWindow;
//        LSPMessageBox  *pDialog;
        timespec        nLastReconnect;
    } jack_wrapper_t;

    static status_t jack_ui_sync(timestamp_t time, void *arg)
    {
        if (arg == NULL)
            return STATUS_BAD_STATE;

        jack_wrapper_t *wrapper = static_cast<jack_wrapper_t *>(arg);
        JACKWrapper *jw         = wrapper->pWrapper;

        // If connection to JACK was lost - notify
        if (jw->connection_lost())
        {
            lsp_trace("Connection to JACK was lost");

            // Perform disconnect action
            jw->disconnect();

            // Remember last connection time
            clock_gettime(CLOCK_REALTIME, &wrapper->nLastReconnect);

//            // Notify user TODO: remove this notification in future
//            if (wrapper->pWindow != NULL)
//            {
//                if (wrapper->pDialog == NULL)
//                {
//                    LSPMessageBox *dlg  = new LSPMessageBox(wrapper->pWindow->display());
//
//                    dlg->init();
//                    dlg->set_title("JACK connection error");
//                    dlg->set_heading("Alert");
//                    dlg->set_message("JACK backend has been shutdown. Further sound processing is not possible.\n"
//                                    "Please consider to save the configuration of the plugin before shutting it down.");
//                    dlg->add_button("OK");
//                    wrapper->pDialog    = dlg;
//                }
//
//                wrapper->pDialog->show(wrapper->pWindow);
//            }
        }

        // If we are currently in disconnected state - try to perform a connection
        if (jw->disconnected())
        {
            timespec ctime;
            clock_gettime(CLOCK_REALTIME, &ctime);
            wssize_t delta = (ctime.tv_sec - wrapper->nLastReconnect.tv_sec) * 1000 + (ctime.tv_nsec - wrapper->nLastReconnect.tv_nsec) / 1000000;

            // Try each second to make new connection
            if (delta >= 1000)
            {
                lsp_trace("Trying to connect to JACK");
                if (jw->connect() == STATUS_OK)
                {
                    lsp_trace("Successful connected to JACK");
                    wrapper->nSync = 0;
                }
                wrapper->nLastReconnect     = ctime;
            }
        }

        // If we are connected - do usual stuff
        if (jw->connected())
        {
            wrapper->pWrapper->transfer_dsp_to_ui();
            if (!(wrapper->nSync++))
                wrapper->pWindow->query_resize();
        }

        return STATUS_OK;
    }

    int jack_plugin_main(plugin_t *plugin, int argc, const char **argv)
    {
        int status                      = STATUS_OK;
        jack_wrapper_t  wrapper;

        // Get metadata
        const plugin_metadata_t *meta   = (plugin != NULL) ? plugin->get_metadata() : NULL;

        if (meta != NULL)
        {
            // Initialize DSP
            lsp_trace("Initializing DSP");
            dsp::init();

            // Create plugin UI
            lsp_trace("Creating UI");
            plugin_ui ui(meta, NULL);

            // Create wrapper
            lsp_trace("Creating wrapper");
            JACKWrapper w(plugin, &ui);

            // Initialize
            lsp_trace("Initializing wrapper");
            status                  = w.init(argc, argv);
            if (status == STATUS_OK)
            {
                dsp::context_t ctx;
                dsp::start(&ctx);

                // Perform initial connection
                lsp_trace("Connecting to JACK server");
                w.connect();
                clock_gettime(CLOCK_REALTIME, &wrapper.nLastReconnect);

                // Create timer for transferring DSP -> UI data
                lsp_trace("Creating timer");
                wrapper.nSync       = 0;
                wrapper.pWrapper    = &w;
                wrapper.pWindow     = ui.root_window();
//                wrapper.pDialog     = NULL;

                LSPTimer tmr;
                tmr.bind(ui.display());
                tmr.set_handler(jack_ui_sync, &wrapper);
                tmr.launch(0, 40); // 25 Hz rate

                // Do UI interaction
                lsp_trace("Calling main function");
                ui.main();
                tmr.cancel();

                // Destroy dialog if present
//                if (wrapper.pDialog != NULL)
//                {
//                    wrapper.pDialog->destroy();
//                    delete wrapper.pDialog;
//                    wrapper.pDialog = NULL;
//                }

                dsp::finish(&ctx);
            }
            else
                lsp_error("Error initializing Jack wrapper");

            // Destroy objects
            w.disconnect();
            ui.destroy();
            w.destroy();
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

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    extern int JACK_MAIN_FUNCTION(const char *plugin_id, int argc, const char **argv)
    {
        lsp_debug_init("jack");

        using namespace lsp;

        init_locale();

        // Call corresponding plugin for execute
        #define MOD_PLUGIN(plugin)    \
            lsp_trace("test plugin uid=%s", plugin::metadata.lv2_uid); \
            if (!strcmp(plugin::metadata.lv2_uid, plugin_id)) \
            { \
                if (plugin::metadata.ui_resource == NULL) \
                    return -STATUS_INVALID_UID; \
                plugin p; \
                return - jack_plugin_main(&p, argc, argv); \
            }

        #include <metadata/modules.h>

        // Output error
        lsp_error("Unknown plugin id=%s", plugin_id);
        return -STATUS_INVALID_UID;
    }

    extern const char *JACK_GET_VERSION()
    {
        return LSP_MAIN_VERSION;
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */
