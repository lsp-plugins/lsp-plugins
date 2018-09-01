/*
 * config.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <core/files/config.h>

using namespace lsp;
using namespace lsp::config;

static const char *FILE_NAME = "test_data/test_config.cfg";

MTEST_BEGIN("core.files", config)

    class CfgHandler: public IConfigHandler
    {
        public:
            CfgHandler() {}
            virtual ~CfgHandler() {}

        public:
            virtual status_t handle_parameter(const LSPString *name, const LSPString *value)
            {
                printf("name=%s, value=%s", name->get_native(), value->get_native());
                return STATUS_OK;
            }
    };

    MTEST_MAIN
    {
        const char *path = (argc > 0) ? argv[0] : FILE_NAME;

        CfgHandler handler;
        MTEST_ASSERT(config::load(path, &handler) == STATUS_OK);
    }

MTEST_END


