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
            printf("  Read GTK3 bookmark: %s -> %s\n", bm.get(i)->path.get_utf8(), bm.get(i)->name.get_utf8());

        UTEST_ASSERT(bm.size() == 4);
        UTEST_ASSERT(bm.get(0)->path.equals_ascii("/path/to/file1"));
        UTEST_ASSERT(bm.get(0)->name.equals_ascii("file1"));
        UTEST_ASSERT(bm.get(1)->path.equals_ascii("path/to/file2"));
        UTEST_ASSERT(bm.get(1)->name.equals_ascii("Some alias"));
        UTEST_ASSERT(bm.get(2)->path.equals_ascii("/path/with spaces/for test"));
        UTEST_ASSERT(bm.get(2)->name.equals_ascii("for test"));

        destroy_bookmarks(&bm);
    }

    void test_qt5_bookmarks()
    {
        cvector<bookmark_t> bm;
        UTEST_ASSERT(read_bookmarks_qt5(&bm, "res/test/bookmarks/qt5-bookmarks.xml") == STATUS_OK);

        for (size_t i=0; i<bm.size(); ++i)
            printf("  Read QT5 bookmark: %s -> %s\n", bm.get(i)->path.get_utf8(), bm.get(i)->name.get_utf8());

        UTEST_ASSERT(bm.size() == 5);
        UTEST_ASSERT(bm.get(0)->path.equals_utf8("/home/vsadovnikov"));
        UTEST_ASSERT(bm.get(0)->name.equals_utf8("Home"));
        UTEST_ASSERT(bm.get(1)->path.equals_utf8("/home/vsadovnikov/Рабочий стол"));
        UTEST_ASSERT(bm.get(1)->name.equals_utf8("Desktop"));
        UTEST_ASSERT(bm.get(2)->path.equals_utf8("/home/vsadovnikov/Загрузки"));
        UTEST_ASSERT(bm.get(2)->name.equals_utf8("Downloads"));
        UTEST_ASSERT(bm.get(3)->path.equals_utf8("/"));
        UTEST_ASSERT(bm.get(3)->name.equals_utf8("Root"));
        UTEST_ASSERT(bm.get(4)->path.equals_utf8("/home/vsadovnikov/eclipse"));
        UTEST_ASSERT(bm.get(4)->name.equals_utf8("eclipse"));

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

        destroy_bookmarks(&bm);
    }

    void add_bookmark(cvector<bookmark_t> &bm, const char *path, const char *name, size_t flags)
    {
        bookmark_t *b = new bookmark_t;
        UTEST_ASSERT(b != NULL);
        UTEST_ASSERT(b->path.set_utf8(path));
        UTEST_ASSERT(b->name.set_utf8(name));
        b->origin = flags;
        UTEST_ASSERT(bm.add(b));
    }

    void test_save_bookmarks()
    {
        cvector<bookmark_t> bm;
        LSPString path;
        UTEST_ASSERT(path.fmt_utf8("tmp/utest-%s.json", this->full_name()));

        add_bookmark(bm, "/path1", "Test path", BM_LSP);
        add_bookmark(bm, "/home/some/path/1", "Additional path", BM_LSP | BM_GTK3 | BM_QT5);
        add_bookmark(bm, "/home/some/path/2", "Additional path 2", 0);
        UTEST_ASSERT(bm.size() == 3);
        UTEST_ASSERT(save_bookmarks(&bm, &path) == STATUS_OK);

        UTEST_ASSERT(read_bookmarks(&bm, &path) == STATUS_OK);
        for (size_t i=0; i<bm.size(); ++i)
        {
            bookmark_t *b = bm.get(i);
            printf("  Read LSP bookmark: %s -> %s\n", b->path.get_utf8(), b->name.get_utf8());
        }

        UTEST_ASSERT(bm.size() == 2); // 1 item should not be stored
        UTEST_ASSERT(bm.get(0)->path.equals_ascii("/path1"));
        UTEST_ASSERT(bm.get(0)->name.equals_ascii("Test path"));
        UTEST_ASSERT(bm.get(0)->origin == BM_LSP);

        UTEST_ASSERT(bm.get(1)->path.equals_ascii("/home/some/path/1"));
        UTEST_ASSERT(bm.get(1)->name.equals_ascii("Additional path"));
        UTEST_ASSERT(bm.get(1)->origin == (BM_LSP | BM_GTK3 | BM_QT5));

        destroy_bookmarks(&bm);
    }

    void test_merge_bookmarks()
    {
        cvector<bookmark_t> dst, src;

        add_bookmark(dst, "/0/0", "0-0", 0);                    // removed
        add_bookmark(dst, "/0/1", "0-1", BM_LSP);               // + BM_GTK3 -> changed
        add_bookmark(dst, "/1/0", "1-0", BM_GTK3);              // not changed
        add_bookmark(dst, "/1/1", "1-1", BM_LSP | BM_GTK3);     // - BM_GTK3 -> changed
        add_bookmark(dst, "/2/0", "2-0", BM_GTK3);              // - BM_GTK3 -> removed
        UTEST_ASSERT(dst.add(NULL));                            // removed

        add_bookmark(src, "/0/1", "0-1", BM_GTK3);
        add_bookmark(src, "/1/0", "1-0", BM_GTK3);
        add_bookmark(src, "/2/1", "2-1", BM_GTK3);              // added
        UTEST_ASSERT(src.add(NULL));                            // ignored

        size_t changes = 0;
        UTEST_ASSERT(merge_bookmarks(&dst, &changes, &src, BM_GTK3) == STATUS_OK);

        for (size_t i=0; i<dst.size(); ++i)
        {
            bookmark_t *b = dst.get(i);
            printf("  Merged LSP bookmark: %s -> %s (0x%x)\n", b->path.get_utf8(), b->name.get_utf8(), int(b->origin));
        }

        UTEST_ASSERT(changes > 0);
        UTEST_ASSERT(dst.size() == 4);

        UTEST_ASSERT(dst.get(0)->path.equals_ascii("/0/1"));
        UTEST_ASSERT(dst.get(0)->name.equals_ascii("0-1"));
        UTEST_ASSERT(dst.get(0)->origin == (BM_LSP | BM_GTK3));

        UTEST_ASSERT(dst.get(1)->path.equals_ascii("/1/0"));
        UTEST_ASSERT(dst.get(1)->name.equals_ascii("1-0"));
        UTEST_ASSERT(dst.get(1)->origin == (BM_GTK3));

        UTEST_ASSERT(dst.get(2)->path.equals_ascii("/1/1"));
        UTEST_ASSERT(dst.get(2)->name.equals_ascii("1-1"));
        UTEST_ASSERT(dst.get(2)->origin == (BM_LSP));

        UTEST_ASSERT(dst.get(3)->path.equals_ascii("/2/1"));
        UTEST_ASSERT(dst.get(3)->name.equals_ascii("2-1"));
        UTEST_ASSERT(dst.get(3)->origin == (BM_GTK3 | BM_LSP));

        destroy_bookmarks(&dst);
        destroy_bookmarks(&src);
    }

    UTEST_MAIN
    {
        printf("Testing read of GTK3 bookmarks...\n");
        test_gtk3_bookmarks();
        printf("Testing read of QT5 bookmarks...\n");
        test_qt5_bookmarks();
        printf("Testing read of LSP bookmarks...\n");
        test_lsp_bookmarks();
        printf("Testing write of LSP bookmarks...\n");
        test_save_bookmarks();
        printf("Testing merge of LSP bookmarks...\n");
        test_merge_bookmarks();
    }

UTEST_END

