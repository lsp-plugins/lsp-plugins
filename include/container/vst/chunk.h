/*
 * chunk.h
 *
 *  Created on: 22 июн. 2019 г.
 *      Author: sadko
 */

#ifndef CONTAINER_VST_CHUNK_H_
#define CONTAINER_VST_CHUNK_H_

#include <core/types.h>
#include <core/status.h>
#include <stdint.h>

namespace lsp
{
    typedef struct vst_chunk_t
    {
        uint8_t    *data;
        size_t      offset;
        size_t      capacity;
        status_t    res;

        explicit vst_chunk_t()
        {
            data        = NULL;
            offset      = 0;
            capacity    = 0;
            res         = STATUS_OK;
        }

        ~vst_chunk_t()
        {
            if (data != NULL)
            {
                ::free(data);
                data    = NULL;
            }
            offset      = 0;
            capacity    = 0;
        }

        void clear()
        {
            offset      = 0;
            res         = STATUS_OK;
        }

        bool ensure_capacity(size_t count)
        {
            size_t cap      = offset + count;
            if (cap <= capacity)
                return true;

            cap            += (cap >> 1);
            uint8_t *ptr    = reinterpret_cast<uint8_t *>(::realloc(data, cap));
            if (ptr == NULL)
            {
                res             = STATUS_NO_MEM;
                return false;
            }

            data            = ptr;
            capacity        = cap;
            return true;
        }

        size_t write(const void *bytes, size_t count)
        {
            if (res != STATUS_OK)
                return 0;

            if (!ensure_capacity(count))
                return 0;

            size_t off      = offset;
            ::memcpy(&data[offset], bytes, count);
            offset         += count;
            return off;
        }

        template <class T>
            size_t write(T value)
            {
                if (res != STATUS_OK)
                    return 0;

                if (!ensure_capacity(sizeof(value)))
                    return 0;

                size_t off      = offset;
                *(reinterpret_cast<T *>(&data[off])) = CPU_TO_BE(value);
                offset         += sizeof(value);
                return off;
            }

        template <class T>
            bool write_at(size_t position, T value)
            {
                if (res != STATUS_OK)
                    return false;

                if ((offset - position) < sizeof(T))
                {
                    res = STATUS_OVERFLOW;
                    return false;
                }

                *(reinterpret_cast<T *>(&data[position])) = CPU_TO_BE(value);
                return true;
            }

        inline size_t write_string(const char *str)
        {
            if (res != STATUS_OK)
                return 0;

            size_t slen     = ::strlen(str)+1;
            if (!ensure_capacity(slen))
                return 0;

            size_t off      = offset;
            ::memcpy(&data[offset], str, slen);
            offset         += slen;
            return off;
        }

        inline size_t write_byte(int b)
        {
            if (!ensure_capacity(sizeof(uint8_t)))
                return 0;

            size_t off      = offset;
            data[offset++]  = uint8_t(b);
            return off;
        }

        template <class T>
            inline T *fetch(size_t offset)
            {
                return reinterpret_cast<T *>(&data[offset]);
            }
    } vst_chunk_t;

}

#endif /* CONTAINER_VST_CHUNK_H_ */
