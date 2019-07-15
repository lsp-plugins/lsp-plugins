/*
 * libpath.cpp
 *
 *  Created on: 27 янв. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <core/types.h>
#include <core/LSPString.h>

#if defined(PLATFORM_WINDOWS)
    #include <container/common/winlib.h>
#else
    #include <container/common/libpath.h>
#endif /* PLATFORM_WINDOWS */

UTEST_BEGIN("container", libpath)

    UTEST_MAIN
    {
#if defined(PLATFORM_WINDOWS)
        WCHAR *path = lsp::get_library_path();
        UTEST_ASSERT(path != NULL);

        lsp::LSPString s;
        s.set_utf16(path);
        printf("  Library path: %s\n", s.get_native());

        lsp::free_library_path(path);
#else
        char **paths = lsp::get_library_paths(NULL);
        UTEST_ASSERT(paths != NULL);

        for (char **p = paths; *p != NULL; ++p)
            printf("  additional path: %s\n", *p);

        char **paths2 = lsp::get_library_paths(const_cast<const char **>(paths));
        UTEST_ASSERT(paths2 == NULL);

        lsp::free_library_paths(paths2);
        lsp::free_library_paths(paths);
#endif /* PLATFORM_WINDOWS */
    }

UTEST_END;


