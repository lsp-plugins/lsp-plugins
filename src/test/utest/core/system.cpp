/*
 * system.cpp
 *
 *  Created on: 17 мар. 2019 г.
 *      Author: sadko
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

        UTEST_ASSERT(system::set_env_var(&name, snull) == STATUS_OK);
        UTEST_ASSERT(system::get_env_var(&name, snull) == STATUS_NOT_FOUND);
        UTEST_ASSERT(system::set_env_var(&name, &value) == STATUS_OK);
        UTEST_ASSERT(system::get_env_var(&name, &rvalue) == STATUS_OK);
        UTEST_ASSERT(value.equals(&rvalue));
        UTEST_ASSERT(system::set_env_var(&name, snull) == STATUS_OK);
        UTEST_ASSERT(system::get_env_var(&name, snull) == STATUS_NOT_FOUND);

        // Test home directory
        LSPString homedir;
        io::Path path;
        UTEST_ASSERT(system::get_home_directory(&homedir) == STATUS_OK);
        UTEST_ASSERT(path.set(&homedir) == STATUS_OK);
        UTEST_ASSERT(path.is_absolute());
        printf("Home directory is: %s\n", path.as_native());
    }
UTEST_END
