/*
 * readwrite.cpp
 *
 *  Created on: 6 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/InSequence.h>
#include <test/utest.h>
#include <core/status.h>

#include <string.h>

#include <core/io/NativeFile.h>
#include <core/io/OutSequence.h>

// Test buffer size is a simple number, more than 0x1000
#define BUFFER_SIZE         4567

using namespace lsp;
using namespace lsp::io;

UTEST_BEGIN("core.io", sequence)

    void testDecodeFile(const LSPString *src, const LSPString *dst, const char *charset)
    {
        InSequence in;
        NativeFile out;

        printf("  decoding %s (%s) -> %s (UTF-32)\n", src->get_native(), charset, dst->get_native());

        UTEST_ASSERT(in.open(src, charset) == STATUS_OK);
        UTEST_ASSERT(out.open(dst, File::FM_WRITE | File::FM_CREATE | File::FM_TRUNC) == STATUS_OK);

        lsp_wchar_t *buf = new lsp_wchar_t[BUFFER_SIZE];
        UTEST_ASSERT(buf != NULL);

        while (true)
        {
            // Read characters
            ssize_t nread = in.read(buf, BUFFER_SIZE);
            if (nread <= 0)
            {
                if ((nread == 0) || (nread == -STATUS_EOF))
                    break;
                UTEST_FAIL_MSG("read() returned status: %d", -int(nread));
            }

            // Write characters
            ssize_t written = out.write(buf, nread * sizeof(lsp_wchar_t));
            UTEST_ASSERT(written == ssize_t(nread * sizeof(lsp_wchar_t)));
        }

        UTEST_ASSERT(out.close() == STATUS_OK);
        UTEST_ASSERT(in.close() == STATUS_OK);

        delete [] buf;
    }

    void testEncodeFile(const LSPString *src, const LSPString *dst, const char *charset)
    {
        NativeFile in;
        OutSequence out;

        printf("  encoding %s (UTF-32) -> %s (%s)\n", src->get_native(), dst->get_native(), charset);

        UTEST_ASSERT(in.open(src, File::FM_READ) == STATUS_OK);
        UTEST_ASSERT(out.open(dst, File::FM_WRITE | File::FM_CREATE | File::FM_TRUNC, charset) == STATUS_OK);

        lsp_wchar_t *buf = new lsp_wchar_t[BUFFER_SIZE];
        UTEST_ASSERT(buf != NULL);

        while (true)
        {
            // Read characters
            ssize_t nread = in.read(buf, BUFFER_SIZE * sizeof(lsp_wchar_t));
            if (nread <= 0)
            {
                if ((nread == 0) || (nread == -STATUS_EOF))
                    break;
                UTEST_FAIL_MSG("read() returned status: %d", -int(nread));
            }
            else
                UTEST_ASSERT((nread % sizeof(lsp_wchar_t)) == 0);

            // Do we need to shift the buffer?
            status_t res = out.write(buf, nread / sizeof(lsp_wchar_t));
            UTEST_ASSERT(res == STATUS_OK);
        }

        UTEST_ASSERT(out.close() == STATUS_OK);
        UTEST_ASSERT(in.close() == STATUS_OK);

        delete [] buf;
    }

    void compareFiles(const LSPString *src, const LSPString *dst)
    {
        NativeFile f1, f2;
        uint8_t *b1, *b2;

        printf("  comparing %s <-> %s\n", src->get_native(), dst->get_native());

        UTEST_ASSERT(f1.open(src, File::FM_READ) == STATUS_OK);
        UTEST_ASSERT(f2.open(dst, File::FM_READ) == STATUS_OK);
        UTEST_ASSERT(f1.size() == f2.size());

        b1 = new uint8_t[0x1000];
        UTEST_ASSERT(b1 != NULL);
        b2 = new uint8_t[0x1000];
        UTEST_ASSERT(b2 != NULL);

        while (true)
        {
            wssize_t offset = f1.position();
            ssize_t read1 = f1.read(b1, 0x1000);
            ssize_t read2 = f2.read(b2, 0x1000);

            // Check read status
            UTEST_ASSERT(read1 == read2);
            if ((read1 == 0) || (read2 == (-STATUS_EOF)))
                break;
            else if (read1 < 0)
                UTEST_FAIL_MSG("read returned %d", int(read1));

            // Perform check
            UTEST_ASSERT_MSG((::memcmp(b1, b2, read1) == 0), "Blocks offset=0x%x, size=0x%x differ", int(offset), int(read1));
        }

        UTEST_ASSERT(f1.close() == STATUS_OK);
        UTEST_ASSERT(f2.close() == STATUS_OK);

        delete [] b1;
        delete [] b2;
    }

    void testFileCoding(const char *src, const char *charset)
    {
        LSPString fenc, fdec, fsrc;
        UTEST_ASSERT(fenc.fmt_utf8("tmp/utest-%s-encoded.tmp", full_name()));
        UTEST_ASSERT(fdec.fmt_utf8("tmp/utest-%s-decoded.tmp", full_name()));

        UTEST_ASSERT(fsrc.set_utf8(src));
        printf("Testing encoders on file %s...\n", fsrc.get_native());
        testDecodeFile(&fsrc, &fenc, charset);
        testEncodeFile(&fenc, &fdec, charset);
        compareFiles(&fsrc, &fdec);
    }

    UTEST_MAIN
    {
        testFileCoding("res/test/io/iconv/01-de-utf16le.txt", "UTF-16LE");
        testFileCoding("res/test/io/iconv/01-de-utf8.txt", "UTF-8");
        testFileCoding("res/test/io/iconv/02-ja-utf16le.txt", "UTF-16LE");
        testFileCoding("res/test/io/iconv/02-ja-utf8.txt", "UTF-8");
        testFileCoding("res/test/io/iconv/03-ru-cp1251.txt", "CP1251");
        testFileCoding("res/test/io/iconv/03-ru-utf16le.txt", "UTF-16LE");
        testFileCoding("res/test/io/iconv/03-ru-utf8.txt", "UTF-8");
    }
UTEST_END
