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

static char *cnull = NULL;
static LSPString *snull = NULL;
static Path *pnull = NULL;

static const char *cpaths[] =
{
#if !defined(PLATFORM_WINDOWS)
    ".", "",
    "./", "",
    "..", "",
    "../", "",
    "./.././a", "a",
    "a/b/c/../../d/./", "a/d",
    "a///b/c/.//../../d/./", "a/d",
    "a/b/c/../../../d/e/./f/", "d/e/f",

    "//", "/",
    "/.", "/",
    "/..", "/",
    "/./", "/",
    "/../", "/",
    "/a/b/c/../../d/./", "/a/d",
    "/a///b/c/.//../../d/./", "/a/d",
    "/a/b/c/../../../d/e/./f/", "/d/e/f",
    "/../a/b/c/../../d/./", "/a/d",
    "/../a///b/c/.//../../d/./", "/a/d",
    "/./../a/b/c/../../../d/e/./f/", "/d/e/f",
#else
    ".", "",
    ".\\", "",
    "..", "",
    "..\\", "",
    ".\\..\\.\\a", "a",
    "a\\b\\c\\..\\..\\d\\.\\", "a\\d",
    "a\\\\\\b\\c\\.\\\\..\\..\\d\\.\\", "a\\d",
    "a\\b\\c\\..\\..\\..\\d\\e\\.\\f\\", "d\\e\\f",

    "C:\\\\", "C:\\",
    "C:\\.", "C:\\",
    "C:\\..", "C:\\",
    "C:\\.\\", "C:\\",
    "C:\\..\\", "C:\\",
    "C:\\a\\b\\c\\..\\..\\d\\.\\", "C:\\a\\d",
    "C:\\a\\\\\\b\\c\\.\\\\..\\..\\d\\.\\", "C:\\a\\d",
    "C:\\a\\b\\c\\..\\..\\..\\d\\e\\.\\f\\", "C:\\d\\e\\f",
    "C:\\..\\a\\b\\c\\..\\..\\d\\.\\", "C:\\a\\d",
    "C:\\..\\a\\\\\\b\\c\\.\\\\..\\..\\d\\.\\", "C:\\a\\d",
    "C:\\.\\..\\a\\b\\c\\..\\..\\..\\d\\e\\.\\f\\", "C:\\d\\e\\f",
#endif

    NULL, NULL
};

UTEST_BEGIN("core.io", path)

//    status_t    get_parent(char *path, size_t maxlen) const;
//    status_t    set_parent(const char *path);
//    status_t    get_parent(LSPString *path) const;
//    status_t    get_parent(Path *path) const;
//    status_t    set_parent(LSPString *path);
//    status_t    set_parent(Path *path);
    void test_get_set_parent()
    {
        Path bp, dp;
        char path[PATH_MAX];
        LSPString spath, dpath;

        UTEST_ASSERT(bp.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(bp.get_parent(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(bp.get_parent(path, 2) == STATUS_TOO_BIG);
        UTEST_ASSERT(bp.get_parent(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(strcmp(path, TEST_PATH4) == 0);

        UTEST_ASSERT(spath.set_utf8(TEST_PATH4));
        UTEST_ASSERT(bp.get_parent(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.get_parent(&dpath) == STATUS_OK);
        UTEST_ASSERT(dpath.equals(&spath));
        UTEST_ASSERT(bp.get_parent(&dp) == STATUS_OK);
        UTEST_ASSERT(dp.equals(&spath));

        UTEST_ASSERT(bp.set(TEST_ROOT) == STATUS_OK);
        UTEST_ASSERT(bp.get_parent(path, PATH_MAX) == STATUS_NOT_FOUND);
        UTEST_ASSERT(bp.get_parent(&dpath) == STATUS_NOT_FOUND);
        UTEST_ASSERT(bp.get_parent(&dp) == STATUS_NOT_FOUND);

        UTEST_ASSERT(bp.set("bin") == STATUS_OK);
        UTEST_ASSERT(bp.get_parent(path, PATH_MAX) == STATUS_NOT_FOUND);
        UTEST_ASSERT(bp.get_parent(&dpath) == STATUS_NOT_FOUND);
        UTEST_ASSERT(bp.get_parent(&dp) == STATUS_NOT_FOUND);
    }

//    status_t    set_last(const char *path);
//    status_t    get_last(char *path, size_t maxlen) const;
//    status_t    set_last(const LSPString *path);
//    status_t    get_last(LSPString *path) const;
//    status_t    set_last(const Path *path);
//    status_t    get_last(Path *path) const;
    void test_get_set_last()
    {
        Path bp, dp, p;
        char path[PATH_MAX];
        LSPString spath, dpath, t1;

        UTEST_ASSERT(dp.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(dp.set_last("bin") == STATUS_OK);
        UTEST_ASSERT(dp.set_last(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(dp.equals(TEST_PATH3));
        UTEST_ASSERT(dp.get_last(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(strcmp(path, "bin") == 0);
        UTEST_ASSERT(dp.set_last("") == STATUS_OK);
        UTEST_ASSERT(dp.equals(TEST_PATH4));

        UTEST_ASSERT(bp.set_last("bin") == STATUS_OK);
        UTEST_ASSERT(bp.set_last(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.equals("bin"));
        UTEST_ASSERT(bp.get_last(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.get_last(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(strcmp(path, "bin") == 0);
        UTEST_ASSERT(bp.set_last("") == STATUS_OK);
        UTEST_ASSERT(bp.is_empty());

        UTEST_ASSERT(dpath.set_utf8("bin"));
        dp.clear();
        UTEST_ASSERT(spath.set_utf8(TEST_PATH2));
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
    }

//    status_t    set(const char *path);
//    status_t    set(const LSPString *path);
//    status_t    set(const Path *path);
//    status_t    get(char *path, size_t maxlen) const;
//    status_t    get(LSPString *path) const;
//    status_t    get(Path *path) const;
    void test_get_set()
    {
        Path p, dp, bp;
        char path[PATH_MAX];
        LSPString spath, dpath;

        UTEST_ASSERT(p.set(TEST_PATH1) == STATUS_OK);
        UTEST_ASSERT(p.set(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(p.get(path, 10) == STATUS_TOO_BIG);
        UTEST_ASSERT(strcmp(path, TEST_PATH1) == 0);

        UTEST_ASSERT(spath.set_utf8(TEST_PATH2));
        UTEST_ASSERT(p.set(&spath) == STATUS_OK);
        UTEST_ASSERT(p.set(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.get(&dpath) == STATUS_OK);
        UTEST_ASSERT(dpath.equals(&spath));


        UTEST_ASSERT(dp.set(&p) == STATUS_OK);
        UTEST_ASSERT(dp.set(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(dp.get(&bp) == STATUS_OK);
        UTEST_ASSERT(dp.get(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(bp.equals(&dp));
        UTEST_ASSERT(bp.equals(&p));
        UTEST_ASSERT(bp.equals(TEST_PATH2));
        UTEST_ASSERT(bp.equals(&spath));
    }

//    status_t    concat(const char *path);
//    status_t    concat(LSPString *path);
//    status_t    concat(Path *path);
    void test_concat()
    {
        Path p;
        LSPString sstr;
        Path spath;

        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(p.concat(FILE_SEPARATOR_S "bin") == STATUS_OK);
        UTEST_ASSERT(p.concat(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        p.clear();
        UTEST_ASSERT(p.concat(TEST_ROOT) == STATUS_OK);
        UTEST_ASSERT(p.is_root());

        p.clear();
        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(sstr.set_utf8(FILE_SEPARATOR_S "bin"));
        UTEST_ASSERT(p.concat(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.concat(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        p.clear();
        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(spath.set(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.concat(&spath) == STATUS_OK);
        UTEST_ASSERT(p.concat(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        p.clear();
        UTEST_ASSERT(sstr.set_utf8(TEST_ROOT));
        UTEST_ASSERT(p.concat(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.is_root());

        p.clear();
        UTEST_ASSERT(spath.set(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.concat(&spath) == STATUS_OK);
        UTEST_ASSERT(p.is_root());
    }

//    status_t    append_child(const char *path);
//    status_t    append_child(LSPString *path);
//    status_t    append_child(Path *path);
    void test_append_child()
    {
        Path p;
        LSPString sstr;
        Path spath;

        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(p.append_child("bin") == STATUS_OK);
        UTEST_ASSERT(p.append_child(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        p.clear();
        UTEST_ASSERT(sstr.set_utf8("bin"));
        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(p.append_child(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.append_child(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        p.clear();
        UTEST_ASSERT(p.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(spath.set(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.append_child(&spath) == STATUS_OK);
        UTEST_ASSERT(p.append_child(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH3));

        UTEST_ASSERT(sstr.set_utf8(TEST_ROOT));
        UTEST_ASSERT(spath.set(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.append_child(TEST_ROOT) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.append_child(&sstr) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.append_child(&spath) == STATUS_INVALID_VALUE);

        sstr.clear();
        spath.clear();
        UTEST_ASSERT(p.append_child("") == STATUS_OK);
        UTEST_ASSERT(p.append_child(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.append_child(&spath) == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_PATH3));
    }

//    status_t    remove_last();
//    status_t    remove_last(char *path, size_t maxlen);
//    status_t    remove_last(LSPString *path);
//    status_t    remove_last(Path *path);
    void test_remove_last()
    {
        Path p;
        char path[PATH_MAX];
        LSPString sstr, xstr;
        Path spath;

        UTEST_ASSERT(p.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(path, PATH_MAX) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.remove_last(path, 3) == STATUS_TOO_BIG);
        UTEST_ASSERT(p.equals(TEST_PATH2));
        UTEST_ASSERT(strcmp(path, TEST_PATH4) == 0);

        UTEST_ASSERT(xstr.set_utf8(TEST_PATH4));
        UTEST_ASSERT(p.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(&sstr) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH2));
        UTEST_ASSERT(sstr.equals(&xstr));

        UTEST_ASSERT(p.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(&spath) == STATUS_OK);
        UTEST_ASSERT(p.remove_last(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals(TEST_PATH2));
        UTEST_ASSERT(spath.equals(&xstr));

        UTEST_ASSERT(p.set(TEST_PATH2) == STATUS_OK);
        UTEST_ASSERT(p.remove_last() == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_PATH4));

        UTEST_ASSERT(p.set(TEST_ROOT) == STATUS_OK);
        UTEST_ASSERT(p.remove_last() == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_ROOT));
    }

//    status_t    remove_base(const char *path);
//    status_t    remove_base(const LSPString *path);
//    status_t    remove_base(const Path *path);
    void test_remove_base()
    {
        Path p, xp;
        LSPString xs;

        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(cnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals("bin"));

        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.is_empty());

        UTEST_ASSERT(xs.set_utf8(TEST_PATH4));
        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(&xs) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(snull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals("bin"));

        UTEST_ASSERT(xs.set_utf8(TEST_PATH3));
        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(&xs) == STATUS_OK);
        UTEST_ASSERT(p.is_empty());

        UTEST_ASSERT(xp.set(TEST_PATH4) == STATUS_OK);
        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(&xp) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(pnull) == STATUS_BAD_ARGUMENTS);
        UTEST_ASSERT(p.equals("bin"));

        UTEST_ASSERT(xp.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.set(TEST_PATH3) == STATUS_OK);
        UTEST_ASSERT(p.remove_base(&xp) == STATUS_OK);
        UTEST_ASSERT(p.is_empty());
    }

//    void        clear()
//    bool        is_absolute() const;
//    bool        is_relative() const;
//    bool        is_empty() const
//    bool        is_root() const;
//    status_t    root();
    void test_flags()
    {
        Path p;

        UTEST_ASSERT(!p.is_absolute());
        UTEST_ASSERT(p.is_relative());
        UTEST_ASSERT(p.is_empty());
        UTEST_ASSERT(!p.is_root());
        UTEST_ASSERT(p.root() == STATUS_BAD_STATE);

        UTEST_ASSERT(p.set(TEST_ROOT) == STATUS_OK);
        UTEST_ASSERT(p.is_absolute());
        UTEST_ASSERT(!p.is_relative());
        UTEST_ASSERT(!p.is_empty());
        UTEST_ASSERT(p.is_root());
        UTEST_ASSERT(p.root() == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_ROOT));

        UTEST_ASSERT(p.set(TEST_PATH1) == STATUS_OK);
        UTEST_ASSERT(p.is_absolute());
        UTEST_ASSERT(!p.is_relative());
        UTEST_ASSERT(!p.is_empty());
        UTEST_ASSERT(!p.is_root());
        UTEST_ASSERT(p.root() == STATUS_OK);
        UTEST_ASSERT(p.equals(TEST_ROOT));
        UTEST_ASSERT(p.is_root());

        UTEST_ASSERT(p.set("bin") == STATUS_OK);
        UTEST_ASSERT(!p.is_absolute());
        UTEST_ASSERT(p.is_relative());
        UTEST_ASSERT(!p.is_empty());
        UTEST_ASSERT(!p.is_root());
        UTEST_ASSERT(p.root() == STATUS_BAD_STATE);
        UTEST_ASSERT(!p.equals(TEST_ROOT));
        UTEST_ASSERT(!p.is_root());
    }

//    status_t    get_canonical(char *path, size_t maxlen) const;
//    status_t    get_canonical(LSPString *path) const;
//    status_t    get_canonical(Path *path) const;
//    bool        is_canonical() const;
//    status_t    canonicalize();
    void test_canonical()
    {
        Path p, xp;
        LSPString xs;
        char path[PATH_MAX];

//        UTEST_ASSERT(p.set(TEST_PATH1) == STATUS_OK);
//        UTEST_ASSERT(p.is_canonical());
//        UTEST_ASSERT(p.set(TEST_ROOT) == STATUS_OK);
//        UTEST_ASSERT(p.is_canonical());
//        p.clear();
//        UTEST_ASSERT(p.is_canonical());

        for (const char **cp=cpaths; *cp != NULL; cp += 2)
        {
            printf("Testing \"%s\" -> \"%s\"\n", cp[0], cp[1]);
            UTEST_ASSERT(p.set(cp[0]) == STATUS_OK);
            UTEST_ASSERT(!p.is_canonical());

            UTEST_ASSERT(p.get_canonical(path, PATH_MAX) == STATUS_OK);
            UTEST_ASSERT(p.get_canonical(cnull, PATH_MAX) == STATUS_BAD_ARGUMENTS);
            UTEST_ASSERT_MSG(strcmp(path, cp[1]) == 0, "canonicalized: \"%s\" -> \"%s\"\n", p.get(), path);
            UTEST_ASSERT(p.equals(cp[0]));

            UTEST_ASSERT(p.get_canonical(&xp) == STATUS_OK);
            UTEST_ASSERT(p.get_canonical(pnull) == STATUS_BAD_ARGUMENTS);
            UTEST_ASSERT_MSG(xp.equals(cp[1]), "canonicalized: \"%s\" -> \"%s\"\n", p.get(), xp.get());
            UTEST_ASSERT(p.equals(cp[0]));

            UTEST_ASSERT(p.get_canonical(&xs) == STATUS_OK);
            UTEST_ASSERT(p.get_canonical(snull) == STATUS_BAD_ARGUMENTS);
            UTEST_ASSERT_MSG(xp.equals(&xs), "canonicalized: \"%s\" -> \"%s\"\n", p.get(), xs.get_utf8());
            UTEST_ASSERT(p.equals(cp[0]));

            UTEST_ASSERT_MSG(p.canonicalize() == STATUS_OK, "canonicalized: \"%s\" -> \"%s\"\n", cp[0], p.get());
            UTEST_ASSERT(p.is_canonical());
            UTEST_ASSERT(p.equals(cp[1]));
        }
    }

    UTEST_MAIN
    {
        test_get_set();
        test_get_set_last();
        test_get_set_parent();
        test_concat();
        test_append_child();
        test_remove_last();
        test_remove_base();
        test_flags();
        test_canonical();
    }
UTEST_END;



