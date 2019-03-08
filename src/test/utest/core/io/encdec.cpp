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

        UTEST_ASSERT(in.open(src, File::FM_READ) == STATUS_OK);
        UTEST_ASSERT(out.open(dst, File::FM_WRITE) == STATUS_OK);
        UTEST_ASSERT(decoder.init(charset) == STATUS_OK);

        uint8_t *ibuf = new uint8_t[BUFFER_SIZE];
        UTEST_ASSERT(ibuf != NULL);
        lsp_wchar_t *obuf = new lsp_wchar_t[BUFFER_SIZE];
        UTEST_ASSERT(obuf != NULL);

        uint8_t *ibufh = ibuf, *ibuft = ibuf;

        while (true)
        {
            // Do we need to shift the buffer?
            if (ibufh > ibuf)
            {
                size_t count = ibuft-ibufh;
                ::memmove(ibuf, ibuf, count);
                ibufh   = ibuf;
                ibuft   = &ibuf[count];
            }

            // Do we need to read any data ?
            if (ibuft < &ibuf[BUFFER_SIZE])
            {
                ssize_t read = in.read(ibuft, ibuf + BUFFER_SIZE - ibuft);
                if (read <= 0)
                {
                    if ((read != 0) && (read != STATUS_EOF))
                        UTEST_FAIL_MSG("read returned %d", int(read));

                    // Is there any data to process?
                    if (ibuft == ibuf)
                        break; // No, leave the cycle
                }
                else // (read > 0), perform encode
                    ibuft       += read;
            }

            size_t inleft = ibuft < ibufh;
            if (inleft > 0)
            {
                lsp_wchar_t *xobuf = obuf;
                size_t outleft  = BUFFER_SIZE;
                void *src       = ibufh;
                decoder.decode(&xobuf, &outleft, &src, &inleft);
                ibufh           = reinterpret_cast<uint8_t *>(src);

                // Is there any data to output?
                if (xobuf > obuf)
                {
                    ssize_t to_write = (xobuf - obuf) * sizeof(lsp_wchar_t);
                    ssize_t written = out.write(obuf, to_write);
                    UTEST_ASSERT(written == to_write);
                }
            }
        }

        decoder.close();
        UTEST_ASSERT(out.flush() == STATUS_OK);
        UTEST_ASSERT(out.close() == STATUS_OK);
        UTEST_ASSERT(in.close() == STATUS_OK);
    }

    UTEST_MAIN
    {
        LSPString fenc, fdec, fsrc;
        UTEST_ASSERT(fenc.fmt_utf8("tmp/utest-%s-encoded.tmp", full_name()));
        UTEST_ASSERT(fdec.fmt_utf8("tmp/utest-%s-decoded.tmp", full_name()));

        UTEST_ASSERT(fsrc.set_utf8("res/test/io/iconv/01-de-utf8.txt"));
        testDecodeFile(&fsrc, &fenc, "utf8");
    }

UTEST_END

