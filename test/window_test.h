#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>
#include <core/windows.h>

namespace window_test
{
    using namespace lsp;

    #define BUF_ITEMS 1024

    int test(int argc, const char **argv)
    {
        dsp::init();

        float *buf      = NULL;
        float *windows[windows::TOTAL];

        size_t count    = BUF_ITEMS * windows::TOTAL;
        buf             = new float[count];
        if (buf == NULL)
            return -1;

        float *ptr      = buf;
        for (size_t i=0; i<windows::TOTAL; ++i)
        {
            windows::window(ptr, BUF_ITEMS, windows::window_t(i + windows::FIRST));

            windows[i]      = ptr;
            ptr            += BUF_ITEMS;
        }

        // Print header
        printf("Index;");
        for (size_t i=0; i<windows::TOTAL; ++i)
            printf("%s;", windows::windows[i]);
        printf("\n");

        // Print items
        for (size_t i=0; i<BUF_ITEMS; ++i)
        {
            printf("%d;", int(i));
            for (size_t j=0; j< windows::TOTAL; ++j)
                printf("%.5f;", windows[j][i]);
            printf("\n");
        }

        delete [] buf;

        return 0;
    }
    
}
