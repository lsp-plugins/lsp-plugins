/*
 * path.cpp
 *
 *  Created on: 9 февр. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <core/status.h>
#include <core/io/Path.h>

#include <string.h>

#ifdef PLATFORM_WINDOWS
    #define TEST_PATH1      "C:\\Windows\\system32"
    #define TEST_PATH2      "C:\\Windows\\system\\lib"
    #define TEST_PATH3      "C:\\Windows\\system\\bin"
    #define TEST_PATH4      "C:\\Windows\\system"
    #define TEST_ROOT       "C:\\"
#else
    #define TEST_PATH1      "/usr/local/bin"
    #define TEST_PATH2      "/usr/share/local/lib"
    #define TEST_PATH3      "/usr/share/local/bin"
    #define TEST_PATH4      "/usr/share/local"
    #define TEST_ROOT       "/"
#endif

using namespace lsp;
using namespace lsp::io;

UTEST_BEGIN("core.io", path)
    UTEST_MAIN
    {
        Path p, dp, bp;
        LSPString spath, dpath, t1, t2;
        char path[PATH_MAX], path2[PATH_MAX];
        char *cnull = NULL;
        LSPString *snull = NULL;
        Path *pnull = NULL;

//        status_t    set(const char *path);
//        status_t    get(char *path, size_t maxlen) const;
        UTEST_ASSERT(p.set(TEST_PATH1) == STATUS_OK);
        UTEST_ASSERT(p.set(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(p.get(path, 10) == STATUS_TOO_BIG);
        UTEST_ASSERT(strcmp(path, TEST_PATH1) == 0);

//        status_t    set(const LSPString *path);
//        status_t    get(LSPString *path) const;
        UTEST_ASSERT(spath.set_utf8(TEST_PATH2));
        UTEST_ASSERT(p.set(&spath) == STATUS_OK);
        UTEST_ASSERT(p.set(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(&dpath) == STATUS_OK);
        UTEST_ASSERT(dpath.equals(&spath));

//        status_t    set(const Path *path);
//        status_t    get(Path *path) const;
        UTEST_ASSERT(dp.set(&p) == STATUS_OK);
        UTEST_ASSERT(dp.set(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(dp.get(&bp) == STATUS_OK);
        UTEST_ASSERT(dp.get(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.equals(&dp));
        UTEST_ASSERT(bp.equals(&p));
        UTEST_ASSERT(bp.equals(TEST_PATH2));
        UTEST_ASSERT(bp.equals(&spath));

//        void        clear()
//        status_t    set_last(const char *path);
//        status_t    get_last(char *path, size_t maxlen) const;
//        bool        is_absolute() const;
//        bool        is_relative() const;
//        bool        is_empty() const
        bp.clear();
        UTEST_ASSERT(dp.set_last("bin") == STATUS_OK);
        UTEST_ASSERT(dp.set_last(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(dp.equals(TEST_PATH3));
        UTEST_ASSERT(dp.is_absolute());
        UTEST_ASSERT(!dp.is_relative());
        UTEST_ASSERT(dp.get_last(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(strcmp(path, "bin") == 0);
        UTEST_ASSERT(dp.set_last("") == STATUS_OK);
        UTEST_ASSERT(dp.equals(TEST_PATH4));

        UTEST_ASSERT(bp.set_last("bin") == STATUS_OK);
        UTEST_ASSERT(bp.set_last(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.equals("bin"));
        UTEST_ASSERT(!bp.is_absolute());
        UTEST_ASSERT(bp.is_relative());
        UTEST_ASSERT(bp.get_last(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.get_last(path2, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(strcmp(path2, "bin") == 0);
        UTEST_ASSERT(bp.set_last("") == STATUS_OK);
        UTEST_ASSERT(bp.is_empty());

//        status_t    set_last(const LSPString *path);
//        status_t    get_last(LSPString *path) const;
        UTEST_ASSERT(dpath.set_utf8("bin"));
        dp.clear();
        UTEST_ASSERT(bp.set(&spath) == STATUS_OK);
        UTEST_ASSERT(bp.set_last(&dpath) == STATUS_OK);
        UTEST_ASSERT(bp.equals(TEST_PATH3));
        UTEST_ASSERT(bp.get_last(&t1) == STATUS_OK);
        UTEST_ASSERT(bp.get_last(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(t1.equals(&dpath));

        UTEST_ASSERT(dp.set_last(&dpath) == STATUS_OK);
        UTEST_ASSERT(dp.equals("bin"));
        UTEST_ASSERT(dp.get_last(&spath) == STATUS_OK);
        UTEST_ASSERT(t1.equals(&spath));

//        status_t    set_last(const Path *path);
//        status_t    get_last(Path *path) const;
        p.clear();
        UTEST_ASSERT(bp.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(dp.set("bin") == STATUS_OK);
        UTEST_ASSERT(bp.set_last(&dp) == STATUS_OK);
        UTEST_ASSERT(bp.set_last(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.equals(TEST_PATH3));
        UTEST_ASSERT(bp.get_last(&p) == STATUS_OK);
        UTEST_ASSERT(p.equals("bin"));
        UTEST_ASSERT(bp.set(TEST_ROOT) == STATUS_OK);
        UTEST_ASSERT(bp.get_last(&p) == STATUS_OK);
        UTEST_ASSERT(p.is_empty());


//        status_t    get_parent(char *path, size_t maxlen) const;
//        status_t    get_parent(LSPString *path) const;
//        status_t    get_parent(Path *path) const;
//
//        status_t    set_parent(const char *path);
//        status_t    set_parent(LSPString *path);
//        status_t    set_parent(Path *path);
//
//        status_t    concat(const char *path);
//        status_t    concat(LSPString *path);
//        status_t    concat(Path *path);
//
//        status_t    append_child(const char *path);
//        status_t    append_child(LSPString *path);
//        status_t    append_child(Path *path);
//
//        status_t    remove_last();
//        status_t    remove_last(char *path, size_t maxlen);
//        status_t    remove_last(LSPString *path);
//        status_t    remove_last(Path *path);
//
//        status_t    remove_base(const char *path);
//        status_t    remove_base(const LSPString *path);
//        status_t    remove_base(const Path *path);
//
//        bool        is_canonical() const;
//        bool        is_root() const;
//
//        status_t    canonicalize();
//
//        status_t    root();
//
//        status_t    get_canonical(char *path, size_t maxlen) const;
//        status_t    get_canonical(LSPString *path) const;
//        status_t    get_canonical(Path *path) const;
//
//        inline status_t as_relative(const char *path);
//        inline status_t as_relative(const LSPString *path);
//        inline status_t as_relative(const Path *path);
    }
UTEST_END;



