/*
 * LSPCAudioWriter.cpp
 *
 *  Created on: 6 нояб. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <dsp/endian.h>
#include <core/files/lspc/LSPCAudioWriter.h>

#define BUFFER_FRAMES   0x400

namespace lsp
{
    LSPCAudioWriter::LSPCAudioWriter()
    {
        sParams.channels        = 0;
        sParams.sample_format   = 0;
        sParams.sample_rate     = 0;
        sParams.codec           = 0;
        sParams.frames          = 0;

        pFD                     = NULL;
        pWD                     = NULL;
        nFlags                  = 0;
        nBPS                    = 0;
        nFrameChannels          = 0;
        pEncode                 = NULL;
        pBuffer                 = NULL;
        pFBuffer                = NULL;
    }
    
    LSPCAudioWriter::~LSPCAudioWriter()
    {
        free_resources();
    }


    status_t LSPCAudioWriter::free_resources()
    {
        status_t res = STATUS_OK;
        if (pWD != NULL)
        {
            status_t xr = STATUS_OK;
            if (nFlags & F_CLOSE_WRITER)
                xr = pWD->close();
            if (nFlags & F_DROP_WRITER)
                delete pWD;
            pWD         = NULL;

            if (res == STATUS_OK)
                res     = xr;
        }

        if ((pFD != NULL) && (nFlags & F_CLOSE_FILE))
        {
            status_t xr = pFD->close();
            pWD         = NULL;

            if (res == STATUS_OK)
                res     = xr;
        }

        if (pFBuffer != NULL)
        {
            delete [] pFBuffer;
            pFBuffer = NULL;
        }

        if (pBuffer != NULL)
        {
            delete [] pBuffer;
            pBuffer = NULL;
        }

        nFlags                  = 0;
        nBPS                    = 0;
        nFrameChannels          = 0;
        pEncode                 = NULL;

        return res;
    }

    status_t LSPCAudioWriter::close()
    {
        // Check open status first
        if (!(nFlags & F_OPENED))
            return STATUS_CLOSED;
        return free_resources();
    }

    status_t LSPCAudioWriter::parse_parameters(const lspc_audio_parameters_t *p)
    {
        if (p == NULL)
            return STATUS_BAD_ARGUMENTS;
        else if (p->channels > 0xff)
            return STATUS_BAD_FORMAT;
        else if (p->sample_rate == 0)
            return STATUS_BAD_FORMAT;
        else if (p->codec != LSPC_CODEC_PCM)
            return STATUS_BAD_FORMAT;

        size_t sb           = 0;
        encode_func_t ef    = NULL;
        bool le             = false;
        bool arch_le        = __IF_LEBE(true, false);

        switch (p->sample_format)
        {
            case LSPC_SAMPLE_FMT_U8LE:
            case LSPC_SAMPLE_FMT_U8BE:
                ef = encode_u8;
                sb = 1;
                le = p->sample_format == LSPC_SAMPLE_FMT_U8LE;
                break;

            case LSPC_SAMPLE_FMT_S8LE:
            case LSPC_SAMPLE_FMT_S8BE:
                ef = encode_s8;
                sb = 1;
                le = p->sample_format == LSPC_SAMPLE_FMT_S8LE;
                break;

            case LSPC_SAMPLE_FMT_U16LE:
            case LSPC_SAMPLE_FMT_U16BE:
                ef = encode_u16;
                sb = 1;
                le = p->sample_format == LSPC_SAMPLE_FMT_U16LE;
                break;

            case LSPC_SAMPLE_FMT_S16LE:
            case LSPC_SAMPLE_FMT_S16BE:
                ef = encode_s16;
                sb = 1;
                le = p->sample_format == LSPC_SAMPLE_FMT_S16LE;
                break;

            case LSPC_SAMPLE_FMT_U24LE:
                ef = encode_u24le;
                sb = 3;
                le = true;
                break;
            case LSPC_SAMPLE_FMT_U24BE:
                ef = encode_u24be;
                sb = 3;
                le = false;
                break;
            case LSPC_SAMPLE_FMT_S24LE:
                ef = encode_s24le;
                sb = 3;
                le = true;
                break;
            case LSPC_SAMPLE_FMT_S24BE:
                ef = encode_s24be;
                sb = 3;
                le = false;
                break;

            case LSPC_SAMPLE_FMT_U32LE:
            case LSPC_SAMPLE_FMT_U32BE:
                ef = encode_u32;
                sb = 4;
                le = p->sample_format == LSPC_SAMPLE_FMT_U32LE;
                break;

            case LSPC_SAMPLE_FMT_S32LE:
            case LSPC_SAMPLE_FMT_S32BE:
                ef = encode_s32;
                sb = 4;
                le = p->sample_format == LSPC_SAMPLE_FMT_S32LE;
                break;

            case LSPC_SAMPLE_FMT_F32LE:
            case LSPC_SAMPLE_FMT_F32BE:
                ef = encode_f32;
                sb = 4;
                le = p->sample_format == LSPC_SAMPLE_FMT_F32LE;
                break;

            case LSPC_SAMPLE_FMT_F64LE:
            case LSPC_SAMPLE_FMT_F64BE:
                ef = encode_f64;
                sb = 8;
                le = p->sample_format == LSPC_SAMPLE_FMT_F64LE;
                break;

            default:
                return STATUS_UNSUPPORTED_FORMAT;
        }

        // Estimate number of bytes to read
        size_t fz               = sb * p->channels;

        // Allocate buffers
        pFBuffer        = new uint8_t[fz * BUFFER_FRAMES];
        if (pFBuffer == NULL)
            return STATUS_NO_MEM;

        pBuffer         = new float[p->channels * BUFFER_FRAMES];
        if (pBuffer == NULL)
        {
            delete [] pFBuffer;
            pFBuffer    = NULL;
            return STATUS_NO_MEM;
        }

        if (le != arch_le)
            nFlags     |= F_REV_BYTES; // Set-up byte-reversal flag

        sParams         = *p;
        nBPS            = sb;
        nFrameChannels  = p->channels;
        pEncode         = ef;

        return STATUS_OK;
    }

    status_t LSPCAudioWriter::write_header(LSPCChunkWriter *wr)
    {
        lspc_chunk_audio_header_t hdr;

        ::memset(&hdr, 0, sizeof(hdr));
        hdr.common.size     = sizeof(lspc_chunk_audio_header_t);
        hdr.common.version  = 1;
        hdr.channels        = sParams.channels;
        hdr.sample_format   = sParams.sample_format;
        hdr.sample_rate     = sParams.sample_rate;
        hdr.codec           = sParams.codec;
        hdr.frames          = sParams.frames;
        hdr.offset          = 0;

        return wr->write_header(&hdr);
    }

    status_t LSPCAudioWriter::write(LSPCFile *lspc, const lspc_audio_parameters_t *params, bool last, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;

        nFlags                  = 0;
        status_t res = parse_parameters(params);
        if (res != STATUS_OK)
            return res;

        LSPCChunkWriter *wr = lspc->write_chunk(LSPC_CHUNK_AUDIO);
        if (wr == NULL)
            return STATUS_NO_MEM;

        res = write_header(wr);
        if (res != STATUS_OK)
        {
            free_resources();
            wr->close();
            delete wr;
            return res;
        }

        pFD         = lspc;
        pWD         = wr;
        nFlags     |= F_OPENED | F_CLOSE_WRITER | F_DROP_WRITER;
        if (auto_close)
            nFlags     |= F_CLOSE_FILE;
        return STATUS_OK;
    }

    status_t LSPCAudioWriter::write_raw(LSPCFile *lspc, const lspc_audio_parameters_t *params, bool last, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;

        nFlags                  = 0;
        status_t res = parse_parameters(params);
        if (res != STATUS_OK)
            return res;

        LSPCChunkWriter *wr = lspc->write_chunk(LSPC_CHUNK_AUDIO);
        if (wr == NULL)
            return STATUS_NO_MEM;

        pFD         = lspc;
        pWD         = wr;
        nFlags     |= F_OPENED | F_CLOSE_WRITER | F_DROP_WRITER;
        if (auto_close)
            nFlags     |= F_CLOSE_FILE;
        return STATUS_OK;
    }

    status_t LSPCAudioWriter::write(LSPCFile *lspc, uint32_t magic, const lspc_audio_parameters_t *params, bool last, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;

        nFlags                  = 0;
        status_t res = parse_parameters(params);
        if (res != STATUS_OK)
            return res;

        LSPCChunkWriter *wr = lspc->write_chunk(magic);
        if (wr == NULL)
            return STATUS_NO_MEM;

        res = write_header(wr);
        if (res != STATUS_OK)
        {
            free_resources();
            wr->close();
            delete wr;
            return res;
        }

        pFD         = lspc;
        pWD         = wr;
        nFlags     |= F_OPENED | F_CLOSE_WRITER | F_DROP_WRITER;
        if (auto_close)
            nFlags     |= F_CLOSE_FILE;
        return STATUS_OK;
    }

    status_t LSPCAudioWriter::write_raw(LSPCFile *lspc, uint32_t magic, const lspc_audio_parameters_t *params, bool last, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;

        nFlags                  = 0;
        status_t res = parse_parameters(params);
        if (res != STATUS_OK)
            return res;

        LSPCChunkWriter *wr = lspc->write_chunk(magic);
        if (wr == NULL)
            return STATUS_NO_MEM;

        pFD         = lspc;
        pWD         = wr;
        nFlags     |= F_OPENED | F_CLOSE_WRITER | F_DROP_WRITER;
        if (auto_close)
            nFlags     |= F_CLOSE_FILE;
        return STATUS_OK;
    }

    status_t LSPCAudioWriter::write(LSPCChunkWriter *wr, const lspc_audio_parameters_t *params, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;

        nFlags                  = 0;
        status_t res = parse_parameters(params);
        if (res != STATUS_OK)
            return res;

        res = write_header(wr);
        if (res != STATUS_OK)
        {
            free_resources();
            return res;
        }

        nFlags     |= F_OPENED;
        if (auto_close)
            nFlags     |= F_CLOSE_WRITER;
        return STATUS_OK;
    }

    status_t LSPCAudioWriter::write_raw(LSPCChunkWriter *wr, const lspc_audio_parameters_t *params, bool auto_close)
    {
        if (nFlags & F_OPENED)
            return STATUS_OPENED;

        nFlags                  = 0;
        status_t res = parse_parameters(params);
        if (res != STATUS_OK)
            return res;

        nFlags     |= F_OPENED;
        if (auto_close)
            nFlags     |= F_CLOSE_WRITER;
        return STATUS_OK;
    }

    status_t LSPCAudioWriter::write_samples(const float **data, size_t frames)
    {
        if (!(nFlags & F_OPENED))
            return STATUS_CLOSED;
        return STATUS_OK;
    }

    status_t LSPCAudioWriter::write_frames(const float *data, size_t frames)
    {
        if (!(nFlags & F_OPENED))
            return STATUS_CLOSED;

        size_t n_written = 0;
        while (n_written < frames)
        {
            // Estimate number of frames to write
            size_t to_write = frames - n_written;
            if (to_write > BUFFER_FRAMES)
                to_write = BUFFER_FRAMES;

            // Copy frames to buffer
            size_t floats = to_write * nFrameChannels;
            pEncode(pFBuffer, data, floats);

            // Reverse bytes (if required)
            if (nFlags & F_REV_BYTES)
            {
                switch (nBPS)
                {
                    case 1:
                    case 3:
                        break;
                    case 2:
                        byte_swap(reinterpret_cast<uint16_t *>(pFBuffer), floats);
                        break;
                    case 4:
                        byte_swap(reinterpret_cast<uint32_t *>(pFBuffer), floats);
                        break;
                    case 8:
                        byte_swap(reinterpret_cast<uint64_t *>(pFBuffer), floats);
                        break;
                    default:
                        return STATUS_BAD_STATE;
                }
            }

            data       += to_write;
            n_written  += to_write;
        }

        return STATUS_OK;
    }

    status_t LSPCAudioWriter::get_parameters(lspc_audio_parameters_t *dst) const
    {
        if (!(nFlags & F_OPENED))
            return STATUS_CLOSED;
        else if (dst == NULL)
            return STATUS_BAD_ARGUMENTS;
        *dst        = sParams;

        return STATUS_OK;
    }

    uint32_t LSPCAudioWriter::unique_id() const
    {
        if (!(nFlags & F_OPENED))
            return 0;
        else if (pWD == NULL)
            return 0;

        return pWD->unique_id();
    }

    uint32_t LSPCAudioWriter::magic() const
    {
        if (!(nFlags & F_OPENED))
            return 0;
        else if (pWD == NULL)
            return 0;

        return pWD->magic();
    }

} /* namespace lsp */
