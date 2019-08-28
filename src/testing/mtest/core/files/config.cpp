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
        private:
            Test *test;

        public:
            CfgHandler(Test *tst): test(tst) {}
            virtual ~CfgHandler() {}

        public:
            virtual status_t handle_parameter(const LSPString *name, const LSPString *value, size_t flags)
            {
                test->printf("name=%s, value=%s, flags=0x%x", name->get_native(), value->get_native(), int(flags));
                return STATUS_OK;
            }
    };

    MTEST_MAIN
    {
        const char *path = (argc > 0) ? argv[0] : FILE_NAME;

        CfgHandler handler(this);
        MTEST_ASSERT(config::load(path, &handler) == STATUS_OK);
    }

MTEST_END


