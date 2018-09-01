/*
 * standalone.cpp
 *
 *  Created on: 24 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>

#include <core/types.h>
#include <core/alloc.h>
#include <data/cvector.h>

#include <plugins/plugins.h>
#include <container/jack/defs.h>

using namespace lsp;

MTEST_BEGIN("", standalone)

    void plugin_not_found(const char *id)
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

        MTEST_FAIL_SILENT();
    }

    MTEST_MAIN
    {
        if (argc <= 0)
            plugin_not_found(NULL);

        const plugin_metadata_t *m = NULL;
        #define MOD_PLUGIN(x) if (!strcmp(argv[0], #x)) m = &x::metadata;
        #include <metadata/modules.h>
        if (m == NULL)
            plugin_not_found(argv[0]);

        printf("Preparing to call JACK_MAIN_FUNCION\n");
        const char ** args = new const char *[argc];
        MTEST_ASSERT(args != NULL);

        args[0] = "utest.standalone";
        for (ssize_t i=1; i < argc; ++i)
            args[i] = argv[i];

        printf("Calling JACK_MAIN_FUNCTION\n");

        // Call the main function
        int result = JACK_MAIN_FUNCTION(argv[0], argc, args);
        MTEST_ASSERT(result == 0);

        // Free data and return result
        delete [] args;
    }

MTEST_END
