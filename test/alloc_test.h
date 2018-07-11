#include <core/types.h>
#include <core/dsp.h>
#include <core/alloc.h>

namespace alloc_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        lsp_trace("Validating normal malloc...");
        uint8_t *data   = reinterpret_cast<uint8_t *>(lsp_malloc(0x20));
        if (data == NULL)
            return -1;
        if (!lsp_validate(data))
            return -2;
        if (!lsp_free(data))
            return -3;

        lsp_trace("Validating alloc/realloc/free...");
        data    = reinterpret_cast<uint8_t *>(lsp_malloc(0x20));
        if (data == NULL)
            return -1;
        if (!lsp_validate(data))
            return -2;
        data    = reinterpret_cast<uint8_t *>(lsp_realloc(data, 0x40));
        if (data == NULL)
            return -1;
        if (!lsp_validate(data))
            return -2;
        if (!lsp_free(data))
            return -3;

        lsp_trace("Corrupting buffer at the head...");
        data    = reinterpret_cast<uint8_t *>(lsp_malloc(0x20));
        if (data == NULL)
            return -1;
        if (!lsp_validate(data))
            return -2;
        data[-1]    = 0xff;
        if (lsp_free(data))
            return -3;

        lsp_trace("Corrupting buffer at the tail...");
        data    = reinterpret_cast<uint8_t *>(lsp_malloc(0x20));
        if (data == NULL)
            return -1;
        if (!lsp_validate(data))
            return -2;
        data[0x21]  = 0xff;
        if (lsp_free(data))
            return -3;

        return 0;
    }
    
}
