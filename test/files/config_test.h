#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>
#include <core/files/config.h>

namespace config_test
{
    using namespace lsp;
    using namespace lsp::config;

    class CfgHandler: public IConfigHandler
    {
        public:
            CfgHandler() {}
            virtual ~CfgHandler() {}

        public:
            virtual status_t handle_parameter(const LSPString *name, const LSPString *value)
            {
                lsp_trace("name=%s, value=%s", name->get_native(), value->get_native());
                return STATUS_OK;
            }
    };

    int test(int argc, const char **argv)
    {
        CfgHandler handler;
        config::load("test_data/test_config.cfg", &handler);

        return 0;
    }
    
}
