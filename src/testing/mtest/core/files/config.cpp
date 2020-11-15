/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 авг. 2018 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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


