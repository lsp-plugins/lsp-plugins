/*
 * ByteBuffer.cpp
 *
 *  Created on: 21 авг. 2018 г.
 *      Author: vsadovnikov
 */

#include <test/ByteBuffer.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define CK_HEAD_SIGNATURE       0xa5c33c5aL
#define CK_TAIL_SIGNATURE       0x35caac53L

template <class T>
    inline T *align_pointer(void *src, size_t align)
    {
        ptrdiff_t x     = ptrdiff_t(src);
        ptrdiff_t off   = (x % align);

        return (off == 0) ?
                reinterpret_cast<T *>(src) :
                reinterpret_cast<T *>(x + align - off);
    }

inline bool check_alignment(void *src, size_t align)
{
    ptrdiff_t x     = ptrdiff_t(src);
    return !(x % align);
}

namespace test
{
    ByteBuffer::ByteBuffer(size_t samples, size_t align, bool aligned)
    {
        allocate(samples, align, aligned);
        randomize();
    }

    ByteBuffer::ByteBuffer(const ByteBuffer &src)
    {
        allocate(src.nLength, src.nAlign, src.bAligned);
        memcpy(pBuffer, src.pBuffer, src.nLength);
    }

    ByteBuffer::ByteBuffer(const void *data, size_t samples, size_t align, bool aligned)
    {
        allocate(samples, align, aligned);
        memcpy(pBuffer, data, samples);
    }

    ByteBuffer::~ByteBuffer()
    {
        if (pData != NULL)
        {
            delete  [] pData;
            pData = NULL;
        }
        pBuffer = NULL;
    }

    void ByteBuffer::allocate(size_t samples, size_t align, bool aligned)
    {
        size_t alloc    = sizeof(uint32_t)*2 + samples + align;
        nAlign          = align;
        nLength         = samples;
        pData           = new uint8_t[alloc];
        bAligned        = aligned;

        uint8_t *head   = &pData[sizeof(uint32_t)];
        if (aligned)
            head            = align_pointer<uint8_t>(head, nAlign);
        else if (check_alignment(head, nAlign))
            head       += sizeof(float);

        pBuffer         = head;

        // Mark the head and the tail of the buffer with signatures
        uint32_t key    = uint32_t(ptrdiff_t(this));
        uint32_t *ptr   = reinterpret_cast<uint32_t *>(&pBuffer[-sizeof(uint32_t)]);
        *ptr            = uint32_t(CK_HEAD_SIGNATURE ^ key);
        ptr             = reinterpret_cast<uint32_t *>(&pBuffer[nLength]);
        *ptr            = uint32_t(CK_TAIL_SIGNATURE ^ key);
    }

    void ByteBuffer::randomize()
    {
        for (size_t i=0; i<nLength; ++i)
            pBuffer[i] = uint8_t(rand());
    }

    void ByteBuffer::fill_zero()
    {
#if defined(PLATFORM_WINDOWS)
        memset(pBuffer, 0, nLength);
#else
        bzero(pBuffer, nLength);
#endif
    }

    bool ByteBuffer::validate() const
    {
        uint32_t key            = uint32_t(ptrdiff_t(this));
        const uint32_t *ptr     = reinterpret_cast<uint32_t *>(&pBuffer[-sizeof(uint32_t)]);
        if (*ptr != (CK_HEAD_SIGNATURE ^ key))
            return false;
        ptr                     = reinterpret_cast<uint32_t *>(&pBuffer[nLength]);
        return (*ptr == (CK_TAIL_SIGNATURE ^ key));
    }

    bool ByteBuffer::equals(const ByteBuffer &src) const
    {
        return memcmp(pBuffer, src.pBuffer, nLength) == 0;
    }

    bool ByteBuffer::equals(const void *src, size_t count) const
    {
        if (count > nLength)
            return false;
        return memcmp(pBuffer, src, count) == 0;
    }

    bool ByteBuffer::equals(const void *src, size_t offset, size_t count) const
    {
        if ((count + offset) > nLength)
            return false;
        return memcmp(&pBuffer[offset], src, count) == 0;
    }

    void ByteBuffer::dump(const char *text) const
    {
        printf("%s: ", text);
        for (size_t i=0; i<nLength; ++i)
            printf("%02x ", pBuffer[i]);
        printf("\n");
    }

    void ByteBuffer::dump(const char *text, size_t from, size_t count) const
    {
        printf("%s: ", text);
        for (size_t i=0; i<nLength; ++i)
            printf("%02x ", pBuffer[i]);
        printf("\n");
    }
}
