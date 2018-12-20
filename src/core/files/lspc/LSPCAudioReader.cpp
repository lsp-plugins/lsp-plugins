/*
 * LSPCSampleReader.cpp
 *
 *  Created on: 6 нояб. 2018 г.
 *      Author: sadko
 */

#include <dsp/endian.h>
#include <string.h>
#include <stdlib.h>
#include <core/debug.h>
#include <core/files/lspc/LSPCAudioReader.h>

#define BUFFER_SIZE     0x2000
#define BUFFER_FRAMES   0x400

namespace lsp
{
    void LSPCAudioReader::decode_u8(float *vp, const void *src, size_t ns)
    {
        const uint8_t *p = reinterpret_cast<const uint8_t *>(src);
        while (ns--)
            *(vp++) = float(*(p++) - 0x80) / 0x7f;
    }

    void LSPCAudioReader::decode_s8(float *vp, const void *src, size_t ns)
    {
        const int8_t *p = reinterpret_cast<const int8_t *>(src);
        while (ns--)
            *(vp++) = float(*(p++)) / 0x7f;
    }

    void LSPCAudioReader::decode_u16(float *vp, const void *src, size_t ns)
    {
        const uint16_t *p = reinterpret_cast<const uint16_t *>(src);
        while (ns--)
            *(vp++) = float(*(p++) - 0x8000) / 0x7fff;
    }

    void LSPCAudioReader::decode_s16(float *vp, const void *src, size_t ns)
    {
        const int16_t *p = reinterpret_cast<const int16_t *>(src);
        while (ns--)
            *(vp++) = float(*(p++)) / 0x7fff;
    }

    void LSPCAudioReader::decode_u24le(float *vp, const void *src, size_t ns)
    {
        const uint8_t *p = reinterpret_cast<const uint8_t *>(src);
        while (ns--)
        {
            int32_t v =
               __IF_LEBE(
                   p[0] | (p[1] << 8) | (p[2] << 16),
                   p[2] | (p[1] << 8) | (p[0] << 16)
               );

            *(vp++) = float(v - 0x800000) / 0x7fffff;
            p += 3;
        }
    }

    void LSPCAudioReader::decode_u24be(float *vp, const void *src, size_t ns)
    {
        const uint8_t *p = reinterpret_cast<const uint8_t *>(src);
        while (ns--)
        {
            int32_t v =
               __IF_LEBE(
                   p[2] | (p[1] << 8) | (p[0] << 16),
                   p[0] | (p[1] << 8) | (p[2] << 16)
               );

            *(vp++) = float(v - 0x800000) / 0x7fffff;
            p += 3;
        }
    }

    void LSPCAudioReader::decode_s24le(float *vp, const void *src, size_t ns)
    {
        const uint8_t *p = reinterpret_cast<const uint8_t *>(src);
        while (ns--)
        {
            int32_t v =
               __IF_LEBE(
                   p[0] | (p[1] << 8) | (p[2] << 16),
                   p[2] | (p[1] << 8) | (p[0] << 16)
               );
            v = (v << 8) >> 8; // Sign-extend value
            *(vp++) = float(v) / 0x7fffff;
            p += 3;
        }
    }

    void LSPCAudioReader::decode_s24be(float *vp, const void *src, size_t ns)
    {
        const uint8_t *p = reinterpret_cast<const uint8_t *>(src);
        while (ns--)
        {
            int32_t v =
               __IF_LEBE(
                   p[2] | (p[1] << 8) | (p[0] << 16),
                   p[0] | (p[1] << 8) | (p[2] << 16)
               );
            v = (v << 8) >> 8; // Sign-extend value
            *(vp++) = float(v) / 0x7fffff;
            p += 3;
        }
    }

    void LSPCAudioReader::decode_u32(float *vp, const void *src, size_t ns)
    {
        const int32_t *p = reinterpret_cast<const int32_t *>(src);
        while (ns--)
        {
            int32_t v = *(p++) - 0x80000000;
            *(vp++) = double(v) / 0x7fffffff;
        }
    }

    void LSPCAudioReader::decode_s32(float *vp, const void *src, size_t ns)
    {
        const int32_t *p = reinterpret_cast<const int32_t *>(src);
        while (ns--)
        {
            int32_t v = *(p++);
            *(vp++) = double(v) / 0x7fffffff;
        }
    }

    void LSPCAudioReader::decode_f32(float *vp, const void *src, size_t ns)
    {
        const float *p = reinterpret_cast<const float *>(src);
        while (ns--)
            *(vp++) = *(p++);
    }

    void LSPCAudioReader::decode_f64(float *vp, const void *src, size_t ns)
    {
        const double *p = reinterpret_cast<const double *>(src);
        while (ns--)
            *(vp++) = *(p++);
    }
    
    LSPCAudioReader::LSPCAudioReader()
    {
        sParams.channels        = 0;
        sParams.sample_format   = 0;
        sParams.sample_rate     = 0;
        sParams.codec           = 0;
        sParams.frames          = 0;

        pFD                     = NULL;
        pRD                     = NULL;
        nFlags                  = 0;
        nBPS                    = 0;
        nFrameSize              = 0;
        nBytesLeft              = 0;
        sBuf.vData              = NULL;
        sBuf.nOff               = 0;
        sBuf.nSize              = 0;
        pDecode                 = NULL;
        pFBuffer                = NULL;
    }
    
    LSPCAudioReader::~LSPCAudioReader()
    {
        close();
    }

    status_t LSPCAudioReader::close()
    {
        // Check open status first
        if (!(nFlags & F_OPENED))
            return STATUS_CLOSED;

        // Close reader (if required)
        status_t res = STATUS_OK;
        if (pRD != NULL)
        {
            status_t xr     = (nFlags & F_CLOSE_READER) ? pRD->close() : STATUS_OK;
            if (nFlags & F_DROP_READER)
                delete pRD;
            pRD             = NULL;
            if (res == STATUS_OK)
                res             = xr;
        }

        // Close LSPC file (if required)
        if ((nFlags & F_CLOSE_FILE) && (pFD != NULL))
        {
            status_t xr     = pFD->close();
            pFD             = NULL;
            if (res == STATUS_OK)
                res             = xr;
        }

        // Drop buffers
        if (sBuf.vData != NULL)
        {
            delete [] sBuf.vData;
            sBuf.vData      = NULL;
        }

        // Drop buffers
        if (pFBuffer != NULL)
        {
            delete [] pFBuffer;
            pFBuffer        = NULL;
        }

        nFlags          = 0;
        nBPS            = 0;
        nFrameSize      = 0;
        nBytesLeft      = 0;
        sBuf.nOff       = 0;
        sBuf.nSize      = 0;
        pDecode         = NULL;
        return res;
    }

    status_t LSPCAudioReader::read_audio_header(LSPCChunkReader *rd)
    {
        lspc_chunk_audio_header_t hdr;
        lspc_audio_parameters_t p;

        // Read audio header from chunk
        ssize_t res = rd->read_header(&hdr, sizeof(lspc_chunk_audio_header_t));
        if (res < 0)
            return status_t(-res);

        // Check version and decode header
        if (hdr.common.version < 1)
            return STATUS_CORRUPTED_FILE;
        if (hdr.common.size < sizeof(lspc_chunk_audio_header_t))
            return STATUS_CORRUPTED_FILE;

        p.channels          = BE_TO_CPU(hdr.channels);
        p.sample_format     = BE_TO_CPU(hdr.sample_format);
        p.sample_rate       = BE_TO_CPU(hdr.sample_rate);
        p.codec             = BE_TO_CPU(hdr.codec);
        p.frames            = BE_TO_CPU(hdr.frames);

        return apply_params(&p);
    }

    status_t LSPCAudioReader::apply_params(const lspc_audio_parameters_t *p)
    {
        if (p->channels <= 0)
            return STATUS_BAD_FORMAT;
        if (p->sample_rate == 0)
            return STATUS_BAD_FORMAT;
        if (p->codec != LSPC_CODEC_PCM)
            return STATUS_UNSUPPORTED_FORMAT;

        // Check sample format support
        size_t sb           = 0;
        decode_func_t df    = NULL;
        bool le             = false;
        bool arch_le        = __IF_LEBE(true, false);

        switch (p->sample_format)
        {
            case LSPC_SAMPLE_FMT_U8LE:
            case LSPC_SAMPLE_FMT_U8BE:
                df = decode_u8;
                sb = 1;
                le = p->sample_format == LSPC_SAMPLE_FMT_U8LE;
                break;

            case LSPC_SAMPLE_FMT_S8LE:
            case LSPC_SAMPLE_FMT_S8BE:
                df = decode_s8;
                sb = 1;
                le = p->sample_format == LSPC_SAMPLE_FMT_S8LE;
                break;

            case LSPC_SAMPLE_FMT_U16LE:
            case LSPC_SAMPLE_FMT_U16BE:
                df = decode_u16;
                sb = 2;
                le = p->sample_format == LSPC_SAMPLE_FMT_U16LE;
                break;

            case LSPC_SAMPLE_FMT_S16LE:
            case LSPC_SAMPLE_FMT_S16BE:
                df = decode_s16;
                sb = 2;
                le = p->sample_format == LSPC_SAMPLE_FMT_S16LE;
                break;

            case LSPC_SAMPLE_FMT_U24LE:
                df = decode_u24le;
                sb = 3;
                le = true;
                break;
            case LSPC_SAMPLE_FMT_U24BE:
                df = decode_u24be;
                sb = 3;
                le = false;
                break;
            case LSPC_SAMPLE_FMT_S24LE:
                df = decode_s24le;
                sb = 3;
                le = true;
                break;
            case LSPC_SAMPLE_FMT_S24BE:
                df = decode_s24be;
                sb = 3;
                le = false;
                break;

            case LSPC_SAMPLE_FMT_U32LE:
            case LSPC_SAMPLE_FMT_U32BE:
                df = decode_u32;
                sb = 4;
                le = p->sample_format == LSPC_SAMPLE_FMT_U32LE;
                break;

            case LSPC_SAMPLE_FMT_S32LE:
            case LSPC_SAMPLE_FMT_S32BE:
                df = decode_s32;
                sb = 4;
                le = p->sample_format == LSPC_SAMPLE_FMT_S32LE;
                break;

            case LSPC_SAMPLE_FMT_F32LE:
            case LSPC_SAMPLE_FMT_F32BE:
                df = decode_f32;
                sb = 4;
                le = p->sample_format == LSPC_SAMPLE_FMT_F32LE;
                break;

            case LSPC_SAMPLE_FMT_F64LE:
            case LSPC_SAMPLE_FMT_F64BE:
                df = decode_f64;
                sb = 8;
                le = p->sample_format == LSPC_SAMPLE_FMT_F64LE;
                break;

            default:
                return STATUS_UNSUPPORTED_FORMAT;
        }

        // Estimate number of bytes to read
        size_t fz               = sb * p->channels;
        size_t bytes_left       = fz * p->frames;

        // Allocate buffers
        sBuf.vData      = new uint8_t[BUFFER_SIZE];
        if (sBuf.vData == NULL)
            return STATUS_NO_MEM;

        pFBuffer        = new float[p->channels * BUFFER_FRAMES];
        if (pFBuffer == NULL)
        {
            delete [] sBuf.vData;
            sBuf.vData = NULL;
            return STATUS_NO_MEM;
        }

        if (le != arch_le)
            nFlags     |= F_REV_BYTES; // Set-up byte-reversal flag

        sParams         = *p;
        nBPS            = sb;
        nFrameSize      = fz;
        nBytesLeft      = bytes_left;
        sBuf.nOff       = 0;
        sBuf.nSize      = 0;
        pDecode         = df;

        return STATUS_OK;
    }

    status_t LSPCAudioReader::open(LSPCFile *lspc, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;
        nFlags      = 0;

        LSPCChunkReader *rd = lspc->find_chunk(LSPC_CHUNK_AUDIO, NULL, 0);
        if (rd == NULL)
            return STATUS_NOT_FOUND;

        status_t res = read_audio_header(rd);
        if (res != STATUS_OK)
        {
            rd->close();
            return res;
        }

        pFD         = lspc;
        pRD         = rd;
        nFlags     |= F_OPENED | F_CLOSE_READER | F_DROP_READER;
        if (auto_close)
            nFlags     |= F_CLOSE_FILE;
        return STATUS_OK;
    }

    status_t LSPCAudioReader::open(LSPCFile *lspc, uint32_t uid, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;
        nFlags      = 0;

        LSPCChunkReader *rd = lspc->read_chunk(uid);
        if (rd == NULL)
            return STATUS_NOT_FOUND;
        else if (rd->magic() != LSPC_CHUNK_AUDIO)
        {
            rd->close();
            return STATUS_BAD_TYPE;
        }

        status_t res = read_audio_header(rd);
        if (res != STATUS_OK)
        {
            rd->close();
            return res;
        }

        pFD         = lspc;
        pRD         = rd;
        nFlags     |= F_OPENED | F_CLOSE_READER | F_DROP_READER;
        if (auto_close)
            nFlags     |= F_CLOSE_FILE;
        return STATUS_OK;
    }

    status_t LSPCAudioReader::open_raw_magic(LSPCFile *lspc, const lspc_audio_parameters_t *params, uint32_t magic, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;
        else if (params == NULL)
            return STATUS_BAD_ARGUMENTS;
        nFlags      = 0;

        LSPCChunkReader *rd = lspc->find_chunk(magic, NULL, 0);
        if (rd == NULL)
            return STATUS_NOT_FOUND;

        status_t res = apply_params(params);
        if (res != STATUS_OK)
        {
            rd->close();
            return res;
        }

        pFD         = lspc;
        pRD         = rd;
        nFlags     |= F_OPENED | F_CLOSE_READER | F_DROP_READER;
        if (auto_close)
            nFlags     |= F_CLOSE_FILE;
        return STATUS_OK;
    }

    status_t LSPCAudioReader::open_raw_uid(LSPCFile *lspc, const lspc_audio_parameters_t *params, uint32_t uid, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;
        else if (params == NULL)
            return STATUS_BAD_ARGUMENTS;
        nFlags      = 0;

        LSPCChunkReader *rd = lspc->read_chunk(uid);
        if (rd == NULL)
            return STATUS_NOT_FOUND;

        status_t res = apply_params(params);
        if (res != STATUS_OK)
        {
            rd->close();
            return res;
        }

        pFD         = lspc;
        pRD         = rd;
        nFlags     |= F_OPENED | F_CLOSE_READER | F_DROP_READER;
        if (auto_close)
            nFlags     |= F_CLOSE_FILE;
        return STATUS_OK;
    }

    status_t LSPCAudioReader::open_raw(LSPCChunkReader *rd, const lspc_audio_parameters_t *params, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;
        else if (params == NULL)
            return STATUS_BAD_ARGUMENTS;
        nFlags      = 0;

        status_t res = apply_params(params);
        if (res != STATUS_OK)
            return res;

        pFD         = NULL;
        pRD         = rd;
        nFlags     |= F_OPENED;
        if (auto_close)
            nFlags     |= F_CLOSE_READER;
        return STATUS_OK;
    }

    status_t LSPCAudioReader::get_parameters(lspc_audio_parameters_t *dst) const
    {
        if (!(nFlags & F_OPENED))
            return STATUS_CLOSED;
        else if (dst == NULL)
            return STATUS_BAD_ARGUMENTS;
        *dst        = sParams;

        return STATUS_OK;
    }

    status_t LSPCAudioReader::fill_buffer()
    {
        // Move buffer data from end to the beginning
        size_t bsize = sBuf.nSize - sBuf.nOff;
        if ((sBuf.nSize > 0) && (bsize > 0))
        {
            ::memmove(sBuf.vData, &sBuf.vData[sBuf.nOff], bsize);
            sBuf.nSize      = bsize;
        }
        else
            sBuf.nSize      = 0;
        sBuf.nOff       = 0;

        // Try to read additional data
        bsize       = BUFFER_SIZE - bsize;
        ssize_t n   = pRD->read(&sBuf.vData[sBuf.nSize], bsize);
        if (n < 0)
            return status_t(-n);
        else if (n == 0) // Number of bytes should be multiple of nFrameSize
        {
            size_t delta = sBuf.nSize - sBuf.nOff;
            if (delta >= nFrameSize)
                return STATUS_OK;
            else if (delta == 0)
                return STATUS_EOF;
            else
                return STATUS_CORRUPTED_FILE;
        }

        sBuf.nSize     += n;
        return STATUS_OK;
    }

    ssize_t LSPCAudioReader::read_samples(float **data, size_t frames)
    {
        if (!(nFlags & F_OPENED))
            return STATUS_CLOSED;

        size_t nc       = sParams.channels;
        float **vp = reinterpret_cast<float **>(alloca(nc * sizeof(float *)));
        for (size_t i=0; i<nc; ++i)
            vp[i]       = data[i];

        size_t n_read   = 0;

        while (n_read < frames)
        {
            // Estimate number of frames to read
            size_t to_read = frames - n_read;
            if (to_read > BUFFER_FRAMES)
                to_read = BUFFER_FRAMES;

            // Read frames to temporary buffer
            ssize_t n   = read_frames(pFBuffer, to_read);
            if (n <= 0)
                return (n_read > 0) ? n_read : n;

            n_read     += n;

            // Unpack frames
            float *p    = pFBuffer;
            while (n--)
            {
                for (size_t j=0; j<nc; ++j, ++p)
                {
                    if (!vp[j])
                        continue;
                    *(vp[j]++)      = *p;
                }
            }
        }

        return n_read;
    }

    ssize_t LSPCAudioReader::read_frames(float *data, size_t frames)
    {
        if (!(nFlags & F_OPENED))
            return STATUS_CLOSED;

        size_t n_read   = 0;
        while (n_read < frames)
        {
            size_t to_read = frames - n_read;

            // Ensure that we have enough bytes to read at least one frame
            size_t avail = sBuf.nSize - sBuf.nOff;
            if (avail < nFrameSize)
            {
                // Try to fill buffer with new data
                status_t st = fill_buffer();
                if (st != STATUS_OK)
                    return (n_read > 0) ? n_read : -st;
                avail = sBuf.nSize - sBuf.nOff;
                if (avail < nFrameSize)
                    return (n_read > 0) ? n_read : STATUS_CORRUPTED_FILE;
            }

            // Perform decode
            avail   /= nFrameSize;
            if (avail > to_read)
                avail   = to_read;
            size_t floats = avail * sParams.channels;
            if (nFlags & F_REV_BYTES)
            {
                switch (nBPS)
                {
                    case 1:
                    case 3:
                        break;
                    case 2:
                        byte_swap(reinterpret_cast<uint16_t *>(&sBuf.vData[sBuf.nOff]), floats);
                        break;
                    case 4:
                        byte_swap(reinterpret_cast<uint32_t *>(&sBuf.vData[sBuf.nOff]), floats);
                        break;
                    case 8:
                        byte_swap(reinterpret_cast<uint64_t *>(&sBuf.vData[sBuf.nOff]), floats);
                        break;
                    default:
                        return STATUS_BAD_STATE;
                }
            }

            // Perform decode
            pDecode(data, &sBuf.vData[sBuf.nOff], floats);

            // Update pointers
            n_read         += avail;
            sBuf.nOff      += avail * nFrameSize;
            data           += floats;
        }

        return n_read;
    }

    ssize_t LSPCAudioReader::skip_frames(size_t frames)
    {
        if (!(nFlags & F_OPENED))
            return STATUS_CLOSED;

        size_t n_skip   = 0;
        while (n_skip < frames)
        {
            size_t to_skip = frames - n_skip;

            // Ensure that we have enough bytes to read at least one frame
            size_t avail = sBuf.nSize - sBuf.nOff;
            if (avail < nFrameSize)
            {
                // Try to fill buffer with new data
                status_t st = fill_buffer();
                if (st != STATUS_OK)
                    return (n_skip > 0) ? n_skip : -st;
                avail = sBuf.nSize - sBuf.nOff;
                if (avail < nFrameSize)
                    return (n_skip > 0) ? n_skip : STATUS_CORRUPTED_FILE;
            }

            // Perform decode
            avail   /= nFrameSize;
            if (avail > to_skip)
                avail   = to_skip;

            // Update pointers
            n_skip         += avail;
            sBuf.nOff      += avail * nFrameSize;
        }

        return n_skip;
    }

    uint32_t LSPCAudioReader::unique_id() const
    {
        if (!(nFlags & F_OPENED))
            return 0;
        else if (pRD == NULL)
            return 0;

        return pRD->unique_id();
    }

    uint32_t LSPCAudioReader::magic() const
    {
        if (!(nFlags & F_OPENED))
            return 0;
        else if (pRD == NULL)
            return 0;

        return pRD->magic();
    }

} /* namespace lsp */
