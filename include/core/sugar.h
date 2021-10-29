/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 17 марта 2016 г.
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

#ifndef CORE_SUGAR_H_
#define CORE_SUGAR_H_

#include <stdlib.h>

// Alignment
#define ALIGN64                         0x40
#define ALIGN_SIZE(x, size)             (((x) + (size) - 1) & (~size_t((size) - 1)))

#define LSP_STRINGIFY__(s)              #s
#define LSP_STRINGIFY(s)                LSP_STRINGIFY__(s)

// Array management
#define DROP_ARRAY(ptr)     \
    if (ptr != NULL) \
    { \
        delete [] ptr; \
        ptr = NULL; \
    }

template <class T>
    T *ALIGN_PTR(T *src, size_t align = DEFAULT_ALIGN)
    {
        ptrdiff_t x     = ptrdiff_t(src);
        ptrdiff_t mask  = align-1;
        return (x & mask) ? reinterpret_cast<T *>((x + align)&(~mask)) : src;
    }

template <class T>
    bool IS_PTR_ALIGNED(T *src, size_t align = DEFAULT_ALIGN)
    {
        ptrdiff_t x     = ptrdiff_t(src);
        ptrdiff_t mask  = align-1;
        return !(x & mask);
    }

/** Allocate aligned pointer
 *
 * @param ptr reference to pointer to store allocated pointer for future free() operation
 * @param count number of elements to allocate
 * @param align alignment, should be power of 2, by default DEFAULT_ALIGN
 * @return aligned pointer as a result of alignment of ptr to align boundary or NULL if allocation failed
 * @example
 *      void *x = NULL;
 *      float *a = alloc_aligned<float>(x, 1000); // Allocate 1000 floats aligned to DEFAULT_ALIGN boundary
 *      if (a == NULL)
 *          return ERROR;
 *      // Do some stuff
 *      free_aligned(x);
 *      a = NULL;
 */
template <class T, class P>
    inline T *alloc_aligned(P * &ptr, size_t count, size_t align=DEFAULT_ALIGN)
    {
        // Check for power of 2
        if ((!align) || (align & (align-1)))
            return NULL;

        // Allocate data
        void *p         = malloc((count * sizeof(T)) + align);
        if (p == NULL)
            return NULL;

        // Store pointer
        ptr             = reinterpret_cast<P *>(p);

        // Return aligned pointer
        ptrdiff_t x     = ptrdiff_t(p);
        ptrdiff_t mask  = align-1;
        return reinterpret_cast<T *>((x & mask) ? ((x + align)&(~mask)) : x);
    }

template <class T>
    inline void swap(T &a, T &b)
    {
        T tmp = a;
        a = b;
        b = tmp;
    }

template <class A, class B>
    inline A lsp_max(A a, B b)
    {
        return (a > b) ? a : b;
    }

template <class A, class B>
    inline A lsp_min(A a, B b)
    {
        return (a <= b) ? a : b;
    }

template <class A, class B, class C>
    inline A lsp_limit(A a, B min, C max)
    {
        return (a < min) ? min : ((a > max) ? max : a);
    }

/** Free aligned pointer and write NULL to it
 *
 * @param ptr pointer to free
 */
template <class P>
    inline void free_aligned(P * &ptr)
    {
        if (ptr == NULL)
            return;
        P *tptr = ptr;
        ptr = NULL;
        free(tptr);
    }

#if defined(ARCH_I386)
    inline uint32_t seed_addr(const void *ptr)
    {
        return uint32_t(ptr);
    }
#elif defined(ARCH_X86_64)
    inline uint32_t seed_addr(const void *ptr)
    {
        return uint32_t(ptrdiff_t(ptr)) ^ uint32_t(ptrdiff_t(ptr) >> 32);
    }
#else
    inline uint32_t seed_addr(const void *ptr)
    {
        return uint32_t(ptrdiff_t(ptr));
    }
#endif

#endif /* CORE_SUGAR_H_ */
