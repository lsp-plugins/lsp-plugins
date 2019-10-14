/*
 * bookmarks.cpp
 *
 *  Created on: 14 окт. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <core/files/bookmarks.h>

using namespace lsp;
using namespace lsp::bookmarks;

UTEST_BEGIN("core.files", bookmarks)

    void test_gtk3_bookmarks()
    {
        printf("Testing GTK3 bookmarks...\n");

        cvector<LSPString> bm;

        UTEST_ASSERT(read_bookmarks_gtk3(&bm, "res/test/bookmarks/gtk-3.0.bookmarks") == STATUS_OK);
        UTEST_ASSERT(bm.size() == 2);
        UTEST_ASSERT(bm.get(0)->equals_ascii("/path/to/file1"));
        UTEST_ASSERT(bm.get(1)->equals_ascii("path/to/file2"));

        destroy_bookmarks(&bm);
    }

    UTEST_MAIN
    {
        test_gtk3_bookmarks();
    }

UTEST_END

