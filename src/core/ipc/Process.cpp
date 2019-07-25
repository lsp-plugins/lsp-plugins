/*
 * Process.cpp
 *
 *  Created on: 24 июл. 2019 г.
 *      Author: sadko
 */

#include <core/ipc/Process.h>

namespace lsp
{
    namespace ipc
    {
        
        Process::Process()
        {
            nStatus             = PSTATUS_CREATED;
            nExitCode           = 0;

#ifdef PLATFORM_WINDOWS
            sPI.hProcess        = 0;
            sPI.hThread         = 0;
            sPI.dwProcessId     = 0;
            sPI.dwThreadId      = 0;
#else
            nPID                = 0;
#endif
        }
        
        Process::~Process()
        {
            do_clear_args(&vArgs);
            do_clear_env(&vEnv);
        }
    
        void Process::do_clear_args(cvector<LSPString> *args)
        {
            for (size_t i=0, n=args->size(); i<n; ++i)
            {
                LSPString *arg = args->at(i);
                delete arg;
            }
            args->flush();
        }

        void Process::do_clear_env(cvector<envvar_t> *env)
        {
            for (size_t i=0, n=env->size(); i<n; ++i)
            {
                envvar_t *var= env->at(i);
                delete var;
            }
            env->flush();
        }

        status_t Process::set_command(const LSPString *cmd)
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;

            if (cmd == NULL)
            {
                sCommand.clear();
                return STATUS_OK;
            }

            return (sCommand.set(cmd)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Process::set_command(const char *cmd)
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;

            if (cmd == NULL)
            {
                sCommand.clear();
                return STATUS_OK;
            }

            return (sCommand.set_utf8(cmd)) ? STATUS_OK : STATUS_NO_MEM;
        }

        size_t Process::args() const
        {
            return vArgs.size();
        }

        status_t Process::add_arg(const LSPString *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;

            LSPString *arg = new LSPString();
            if (arg == NULL)
                return STATUS_NO_MEM;

            if (!arg->set(value))
            {
                delete arg;
                return STATUS_NO_MEM;
            }
            if (!vArgs.add(arg))
            {
                delete arg;
                return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        status_t Process::add_arg(const char *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;

            LSPString *arg = new LSPString();
            if (arg == NULL)
                return STATUS_NO_MEM;

            if (!arg->set_utf8(value))
            {
                delete arg;
                return STATUS_NO_MEM;
            }
            if (!vArgs.add(arg))
            {
                delete arg;
                return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        status_t Process::set_arg(size_t index, const LSPString *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;

            LSPString *ptr = vArgs.get(index);
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (ptr->set(value)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Process::set_arg(size_t index, const char *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;

            LSPString *ptr = vArgs.get(index);
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (ptr->set_utf8(value)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Process::get_arg(size_t index, LSPString *value)
        {
            LSPString *ptr = vArgs.get(index);
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (value == NULL)
                return STATUS_OK;
            return (value->set(ptr)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Process::get_arg(size_t index, char **value)
        {
            LSPString *ptr = vArgs.get(index);
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (value == NULL)
                return STATUS_OK;

            const char *p = ptr->get_utf8();
            if (p == NULL)
                return STATUS_NO_MEM;
            char *dup   = ::strdup(p);
            if (dup == NULL)
                return STATUS_NO_MEM;

            *value      = dup;
            return STATUS_OK;
        }

        status_t Process::remove_arg(size_t index, LSPString *value)
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;

            LSPString *ptr = vArgs.get(index);
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (value != NULL)
                value->swap(ptr);

            vArgs.remove(index);
            delete ptr;
            return STATUS_OK;
        }

        status_t Process::remove_arg(size_t index, char **value)
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;

            LSPString *ptr = vArgs.get(index);
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (value != NULL)
            {
                const char *p = ptr->get_utf8();
                if (p == NULL)
                    return STATUS_NO_MEM;
                char *dup   = ::strdup(p);
                if (dup == NULL)
                    return STATUS_NO_MEM;

                *value = dup;
            }

            vArgs.remove(index);
            delete ptr;
            return STATUS_OK;
        }

        status_t Process::insert_arg(size_t index, const LSPString *value)
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString *arg = new LSPString();
            if (arg == NULL)
                return STATUS_NO_MEM;

            if (!arg->set(value))
            {
                delete arg;
                return STATUS_NO_MEM;
            }
            if (!vArgs.insert(arg, index))
            {
                delete arg;
                return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        status_t Process::insert_arg(size_t index, const char *value)
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString *arg = new LSPString();
            if (arg == NULL)
                return STATUS_NO_MEM;

            if (!arg->set_utf8(value))
            {
                delete arg;
                return STATUS_NO_MEM;
            }
            if (!vArgs.insert(arg, index))
            {
                delete arg;
                return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        status_t Process::clear_args()
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;
            do_clear_args(&vArgs);
            return STATUS_OK;
        }

        status_t Process::envs() const
        {
            return vEnv.size();
        }

        status_t Process::set_env(const LSPString *key, const LSPString *value)
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;
            if ((key == NULL) || (value == NULL))
                return STATUS_BAD_ARGUMENTS;

            envvar_t *var;
            for (size_t i=0, n=vEnv.size(); i<n; ++i)
            {
                var = vEnv.at(i);
                if (var->name.equals(key))
                    return (var->value.set(value)) ? STATUS_OK : STATUS_NO_MEM;
            }

            if ((var = new envvar_t) == NULL)
                return STATUS_NO_MEM;

            if ((!var->name.set(key)) || (!var->value.set(key)))
            {
                delete var;
                return STATUS_NO_MEM;
            }
            if (!vEnv.add(var))
            {
                delete var;
                return STATUS_NO_MEM;
            }
            return STATUS_OK;
        }

        status_t Process::set_env(const char *key, const char *value)
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;
            if ((key == NULL) || (value == NULL))
                return STATUS_BAD_ARGUMENTS;
            LSPString k, v;
            if ((!k.set_utf8(key)) || (!v.set_utf8(value)))
                return STATUS_NO_MEM;
            return set_env(&k, &v);
        }

        status_t Process::remove_env(const LSPString *key, LSPString *value)
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;
            if (key == NULL)
                return STATUS_BAD_ARGUMENTS;

            for (size_t i=0, n=vEnv.size(); i<n; ++i)
            {
                envvar_t *var = vEnv.at(i);
                if (var->name.equals(key))
                {
                    if (value != NULL)
                        value->swap(&var->value);
                    delete var;
                    vEnv.remove(i, true);
                    return STATUS_OK;
                }
            }

            return STATUS_NOT_FOUND;
        }

        status_t Process::remove_env(const char *key, char **value)
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;
            if (key == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString k;
            if (!k.set_utf8(key))
                return STATUS_NO_MEM;

            for (size_t i=0, n=vEnv.size(); i<n; ++i)
            {
                envvar_t *var = vEnv.at(i);
                if (var->name.equals(&k))
                {
                    if (value != NULL)
                    {
                        const char *v   = var->value.get_utf8();
                        if (v == NULL)
                            return STATUS_NO_MEM;
                        char *dup       = ::strdup(v);
                        if (dup == NULL)
                            return STATUS_NO_MEM;
                        *value          = dup;
                    }
                    delete var;
                    vEnv.remove(i, true);
                    return STATUS_OK;
                }
            }

            return STATUS_NOT_FOUND;
        }

        status_t Process::get_env(const LSPString *key, LSPString *value)
        {
            if (key == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;
            if (key == NULL)
                return STATUS_BAD_ARGUMENTS;

            for (size_t i=0, n=vEnv.size(); i<n; ++i)
            {
                envvar_t *var = vEnv.at(i);
                if (var->name.equals(key))
                {
                    if (value != NULL)
                    {
                        if (!value->set(&var->value))
                            return STATUS_NO_MEM;
                    }
                    return STATUS_OK;
                }
            }

            return STATUS_NOT_FOUND;
        }

        status_t Process::get_env(const char *key, char **value)
        {
            if (key == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString k;
            if (!k.set_utf8(key))
                return STATUS_NO_MEM;

            for (size_t i=0, n=vEnv.size(); i<n; ++i)
            {
                envvar_t *var = vEnv.at(i);
                if (var->name.equals(&k))
                {
                    if (value != NULL)
                    {
                        const char *v   = var->value.get_utf8();
                        if (v == NULL)
                            return STATUS_NO_MEM;
                        char *dup       = ::strdup(v);
                        if (dup == NULL)
                            return STATUS_NO_MEM;
                        *value          = dup;
                    }
                    return STATUS_OK;
                }
            }

            return STATUS_NOT_FOUND;
        }

        status_t Process::read_env(size_t idx, LSPString *key, LSPString *value)
        {
            if ((key == NULL) && (value == NULL))
                return STATUS_BAD_ARGUMENTS;

            envvar_t *var = vEnv.get(idx);
            if (var == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString tk, tv;
            if (key != NULL)
            {
                if (!tk.set(&var->name))
                    return STATUS_NO_MEM;
                if (value != NULL)
                {
                    if (!tv.set(&var->value))
                        return STATUS_NO_MEM;
                    value->swap(&tv);
                }

                key->swap(&tk);
                return STATUS_OK;
            }

            if (!tv.set(&var->value))
                return STATUS_NO_MEM;

            value->swap(&tv);
            return STATUS_OK;
        }

        status_t Process::read_env(size_t idx, char **key, char **value)
        {
            if ((key == NULL) && (value == NULL))
                return STATUS_BAD_ARGUMENTS;

            envvar_t *var = vEnv.get(idx);
            if (var == NULL)
                return STATUS_BAD_ARGUMENTS;

            char *tk, *tv;
            const char *s;

            if (key != NULL)
            {
                s = var->name.get_utf8();
                tk = (s != NULL) ? ::strdup(s) : NULL;
                if (tk == NULL)
                    return STATUS_NO_MEM;

                if (value != NULL)
                {
                    s = var->value.get_utf8();
                    tv = (s != NULL) ? ::strdup(s) : NULL;
                    if (tv == NULL)
                    {
                        ::free(tk);
                        return STATUS_NO_MEM;
                    }

                    *value = tv;
                }

                *key = tk;
                return STATUS_OK;
            }

            s = var->value.get_utf8();
            tv = (s != NULL) ? ::strdup(s) : NULL;
            if (tv == NULL)
                return STATUS_NO_MEM;

            *value = tv;
            return STATUS_OK;
        }

        status_t Process::clear_env()
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;
            do_clear_env(&vEnv);
            return STATUS_OK;
        }

        bool Process::exited()
        {
            if (nStatus == PSTATUS_EXITED)
                return true;
            else if (nStatus == PSTATUS_CREATED)
                return false;
            status_t res = wait(0);
            if (res != STATUS_OK)
                return false;
            return nStatus == PSTATUS_EXITED;
        }

        bool Process::running()
        {
            if (nStatus != PSTATUS_RUNNING)
                return false;
            status_t res = wait(0);
            if (res != STATUS_OK)
                return true;
            return nStatus == PSTATUS_RUNNING;
        }

        status_t Process::exit_code(int *code)
        {
            if (code == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (nStatus == PSTATUS_CREATED)
                return STATUS_BAD_STATE;
            if (nStatus == PSTATUS_RUNNING)
            {
                status_t res = wait(0);
                if (res != STATUS_OK)
                    return STATUS_BAD_STATE;
            }

            *code   = nExitCode;
            return STATUS_OK;
        }

        status_t Process::copy_env()
        {
            // TODO: platform-specific stuff
            return STATUS_OK;
        }

        status_t Process::launch()
        {
            if (nStatus != PSTATUS_CREATED)
                return STATUS_BAD_STATE;

            // TODO: platform-specific stuff
            return STATUS_OK;
        }

        status_t Process::wait(wsize_t millis)
        {
            // TODO: platform-specific stuff
            return STATUS_OK;
        }

    } /* namespace ipc */
} /* namespace lsp */
