#include <core/types.h>

namespace lsp
{
    extern int gen_xml_resource_file(const char *path, const char *fname);
}

namespace res_gen
{
    int test(int argc, const char **argv)
    {
        return lsp::gen_xml_resource_file("res/ui", "xml_resources.gen");
    }
}
