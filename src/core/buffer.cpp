/*
 * buffer.cpp
 *
 *  Created on: 12 апр. 2016 г.
 *      Author: sadko
 */
#include <core/buffer.h>
#include <core/alloc.h>

namespace lsp
{
    bool init_buf(buffer_t *buf)
    {
        buf->nCapacity      = 0;
        buf->nLength        = 0;
        buf->pString        = NULL;
        return true;
    }

    bool init_buf(buffer_t *buf, size_t reserve)
    {
        reserve             = ALIGN_SIZE((reserve + 1) * sizeof(char), 32);
        buf->pString        = reinterpret_cast<char *>(lsp_malloc(reserve));
        if (buf->pString == NULL)
            return false;
        buf->nCapacity      = reserve;
        buf->nLength        = 0;
        buf->pString[0]     = '\0';
        return true;
    }

    bool append_buf(buffer_t *buf, const char *str, size_t count)
    {
        if (count <= 0)
            return true;

        // Validate capacity
        size_t new_cap      = buf->nLength + count + 1;
        if (new_cap > buf->nCapacity)
        {
            if (!extend_buf(buf, count + 1))
                return false;
        }

        // Append buffer
        memcpy(&buf->pString[buf->nLength], str, count);
        buf->nLength   += count;
        buf->pString[buf->nLength] = '\0';

        return true;
    }

    bool extend_buf(buffer_t *buf, size_t reserve)
    {
        size_t new_cap  = buf->nCapacity + ALIGN_SIZE(reserve, 32);
        char *newstr    = reinterpret_cast<char *>(lsp_realloc(buf->pString, new_cap * sizeof(char)));
        if (newstr == NULL)
            return false;

        buf->pString    = newstr;
        buf->nCapacity  = new_cap;
        return true;
    }

    void destroy_buf(buffer_t *buf)
    {
        if (buf->pString != NULL)
        {
            lsp_free(buf->pString);
            buf->pString        = NULL;
        }
        buf->nCapacity      = 0;
        buf->nLength        = 0;
    }

    void clear_buf(buffer_t *buf)
    {
        buf->nLength        = 0;
        if (buf->pString != NULL)
            buf->pString[0]     = '\0';
    }

    char *release_buf(buffer_t *buf)
    {
        char *result        = buf->pString;
        buf->pString        = NULL;
        buf->nCapacity      = 0;
        buf->nLength        = 0;
        return result;
    }

    bool append_buf(buffer_t *buf, const char *str)
    {
        return append_buf(buf, str, strlen(str));
    }

    bool append_buf(buffer_t *buf, char ch)
    {
        return append_buf(buf, &ch, sizeof(char));
    }
}

