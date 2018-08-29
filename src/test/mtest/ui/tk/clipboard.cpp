/*
 * clipboard.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <ui/tk/tk.h>
#include <core/io/IInputStream.h>
#include <core/io/IOutputStream.h>

using namespace lsp::tk;
using namespace lsp::io;

MTEST_BEGIN("ui.tk", clipboard)

    MTEST_MAIN
    {
        LSPClipboard *clip = new LSPClipboard();

        FILE *fd = fopen("tmp/lsp-plugins-dbg.profile", "rb");
        uint8_t buf[4096], *ptr;

        // Write to stream
        IOutputStream *os = clip->write("application/octet-stream");
        printf("Stream position: %d", int(os->position()));

        while (true)
        {
            ssize_t n = fread(buf, sizeof(uint8_t), sizeof(buf)/sizeof(uint8_t), fd);
            if (n <= 0)
                break;
            ptr = buf;
            while (n > 0)
            {
                printf("Stream position: %d", int(os->position()));
                ssize_t k = os->write(ptr, n);
                MTEST_ASSERT_MSG(k >= 0, "Could not write %d bytes", int(n));
                n -= k;
                ptr += k;
            }
        }

        // Reset the position
        printf("Clipboard object size: %d", int(clip->size()));
        printf("Clipboard object capacity: %d", int(clip->capacity()));
        os->seek(1234);
        printf("Stream position: %d", int(os->position()));
        fseek(fd, 1234, SEEK_SET);
        while (true)
        {
            ssize_t n = fread(buf, sizeof(uint8_t), sizeof(buf)/sizeof(uint8_t), fd);
            if (n <= 0)
                break;
            ptr = buf;
            while (n > 0)
            {
                printf("Stream position: %d", int(os->position()));
                ssize_t k = os->write(ptr, n);
                MTEST_ASSERT_MSG(k >= 0, "Could not write %d bytes", int(n));
                n -= k;
                ptr += k;
            }
        }

        printf("Stream position: %d", int(os->position()));

        // Close stream and file
        os->close();
        fclose(fd);

        fd = fopen("output.bin", "w");
        IInputStream *is = clip->read("text/html");
        MTEST_ASSERT_MSG(is == NULL, "Invalid content type accepted");

        is = clip->read("application/octet-stream");

        while (true)
        {
            printf("Stream position: %d", int(is->position()));
            ssize_t n = is->read(buf, sizeof(buf));
            if (n <= 0)
                break;
            ptr = buf;
            while (n > 0)
            {
                ssize_t k = fwrite(buf, sizeof(uint8_t), n, fd);
                MTEST_ASSERT_MSG(k >= 0, "Could not write %d bytes", int(n));
                n -= k;
                ptr += k;
            }
        }

        is->seek(1234);
        printf("Stream position: %d", int(is->position()));
        fseek(fd, 1234, SEEK_SET);

        while (true)
        {
            printf("Stream position: %d", int(is->position()));
            ssize_t n = is->read(buf, sizeof(buf));
            if (n <= 0)
                break;
            ptr = buf;
            while (n > 0)
            {
                ssize_t k = fwrite(buf, sizeof(uint8_t), n, fd);
                MTEST_ASSERT_MSG(k >= 0, "Could not write %d bytes", int(n));
                n -= k;
                ptr += k;
            }
        }

        is->close();
        fclose(fd);

        clip->close(); // Should automatically call delete
    }
MTEST_END


