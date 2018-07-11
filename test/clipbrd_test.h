#include <ui/tk/tk.h>
#include <core/io/IInputStream.h>
#include <core/io/IOutputStream.h>

namespace clipbrd_test
{
    using namespace lsp::tk;
    using namespace lsp::io;

    int test(int argc, const char **argv)
    {
        LSPClipboard *clip = new LSPClipboard();

        FILE *fd = fopen("tmp/lsp-plugins-dbg.profile", "rb");
        uint8_t buf[4096], *ptr;

        // Write to stream
        IOutputStream *os = clip->write("application/octet-stream");
        lsp_trace("Stream position: %d", int(os->position()));

        while (true)
        {
            ssize_t n = fread(buf, sizeof(uint8_t), sizeof(buf)/sizeof(uint8_t), fd);
            if (n <= 0)
                break;
            ptr = buf;
            while (n > 0)
            {
                lsp_trace("Stream position: %d", int(os->position()));
                ssize_t k = os->write(ptr, n);
                if (k < 0)
                {
                    lsp_error("Could not write %d bytes", int(n));
                    return -1;
                }
                n -= k;
                ptr += k;
            }
        }

        // Reset the position
        lsp_trace("Clipboard object size: %d", int(clip->size()));
        lsp_trace("Clipboard object capacity: %d", int(clip->capacity()));
        os->seek(1234);
        lsp_trace("Stream position: %d", int(os->position()));
        fseek(fd, 1234, SEEK_SET);
        while (true)
        {
            ssize_t n = fread(buf, sizeof(uint8_t), sizeof(buf)/sizeof(uint8_t), fd);
            if (n <= 0)
                break;
            ptr = buf;
            while (n > 0)
            {
                lsp_trace("Stream position: %d", int(os->position()));
                ssize_t k = os->write(ptr, n);
                if (k < 0)
                {
                    lsp_error("Could not write %d bytes", int(n));
                    return -1;
                }
                n -= k;
                ptr += k;
            }
        }

        lsp_trace("Stream position: %d", int(os->position()));

        // Close stream and file
        os->close();
        fclose(fd);

        fd = fopen("output.bin", "w");
        IInputStream *is = clip->read("text/html");
        if (is != NULL)
        {
            lsp_error("Invalid content type accepted");
            return -3;
        }

        is = clip->read("application/octet-stream");

        while (true)
        {
            lsp_trace("Stream position: %d", int(is->position()));
            ssize_t n = is->read(buf, sizeof(buf));
            if (n <= 0)
                break;
            ptr = buf;
            while (n > 0)
            {
                ssize_t k = fwrite(buf, sizeof(uint8_t), n, fd);
                if (k < 0)
                {
                    lsp_error("Could not write %d bytes", int(n));
                    return -1;
                }
                n -= k;
                ptr += k;
            }
        }

        is->seek(1234);
        lsp_trace("Stream position: %d", int(is->position()));
        fseek(fd, 1234, SEEK_SET);

        while (true)
        {
            lsp_trace("Stream position: %d", int(is->position()));
            ssize_t n = is->read(buf, sizeof(buf));
            if (n <= 0)
                break;
            ptr = buf;
            while (n > 0)
            {
                ssize_t k = fwrite(buf, sizeof(uint8_t), n, fd);
                if (k < 0)
                {
                    lsp_error("Could not write %d bytes", int(n));
                    return -1;
                }
                n -= k;
                ptr += k;
            }
        }

        is->close();
        fclose(fd);

        clip->close(); // Should automatically call delete

        return 0;
    }
}
