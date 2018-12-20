#include <stdio.h>
#include <string.h>

#include <core/types.h>
#include <core/status.h>
#include <plugins/plugins.h>
#include <data/cvector.h>
#include <container/jack/defs.h>

#ifdef LSP_PROFILING_MAIN
namespace lsp
{
    int profile_plugin_not_found(const char *id)
    {
        cvector<const char> plugin_ids;

        // Generate the list of plugins
        #define MOD_PLUGIN(x) \
            if (x::metadata.ui_resource != NULL) \
                plugin_ids.add(x::metadata.lv2_uid);
        #include <metadata/modules.h>

        // Sort the list of plugins
        for (size_t i=0; i<plugin_ids.size()-1; ++i)
            for (size_t j=i+1; j<plugin_ids.size(); ++j)
                if (strcmp(plugin_ids[i], plugin_ids[j]) > 0)
                    plugin_ids.swap(i, j);

        // Output list
        if (id == NULL)
            fprintf(stderr, "\nPlugin identifier required to be passed as first parameter. Available identifiers:\n");
        else
            fprintf(stderr, "\nInvalid plugin identifier '%s'. Available identifiers:\n", id);
        for (size_t i=0; i<plugin_ids.size(); ++i)
            fprintf(stderr, "  %s\n", plugin_ids[i]);

        return -STATUS_INVALID_UID;
    }
}

int main(int argc, const char**argv)
{
    using namespace lsp;

    printf("(C) " LSP_COPYRIGHT "\n");
    printf("  " LSP_BASE_URI "\n");
    printf("  Version " LSP_MAIN_VERSION "\n");

    if (argc < 2)
        return profile_plugin_not_found(NULL);

    // Find the plugin
    const plugin_metadata_t *m = NULL;
    #define MOD_PLUGIN(x) if (!strcmp(argv[1], #x)) m = &x::metadata;
    #include <metadata/modules.h>
    if (m == NULL)
        return profile_plugin_not_found(argv[1]);
    if (m->ui_resource == NULL)
        return -STATUS_INVALID_UID;

    // Output plugin info
    printf("Plugin name:         %s (%s)\n", m->name, m->description);
    printf("Plugin version:      %d.%d.%d\n",
                int(LSP_VERSION_MAJOR(m->version)),
                int(LSP_VERSION_MINOR(m->version)),
                int(LSP_VERSION_MICRO(m->version))
            );
    if (m->lv2_uid != NULL)
        printf("LV2 URI:             %s%s\n", LSP_URI(lv2), m->lv2_uid);
    if (m->vst_uid != NULL)
        printf("VST identifier:      %s\n", m->vst_uid);
    if (m->ladspa_id > 0)
        printf("LADSPA identifier:   %d\n", m->ladspa_id);

    // Remove first parameter from command-line arguments
    lsp_trace("Preparing to call JACK_MAIN_FUNCION");
    const char ** args = new const char *[argc-1];
    if (args == NULL)
        return -STATUS_NO_MEM;

    size_t idx = 0;
    args[idx++] = argv[0];
    for (ssize_t i=2; i < argc; ++i)
        args[idx++] = argv[i];

    lsp_trace("Calling JACK_MAIN_FUNCTION");

    // Call the main function
    int result = JACK_MAIN_FUNCTION(argv[1], argc - 1, args);

    // Free data and return result
    delete [] args;
    return result;
}

#endif /* LSP_PROFILING_MAIN */
