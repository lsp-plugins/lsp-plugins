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

    // frame_buffer_t methods
    void frame_buffer_t::clear()
    {
        dsp::fill_zero(vData, nCapacity * nCols);
        nRowId         += nRows;
    }

    void frame_buffer_t::seek(size_t row_id)
    {
        nRowId          = row_id;
    }

    void frame_buffer_t::read_row(float *dst, size_t row_id)
    {
        size_t off      = (nRowId - row_id) & (nCapacity - 1);
        dsp::copy(dst, &vData[off * nCols], nCols);
    }

    float *frame_buffer_t::get_row(size_t row_id)
    {
        size_t off      = (nRowId - row_id) & (nCapacity - 1);
        return &vData[off * nCols];
    }

    void frame_buffer_t::write_row(const float *row)
    {
        size_t off      = (nRowId++) & (nCapacity - 1);
        dsp::copy(&vData[off * nCols], row, nCols);
    }

    frame_buffer_t  *frame_buffer_t::create(size_t rows, size_t cols)
    {
        // Estimate capacity
        size_t cap          = rows * 4;
        size_t hcap         = 1;
        while (hcap < cap)
            hcap                <<= 1;

        // Estimate amount of data to allocate
        size_t h_size       = ALIGN_SIZE(sizeof(frame_buffer_t), ALIGN64);
        size_t b_size       = hcap * cols * sizeof(float);

        // Allocate memory
        uint8_t *ptr = NULL, *data = NULL;
        ptr     = alloc_aligned<uint8_t>(data, h_size + b_size);
        if (ptr == NULL)
            return NULL;

        // Create object
        frame_buffer_t *fb  = reinterpret_cast<frame_buffer_t *>(ptr);
        ptr                += h_size;

        fb->nRows           = rows;
        fb->nCols           = cols;
        fb->nCapacity       = hcap;
        fb->nRowId          = rows;
        fb->vData           = reinterpret_cast<float *>(ptr);
        fb->pData           = data;

        dsp::fill_zero(fb->vData, rows * cols);
        return fb;
    }

    void frame_buffer_t::destroy(frame_buffer_t *buf)
    {
        free_aligned(buf->pData);
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
