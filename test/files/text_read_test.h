#include <core/io/FileReader.h>
#include <core/io/FileWriter.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>

namespace text_read_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        io::FileReader in;
        io::FileWriter out;
        LSPString s;

        in.open("test_data/simd-003p1.txt", "UTF-8");
        while (in.read_line(&s, true) == STATUS_OK)
            puts(s.get_native());

        in.close();

        in.open("test_data/simd-003p1.txt", "UTF-8");
        out.open("test_data/simd-003p1.out", "UTF-8");

        while (in.read_line(&s, true) == STATUS_OK)
        {
            out.write(&s);
            out.write('\n');
        }
        out.flush();
        in.close();
        out.close();

        return 0;
    }
    
}
