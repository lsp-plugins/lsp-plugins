/*
 * system.cpp
 *
 *  Created on: 17 мар. 2019 г.
 *      Author: sadko
 */


#include <core/system.h>

#ifdef PLATFORM_WINDOWS
    #include <winbase.h>
#else
    #include <stdlib.h>
    #include <errno.h>
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    namespace system
    {
        status_t get_env_var(const LSPString *name, LSPString *dst)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;

#ifdef PLATFORM_WINDOWS
#else
            const char *nname = name->get_native();
            if (nname == NULL)
                return STATUS_NO_MEM;
#ifdef _GNU_SOURCE
            char *var = secure_getenv(nname);
#else
            char *var = getenv(nname);
#endif
            if (var == NULL)
                return STATUS_NOT_FOUND;
            if (dst != NULL)
            {
                if (!dst->set_native(var))
                    return STATUS_NO_MEM;
            }
            return STATUS_OK;
#endif /* PLATFORM_WINDOWS */
        }

        status_t get_env_var(const char *name, LSPString *dst)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString sname;
            if (!sname.set_utf8(name))
                return STATUS_NO_MEM;
            return get_env_var(&sname, dst);
        }

        status_t set_env_var(const LSPString *name, const LSPString *value)
        {
#ifdef PLATFORM_WINDOWS
#else
            const char *nname = name->get_native();
            if (nname == NULL)
                return STATUS_NO_MEM;

            int res;
            if (value != NULL)
            {
                const char *nvalue = value->get_native();
                if (nvalue == NULL)
                    return STATUS_NO_MEM;
                res = ::setenv(nname, nvalue, 1);
            }
            else
                res = ::unsetenv(nname);

            if (res == 0)
                return STATUS_OK;
            switch (res)
            {
                case EINVAL: return STATUS_INVALID_VALUE;
                case ENOMEM: return STATUS_NO_MEM;
                default: break;
            }
            return STATUS_UNKNOWN_ERR;
#endif /* PLATFORM_WINDOWS */
        }

        status_t set_env_var(const char *name, const char *value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString sname;
            if (!sname.set_utf8(name))
                return STATUS_NO_MEM;
            if (value == NULL)
                return set_env_var(&sname, NULL);

            LSPString svalue;
            if (!svalue.set_utf8(value))
                return STATUS_NO_MEM;
            return set_env_var(&sname, &svalue);
        }

        status_t set_env_var(const char *name, const LSPString *value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString sname;
            if (!sname.set_utf8(name))
                return STATUS_NO_MEM;
            return set_env_var(&sname, value);
        }

        status_t remove_env_var(const LSPString *name)
        {
#ifdef PLATFORM_WINDOWS
#else
            const char *nname = name->get_native();
            if (nname == NULL)
                return STATUS_NO_MEM;
            int res = ::unsetenv(nname);
            if (res == 0)
                return STATUS_OK;
            switch (res)
            {
                case EINVAL: return STATUS_INVALID_VALUE;
                case ENOMEM: return STATUS_NO_MEM;
                default: break;
            }
            return STATUS_UNKNOWN_ERR;
#endif
        }

        status_t remove_env_var(const char *name)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString sname;
            if (!sname.set_utf8(name))
                return STATUS_NO_MEM;
            return remove_env_var(&sname);
        }
    }
}


