/*
 * types.cpp
 *
 *  Created on: 02 февр. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/alloc.h>
#include <dsp/dsp.h>

namespace lsp
{
    float_buffer_t  *float_buffer_t::create(size_t lines, size_t items)
    {
        size_t b_size       = ALIGN_SIZE(sizeof(float_buffer_t) + lines * sizeof(float *), ALIGN64);
        size_t v_size       = ALIGN_SIZE(sizeof(float) * items, ALIGN64);
        size_t alloc        = b_size + v_size*lines + ALIGN64; // Additional 64-byte alignment for buffes
        uint8_t *ptr        = lsp_tmalloc(uint8_t, alloc);
        if (ptr == NULL)
            return NULL;
        float_buffer_t *r   = reinterpret_cast<float_buffer_t *>(ptr);
        ptr                 = ALIGN_PTR(ptr + b_size, ALIGN64);

        for (size_t i=0; i<lines; ++i)
        {
            r->v[i]             = reinterpret_cast<float *>(ptr);
            ptr                += v_size;
        }

        r->lines            = lines;
        r->items            = items;
        return r;
    }

    float_buffer_t *float_buffer_t::resize(size_t l, size_t i)
    {
        if ((lines == l) && (items == i))
            return this;

        lsp_free(this);
        return create(l, i);
    }

    float_buffer_t *float_buffer_t::reuse(float_buffer_t *buf, size_t lines, size_t items)
    {
        return (buf != NULL) ? buf->resize(lines, items) : create(lines, items);
    }

    void float_buffer_t::detroy()
    {
        lsp_free(this);
    }

}
