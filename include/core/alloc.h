/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 25 апр. 2016 г.
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

#ifndef CORE_ALLOC_H_
#define CORE_ALLOC_H_

#include <stdlib.h>

//#define LSP_MEMORY_PROFILING

#if defined(LSP_DEBUG) && defined(LSP_MEMORY_PROFILING)

    #define LSP_ALLOC_MODE_MALLOC           (1 << 0)
    #define LSP_ALLOC_MODE_CALLOC           (1 << 1)
    #define LSP_ALLOC_MODE_NEW              (1 << 2)
    #define LSP_ALLOC_MODE_ARRNEW           (1 << 3)

    #define LSP_ALLOC_MODE_ALL              (LSP_ALLOC_MODE_MALLOC | LSP_ALLOC_MODE_CALLOC | LSP_ALLOC_MODE_NEW | LSP_ALLOC_MODE_ARRNEW)
    #define LSP_ALLOC_MODE_C_ONLY           (LSP_ALLOC_MODE_MALLOC | LSP_ALLOC_MODE_CALLOC)
    #define LSP_ALLOC_MODE_CPP_ONLY         (LSP_ALLOC_MODE_NEW | LSP_ALLOC_MODE_ARRNEW)

    #define ____lsp_location(file, line)                file ":" #line
    #define ___lsp_location(file, line)                 ____lsp_location(file, line)
    #define __lsp_location                              ___lsp_location(__FILE__, __LINE__)

    #define lsp_malloc(size)                lsp::safe_malloc(__lsp_location, size, LSP_ALLOC_MODE_MALLOC)
    #define lsp_tmalloc(type, items)        reinterpret_cast<type *>(lsp::safe_malloc(__lsp_location, sizeof(type) * (items), LSP_ALLOC_MODE_MALLOC))
    #define lsp_calloc(count, size)         lsp::safe_calloc(__lsp_location, count, size)
    #define lsp_realloc(ptr, size)          lsp::safe_realloc(__lsp_location, ptr, size, LSP_ALLOC_MODE_C_ONLY)
    #define lsp_free(ptr)                   lsp::safe_free(__lsp_location, ptr, LSP_ALLOC_MODE_C_ONLY)
    #define lsp_strdup(ptr)                 lsp::safe_strdup(__lsp_location, ptr)
    #define lsp_validate(ptr)               lsp::safe_validate(__lsp_location, ptr, LSP_ALLOC_MODE_ALL)


//    #define lsp_vdelete(ptr)                lsp::safe_delete(__lsp_location, ptr)
//    #define lsp_delete(ptr)                 lsp::safe_delete(__lsp_location, ptr)

    namespace lsp
    {
        void       *safe_malloc(const char *location, size_t size, uint32_t mode);
        void       *safe_calloc(const char *location, size_t count, size_t size);
        void       *safe_realloc(const char *location, void *ptr, size_t size, uint32_t mode);
        bool        safe_free(const char *location, void *ptr, uint32_t mode);
        bool        safe_validate(const char *location, const void *ptr, uint32_t mode);
        char       *safe_strdup(const char *location, const char *str);

//        template <class T>
//            inline T *safe_alloc_array(const char *location, size_t items)
//            {
//                T *ptr  = reinterpret_cast<T *>(lsp::safe_malloc(__lsp_location, items * sizeof(T)));
//                if (ptr == NULL)
//                    return NULL;
//                return new (ptr) T[items];
//            }
    }

//    inline void *operator new(size_t size, const char *location, int tag)
//    {
//        return lsp::safe_malloc(location, size, LSP_ALLOC_MODE_NEW);
//    }
//
//    inline void *operator new[](size_t size, const char *location, int tag)
//    {
//        return lsp::safe_malloc(location, size, LSP_ALLOC_MODE_ARRNEW);
//    }


#else
    #define lsp_malloc(size)                ::malloc(size)
    #define lsp_tmalloc(type, items)        reinterpret_cast<type *>(::malloc(sizeof(type) * (items)))
    #define lsp_calloc(count, size)         ::calloc(count, size)
    #define lsp_realloc(ptr, size)          ::realloc(ptr, size)
    #define lsp_trealloc(type, ptr, items)  reinterpret_cast<type *>(::realloc(ptr, sizeof(type) * (items)))
    #define lsp_free(ptr)                   lsp::safe_free(ptr)
    #define lsp_strdup(ptr)                 ::strdup(ptr)
    #define lsp_validate(ptr)               true
//    #define lsp_new                         new

    namespace lsp
    {
        inline bool safe_free(void *ptr)
        {
            ::free(ptr);
            return true;
        }
    }
#endif /* LSP_DEBUG */

namespace lsp
{
    char *lsp_strbuild(const char *str, size_t len);
}


#endif /* CORE_ALLOC_H_ */
