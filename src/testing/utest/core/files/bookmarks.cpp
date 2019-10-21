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

    void test_lsp_bookmarks()
    {
        cvector<bookmark_t> bm;
        UTEST_ASSERT(read_bookmarks(&bm, "res/test/bookmarks/lsp-bookmarks.json") == STATUS_OK);
        for (size_t i=0; i<bm.size(); ++i)
        {
            bookmark_t *b = bm.get(i);
            printf("  Read LSP bookmark: %s -> %s\n", b->path.get_utf8(), b->name.get_utf8());
        }
        UTEST_ASSERT(bm.size() == 4);
        UTEST_ASSERT(bm.get(0)->path.equals_ascii("/path1"));
        UTEST_ASSERT(bm.get(0)->name.equals_ascii("PATH1"));
        UTEST_ASSERT(bm.get(0)->origin == BM_LSP);

        UTEST_ASSERT(bm.get(1)->path.equals_ascii("/some/another/path"));
        UTEST_ASSERT(bm.get(1)->name.equals_ascii("Another path"));
        UTEST_ASSERT(bm.get(1)->origin == (BM_LSP | BM_GTK3 | BM_QT5));

        UTEST_ASSERT(bm.get(2)->path.equals_ascii("/some/another/path2"));
        UTEST_ASSERT(bm.get(2)->name.equals_ascii("Another path 2"));
        UTEST_ASSERT(bm.get(2)->origin == 0);

        UTEST_ASSERT(bm.get(3)->path.is_empty());
        UTEST_ASSERT(bm.get(3)->name.is_empty());
        UTEST_ASSERT(bm.get(3)->origin == 0);
    }

    UTEST_MAIN
    {
        printf("Testing GTK3 bookmarks...\n");
        test_gtk3_bookmarks();
        printf("Testing LSP bookmarks...\n");
        test_lsp_bookmarks();
    }

UTEST_END

