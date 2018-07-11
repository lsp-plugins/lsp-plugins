#include <core/files/3d/ObjFileParser.h>
#include <core/files/3d/IFileHandler3D.h>

namespace objfile3d_test
{
    const char *FILE_NAME = "test_data/3d/test-model.obj";
    const size_t DUMP_SAMPLES = 32;

    using namespace lsp;

    int test(int argc, const char **argv)
    {
        using namespace lsp;
        dsp::init();

        IFileHandler3D hdl;

        status_t status = ObjFileParser::parse(FILE_NAME, &hdl);

        return (status == STATUS_OK) ? 0 : -1;
    }
}
