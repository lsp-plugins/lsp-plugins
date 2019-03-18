/*
 * dir.cpp
 *
 *  Created on: 18 мар. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <core/io/Dir.h>

using namespace lsp;

UTEST_BEGIN("core.io", dir)

    void testReadDir(const char *dir, bool full)
    {
        io::Dir dh;
        io::Path path;
        LSPString xdir;
        size_t n_read1 = 0, n_read2 = 0;

        UTEST_ASSERT(xdir.set_utf8(dir));

        // Try to read non-empty directory
        UTEST_ASSERT(dh.open(&xdir) == STATUS_OK);
        printf("Reading directory %s...\n", xdir.get_native());
        while (dh.read(&path, full) == STATUS_OK)
        {
            if (path.is_dot() || path.is_dotdot())
                continue;

            printf("  found file: %s\n", path.as_native());
            ++n_read1;
        }
        UTEST_ASSERT(dh.last_error() == STATUS_EOF);
        UTEST_ASSERT(n_read1 >= 7);

        // Try to re-read non-empty directory
        UTEST_ASSERT(dh.rewind() == STATUS_OK);
        printf("Re-reading directory %s...\n", xdir.get_native());
        while (dh.read(&path, full) == STATUS_OK)
        {
            if (path.is_dot() || path.is_dotdot())
                continue;

            printf("  found file: %s\n", path.as_native());
            ++n_read2;
        }
        UTEST_ASSERT(dh.last_error() == STATUS_EOF);
        UTEST_ASSERT(n_read1 == n_read2);
        UTEST_ASSERT(dh.close() == STATUS_OK);
    }

    UTEST_MAIN
    {
        testReadDir("res/test/io/iconv", false);
        testReadDir("res/test/io/iconv", true);
    }

UTEST_END



