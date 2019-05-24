/*
 * port_data.cpp
 *
 *  Created on: 24 мая 2019 г.
 *      Author: sadko
 */

#include <core/port_data.h>
#include <dsp/dsp.h>
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

    // frame_buffer_t methods
    void frame_buffer_t::clear()
    {
        dsp::fill_zero(vData, nCapacity * nCols);
        nRowID         += nRows;
    }

    void frame_buffer_t::seek(uint32_t row_id)
    {
        nRowID          = row_id;
    }

    void frame_buffer_t::read_row(float *dst, size_t row_id) const
    {
        uint32_t off    = row_id & (nCapacity - 1);
        dsp::copy(dst, &vData[off * nCols], nCols);
    }

    float *frame_buffer_t::get_row(size_t row_id) const
    {
        uint32_t off    = row_id & (nCapacity - 1);
        return &vData[off * nCols];
    }

    float *frame_buffer_t::next_row() const
    {
        uint32_t off    = nRowID & (nCapacity - 1);
        return &vData[off * nCols];
    }

    void frame_buffer_t::write_row(const float *row)
    {
        uint32_t off    = nRowID & (nCapacity - 1);
        dsp::copy(&vData[off * nCols], row, nCols);
        nRowID          ++; // Increment row identifier after bulk write
    }

    void frame_buffer_t::write_row(uint32_t row_id, const float *row)
    {
        uint32_t off    = row_id & (nCapacity - 1);
        dsp::copy(&vData[off * nCols], row, nCols);
    }

    void frame_buffer_t::write_row()
    {
        nRowID          ++; // Just increment row identifier
    }

    bool frame_buffer_t::sync(const frame_buffer_t *fb)
    {
        // Check if there is data for viewing
        if (fb == NULL)
            return false;

        // Estimate what to do
        uint32_t src_rid = fb->next_rowid(), dst_rid = nRowID;
        uint32_t delta = src_rid - dst_rid;
        if (delta == 0)
            return false; // No changes
        else if (delta > nRows)
            dst_rid = src_rid - nRows;

        // Synchronize buffer data
        while (dst_rid != src_rid)
        {
            const float *row = fb->get_row(dst_rid);
            size_t off      = (dst_rid) & (nCapacity - 1);
            dsp::copy(&vData[off * nCols], row, nCols);
            dst_rid++;
        }

        nRowID      = dst_rid;
        return true;
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
        fb->nRowID          = rows;
        fb->vData           = reinterpret_cast<float *>(ptr);
        fb->pData           = data;

        dsp::fill_zero(fb->vData, rows * cols);
        return fb;
    }

    status_t frame_buffer_t::init(size_t rows, size_t cols)
    {
        // Estimate capacity
        size_t cap          = rows * 4;
        size_t hcap         = 1;
        while (hcap < cap)
            hcap                <<= 1;

        // Estimate amount of data to allocate
        size_t b_size       = hcap * cols * sizeof(float);

        // Allocate memory
        uint8_t *ptr = NULL, *data = NULL;
        ptr     = alloc_aligned<uint8_t>(data, b_size);
        if (ptr == NULL)
            return STATUS_NO_MEM;

        // Create object
        nRows               = rows;
        nCols               = cols;
        nCapacity           = hcap;
        nRowID              = rows;
        vData               = reinterpret_cast<float *>(ptr);
        pData               = data;

        dsp::fill_zero(vData, rows * cols);
        return STATUS_OK;
    }

    void frame_buffer_t::destroy()
    {
        void *ptr = pData;
        vData = NULL;
        pData = NULL;
        free_aligned(ptr);
    }

    void frame_buffer_t::destroy(frame_buffer_t *buf)
    {
        void *ptr = buf->pData;
        buf->vData = NULL;
        buf->pData = NULL;
        free_aligned(ptr);
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



