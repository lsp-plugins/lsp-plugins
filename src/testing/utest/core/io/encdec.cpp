/*
 * encdec.cpp
 *
 *  Created on: 9 мар. 2019 г.
 *      Author: sadko
 */


#include <test/utest.h>
#include <core/status.h>
#include <core/LSPString.h>
#include <core/io/NativeFile.h>
#include <core/io/CharsetEncoder.h>
#include <core/io/CharsetDecoder.h>

#include <string.h>

using namespace lsp;
using namespace lsp::io;

// Test buffer size is a simple number, more than 0x1000
#define BUFFER_SIZE         4567

UTEST_BEGIN("core.io", encdec)

    void testDecodeFile(const LSPString *src, const LSPString *dst, const char *charset)
    {
        NativeFile in, out;
        CharsetDecoder decoder;

        printf("  decoding %s (%s) -> %s (UTF-32)\n", src->get_native(), charset, dst->get_native());

        UTEST_ASSERT(in.open(src, File::FM_READ) == STATUS_OK);
        UTEST_ASSERT(out.open(dst, File::FM_WRITE | File::FM_CREATE | File::FM_TRUNC) == STATUS_OK);
        UTEST_ASSERT(decoder.init(charset) == STATUS_OK);

        lsp_wchar_t *obuf = new lsp_wchar_t[BUFFER_SIZE];
        UTEST_ASSERT(obuf != NULL);

        ssize_t fetched, filled;

        do
        {
            // Is there any data on output?
            fetched = decoder.fetch(obuf, BUFFER_SIZE);
            if (fetched > 0)
            {
                ssize_t to_write = fetched * sizeof(lsp_wchar_t);
                ssize_t written = out.write(obuf, to_write);
                UTEST_ASSERT(written == to_write);
            }
            else if (fetched < 0)
                UTEST_FAIL_MSG("decoder.fetch() failed with error %d", int(-fetched));

            // Is there any data on input?
            filled = decoder.fill(&in);
            if ((filled < 0) && (filled != -STATUS_EOF))
                UTEST_FAIL_MSG("decoder.fill() failed with error %d", int(-filled));

        } while ((fetched > 0) || (filled > 0));

        decoder.close();
        UTEST_ASSERT(out.flush() == STATUS_OK);
        UTEST_ASSERT(out.size() > 0);
        UTEST_ASSERT(out.close() == STATUS_OK);
        UTEST_ASSERT(in.close() == STATUS_OK);

        delete [] obuf;
    }

    void testEncodeFile(const LSPString *src, const LSPString *dst, const char *charset)
    {
        NativeFile in, out;
        CharsetEncoder encoder;

        printf("  encoding %s (UTF-32) -> %s (%s)\n", src->get_native(), dst->get_native(), charset);

        UTEST_ASSERT(in.open(src, File::FM_READ) == STATUS_OK);
        UTEST_ASSERT(out.open(dst, File::FM_WRITE | File::FM_CREATE | File::FM_TRUNC) == STATUS_OK);
        UTEST_ASSERT(encoder.init(charset) == STATUS_OK);

        lsp_wchar_t *ibuf = new lsp_wchar_t[BUFFER_SIZE];
        UTEST_ASSERT(ibuf != NULL);
        lsp_wchar_t *ibufh = ibuf, *ibuft = ibuf;

        ssize_t fetched, filled;

        do
        {
            // Is there any data on output?
            fetched = encoder.fetch(&out, BUFFER_SIZE);
            if ((fetched < 0) && (fetched != -STATUS_EOF))
                UTEST_FAIL_MSG("decoder.fetch() failed with error %d", int(-fetched));

            // Do we need to fill the buffer with new data?
            size_t count = ibuft - ibufh;
            if (count < (BUFFER_SIZE >> 1))
            {
                if (count > 0)
                    ::memmove(ibuf, ibufh, count * sizeof(lsp_wchar_t));
                ibufh   = ibuf;
                ibuft   = &ibuf[count];

                // Try to perform additional read
                size_t to_read = BUFFER_SIZE - count;
                filled = in.read(ibuft, to_read * sizeof(lsp_wchar_t));
                if (filled > 0)
                {
                    UTEST_ASSERT((filled % sizeof(lsp_wchar_t)) == 0);
                    count  += filled / sizeof(lsp_wchar_t) ;
                    ibuft   = &ibufh[count];
                }
            }

            // Is there any data on input?
            filled  = encoder.fill(ibufh, count);
            if (filled > 0)
                ibufh      += filled;
            else if ((filled < 0) && (filled != -STATUS_EOF))
                UTEST_FAIL_MSG("decoder.fill() failed with error %d", int(-filled));

        } while ((fetched > 0) || (filled > 0));

        encoder.close();
        UTEST_ASSERT(out.flush() == STATUS_OK);
        UTEST_ASSERT(out.size() > 0);
        UTEST_ASSERT(out.close() == STATUS_OK);
        UTEST_ASSERT(in.close() == STATUS_OK);

        delete [] ibuf;
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

