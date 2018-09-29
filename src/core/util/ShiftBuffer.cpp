/*
 * ShiftBuffer.cpp
 *
 *  Created on: 29 янв. 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/types.h>
#include <core/debug.h>
#include <core/util/ShiftBuffer.h>

namespace lsp
{
    ShiftBuffer::ShiftBuffer()
    {
        pData       = NULL;
        nCapacity   = 0;
        nHead       = 0;
        nTail       = 0;
    }

    ShiftBuffer::~ShiftBuffer()
    {
        destroy();
    }

    bool ShiftBuffer::init(size_t size, size_t gap)
    {
        // Check gap
        if (gap > size)
            return false;

        // Make size multiple of 0x10
        size_t new_capacity     = ALIGN_SIZE(size, 0x10);
        if ((pData == NULL) || (new_capacity != nCapacity))
        {
            // Allocate new buffer
            float *new_data     = new float[new_capacity];
            if (new_data == NULL)
                return false;

            // Delete old buffer
            if (pData != NULL)
                delete [] pData;
            pData       = new_data;
        }
        nCapacity   = new_capacity;
        nHead       = 0;
        nTail       = gap;

//        lsp_trace("capacity = %d, head = %d, tail = %d", int(nCapacity), int(nHead), int(nTail));

        // Zero the gap
        dsp::fill_zero(pData, gap);
        return true;
    }

    bool ShiftBuffer::resize(size_t size, size_t gap)
    {
        // Check that need simply allocate new buffer
        if (pData == NULL)
            return init(size, gap);

        // Check gap
        if (gap > size)
            return false;

        // Make size multiple of 0x10
        size_t new_capacity = ALIGN_SIZE(size, 0x10);
        size_t avail        = nTail - nHead;            // Current gap size
        ssize_t fill        = gap - avail;              // Number of additional gap elements

        if (new_capacity != nCapacity)
        {
            // Allocate new buffer
            float *dst     = new float[new_capacity];
            if (dst == NULL)
                return false;

            // Copy data
            if (fill > 0)
            {
                dsp::fill_zero(dst, fill);
                dsp::copy(&dst[fill], &pData[nHead], avail);
            }
            else
                dsp::copy(dst, &pData[nTail + fill], gap);
            delete [] pData;

            // Update pointers
            pData       = dst;
            nCapacity   = new_capacity;
            nHead       = 0;
            nTail       = gap;
        }
        else
        {
            // Process data if gap size changed
            if (fill > 0)
            {
                // Check that there is possibility simply to zero data before the head
                ssize_t reserve     = fill - nHead;
                if (reserve > 0)
                {
                    // We need to free some space before head
                    dsp::move(&pData[nHead + reserve], &pData[nHead], avail);
                    nTail   += reserve;
                    nHead   += reserve;
                }

                nHead      -= fill;
                dsp::fill_zero(&pData[nHead], fill);
            }
            else if (fill < 0) // Simply forget data at the head
                nHead -= fill;
        }

        return true;
    }

    void ShiftBuffer::destroy()
    {
        if (pData != NULL)
        {
            delete [] pData;
            pData       = NULL;
        }
        nCapacity   = 0;
        nHead       = 0;
        nTail       = 0;
    }

    size_t ShiftBuffer::append(const float *data, size_t count)
    {
        // Check state
        if (pData == NULL)
            return 0;

        // Check free space in buffer
        size_t can_append       = nCapacity - nTail;
        if (can_append <= 0)
        {
            if (nHead <= 0)
                return 0;
            dsp::move(pData, &pData[nHead], nTail - nHead);
            can_append  = nHead;
            nTail      -= nHead;
            nHead       = 0;
        }
        else if ((can_append < count) && (nHead > 0))
        {
            dsp::move(pData, &pData[nHead], nTail - nHead);
            can_append += nHead;
            nTail      -= nHead;
            nHead       = 0;
        }

        // Determine the amount of samples to copy
        if (count > can_append)
            count               = can_append;

        // Fill the buffer
        if (data != NULL)
            dsp::copy(&pData[nTail], data, count);
        else
            dsp::fill_zero(&pData[nTail], count);
        nTail      += count;

        return count;
    }

    size_t ShiftBuffer::append(float data)
    {
        // Check state
        if (pData == NULL)
            return 0;

        // Check free space in buffer
        if (nTail >= nCapacity)
        {
            if (nHead <= 0)
                return 0;
            dsp::move(pData, &pData[nHead], nTail - nHead);
            nTail  -= nHead;
            nHead   = 0;
        }

        // Append sample
        pData[nTail++]  = data;
        return 1;
    }

    size_t ShiftBuffer::shift(float *data, size_t count)
    {
        // Check state
        if (pData == NULL)
            return 0;

        // Determine the amount of samples to copy
        size_t can_shift    = nTail - nHead;
        if (count > can_shift)
            count   = can_shift;

        // Flush the buffer
        if (data != NULL)
            dsp::copy(data, &pData[nHead], count);
        nHead      += count;

//        lsp_trace("count=%d, capacity=%d, head=%d, tail=%d", int(count), int(nCapacity), int(nHead), int(nTail));

        return count;
    }

    size_t ShiftBuffer::shift(size_t count)
    {
        // Check state
        if (pData == NULL)
            return 0;

        // Determine the amount of samples to copy
        size_t can_shift    = nTail - nHead;
        if (count > can_shift)
            count   = can_shift;

        // Flush the buffer
        nHead      += count;
//        lsp_trace("count=%d, capacity=%d, head=%d, tail=%d", int(count), int(nCapacity), int(nHead), int(nTail));

        return count;
    }

    float ShiftBuffer::shift()
    {
        // Check state
        if ((pData == NULL) || (nTail <= nHead))
            return 0.0f;
        return pData[nHead++];
    }

    void ShiftBuffer::copy(const ShiftBuffer *src)
    {
        size_t amount   = nTail - nHead;
        size_t can      = src->nTail - src->nHead;
        if (amount > can)
            amount = can;
    }

} /* namespace lsp */
