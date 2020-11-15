/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 04 апр. 2016 г.
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

#ifndef DSP_ARCH_NATIVE_BSWAP_H_
#define DSP_ARCH_NATIVE_BSWAP_H_

inline uint16_t __lsp_forced_inline     byte_swap(uint16_t v)
{
    return (v >> 8) | (v << 8);
}

inline uint32_t __lsp_forced_inline     byte_swap(uint32_t v)
{
    v   = ((v & 0xff00ff00) >> 8) | ((v & 0x00ff00ff) << 8);
    return (v >> 16) | (v << 16);
}

inline uint64_t __lsp_forced_inline     byte_swap(uint64_t v)
{
    v   = ((v & 0xff00ff00ff00ff00ULL) >> 8)  | ((v & 0x00ff00ff00ff00ffULL) << 8);
    v   = ((v & 0xffff0000ffff0000ULL) >> 16) | ((v & 0x0000ffff0000ffffULL) << 16);
    return (v >> 32) | (v << 32);
}

inline float __lsp_forced_inline byte_swap(float v)
{
    union {
        float       fValue;
        uint32_t    uValue;
    } tmp;
    tmp.fValue      = v;
    tmp.uValue      = byte_swap(tmp.uValue);
    return tmp.fValue;
}

inline double __lsp_forced_inline byte_swap(double v)
{
    union {
        double   fValue;
        uint64_t uValue;
    } tmp;
    tmp.fValue      = v;
    tmp.uValue      = byte_swap(tmp.uValue);
    return tmp.fValue;
}

inline void __lsp_forced_inline byte_swap(uint16_t *v, size_t n)
{
    while (n--)
    {
        *v          = byte_swap(*v);
        v           ++;
    }
}

inline void __lsp_forced_inline byte_swap(int16_t *v, size_t n)
{
    while (n--)
    {
        *v          = byte_swap(uint16_t(*v));
        v           ++;
    }
}

inline void __lsp_forced_inline byte_swap(uint32_t *v, size_t n)
{
    while (n--)
    {
        *v          = byte_swap(*v);
        v           ++;
    }
}

inline void __lsp_forced_inline byte_swap(int32_t *v, size_t n)
{
    while (n--)
    {
        *v          = byte_swap(uint32_t(*v));
        v           ++;
    }
}

inline void __lsp_forced_inline byte_swap(uint64_t *v, size_t n)
{
    while (n--)
    {
        *v          = byte_swap(*v);
        v           ++;
    }
}

inline void __lsp_forced_inline byte_swap(int64_t *v, size_t n)
{
    while (n--)
    {
        *v          = byte_swap(uint64_t(*v));
        v           ++;
    }
}

inline void __lsp_forced_inline byte_swap(float *v, size_t n)
{
    union {
        float      *fValue;
        uint32_t   *uValue;
    } tmp;
    tmp.fValue      = v;
    uint32_t *p     = tmp.uValue;

    while (n--)
    {
        *p          = byte_swap(*p);
        p           ++;
    }
}

inline void __lsp_forced_inline byte_swap(double *v, size_t n)
{
    union {
        double     *fValue;
        uint64_t   *uValue;
    } tmp;
    tmp.fValue      = v;
    uint64_t *p     = tmp.uValue;

    while (n--)
    {
        *p          = byte_swap(*p);
        p           ++;
    }
}

#if defined(WCHART_16BIT)
    inline wchar_t byte_swap(wchar_t v)
    {
        return wchar_t(byte_swap(uint16_t(v)));
    }
#elif defined(WCHART_32BIT)
    inline wchar_t byte_swap(wchar_t v)
    {
        return wchar_t(byte_swap(uint32_t(v)));
    }
#endif

#endif /* DSP_ARCH_NATIVE_BSWAP_H_ */
