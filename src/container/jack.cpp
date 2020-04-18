//#include <gtk/gtk.h>
#include <core/types.h>

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

#if defined(LSP_TESTING) && (defined(PLATFORM_LINUX) || defined(PLATFORM_BSD))
    #define XDND_PROXY_SUPPORT
    #define IF_XDND_PROXY_SUPPORT(...)  __VA_ARGS__
#else
    #define IF_XDND_PROXY_SUPPORT(...)
#endif

namespace lsp
{
    typedef struct jack_wrapper_t
    {
        size_t          nSync;
        JACKWrapper    *pWrapper;
        LSPWindow      *pWindow;
        timespec        nLastReconnect;
    } jack_wrapper_t;

    typedef struct jack_config_t
    {
        const char     *cfg_file;
        void           *parent_id;
    } jack_config_t;

    status_t jack_parse_config(jack_config_t *cfg, int argc, const char **argv)
    {
        // Initialize config with default values
        cfg->cfg_file       = NULL;
        cfg->parent_id      = NULL;

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
                IF_XDND_PROXY_SUPPORT(
                    printf("  --dnd-proxy <id>      Create window as child and DnD proxy of specified window ID\n");
                )
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
        #ifdef XDND_PROXY_SUPPORT
            else if (!::strcmp(arg, "--dnd-proxy"))
            {
                if (i >= argc)
                {
                    fprintf(stderr, "Not specified window hex identifier for '%s' parameter\n", arg);
                    return STATUS_BAD_ARGUMENTS;
                }

                union {
                    void *ptr;
                    long data;
                } parent;
                parent.data = long(::strtol(argv[i++], NULL, 16));
                cfg->parent_id  = parent.ptr;
            }
        #endif
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
            if (!(wrapper->nSync++))
                wrapper->pWindow->query_resize();
        }

        // Transfer changes from DSP to UI
        wrapper->pWrapper->transfer_dsp_to_ui();

        return STATUS_OK;
    }

    int jack_plugin_main(jack_config_t &cfg, plugin_t *plugin, plugin_ui *pui, int argc, const char **argv)
    {
        int status               = STATUS_OK;
        jack_wrapper_t  wrapper;

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
            w.show_ui();
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

        jack_config_t cfg;
        plugin_t  *p    = NULL;
        plugin_ui *pui  = NULL;
        status_t res    = STATUS_OK;

        // Parse command-line arguments
        if ((res = jack_parse_config(&cfg, argc, argv)) != STATUS_OK)
            return (res == STATUS_CANCELLED) ? 0 : res;

        // Call corresponding plugin for execute
        #define MOD_PLUGIN(plugin, ui)    \
            lsp_trace("test plugin uid=%s", plugin::metadata.lv2_uid); \
            if ((!p) && (!pui) && (!strcmp(plugin::metadata.lv2_uid, plugin_id))) \
            { \
                p = new plugin(); \
                if (plugin::metadata.ui_resource != NULL) \
                    pui = new ui(&plugin::metadata, cfg.parent_id); \
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
            res = jack_plugin_main(cfg, p, pui, argc, argv);
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
