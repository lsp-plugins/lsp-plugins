/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 17 мар. 2019 г.
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

#include <test/utest.h>
#include <core/system.h>
#include <core/LSPString.h>
#include <core/io/Path.h>

using namespace lsp;

UTEST_BEGIN("core", system)
    UTEST_MAIN
    {
        LSPString name, value, rvalue;
        LSPString *snull = NULL;

        // Test environment variables
        UTEST_ASSERT(name.set_utf8("LSP_TEST_ENVIRONMENT_VARIABLE"));
        UTEST_ASSERT(value.set_utf8("LSP_TEST_ENVIRONMENT_VALUE"));

        printf("Cleaning env variable %s\n", name.get_native());
        UTEST_ASSERT(system::set_env_var(&name, snull) == STATUS_OK);
        printf("Checking env variable %s not set\n", name.get_native());
        UTEST_ASSERT(system::get_env_var(&name, snull) == STATUS_NOT_FOUND);
        printf("Setting env variable %s to %s\n", name.get_native(), value.get_native());
        UTEST_ASSERT(system::set_env_var(&name, &value) == STATUS_OK);
        printf("Reading env variable %s\n", name.get_native());
        UTEST_ASSERT(system::get_env_var(&name, &rvalue) == STATUS_OK);
        printf("Read env variable %s as %s\n", name.get_native(), rvalue.get_native());
        UTEST_ASSERT(value.equals(&rvalue));
        printf("Cleaning env variable %s\n", name.get_native());
        UTEST_ASSERT(system::set_env_var(&name, snull) == STATUS_OK);
        printf("Checking env variable %s not set\n", name.get_native());
        UTEST_ASSERT(system::get_env_var(&name, snull) == STATUS_NOT_FOUND);

        // Test home directory
        LSPString homedir;
        io::Path path;
        UTEST_ASSERT(system::get_home_directory(&homedir) == STATUS_OK);
        UTEST_ASSERT(path.set(&homedir) == STATUS_OK);
        UTEST_ASSERT(path.is_absolute());
        printf("Home directory is: %s\n", path.as_native());

        // Test config directory
        LSPString config;
        io::Path cpath;
        UTEST_ASSERT(system::get_user_config_path(&config) == STATUS_OK);
        UTEST_ASSERT(cpath.set(&config) == STATUS_OK);
        UTEST_ASSERT(cpath.is_absolute());
        printf("Configuration directory is: %s\n", cpath.as_native());
    }
UTEST_END
