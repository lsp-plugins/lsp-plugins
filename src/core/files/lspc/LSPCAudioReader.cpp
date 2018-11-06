/*
 * LSPCSampleReader.cpp
 *
 *  Created on: 6 нояб. 2018 г.
 *      Author: sadko
 */

#include <dsp/endian.h>
#include <string.h>
#include <stdlib.h>
#include <core/files/lspc/LSPCAudioReader.h>

#define BUFFER_SIZE     0x1000

namespace lsp
{
    
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
        if ((nFlags & F_CLOSE_READER) && (pRD != NULL))
        {
            status_t xr     = pRD->close();
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

        // Drop buffer
        if (sBuf.vData != NULL)
        {
            delete [] sBuf.vData;
            sBuf.vData      = NULL;
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
        size_t sb = 0;

        switch (p->sample_format)
        {
            case LSPC_SAMPLE_FMT_U8LE:
            case LSPC_SAMPLE_FMT_U8BE:
            case LSPC_SAMPLE_FMT_S8LE:
            case LSPC_SAMPLE_FMT_S8BE:
                sb = 1;
                break;
            case LSPC_SAMPLE_FMT_U16LE:
            case LSPC_SAMPLE_FMT_U16BE:
            case LSPC_SAMPLE_FMT_S16LE:
            case LSPC_SAMPLE_FMT_S16BE:
                sb = 2;
                break;
            case LSPC_SAMPLE_FMT_U24LE:
            case LSPC_SAMPLE_FMT_U24BE:
            case LSPC_SAMPLE_FMT_S24LE:
            case LSPC_SAMPLE_FMT_S24BE:
                sb = 3;
                break;
            case LSPC_SAMPLE_FMT_U32LE:
            case LSPC_SAMPLE_FMT_U32BE:
            case LSPC_SAMPLE_FMT_S32LE:
            case LSPC_SAMPLE_FMT_S32BE:
            case LSPC_SAMPLE_FMT_F32LE:
            case LSPC_SAMPLE_FMT_F32BE:
                sb = 4;
                break;
            case LSPC_SAMPLE_FMT_F64LE:
            case LSPC_SAMPLE_FMT_F64BE:
                sb = 8;
                break;
            default:
                return STATUS_UNSUPPORTED_FORMAT;
        }

        // Estimate number of bytes to read
        size_t fz               = sb * p->channels;
        size_t bytes_left       = fz * p->frames;

        sBuf.vData      = new uint8_t[BUFFER_SIZE];
        if (sBuf.vData == NULL)
            return STATUS_NO_MEM;

        nBPS            = sb;
        nFrameSize      = fz;
        nBytesLeft      = bytes_left;
        sBuf.nOff       = 0;
        sBuf.nSize      = 0;

        return STATUS_OK;
    }

    status_t LSPCAudioReader::open(LSPCFile *lspc, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;

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
        nFlags      = (auto_close) ? F_OPENED | F_CLOSE_READER | F_CLOSE_FILE : F_OPENED | F_CLOSE_READER;
        return STATUS_OK;
    }

    status_t LSPCAudioReader::open(LSPCFile *lspc, uint32_t uid, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;

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
        nFlags      = (auto_close) ? F_OPENED | F_CLOSE_READER | F_CLOSE_FILE : F_OPENED | F_CLOSE_READER;
        return STATUS_OK;
    }

    status_t LSPCAudioReader::open_raw_magic(LSPCFile *lspc, const lspc_audio_parameters_t *params, uint32_t magic, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;
        else if (params == NULL)
            return STATUS_BAD_ARGUMENTS;

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
        nFlags      = (auto_close) ? F_OPENED | F_CLOSE_READER | F_CLOSE_FILE : F_OPENED | F_CLOSE_READER;
        return STATUS_OK;
    }

    status_t LSPCAudioReader::open_raw_uid(LSPCFile *lspc, const lspc_audio_parameters_t *params, uint32_t uid, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;
        else if (params == NULL)
            return STATUS_BAD_ARGUMENTS;

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
        nFlags      = (auto_close) ? F_OPENED | F_CLOSE_READER | F_CLOSE_FILE : F_OPENED | F_CLOSE_READER;
        return STATUS_OK;
    }

    status_t LSPCAudioReader::open_raw(LSPCChunkReader *rd, const lspc_audio_parameters_t *params, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;
        else if (params == NULL)
            return STATUS_BAD_ARGUMENTS;

        status_t res = apply_params(params);
        if (res != STATUS_OK)
            return res;

        pFD         = NULL;
        pRD         = rd;
        nFlags      = (auto_close) ? F_OPENED | F_CLOSE_READER : F_OPENED;
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
            sBuf.nOff       = 0;
        }

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

    ssize_t LSPCAudioReader::read(float **data, size_t frames)
    {
        if (!(nFlags & F_OPENED))
            return STATUS_CLOSED;

        float **vp = reinterpret_cast<float **>(alloca(sParams.channels) * sizeof(float *));
        for (size_t i=0; i<sParams.channels; ++i)
            vp[i]       = data[i];

        size_t bytes;
        size_t n_read   = 0;
        while (frames > 0)
        {
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
            avail   /= nBPS;
            if (avail > n_read)
                avail   = n_read;
            if (nFlags & F_REV_BYTES)
            {
                switch (nBPS)
                {
                    case 1:
                        bytes   = avail;
                        break;
                    case 2:
                        byte_swap(reinterpret_cast<uint16_t *>(&sBuf.vData[sBuf.nOff]), avail << 1);
                        bytes   = avail << 1;
                        break;
                    case 3:
                        bytes   = avail + (avail << 1);
                        break;
                    case 4:
                        byte_swap(reinterpret_cast<uint32_t *>(&sBuf.vData[sBuf.nOff]), avail << 2);
                        bytes   = avail << 2;
                        break;
                    case 8:
                        byte_swap(reinterpret_cast<uint64_t *>(&sBuf.vData[sBuf.nOff]), avail << 3);
                        bytes   = avail << 3;
                        break;
                    default:
                        return STATUS_BAD_STATE;
                }
            }

            // Perform decode
            pDecode(vp, sParams.channels, avail);

            // Update pointers
            n_read         += avail;
            avail          -= bytes;
            sBuf.nOff      += bytes;
        }

        return n_read;
    }

} /* namespace lsp */
