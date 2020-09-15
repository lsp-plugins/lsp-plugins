/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 24 авг. 2018 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
        #define MOD_PLUGIN(plugin, ui) \
            if (plugin::metadata.ui_resource != NULL) \
                plugin_ids.add(plugin::metadata.lv2_uid);
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
        #define MOD_PLUGIN(plugin, ui) if (!strcmp(argv[0], #plugin)) m = &plugin::metadata;
        #include <metadata/modules.h>
        if (m == NULL)
            plugin_not_found(argv[0]);

        printf("Preparing to call JACK_MAIN_FUNCION\n");
        const char ** args = reinterpret_cast<const char **>(alloca(argc * sizeof(const char *)));
        MTEST_ASSERT(args != NULL);

        args[0] = full_name();
        for (ssize_t i=1; i < argc; ++i)
            args[i] = argv[i];

        printf("Calling JACK_MAIN_FUNCTION\n");

        // Call the main function
        int result = JACK_MAIN_FUNCTION(argv[0], argc, args);
        MTEST_ASSERT(result == 0);
    }

MTEST_END
