/*
 * system.cpp
 *
 *  Created on: 17 мар. 2019 г.
 *      Author: sadko
 */


#include <core/system.h>

#ifdef PLATFORM_WINDOWS
    #include <winbase.h>
    #include <sysinfoapi.h>
#else
    #include <stdlib.h>
    #include <errno.h>
    #include <time.h>
    #include <sys/time.h>
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
            const lsp_utf16_t *nname = name->get_utf16();
            if (nname == NULL)
                return STATUS_NO_MEM;

            DWORD bufsize = ::GetEnvironmentVariableW(nname, NULL, 0);
            if (bufsize == 0)
            {
                if (::GetLastError() == ERROR_ENVVAR_NOT_FOUND)
                    return STATUS_NOT_FOUND;
                return STATUS_UNKNOWN_ERR;
            }
            else if (dst == NULL)
                return STATUS_OK;

            lsp_utf16_t *buf = reinterpret_cast<lsp_utf16_t *>(::malloc(bufsize * sizeof(lsp_utf16_t)));
            if (buf == NULL)
                return STATUS_NO_MEM;
            bufsize = ::GetEnvironmentVariableW(nname, buf, bufsize);
            if (bufsize == 0)
            {
                ::free(buf);
                return STATUS_UNKNOWN_ERR;
            }

            bool res = dst->set_utf16(buf, bufsize);
            ::free(buf);
            return (res) ? STATUS_OK : STATUS_NO_MEM;
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
            const lsp_utf16_t *nname = name->get_utf16();
            if (nname == NULL)
                return STATUS_NO_MEM;

            if (value != NULL)
            {
                const lsp_utf16_t *nvalue = value->get_utf16();
                if (nvalue == NULL)
                    return STATUS_NO_MEM;
                if (::SetEnvironmentVariableW(nname, nvalue))
                    return STATUS_OK;
            }
            else
            {
                if (::SetEnvironmentVariableW(nname, NULL))
                    return STATUS_OK;
            }
            return STATUS_UNKNOWN_ERR;
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
            const char *nname = name->get_native();
            if (nname == NULL)
                return STATUS_NO_MEM;

#ifdef PLATFORM_WINDOWS
            if (SetEnvironmentVariable(nname, NULL))
                return STATUS_OK;
            return STATUS_UNKNOWN_ERR;
#else
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
#endif /* PLATFORM_WINDOWS */
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

        status_t get_home_directory(LSPString *homedir)
        {
            if (homedir == NULL)
                return STATUS_BAD_ARGUMENTS;
#ifdef PLATFORM_WINDOWS
            LSPString drv, path;
            status_t res = get_env_var("HOMEDRIVE", &drv);
            if (res != STATUS_OK)
                return res;
            res = get_env_var("HOMEPATH", &path);
            if (res != STATUS_OK)
                return res;
            if (!drv.append(&path))
                return STATUS_NO_MEM;

            homedir->take(&drv);
            return STATUS_OK;
#else
            return get_env_var("HOME", homedir);
#endif /* PLATFORM_WINDOWS */
        }

        status_t get_home_directory(io::Path *homedir)
        {
            if (homedir == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString path;
            status_t res = get_home_directory(&path);
            if (res != STATUS_OK)
                return res;
            return homedir->set(&path);
        }

        status_t get_user_config_path(LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString upath;

#ifdef PLATFORM_WINDOWS
            status_t res = get_env_var("LOCALAPPDATA", &upath);
            if (res != STATUS_OK)
            {
                res = get_env_var("APPDATA", &upath);
                if (res != STATUS_OK)
                {
                    res = get_home_directory(upath);
                    if (res != STATUS_OK)
                        return res;
                    if (!upath.append_ascii(FILE_SEPARATOR_S ".config"))
                        return STATUS_NO_MEM;
                }
            }
#else
            status_t res = get_env_var("HOME", &upath);
            if (res != STATUS_OK)
                return res;
            if (!upath.append_ascii(FILE_SEPARATOR_S ".config"))
                return STATUS_NO_MEM;
#endif /* PLATFORM_WINDOWS */

            path->swap(&upath);
            return STATUS_OK;
        }

        status_t get_user_config_path(io::Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString upath;
            status_t res = get_user_config_path(&upath);
            if (res != STATUS_OK)
                return res;
            return path->set(&upath);
        }

#ifdef PLATFORM_WINDOWS
        void get_time(time_t *time)
        {
            FILETIME t;
            ::GetSystemTimeAsFileTime(&t);
            uint64_t itime  = (uint64_t(t.dwHighDateTime) << 32) | t.dwLowDateTime;

            time->seconds   = itime / 10000000;
            time->nanos     = (itime % 10000000) * 100;
        }
#else
        void get_time(time_t *time)
        {
            struct timespec t;
            ::clock_gettime(CLOCK_REALTIME, &t);

            time->seconds   = t.tv_sec;
            time->nanos     = t.tv_nsec;
        }
#endif /* PLATFORM_WINDOWS */
    }
}


