/*
 * Library.cpp
 *
 *  Created on: 24 апр. 2019 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/ipc/Library.h>

namespace lsp
{
    namespace ipc
    {
        int Library::hTag    = 0x12345678;
        
        Library::Library()
        {
            hDlSym      = NULL;
            nLastError  = STATUS_OK;
        }
        
        Library::~Library()
        {
            close();
        }
    
        status_t Library::open(const char *path)
        {
            if (path == NULL)
                return nLastError = STATUS_BAD_ARGUMENTS;
            LSPString tmp;
            if (!tmp.set_utf8(path))
                return nLastError = STATUS_NO_MEM;
            return open(&tmp);
        }

        status_t Library::open(const LSPString *path)
        {
            if (path == NULL)
                return nLastError = STATUS_BAD_ARGUMENTS;
            if (hDlSym != NULL)
                return nLastError = STATUS_OPENED;

            #ifdef PLATFORM_WINDOWS
                const WCHAR *str  = path->get_utf16();
                if (str == NULL)
                    return STATUS_NO_MEM;
                HMODULE handle  = ::LoadLibraryW(str);

                if (handle == NULL)
                    return nLastError = STATUS_NOT_FOUND;
            #else
                const char *str  = path->get_utf8();
                if (str == NULL)
                    return STATUS_NO_MEM;
                void *handle    = ::dlopen(str, RTLD_NOW);
                if (handle == NULL)
                {
                    lsp_warn("Error loading module %s: %s", path->get_native(), ::dlerror());
                    return nLastError = STATUS_NOT_FOUND;
                }
            #endif

            hDlSym  = handle;
            return nLastError = STATUS_OK;
        }

        status_t Library::open(const io::Path *path)
        {
            if (path == NULL)
                return nLastError = STATUS_BAD_ARGUMENTS;
            return open(path->as_string());
        }

        void *Library::import(const char *name)
        {
            if (name == NULL)
            {
                nLastError = STATUS_BAD_ARGUMENTS;
                return NULL;
            }
            if (hDlSym == NULL)
            {
                nLastError = STATUS_BAD_STATE;
                return NULL;
            }

            #ifdef PLATFORM_WINDOWS
                void *ptr   = reinterpret_cast<void *>(::GetProcAddress(hDlSym, name));
            #else
                void *ptr   = ::dlsym(hDlSym, name);
            #endif

            if (ptr == NULL)
                nLastError  = STATUS_NOT_FOUND;
            else
                nLastError  = STATUS_OK;

            return ptr;
        }

        void *Library::import(const LSPString *name)
        {
            if (name == NULL)
            {
                nLastError = STATUS_BAD_ARGUMENTS;
                return NULL;
            }
            if (hDlSym == NULL)
            {
                nLastError = STATUS_BAD_STATE;
                return NULL;
            }
            return import(name->get_utf8());
        }

        status_t Library::close()
        {
            if (hDlSym == NULL)
                return nLastError = STATUS_OK;
        #ifdef PLATFORM_WINDOWS
            ::FreeLibrary(hDlSym);
        #else
            ::dlclose(hDlSym);
        #endif

            hDlSym      = NULL;
            return nLastError = STATUS_OK;
        }

        void Library::swap(Library *dst)
        {
            ::swap(dst->hDlSym, hDlSym);
            ::swap(dst->nLastError, nLastError);
        }

        status_t Library::get_module_file(LSPString *path, const void *ptr)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

#ifdef PLATFORM_WINDOWS
            WCHAR *xpath = reinterpret_cast<WCHAR *>(::malloc((PATH_MAX + 1) * sizeof(WCHAR)));
            if (path == NULL)
                return STATUS_NO_MEM;

            HMODULE hm = NULL;

            if (!::GetModuleHandleExW(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCWSTR>(ptr),
                    &hm)
               )
            {
                ::free(xpath);
                return STATUS_NOT_FOUND;
            }

            if (::GetModuleFileNameW(hm, xpath, sizeof(path)) == 0)
            {
                ::free(xpath);
                return STATUS_NOT_FOUND;
            }

            if (!path->set_utf16(xpath))
            {
                ::free(path);
                return STATUS_NO_MEM;
            }
#else
            Dl_info dli;
            int res     = ::dladdr(const_cast<void *>(ptr), &dli);
            if ((res == 0) || (dli.dli_fname == NULL))
                return STATUS_NOT_FOUND;

            if (!path->set_native(dli.dli_fname))
                return STATUS_NO_MEM;
#endif /* PLATFORM_WINDOWS */

            return STATUS_OK;
        }

        status_t Library::get_module_file(io::Path *path, const void *ptr)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString tmp;
            status_t res = get_module_file(&tmp, ptr);
            if (res != STATUS_OK)
                return res;

            return path->set(&tmp);
        }

    } /* namespace io */
} /* namespace lsp */
