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

        cvector<bookmark_t> bm;

        UTEST_ASSERT(read_bookmarks_gtk3(&bm, "res/test/bookmarks/gtk-3.0.bookmarks") == STATUS_OK);

        for (size_t i=0; i<bm.size(); ++i)
            printf("  Read GTK3 bookmark: %s\n", bm.get(i)->path.get_utf8());

        UTEST_ASSERT(bm.size() == 4);
        UTEST_ASSERT(bm.get(0)->path.equals_ascii("/path/to/file1"));
        UTEST_ASSERT(bm.get(1)->path.equals_ascii("path/to/file2"));
        UTEST_ASSERT(bm.get(2)->path.equals_ascii("/path/with spaces/for test"));

        destroy_bookmarks(&bm);
    }

    UTEST_MAIN
    {
        test_gtk3_bookmarks();
    }

UTEST_END

