//#include <gtk/gtk.h>

#include <jack/jack.h>
#include <jack/transport.h>
#include <jack/midiport.h>

#include <core/types.h>
#include <core/lib.h>
#include <core/debug.h>
#include <core/status.h>
#include <core/ipc/NativeExecutor.h>

#include <dsp/dsp.h>

#include <ui/ui_locale.h>
#include <core/stdlib/string.h>

#include <plugins/plugins.h>
#include <metadata/plugins.h>

#include <container/const.h>
#include <container/jack/wrapper.h>

#include <signal.h>

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

    typedef struct jack_config_t
    {
        const char *cfg_file;
    } jack_config_t;

    status_t jack_parse_config(jack_config_t *cfg, int argc, const char **argv)
    {
        // Initialize config with default values
        cfg->cfg_file       = NULL;

        // Parse arguments
        int i = 1;
        while (i < argc)
        {
            const char *arg = argv[i++];
            if ((!::strcmp(arg, "--help")) || (!::strcmp(arg, "-h")))
            {
                printf("Usage: %s [parameters]\n\n", argv[0]);
                printf("Available parameters:\n");
                printf("  -c, --config <file>   Load settings file on startup\n");
                printf("  -h, --help            Output help\n");
                printf("\n");

                return STATUS_CANCELLED;
            }
            else if ((!::strcmp(arg, "--config")) || (!::strcmp(arg, "-c")))
            {
                if (i >= argc)
                {
                    fprintf(stderr, "Not specified file name for '%s' parameter\n", arg);
                    return STATUS_BAD_ARGUMENTS;
                }
                cfg->cfg_file = argv[i++];
            }
            else
            {
                fprintf(stderr, "Unknown parameter: %s\n", arg);
                return STATUS_BAD_ARGUMENTS;
            }
        }

        return STATUS_OK;
    }

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

    int jack_plugin_main(plugin_t *plugin, plugin_ui *pui, int argc, const char **argv)
    {
        int status               = STATUS_OK;
        jack_wrapper_t  wrapper;

        // Parse command-line arguments
        jack_config_t cfg;
        status_t res = jack_parse_config(&cfg, argc, argv);
        if (res != STATUS_OK)
            return (res == STATUS_CANCELLED) ? 0 : res;

        // Create wrapper
        lsp_trace("Creating wrapper");
        JACKWrapper w(plugin, pui);

        // Initialize
        lsp_trace("Initializing wrapper");
        status                  = w.init(argc, argv);

        // Load configuration (if specified in parameters)
        if ((status == STATUS_OK) && (cfg.cfg_file != NULL))
        {
            status = pui->import_settings(cfg.cfg_file, false);
            if (status != STATUS_OK)
                fprintf(stderr, "Error loading configuration file: %s\n", get_status(status));
        }

        // Enter the main lifecycle
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
            wrapper.pWindow     = pui->root_window();

            LSPTimer tmr;
            tmr.bind(pui->display());
            tmr.set_handler(jack_ui_sync, &wrapper);
            tmr.launch(0, 40); // 25 Hz rate

            // Do UI interaction
            lsp_trace("Calling main function");
            pui->main();
            tmr.cancel();

            dsp::finish(&ctx);
        }
        else
            lsp_error("Error initializing Jack wrapper");

        // Destroy objects
        w.disconnect();
        if (pui != NULL)
        {
            pui->destroy();
            delete pui;
        }
        w.destroy();

        return status;
    }

}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    LSP_LIBRARY_EXPORT
    int JACK_MAIN_FUNCTION(const char *plugin_id, int argc, const char **argv)
    {
        using namespace lsp;
        
        signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE signal since JACK can suddenly lose socket connection

        lsp_debug_init("jack");
        init_locale();
        
        // Initialize DSP
        lsp_trace("Initializing DSP");
        dsp::init();

        plugin_t  *p    = NULL;
        plugin_ui *pui  = NULL;
        status_t res    = STATUS_OK;

        // Call corresponding plugin for execute
        #define MOD_PLUGIN(plugin, ui)    \
            lsp_trace("test plugin uid=%s", plugin::metadata.lv2_uid); \
            if ((!p) && (!pui) && (!strcmp(plugin::metadata.lv2_uid, plugin_id))) \
            { \
                p = new plugin(); \
                if (plugin::metadata.ui_resource != NULL) \
                    pui = new ui(&plugin::metadata, NULL); \
            }

        #include <metadata/modules.h>

        // Try to launch instantiated objects
        if (p == NULL)
        {
            lsp_error("Unknown plugin id=%s", plugin_id);
            res = STATUS_INVALID_UID;
        }
        else if (pui == NULL)
        {
            lsp_error("No UI found for plugin id=%s", plugin_id);
            res = STATUS_BAD_STATE;
        }
        else
        {
            res = jack_plugin_main(p, pui, argc, argv);
            pui = NULL; // Already destroyed by jack_plugin_main
        }

        // Destroy objects
        if (pui != NULL)
        {
            pui->destroy();
            delete pui;
        }
        if (p!= NULL)
        {
            p->destroy();
            delete p;
        }

        // Output error
        return -res;
    }

    LSP_LIBRARY_EXPORT
    const char *JACK_GET_VERSION()
    {
        return LSP_MAIN_VERSION;
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */
