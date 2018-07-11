/*
 * sugar.h
 *
 *  Created on: 17 марта 2016 г.
 *      Author: sadko
 */

#ifndef CORE_SUGAR_H_
#define CORE_SUGAR_H_

// Alignment
#define DEFAULT_ALIGN                   0x10
#define ALIGN64                         0x40
#define ALIGN_SIZE(x, size)             (((x) + size - 1) & (~size_t(size - 1)))

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
