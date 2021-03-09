/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 24 мая 2019 г.
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

#include <core/port_data.h>
#include <dsp/dsp.h>
#include <dsp/atomic.h>
#include <dsp/endian.h>
#include <core/alloc.h>
#include <core/status.h>
#include <core/sugar.h>

#define STREAM_MESH_ALIGN   0x40

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

    size_t path_t::get_flags()
    {
        return 0;
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

    //-------------------------------------------------------------------------
    // mesh_t methods
    mesh_t *mesh_t::create(size_t buffers, size_t length)
    {
        size_t buf_size     = length * sizeof(float);
        size_t mesh_size    = sizeof(mesh_t) + sizeof(float *) * buffers;

        // Align values to 64-byte boundaries
        buf_size            = ALIGN_SIZE(buf_size, 0x40);
        mesh_size           = ALIGN_SIZE(mesh_size, 0x40);

        // Allocate pointer
        uint8_t *ptr        = reinterpret_cast<uint8_t *>(lsp_malloc(mesh_size + buf_size * buffers));
        if (ptr == NULL)
            return NULL;

        // Initialize references
        mesh_t *mesh        = reinterpret_cast<mesh_t *>(ptr);
        mesh->nState        = M_EMPTY;
        mesh->nBuffers      = 0;
        mesh->nItems        = 0;
        ptr                += mesh_size;
        for (size_t i=0; i<buffers; ++i)
        {
            mesh->pvData[i]    = reinterpret_cast<float *>(ptr);
            ptr                += buf_size;
        }

        return mesh;
    }

    void mesh_t::destroy(mesh_t *mesh)
    {
        if (mesh == NULL)
            return;

        lsp_free(mesh);
        mesh = NULL;
    }

    //-------------------------------------------------------------------------
    // stream_mesh_t methods
    stream_t *stream_t::create(size_t channels, size_t frames, size_t capacity)
    {
        // Estimate the number of power-of-two frames
        size_t nframes  = frames * 8;
        size_t fcap         = 1;
        while (fcap < nframes)
            fcap                <<= 1;

        size_t bcap     = ((capacity*2 + STREAM_MAX_FRAME_SIZE - 1) / STREAM_MAX_FRAME_SIZE) * STREAM_MAX_FRAME_SIZE;
        size_t sz_of    = ALIGN_SIZE(sizeof(stream_t), STREAM_MESH_ALIGN);
        size_t sz_chan  = ALIGN_SIZE(sizeof(float *)*channels, STREAM_MESH_ALIGN);
        size_t sz_frm   = ALIGN_SIZE(sizeof(frame_t)*fcap, STREAM_MESH_ALIGN);
        size_t sz_buf   = ALIGN_SIZE(bcap * sizeof(float), STREAM_MESH_ALIGN);
        size_t to_alloc = sz_of + sz_frm + sz_chan + sz_buf * channels;

        uint8_t *pdata  = NULL;
        uint8_t *ptr    = alloc_aligned<uint8_t>(pdata, to_alloc, STREAM_MESH_ALIGN);
        if (ptr == NULL)
            return NULL;

        // Allocate and initialize space
        stream_t *mesh          = reinterpret_cast<stream_t *>(ptr);
        ptr                    += sz_of;

        mesh->nFrames           = frames;
        mesh->nChannels         = channels;
        mesh->nBufMax           = capacity;
        mesh->nBufCap           = bcap;
        mesh->nFrameCap         = fcap;

        mesh->nFrameId          = 0;

        mesh->vFrames           = reinterpret_cast<frame_t *>(ptr);
        ptr                    += sz_frm;

        for (size_t i=0; i<fcap; ++i)
        {
            frame_t *f              = &mesh->vFrames[i];
            f->id                   = 0;
            f->head                 = 0;
            f->tail                 = 0;
            f->length               = 0;
        }

        mesh->vChannels         = reinterpret_cast<float **>(ptr);
        ptr                    += sz_chan;

        float *buf              = reinterpret_cast<float *>(ptr);
        dsp::fill_zero(buf, bcap * channels);
        for (size_t i=0; i<channels; ++i, buf += bcap)
            mesh->vChannels[i] = buf;

        mesh->pData             = pdata;

        return mesh;
    }

    void stream_t::clear(uint32_t current)
    {
        frame_t *f;
        for (size_t i=0; i<nFrameCap; ++i)
        {
            f                       = &vFrames[i];
            f->id                   = 0;
            f->head                 = 0;
            f->tail                 = 0;
            f->length               = 0;
        }

        nFrameId                = current;
    }

    void stream_t::clear()
    {
        clear(nFrameId + 1);
    }

    void stream_t::destroy(stream_t *buf)
    {
        if (buf == NULL)
            return;
        uint8_t *data   = buf->pData;
        if (data == NULL)
            return;

        buf->vChannels      = NULL;
        buf->pData          = NULL;
        free_aligned(data);
    }

    ssize_t stream_t::get_tail(uint32_t frame) const
    {
        const frame_t *f = &vFrames[frame & (nFrameCap - 1)];
        size_t tail = f->tail;
        return (f->id == frame) ? tail : -STATUS_NOT_FOUND;
    }

    ssize_t stream_t::get_head(uint32_t frame) const
    {
        const frame_t *f = &vFrames[frame & (nFrameCap - 1)];
        size_t head = f->head;
        return (f->id == frame) ? head : -STATUS_NOT_FOUND;
    }

    ssize_t stream_t::get_size(uint32_t frame) const
    {
        const frame_t *f = &vFrames[frame & (nFrameCap - 1)];
        ssize_t size = f->tail - f->head;
        if (size < 0)
            size       += nBufCap;
        return (f->id == frame) ? size : -STATUS_NOT_FOUND;
    }

    ssize_t stream_t::get_position(uint32_t frame) const
    {
        const frame_t *f = &vFrames[frame & (nFrameCap - 1)];
        ssize_t pos = f->tail - f->length;
        if (pos < 0)
            pos        += nBufCap;
        return (f->id == frame) ? pos : -STATUS_NOT_FOUND;
    }

    ssize_t stream_t::get_length(uint32_t frame) const
    {
        const frame_t *f = &vFrames[frame & (nFrameCap - 1)];
        size_t size = f->length;
        return (f->id == frame) ? size : -STATUS_NOT_FOUND;
    }

    size_t stream_t::add_frame(size_t size)
    {
        size_t frame_id = nFrameId + 1;
        frame_t *curr   = &vFrames[nFrameId & (nFrameCap - 1)];
        frame_t *next   = &vFrames[frame_id & (nFrameCap - 1)];

        size            = lsp_min(size, size_t(STREAM_MAX_FRAME_SIZE));

        // Write data for new frame
        next->id        = frame_id;
        next->head      = curr->tail;
        next->tail      = next->head + size;
        next->length    = size;

        // Clear data for all buffers
        if (next->tail < nBufCap)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                float *dst = vChannels[i];
                dsp::fill_zero(&dst[next->head], size);
            }
        }
        else
        {
            next->tail     -= nBufCap;

            for (size_t i=0; i<nChannels; ++i)
            {
                float *dst = vChannels[i];
                dsp::fill_zero(&dst[next->head], nBufCap - next->head);
                dsp::fill_zero(dst, next->tail);
            }
        }

        return size;
    }

    ssize_t stream_t::write_frame(size_t channel, const float *data, size_t off, size_t count)
    {
        if (channel >= nChannels)
            return -STATUS_INVALID_VALUE;

        size_t frame_id = nFrameId + 1;
        frame_t *next   = &vFrames[frame_id & (nFrameCap - 1)];
        if (next->id != frame_id)
            return -STATUS_BAD_STATE;

        // Estimate number of items to copy
        size_t last     = lsp_min(off + count, next->length);
        if (last > next->length)
            return 0;

        // Copy data
        float *dst      = vChannels[channel];
        count           = last - off;
        last            = next->head + count;
        off            += next->head;
        if (last > nBufCap)
        {
            dsp::copy(&dst[off], data, nBufCap - off);
            dsp::copy(dst, &data[nBufCap - off], last - nBufCap);
        }
        else
            dsp::copy(&dst[off], data, count);

        return count;
    }

    ssize_t stream_t::read(size_t channel, float *data, size_t off, size_t count)
    {
        if (channel >= nChannels)
            return -STATUS_INVALID_VALUE;

        // Check that we're reading proper frame
        size_t frame_id     = nFrameId;
        frame_t *frm        = &vFrames[frame_id & (nFrameCap - 1)];
        if (frm->id != frame_id)
            return -STATUS_BAD_STATE;

        // Estimate the offset and number of items to read
        if (off >= frm->length)
            return -STATUS_EOF;
        count               = lsp_min(count, frm->length - off);

        // Determine position of head
        ssize_t head        = frm->tail - frm->length + off;
        if (head < 0)
            head               += nBufCap;

        size_t tail         = head + count;
        const float *s      = vChannels[channel];
        if (tail > nBufCap)
        {
            dsp::copy(data, &s[head], nBufCap - head);
            dsp::copy(&data[nBufCap - head], s, tail - nBufCap);
        }
        else
            dsp::copy(data, &s[head], count);

        return count;
    }

    bool stream_t::commit_frame()
    {
        size_t frame_id = nFrameId + 1;
        frame_t *curr   = &vFrames[nFrameId & (nFrameCap - 1)];
        frame_t *next   = &vFrames[frame_id & (nFrameCap - 1)];
        if (next->id != frame_id)
            return false;

        // Commit new frame size and update frame identifier
        next->length    = lsp_min(curr->length + next->length, nBufMax);
        nFrameId        = frame_id;

        return true;
    }

    bool stream_t::sync(const stream_t *src)
    {
        // Check if there is data to sync
        if ((src == NULL) || (src->nChannels != nChannels))
            return false;

        // Estimate what to do
        uint32_t src_frm = src->nFrameId, dst_frm = nFrameId;
        uint32_t delta = src_frm - dst_frm;
        if (delta == 0)
            return false; // No changes

        if (delta > nFrames)
        {
            // Need to perform full sync
            frame_t *df         = &vFrames[src_frm & (nFrameCap - 1)];
            frame_t sf          = src->vFrames[src_frm & (src->nFrameCap - 1)];

            df->id              = src_frm;
            df->length          = lsp_min(sf.length, nBufMax);
            df->tail            = df->length;

            // Copy data from the source frame
            ssize_t head        = sf.tail - df->length;
            if (head < 0)
            {
                head += src->nBufMax;
                for (size_t i=0; i<nChannels; ++i)
                {
                    const float *s  = src->vChannels[i];
                    float *d        = vChannels[i];

                    dsp::copy(d, &s[head], src->nBufMax - head);
                    dsp::copy(&d[src->nBufMax - head], s, sf.tail);
                }
            }
            else
            {
                for (size_t i=0; i<nChannels; ++i)
                {
                    const float *s  = src->vChannels[i];
                    float *d        = vChannels[i];
                    dsp::copy(d, &s[head], df->length);
                }
            }

            // Compute destination frame size and compute the head value of the frame
            ssize_t df_sz       = sf.tail - sf.head;
            if (df_sz < 0)
                df_sz              += src->nBufMax;
            df_sz               = lsp_min(df_sz, ssize_t(df->length));
            df_sz               = lsp_min(df_sz, ssize_t(STREAM_MAX_FRAME_SIZE));
            df->head            = df->tail - df_sz;
        }
        else
        {
            uint32_t last_frm = src_frm + 1;

            // Need to perform incremental sync
            while (dst_frm != last_frm)
            {
                // Determine the frames to sync
                frame_t *pf         = &vFrames[(dst_frm - 1) & (nFrameCap - 1)];
                frame_t *df         = &vFrames[dst_frm & (nFrameCap - 1)];
                frame_t sf          = src->vFrames[dst_frm & (src->nFrameCap - 1)];

                ssize_t fsize       = sf.tail - sf.head;
                if (fsize < 0)
                    fsize              += src->nBufCap;

                df->id          = dst_frm;
                df->head        = pf->tail;
                df->tail        = df->head;
                df->length      = fsize;

                // Copy frame data
                for (ssize_t n=0; n<fsize; )
                {
                    // Estimate the amount of samples to copy
                    size_t ns   = (sf.tail >= sf.head) ? sf.tail - sf.head : src->nBufCap - sf.head;
                    size_t nd   = nBufCap - df->tail;
                    size_t count= lsp_min(ns, nd);

                    // Synchronously copy samples for each channel
                    for (size_t i=0; i<nChannels; ++i)
                    {
                        const float *s  = src->vChannels[i];
                        float *d        = vChannels[i];
                        dsp::copy(&d[df->tail], &s[sf.head], count);
                    }

                    // Update positions
                    sf.head        += count;
                    df->tail       += count;
                    n              += count;

                    // Fixup positions
                    if (sf.head >= src->nBufCap)
                        sf.head        -= src->nBufCap;
                    if (df->tail >= nBufCap)
                        df->tail       -= nBufCap;
                }

                // Update frame size and increment frame number
                df->length      = lsp_min(df->length + pf->length, nBufMax);
                ++dst_frm;
            }
        }

        // Update current frame
        nFrameId    = src_frm;

        return true;
    }

    //-------------------------------------------------------------------------
    // frame_buffer_t methods
    void frame_buffer_t::clear()
    {
        dsp::fill_zero(vData, nCapacity * nCols);
        atomic_add(&nRowID, nRows);
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
        atomic_add(&nRowID, 1); // Increment row identifier after bulk write
    }

    void frame_buffer_t::write_row(uint32_t row_id, const float *row)
    {
        uint32_t off    = row_id & (nCapacity - 1);
        dsp::copy(&vData[off * nCols], row, nCols);
    }

    void frame_buffer_t::write_row()
    {
        atomic_add(&nRowID, 1); // Just increment row identifier
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

    //-------------------------------------------------------------------------
    // position_t methods
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

    //-------------------------------------------------------------------------
    // osc_buffer_t methods
    osc_buffer_t *osc_buffer_t::create(size_t capacity)
    {
        if (capacity % sizeof(uint32_t))
            return NULL;

        uint8_t *tmp        = reinterpret_cast<uint8_t *>(malloc(0x1000));
        if (tmp == NULL)
            return NULL;

        size_t to_alloc     = sizeof(osc_buffer_t) + capacity + DEFAULT_ALIGN;
        void *data          = NULL;
        uint8_t *ptr        = alloc_aligned<uint8_t>(data, to_alloc, DEFAULT_ALIGN);
        if (ptr == NULL)
        {
            free(tmp);
            return NULL;
        }

        osc_buffer_t *res   = reinterpret_cast<osc_buffer_t *>(ptr);
        ptr                += ALIGN_SIZE(sizeof(osc_buffer_t), DEFAULT_ALIGN);

        res->nSize          = 0;
        res->nCapacity      = capacity;
        res->nHead          = 0;
        res->nTail          = 0;
        res->pBuffer        = ptr;
        res->pTempBuf       = tmp;
        res->nTempSize      = 0x1000;
        res->pData          = data;

        return res;
    }

    void osc_buffer_t::destroy(osc_buffer_t *buf)
    {
        if (buf->pTempBuf != NULL)
        {
            free(buf->pTempBuf);
            buf->pTempBuf   = NULL;
        }
        if ((buf != NULL) && (buf->pData != NULL))
            free_aligned(buf->pData);
    }

    status_t osc_buffer_t::submit(const void *data, size_t size)
    {
        if ((!size) || (size % sizeof(uint32_t)))
            return STATUS_BAD_ARGUMENTS;

        // Ensure that there is enough space in buffer
        size_t oldsize  = nSize;
        size_t newsize  = oldsize + size + sizeof(uint32_t);
        if (newsize > nCapacity)
            return (oldsize == 0) ? STATUS_TOO_BIG : STATUS_OVERFLOW;

        // Store packet size to the buffer and move the tail
        *(reinterpret_cast<uint32_t *>(&pBuffer[nTail])) = CPU_TO_BE(uint32_t(size));
        nTail          += sizeof(uint32_t);
        if (nTail > nCapacity)
            nTail          -= nCapacity;

        // Store packet data and move the tail
        size_t head     = nCapacity - nTail;
        if (size > head)
        {
            const uint8_t *src  = reinterpret_cast<const uint8_t *>(data);
            ::memcpy(&pBuffer[nTail], src, head);
            ::memcpy(pBuffer, &src[head], size - head);
        }
        else
            ::memcpy(&pBuffer[nTail], data, size);

        nTail          += size;
        if (nTail > nCapacity)
            nTail          -= nCapacity;

        // Update the size
        nSize           = newsize;
        return STATUS_OK;
    }

    status_t osc_buffer_t::reserve(size_t size)
    {
        if (nTempSize >= size)
            return STATUS_OK;
        else if (size > nCapacity)
            return STATUS_OVERFLOW;

        uint8_t *tmp    = reinterpret_cast<uint8_t *>(realloc(pTempBuf, size));
        if (tmp == NULL)
            return STATUS_NO_MEM;

        pTempBuf        = tmp;
        nTempSize       = size;

        return STATUS_OK;
    }

    status_t osc_buffer_t::submit(const osc::packet_t *packet)
    {
        return (packet != NULL) ? submit(packet->data, packet->size) : STATUS_BAD_ARGUMENTS;
    }

    void osc_buffer_t::clear()
    {
        nSize   = 0;
        nHead   = 0;
        nTail   = 0;
    }

#define SUBMIT_SIMPLE_IMPL(address, func, ...) \
        osc::packet_t packet; \
        osc::forge_t forge; \
        osc::forge_frame_t sframe, message; \
        \
        status_t res = osc::forge_begin_fixed(&sframe, &forge, pTempBuf, nTempSize); \
        status_t res2; \
        if (res == STATUS_OK) {\
            res     = osc::forge_begin_message(&message, &sframe, address); \
            if (res == STATUS_OK) \
                res = osc::func(&message, ## __VA_ARGS__); \
            osc::forge_end(&message); \
        } \
        res2 = osc::forge_end(&sframe); \
        if (res == STATUS_OK) res = res2; \
        res2   = osc::forge_close(&packet, &forge); \
        if (res == STATUS_OK) res = res2; \
        res2   = osc::forge_destroy(&forge); \
        if (res == STATUS_OK) res = res2; \
        return (res == STATUS_OK) ? submit(&packet) : res;

    status_t osc_buffer_t::submit_int32(const char *address, int32_t value)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_int32, value);
    }

    status_t osc_buffer_t::submit_float32(const char *address, float value)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_float32, value);
    }

    status_t osc_buffer_t::submit_string(const char *address, const char *s)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_string, s);
    }

    status_t osc_buffer_t::submit_blob(const char *address, const void *data, size_t bytes)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_blob, data, bytes);
    }

    status_t osc_buffer_t::submit_int64(const char *address, int64_t value)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_int64, value);
    }

    status_t osc_buffer_t::submit_double64(const char *address, double value)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_double64, value);
    }

    status_t osc_buffer_t::submit_time_tag(const char *address, uint64_t value)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_time_tag, value);
    }

    status_t osc_buffer_t::submit_type(const char *address, const char *s)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_type, s);
    }

    status_t osc_buffer_t::submit_symbol(const char *address, const char *s)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_symbol, s);
    }

    status_t osc_buffer_t::submit_ascii(const char *address, char c)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_ascii, c);
    }

    status_t osc_buffer_t::submit_rgba(const char *address, const uint32_t rgba)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_rgba, rgba);
    }

    status_t osc_buffer_t::submit_midi(const char *address, const midi::event_t *event)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_midi, event);
    }

    status_t osc_buffer_t::submit_midi_raw(const char *address, const void *event, size_t bytes)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_midi_raw, event, bytes);
    }

    status_t osc_buffer_t::submit_bool(const char *address, bool value)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_bool, value);
    }

    status_t osc_buffer_t::submit_null(const char *address)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_null);
    }

    status_t osc_buffer_t::submit_inf(const char *address)
    {
        SUBMIT_SIMPLE_IMPL(address, forge_inf);
    }

    #undef SUBMIT_SIMPLE_IMPL

    status_t osc_buffer_t::submit_message(const char *address, const char *params...)
    {
        va_list args;
        va_start(args, params);
        status_t res = submit_messagev(address, params, args);
        va_end(args);
        return res;
    }

    status_t osc_buffer_t::submit_messagev(const char *address, const char *params, va_list args)
    {
        osc::packet_t packet;
        osc::forge_t forge;
        osc::forge_frame_t sframe;

        status_t res = osc::forge_begin_fixed(&sframe, &forge, pTempBuf, nTempSize);
        if (res == STATUS_OK)
            res     = osc::forge_message(&sframe, address, params, args);

        status_t res2   = osc::forge_end(&sframe);
        if (res == STATUS_OK)
            res = res2;

        if (res == STATUS_OK)
            res         = osc::forge_close(&packet, &forge);

        res2   = osc::forge_destroy(&forge);
        if (res == STATUS_OK)
            res = res2;

        return (res == STATUS_OK) ? submit(&packet) : res;
    }

    status_t osc_buffer_t::fetch(void *data, size_t *size, size_t limit)
    {
        if ((data == NULL) || (size == NULL) || (!limit))
            return STATUS_BAD_ARGUMENTS;

        // There is enough space in the buffer?
        size_t bufsz    = nSize;
        if (bufsz < sizeof(uint32_t))
            return STATUS_NO_DATA;

        // Read size, analyze state of the record and update head
        size_t psize    = BE_TO_CPU(*(reinterpret_cast<uint32_t *>(&pBuffer[nHead])));
        if (psize > limit) // We have enough space to store the data?
            return STATUS_OVERFLOW;
        if ((psize + sizeof(uint32_t)) > bufsz) // Record is valid?
            return STATUS_CORRUPTED;
        *size           = psize;
        nHead          += sizeof(uint32_t);
        if (nHead > nCapacity)
            nHead          -= nCapacity;

        // Copy the buffer contents
        size_t head     = nCapacity - nHead;
        if (head < psize)
        {
            uint8_t *dst    = reinterpret_cast<uint8_t *>(data);
            ::memcpy(dst, &pBuffer[nHead], head);
            ::memcpy(&dst[head], pBuffer, psize - head);
        }
        else
            ::memcpy(data, &pBuffer[nHead], psize);

        nHead          += psize;
        if (nHead > nCapacity)
            nHead          -= nCapacity;

        // Decrement size
        atomic_add(&nSize, -(psize + sizeof(uint32_t)));

        return STATUS_OK;
    }

    status_t osc_buffer_t::fetch(osc::packet_t *packet, size_t limit)
    {
        return (packet != NULL) ? fetch(packet->data, &packet->size, limit) : STATUS_BAD_ARGUMENTS;
    }

    size_t osc_buffer_t::skip()
    {
        if (nSize <= sizeof(uint32_t))
            return 0;

        size_t bufsz    = nSize;
        if (bufsz < sizeof(uint32_t))
            return STATUS_NO_DATA;

        size_t ihead    = nHead;
        uint32_t *head  = reinterpret_cast<uint32_t *>(&pBuffer[ihead]);
        size_t psize    = BE_TO_CPU(*head);

        if ((psize + sizeof(uint32_t)) > bufsz) // Record is valid?
            return 0;

        // Decrement the size and update the head
        nHead           = (ihead + psize + sizeof(uint32_t)) % nCapacity;
        atomic_add(&nSize, -(psize + sizeof(uint32_t)));

        return psize;
    }
}



