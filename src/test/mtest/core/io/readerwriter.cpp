/*
 * readerwriter.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <core/io/FileReader.h>
#include <core/io/FileWriter.h>

using namespace lsp;

static const char *SRC_FILE = "test_data/utf8_in.txt";
static const char *DST_FILE = "test_data/utf8_out.txt";

MTEST_BEGIN("core.io", readerwriter)

    MTEST_MAIN
    {
        const char *src = (argc >= 1) ? SRC_FILE : argv[0];
        const char *dst = (argc >= 2) ? DST_FILE : argv[1];

        io::FileReader in;
        io::FileWriter out;
        LSPString s;

        MTEST_ASSERT(in.open(src, "UTF-8") == STATUS_OK);
        while (in.read_line(&s, true) == STATUS_OK)
            puts(s.get_native());

        MTEST_ASSERT(in.close());

        MTEST_ASSERT(in.open(src, "UTF-8"));
        MTEST_ASSERT(out.open(dst, "UTF-8"));

        while (in.read_line(&s, true) == STATUS_OK)
            out.writeln(&s);

        MTEST_ASSERT(out.flush() == STATUS_OK);
        MTEST_ASSERT(in.close());
        MTEST_ASSERT(out.close());
    }

MTEST_END


