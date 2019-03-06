/*
 * stdio_file.cpp
 *
 *  Created on: 6 мар. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <core/status.h>
#include <core/stdlib/string.h>
#include <core/io/StdioFile.h>

using namespace lsp;
using namespace lsp::io;

UTEST_BEGIN("core.io", stdio_file)

    void testClosedFile(const char *label)
    {
        uint8_t tmpbuf[0x100];
        StdioFile fd;

        printf("Testing %s...\n", label);

        // Test for read failues
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.pread(0, tmpbuf, sizeof(tmpbuf)) < 0);

        // Test for write failures
        UTEST_ASSERT(fd.write(tmpbuf, sizeof(tmpbuf) < 0));
        UTEST_ASSERT(fd.pwrite(0, tmpbuf, sizeof(tmpbuf) < 0));
        UTEST_ASSERT(fd.position() < 0);

        // Test for supplementary failures
        UTEST_ASSERT(fd.seek(0, File::FSK_SET) != STATUS_OK);
        UTEST_ASSERT(fd.position() < 0);
        UTEST_ASSERT(fd.flush() != STATUS_OK);
        UTEST_ASSERT(fd.sync() != STATUS_OK);

        // Test for close success
        UTEST_ASSERT(fd.close() == STATUS_OK);
    }

    void testWriteonlyFile(StdioFile &fd)
    {
        uint32_t tmpbuf[0x100];

        UTEST_ASSERT(fd.seek(0, SEEK_END) == STATUS_OK);
        UTEST_ASSERT(fd.position() == 0);

        // Write data to file
        wssize_t written = 0;
        for (size_t i=0; i<0x100; ++i)
        {
            for (size_t j=0; j<0x100; ++j)
                tmpbuf[j] = (i << 8) | j;

            UTEST_ASSERT(fd.write(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
            written += sizeof(tmpbuf);
        }

        // Obtain position and change it
        UTEST_ASSERT(fd.position() == written);
        wssize_t position = 0x1000;
        UTEST_ASSERT(fd.seek(position, SEEK_SET) == STATUS_OK);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that read gives failures
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.pread(0x10000, tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that positional write gives no failures
        ::memset(tmpbuf, 0x55, sizeof(tmpbuf));
        UTEST_ASSERT(fd.write(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        position    += sizeof(tmpbuf);
        UTEST_ASSERT(fd.position() == position);
        ::memset(tmpbuf, 0xaa, sizeof(tmpbuf));
        UTEST_ASSERT(fd.pwrite(written, tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(fd.position() == position);

        // Ensure that sync() and flush() work properly
        UTEST_ASSERT(fd.flush() == STATUS_OK)
        UTEST_ASSERT(fd.sync() == STATUS_OK)

        // Try to truncate file
        UTEST_ASSERT(fd.truncate(written) == STATUS_OK);
        UTEST_ASSERT(fd.seek(0, SEEK_END) == STATUS_OK);
        UTEST_ASSERT(fd.position() == written);
        UTEST_ASSERT(fd.pwrite(written, tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));

        // Test for close success
        UTEST_ASSERT(fd.close() == STATUS_OK);
    }

    void testReadonlyFile(StdioFile &fd)
    {
        uint32_t tmpbuf[0x100], ckbuf[0x100];
        UTEST_ASSERT(fd.position() == 0);

        // Read data from file
        wssize_t read = 0;
        for (size_t i=0; i<0x100; ++i)
        {
            if (read != 0x1000)
            {
                for (size_t j=0; j<0x100; ++j)
                    ckbuf[j] = (i << 8) | j;
            }
            else
                ::memset(ckbuf, 0x55, sizeof(ckbuf));

            UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
            UTEST_ASSERT(::memcmp(tmpbuf, ckbuf, sizeof(tmpbuf)) == 0);
            read += sizeof(tmpbuf);
        }

        // Read the last chunk and check EOF detection
        ::memset(ckbuf, 0xaa, sizeof(ckbuf));
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(::memcmp(tmpbuf, ckbuf, sizeof(tmpbuf)) == 0);
        read += sizeof(tmpbuf);
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) == (-STATUS_EOF));

        // Obtain position and change it
        UTEST_ASSERT(fd.position() == read);
        wssize_t position = 0x1000;
        UTEST_ASSERT(fd.seek(position, SEEK_SET) == STATUS_OK);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that write gives failures
        UTEST_ASSERT(fd.write(tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.pwrite(0x10000, tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that positional read gives no failures
        ::memset(ckbuf, 0x55, sizeof(ckbuf));
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(::memcmp(tmpbuf, ckbuf, sizeof(tmpbuf)) == 0);
        position    += sizeof(tmpbuf);
        UTEST_ASSERT(fd.position() == position);
        ::memset(ckbuf, 0xaa, sizeof(ckbuf));
        UTEST_ASSERT(fd.pread(read - sizeof(tmpbuf), tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(fd.position() == position);

        // Ensure that sync() and flush() do not work
        UTEST_ASSERT(fd.flush() != STATUS_OK)
        UTEST_ASSERT(fd.sync() != STATUS_OK)

        // Try to truncate file
        UTEST_ASSERT(fd.truncate(0x1000) != STATUS_OK);
        UTEST_ASSERT(fd.seek(0, SEEK_END) == STATUS_OK);
        UTEST_ASSERT(fd.position() == read);

        // Test for close success
        UTEST_ASSERT(fd.close() == STATUS_OK);
    }

    void testWriteonlyFileName(const char *label, const LSPString *path)
    {
        printf("Testing %s...\n", label);

        // Open file with creation and truncation
        StdioFile fd;
        UTEST_ASSERT(fd.open(path, File::FM_WRITE | File::FM_CREATE | File::FM_TRUNC) == STATUS_OK);

        testWriteonlyFile(fd);
    }

    void testReadonlyFileName(const char *label, const LSPString *path)
    {
        printf("Testing %s...\n", label);

        // Open file
        StdioFile fd;
        UTEST_ASSERT(fd.open(path, File::FM_READ) == STATUS_OK);
        testReadonlyFile(fd);
    }

    void testWriteonlyDescriptor(const char *label, FILE *f)
    {
        printf("Testing %s...\n", label);
        StdioFile fd;
        UTEST_ASSERT(fd.wrap(f, File::FM_WRITE, false)  == STATUS_OK);
        testWriteonlyFile(fd);
    }

    void testReadonlyDescriptor(const char *label, FILE *f)
    {
        printf("Testing %s...\n", label);
        StdioFile fd;
        UTEST_ASSERT(fd.wrap(f, File::FM_READ, false)  == STATUS_OK);
        testReadonlyFile(fd);
    }

    void testUnexistingFile(const char *label)
    {
        printf("Testing %s...\n", label);

        LSPString path;
        UTEST_ASSERT(path.fmt_utf8("tmp/utest-nonexisting-%s.tmp", full_name()));
        StdioFile fd;

        UTEST_ASSERT(fd.open(&path, File::FM_WRITE) != STATUS_OK);
        UTEST_ASSERT(fd.close() == STATUS_OK);
    }

    UTEST_MAIN
    {
        LSPString path;
        UTEST_ASSERT(path.fmt_utf8("tmp/utest-%s.tmp", full_name()));

        testClosedFile("test_closed_file");
        testWriteonlyFileName("test_writeonly_filename", &path);
        testReadonlyFileName("test_readonly_filename", &path);
        testUnexistingFile("test_unexsiting_file");

        FILE *fd = fopen(path.get_native(), "wb+");
        UTEST_ASSERT(fd != NULL);
        testWriteonlyDescriptor("test_writeonly_descriptor", fd);
        UTEST_ASSERT(fseek(fd, 0, SEEK_SET) == 0);
        testReadonlyDescriptor("test_readonly_descriptor", fd);
        UTEST_ASSERT(fclose(fd) == 0);
    }

UTEST_END

