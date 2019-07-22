/*
 * LSPCAudioWriter.cpp
 *
 *  Created on: 6 нояб. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <dsp/endian.h>
#include <core/files/lspc/LSPCAudioWriter.h>
#include <stdlib.h>

#define BUFFER_FRAMES   0x400

namespace lsp
{
    void LSPCAudioWriter::encode_u8(void *vp, const float *src, size_t ns)
    {
        uint8_t *p = reinterpret_cast<uint8_t *>(vp);
        while (ns--)
            *(p++)  = int8_t((*src++) * 0x7f) + 0x80;
    }

    void LSPCAudioWriter::encode_s8(void *vp, const float *src, size_t ns)
    {
        int8_t *p = reinterpret_cast<int8_t *>(vp);
        while (ns--)
            *(p++)  = int8_t((*src++) * 0x7f);
    }

    void LSPCAudioWriter::encode_u16(void *vp, const float *src, size_t ns)
    {
        uint16_t *p = reinterpret_cast<uint16_t *>(vp);
        while (ns--)
            *(p++)  = int16_t((*src++) * 0x7fff) + 0x8000;
    }

    void LSPCAudioWriter::encode_s16(void *vp, const float *src, size_t ns)
    {
        int16_t *p = reinterpret_cast<int16_t *>(vp);
        while (ns--)
            *(p++)  = int16_t((*src++) * 0x7fff);
    }

    void LSPCAudioWriter::encode_u24le(void *vp, const float *src, size_t ns)
    {
        uint8_t *p = reinterpret_cast<uint8_t *>(vp);
        while (ns--)
        {
            uint32_t s = int32_t(*(src++) * 0x7fffff) + 0x800000;
            __IF_LE(p[0] = uint8_t(s); p[1] = uint8_t(s >> 8); p[2] = uint8_t(s >> 16));
            __IF_BE(p[0] = uint8_t(s >> 16); p[1] = uint8_t(s >> 8); p[2] = uint8_t(s));
            p += 3;
        }
    }

    void LSPCAudioWriter::encode_u24be(void *vp, const float *src, size_t ns)
    {
        uint8_t *p = reinterpret_cast<uint8_t *>(vp);
        while (ns--)
        {
            uint32_t s = int32_t(*(src++) * 0x7fffff) + 0x800000;
            __IF_BE(p[0] = uint8_t(s); p[1] = uint8_t(s >> 8); p[2] = uint8_t(s >> 16));
            __IF_LE(p[0] = uint8_t(s >> 16); p[1] = uint8_t(s >> 8); p[2] = uint8_t(s));
            p += 3;
        }
    }

    void LSPCAudioWriter::encode_s24le(void *vp, const float *src, size_t ns)
    {
        uint8_t *p = reinterpret_cast<uint8_t *>(vp);
        while (ns--)
        {
            int32_t s   = int32_t(*(src++) * 0x7fffff);
            __IF_LE(p[0] = uint8_t(s); p[1] = uint8_t(s >> 8); p[2] = uint8_t(s >> 16));
            __IF_BE(p[0] = uint8_t(s >> 16); p[1] = uint8_t(s >> 8); p[2] = uint8_t(s));
            p += 3;
        }
    }

    void LSPCAudioWriter::encode_s24be(void *vp, const float *src, size_t ns)
    {
        uint8_t *p = reinterpret_cast<uint8_t *>(vp);
        while (ns--)
        {
            int32_t s   = int32_t(*(src++) * 0x7fffff);
            __IF_BE(p[0] = uint8_t(s); p[1] = uint8_t(s >> 8); p[2] = uint8_t(s >> 16));
            __IF_LE(p[0] = uint8_t(s >> 16); p[1] = uint8_t(s >> 8); p[2] = uint8_t(s));
            p += 3;
        }
    }

    void LSPCAudioWriter::encode_u32(void *vp, const float *src, size_t ns)
    {
        uint32_t *p = reinterpret_cast<uint32_t *>(vp);
        while (ns--)
        {
            int32_t v = double(*(src++)) * 0x7fffffff;
            *(p++)  = v + 0x80000000;
        }
    }

    void LSPCAudioWriter::encode_s32(void *vp, const float *src, size_t ns)
    {
        int32_t *p = reinterpret_cast<int32_t *>(vp);
        while (ns--)
            *(p++)  = double(*(src++)) * 0x7fffffff;
    }

    void LSPCAudioWriter::encode_f32(void *vp, const float *src, size_t ns)
    {
        float *p = reinterpret_cast<float *>(vp);
        dsp::copy(p, src, ns);
    }

    void LSPCAudioWriter::encode_f64(void *vp, const float *src, size_t ns)
    {
        double *p = reinterpret_cast<double *>(vp);
        while (ns--)
            *(p++)  = *(src++);
    }

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

        if (pFD != NULL)
        {
            if (nFlags & F_CLOSE_FILE)
            {
                status_t xr = pFD->close();
                if (res == STATUS_OK)
                    res     = xr;
            }
            if (nFlags & F_DROP_FILE)
                delete pFD;

            pFD         = NULL;
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
        bool int_sample     = true;
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
                sb = 2;
                le = p->sample_format == LSPC_SAMPLE_FMT_U16LE;
                break;

            case LSPC_SAMPLE_FMT_S16LE:
            case LSPC_SAMPLE_FMT_S16BE:
                ef = encode_s16;
                sb = 2;
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
                int_sample = false;
                sb = 4;
                le = p->sample_format == LSPC_SAMPLE_FMT_F32LE;
                break;

            case LSPC_SAMPLE_FMT_F64LE:
            case LSPC_SAMPLE_FMT_F64BE:
                ef = encode_f64;
                int_sample = false;
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
        if (int_sample)
            nFlags     |= F_INTEGER_SAMPLE;

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

        hdr.channels        = CPU_TO_BE(hdr.channels);
        hdr.sample_format   = CPU_TO_BE(hdr.sample_format);
        hdr.sample_rate     = CPU_TO_BE(hdr.sample_rate);
        hdr.codec           = CPU_TO_BE(hdr.codec);
        hdr.frames          = CPU_TO_BE(hdr.frames);
        hdr.offset          = CPU_TO_BE(hdr.offset);

        return wr->write_header(&hdr);
    }

    status_t LSPCAudioWriter::create(const char *path, const lspc_audio_parameters_t *params)
    {
        LSPString tmp;
        if (!tmp.set_utf8(path))
            return STATUS_NO_MEM;
        return create(&tmp, params);
    }

    status_t LSPCAudioWriter::create(const LSPString *path, const lspc_audio_parameters_t *params)
    {
        LSPCFile *fd = new LSPCFile();
        if (fd == NULL)
            return STATUS_NO_MEM;

        status_t res = fd->create(path);
        if (res == STATUS_OK)
        {
            res     = open(fd, params, true);
            if (res == STATUS_OK)
                nFlags     |= F_CLOSE_FILE | F_DROP_FILE;
        }

        if (res != STATUS_OK)
        {
            fd->close();
            delete fd;
            return res;
        }

        return STATUS_OK;
    }

    status_t LSPCAudioWriter::create(const io::Path *path, const lspc_audio_parameters_t *params)
    {
        return create(path->as_string(), params);
    }

    status_t LSPCAudioWriter::open(LSPCFile *lspc, const lspc_audio_parameters_t *params, bool auto_close)
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

    status_t LSPCAudioWriter::open_raw(LSPCFile *lspc, const lspc_audio_parameters_t *params, bool auto_close)
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

    status_t LSPCAudioWriter::open(LSPCFile *lspc, uint32_t magic, const lspc_audio_parameters_t *params, bool auto_close)
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

    status_t LSPCAudioWriter::open_raw(LSPCFile *lspc, uint32_t magic, const lspc_audio_parameters_t *params, bool auto_close)
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

    status_t LSPCAudioWriter::open(LSPCChunkWriter *wr, const lspc_audio_parameters_t *params, bool auto_close)
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

    status_t LSPCAudioWriter::open_raw(LSPCChunkWriter *wr, const lspc_audio_parameters_t *params, bool auto_close)
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

        size_t nc       = sParams.channels;
        const float **vp = reinterpret_cast<const float **>(alloca(nc * sizeof(float *)));
        for (size_t i=0; i<nc; ++i)
            vp[i]       = data[i];

        size_t n_written = 0;
        while (n_written < frames)
        {
            // Estimate number of frames to write
            size_t to_write = frames - n_written;
            if (to_write > BUFFER_FRAMES)
                to_write = BUFFER_FRAMES;

            // Pack frames
            float *p    = pBuffer;
            for (size_t i=0; i<to_write; ++i)
                for (size_t j=0; j<nc; ++j, ++p)
                    *p = (vp[j]) ? *(vp[j]++) : 0.0f;

            // Write frames
            status_t res    = write_frames(pBuffer, to_write);
            if (res != STATUS_OK)
                return res;
            n_written  += to_write;
        }

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
            if (nFlags & F_INTEGER_SAMPLE)
            {
                dsp::limit_saturate2(pBuffer, data, floats);
                pEncode(pFBuffer, pBuffer, floats);
            }
            else
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

            // Write data to LSPC
            status_t res = pWD->write(pFBuffer, floats * nBPS);
            if (res != STATUS_OK)
                return res;

            data       += floats;
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
