/*
 * file.cpp
 *
 *  Created on: 6 мар. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <core/status.h>
#include <core/stdlib/string.h>
#include <core/io/StdioFile.h>
#include <core/io/NativeFile.h>

using namespace lsp;
using namespace lsp::io;

UTEST_BEGIN("core.io", file)

    void testClosedFile(const char *label, File &fd)
    {
        uint8_t tmpbuf[0x100];

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

    void testWriteonlyFile(File &fd)
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
        UTEST_ASSERT(fd.flush() == STATUS_OK)

        // Get status
        io::fattr_t attr;
        UTEST_ASSERT(fd.stat(&attr) == STATUS_OK);
        UTEST_ASSERT(attr.type == io::fattr_t::FT_REGULAR);
        UTEST_ASSERT(attr.size == wsize_t(written));

        // Obtain position and change it
        UTEST_ASSERT(fd.position() == written);
        wssize_t position = 0x1000;
        UTEST_ASSERT(fd.seek(position, SEEK_SET) == STATUS_OK);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that read gives failures
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.read(tmpbuf, 0) < 0);
        UTEST_ASSERT(fd.pread(0x10000, tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.pread(0x10000, tmpbuf, 0) < 0);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that positional write gives no failures
        ::memset(tmpbuf, 0x55, sizeof(tmpbuf));
        UTEST_ASSERT(fd.write(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(fd.write(tmpbuf, 0) == 0);
        position    += sizeof(tmpbuf);
        UTEST_ASSERT(fd.position() == position);
        ::memset(tmpbuf, 0xaa, sizeof(tmpbuf));
        UTEST_ASSERT(fd.pwrite(written, tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(fd.pwrite(written, tmpbuf, 0) == 0);
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

    void testReadonlyFile(File &fd)
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

        // Get status
        io::fattr_t attr;
        UTEST_ASSERT(fd.stat(&attr) == STATUS_OK);
        UTEST_ASSERT(attr.type == io::fattr_t::FT_REGULAR);
        UTEST_ASSERT(attr.size == wsize_t(read));

        // Obtain position and change it
        UTEST_ASSERT(fd.position() == read);
        wssize_t position = 0x1000;
        UTEST_ASSERT(fd.seek(position, SEEK_SET) == STATUS_OK);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that write gives failures
        UTEST_ASSERT(fd.write(tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.write(tmpbuf, 0) < 0);
        UTEST_ASSERT(fd.pwrite(0x10000, tmpbuf, sizeof(tmpbuf)) < 0);
        UTEST_ASSERT(fd.pwrite(0x10000, tmpbuf, 0) < 0);
        UTEST_ASSERT(fd.position() == position);

        // Ensure that positional read gives no failures
        ::memset(ckbuf, 0x55, sizeof(ckbuf));
        UTEST_ASSERT(fd.read(tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(fd.read(tmpbuf, 0) == 0);
        UTEST_ASSERT(::memcmp(tmpbuf, ckbuf, sizeof(tmpbuf)) == 0);
        position    += sizeof(tmpbuf);
        UTEST_ASSERT(fd.position() == position);
        ::memset(ckbuf, 0xaa, sizeof(ckbuf));
        UTEST_ASSERT(fd.pread(read - sizeof(tmpbuf), tmpbuf, sizeof(tmpbuf)) == sizeof(tmpbuf));
        UTEST_ASSERT(fd.pread(read - sizeof(tmpbuf), tmpbuf, 0) == 0);
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

    template <class TemplateFile>
        void testWriteonlyFileName(const char *label, const LSPString *path, TemplateFile &fd)
        {
            printf("Testing %s...\n", label);

            // Open file with creation and truncation
            UTEST_ASSERT(fd.open(path, File::FM_WRITE | File::FM_CREATE | File::FM_TRUNC) == STATUS_OK);
            testWriteonlyFile(fd);
        }

    template <class TemplateFile>
        void testReadonlyFileName(const char *label, const LSPString *path, TemplateFile &fd)
        {
            printf("Testing %s...\n", label);

            // Open file
            UTEST_ASSERT(fd.open(path, File::FM_READ) == STATUS_OK);
            testReadonlyFile(fd);
        }

    void testWriteonlyDescriptor(const char *label, FILE *f, StdioFile &fd)
    {
        printf("Testing %s...\n", label);
        UTEST_ASSERT(fd.wrap(f, File::FM_WRITE, false)  == STATUS_OK);
        testWriteonlyFile(fd);
    }

    void testReadonlyDescriptor(const char *label, FILE *f, StdioFile &fd)
    {
        printf("Testing %s...\n", label);
        UTEST_ASSERT(fd.wrap(f, File::FM_READ, false)  == STATUS_OK);
        testReadonlyFile(fd);
    }

    template <class TemplateFile>
        void testUnexistingFile(const char *label, TemplateFile &fd)
        {
            printf("Testing %s...\n", label);

            LSPString path;
            UTEST_ASSERT(path.fmt_utf8("tmp/utest-nonexisting-%s.tmp", full_name()));

            UTEST_ASSERT(fd.open(&path, File::FM_WRITE) != STATUS_OK);
            UTEST_ASSERT(fd.close() == STATUS_OK);
        }

    UTEST_MAIN
    {
        LSPString path;

        File none_fd;
        StdioFile std_fd;
        NativeFile native_fd;

        UTEST_ASSERT(path.fmt_utf8("tmp/utest-%s.tmp", full_name()));

        // Test closed files, all should fail the same way
        testClosedFile("test_closed_file (abstract)", none_fd);
        testClosedFile("test_closed_file (stdio)", std_fd);
        testClosedFile("test_closed_file (native)", native_fd);

        // Test stdio file
        testWriteonlyFileName("test_writeonly_filename (stdio)", &path, std_fd);
        testReadonlyFileName("test_readonly_filename (stdio)", &path, std_fd);
        testUnexistingFile("test_unexsiting_file (stdio)", std_fd);

        // Test stdio file as a wrapper
        FILE *fd = fopen(path.get_native(), "wb+");
        UTEST_ASSERT(fd != NULL);
        testWriteonlyDescriptor("test_writeonly_descriptor (stdio)", fd, std_fd);
        UTEST_ASSERT(fseek(fd, 0, SEEK_SET) == 0);
        testReadonlyDescriptor("test_readonly_descriptor (stdio)", fd, std_fd);
        UTEST_ASSERT(fclose(fd) == 0);

        // Test native file
        testWriteonlyFileName("test_writeonly_filename (native)", &path, native_fd);
        testReadonlyFileName("test_readonly_filename (native)", &path, native_fd);
        testUnexistingFile("test_unexsiting_file (native)", native_fd);
    }

UTEST_END

