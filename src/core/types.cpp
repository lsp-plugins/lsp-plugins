/*
 * types.cpp
 *
 *  Created on: 02 февр. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/alloc.h>

namespace lsp
{
    path_t::~path_t()
    {
    }

    void path_t::init()
    {
    }

    const char *path_t::get_path()
    {
        return "";
    }

    void path_t::accept()
    {
    }

    void path_t::commit()
    {
    }

    bool path_t::pending()
    {
        return false;
    }

    bool path_t::accepted()
    {
        return false;
    }

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

    void position_t::init(position_t *pos)
    {
        pos->sampleRate     = DEFAULT_SAMPLE_RATE;
        pos->speed          = 1.0;
        pos->frame          = 0;
        pos->numerator      = 4.0;
        pos->denominator    = 4.0;
        pos->beatsPerMinute = BPM_DEFAULT;
        pos->tick           = 0;
        pos->ticksPerBeat   = DEFAULT_TICKS_PER_BEAT;
    }
}
