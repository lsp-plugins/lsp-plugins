/*
 * libpath.cpp
 *
 *  Created on: 27 янв. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <container/common/libpath.h>

UTEST_BEGIN("container", libpath)

    UTEST_MAIN
    {
        char **paths = lsp::get_library_paths(NULL);
        UTEST_ASSERT(paths != NULL);

        for (char **p = paths; *p != NULL; ++p)
            printf("  additional path: %s\n", *p);

        char **paths2 = lsp::get_library_paths(const_cast<const char **>(paths));
        UTEST_ASSERT(paths2 == NULL);

        lsp::free_library_paths(paths2);
        lsp::free_library_paths(paths);
    }

UTEST_END;


