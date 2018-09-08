/*
 * FloatBuffer.cpp
 *
 *  Created on: 21 авг. 2018 г.
 *      Author: vsadovnikov
 */

#include <test/helpers.h>
#include <test/FloatBuffer.h>
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
    FloatBuffer::FloatBuffer(size_t samples, size_t align, bool aligned)
    {
        allocate(samples, align, aligned);
        randomize();
//        memset(pBuffer, 0, nLength * sizeof(float));
    }

    FloatBuffer::FloatBuffer(const FloatBuffer &src)
    {
        allocate(src.nLength, src.nAlign, src.bAligned);
        memcpy(pBuffer, src.pBuffer, src.nLength * sizeof(float));
    }

    FloatBuffer::~FloatBuffer()
    {
        if (pData != NULL)
        {
            delete  [] pData;
            pData = NULL;
        }
        pBuffer = NULL;
    }

    void FloatBuffer::allocate(size_t samples, size_t align, bool aligned)
    {
        size_t alloc    = sizeof(uint32_t)*2 + (samples + 1)*sizeof(float) + align;
        nAlign          = align;
        nLength         = samples;
        pData           = new uint8_t[alloc];
        bAligned        = aligned;

        uint8_t *head   = &pData[sizeof(uint32_t)];
        if (aligned)
            head            = align_pointer<uint8_t>(head, nAlign);
        else if (check_alignment(head, nAlign))
            head       += sizeof(float);

        pBuffer         = reinterpret_cast<float *>(head);

        // Mark the head and the tail of the buffer with signatures
        uint32_t key    = uint32_t(ptrdiff_t(this));
        uint32_t *ptr   = reinterpret_cast<uint32_t *>(&head[-sizeof(uint32_t)]);
        *ptr            = uint32_t(CK_HEAD_SIGNATURE ^ key);
        ptr             = reinterpret_cast<uint32_t *>(&head[nLength*sizeof(float)]);
        *ptr            = uint32_t(CK_TAIL_SIGNATURE ^ key);
    }

    void FloatBuffer::randomize_positive()
    {
        for (size_t i=0; i<nLength; ++i)
            pBuffer[i] = (float(rand())/RAND_MAX) + 0.001f;
    }

    void FloatBuffer::randomize_negative()
    {
        for (size_t i=0; i<nLength; ++i)
            pBuffer[i] = - ((float(rand())/RAND_MAX) + 0.001f);
    }

    void FloatBuffer::randomize_sign()
    {
        for (size_t i=0; i<nLength; ++i)
        {
            float tmp = (float(rand())/RAND_MAX) + 0.001f;
            pBuffer[i] = (rand() >= (RAND_MAX >> 1)) ? tmp : -tmp;
        }
    }

    void FloatBuffer::randomize()
    {
        randomize_positive();
    }

    void FloatBuffer::fill_zero()
    {
        for (size_t i=0; i<nLength; ++i)
            pBuffer[i] = 0.0f;
    }

    bool FloatBuffer::validate() const
    {
        uint32_t key            = uint32_t(ptrdiff_t(this));
        uint8_t *head           = reinterpret_cast<uint8_t *>(pBuffer);
        uint32_t *ptr           = reinterpret_cast<uint32_t *>(&head[-sizeof(uint32_t)]);
        if (*ptr != (CK_HEAD_SIGNATURE ^ key))
            return false;
        ptr                     = reinterpret_cast<uint32_t *>(&head[nLength * sizeof(float)]);
        return (*ptr == (CK_TAIL_SIGNATURE ^ key));
    }

    bool FloatBuffer::equals_relative(const FloatBuffer &src, float tolerance) const
    {
        if (src.nLength != nLength)
            return false;
        if (!(validate() && src.validate()))
            return false;
        const float *a = pBuffer, *b = src.pBuffer;
        for (size_t i=0; i<nLength; ++i)
        {
            if (!float_equals_relative(a[i], b[i], tolerance))
                return false;
        }
        return true;
    }

    bool FloatBuffer::equals_absolute(const FloatBuffer &src, float tolerance) const
    {
        if (src.nLength != nLength)
            return false;
        if (!(validate() && src.validate()))
            return false;
        const float *a = pBuffer, *b = src.pBuffer;
        for (size_t i=0; i<nLength; ++i)
        {
            if (!float_equals_absolute(a[i], b[i], tolerance))
                return false;
        }
        return true;
    }

    void FloatBuffer::dump(const char *text) const
    {
        printf("%s: ", text);
        for (size_t i=0; i<nLength; ++i)
            printf("%.5f ", pBuffer[i]);
        printf("\n");
    }

    void FloatBuffer::dump(const char *text, size_t from, size_t count) const
    {
        printf("%s: ", text);
        for (size_t i=from; (i<nLength) && (count > 0); ++i, --count)
            printf("%.5f ", pBuffer[i]);
        printf("\n");
    }
}
