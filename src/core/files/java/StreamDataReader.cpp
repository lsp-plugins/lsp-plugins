/*
 * StreamDataReader.cpp
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#include <dsp/endian.h>
#include <core/stdlib/string.h>
#include <core/io/InFileStream.h>
#include <core/io/InMemoryStream.h>

#include <core/files/java/defs.h>
#include <core/files/java/StreamDataReader.h>

namespace lsp
{
    namespace java
    {
        
        StreamDataReader::StreamDataReader()
        {
            pIS         = NULL;
            nFlags      = 0;
            nToken      = -1;
            enToken     = -STATUS_CLOSED;
            nVersion    = -STATUS_CLOSED;
            nHandle     = 0;
        }
        
        StreamDataReader::~StreamDataReader()
        {
            if (pIS != NULL)
            {
                if (nFlags & WRAP_CLOSE)
                    pIS->close();
                if (nFlags & WRAP_DELETE)
                    delete pIS;
                pIS     = NULL;
            }
            nFlags  = 0;
            nToken      = -1;
            enToken     = -STATUS_CLOSED;
            nVersion    = -STATUS_CLOSED;
        }

        status_t StreamDataReader::open(const char *file)
        {
            io::InFileStream *is = new io::InFileStream();
            status_t res = is->open(file);
            if (res == STATUS_OK)
            {
                res     = initial_read(is);
                if (res == STATUS_OK)
                {
                    pIS     = is;
                    nFlags  = WRAP_CLOSE | WRAP_DELETE;
                }
                is->close();
            }
            delete is;
            return res;
        }

        status_t StreamDataReader::open(const LSPString *file)
        {
            io::InFileStream *is = new io::InFileStream();
            status_t res = is->open(file);
            if (res == STATUS_OK)
            {
                res     = initial_read(is);
                if (res == STATUS_OK)
                {
                    pIS     = is;
                    nFlags  = WRAP_CLOSE | WRAP_DELETE;
                    return res;
                }
                is->close();
            }
            delete is;
            return res;
        }

        status_t StreamDataReader::open(const io::Path *file)
        {
            io::InFileStream *is = new io::InFileStream();
            status_t res = is->open(file);
            if (res == STATUS_OK)
            {
                res     = initial_read(is);
                if (res == STATUS_OK)
                {
                    pIS     = is;
                    nFlags  = WRAP_CLOSE | WRAP_DELETE;
                    return res;
                }
                is->close();
            }
            delete is;
            return res;
        }

        status_t StreamDataReader::wrap(const void *buf, size_t count)
        {
            io::InMemoryStream *is = new io::InMemoryStream(buf, count);
            status_t res     = initial_read(is);
            if (res == STATUS_OK)
            {
                pIS     = is;
                nFlags  = WRAP_CLOSE | WRAP_DELETE;
                return res;
            }
            is->close();
            delete is;
            return res;
        }

        status_t StreamDataReader::wrap(void *buf, size_t count, lsp_memdrop_t drop)
        {
            io::InMemoryStream *is = new io::InMemoryStream(buf, count, drop);
            status_t res     = initial_read(is);
            if (res == STATUS_OK)
            {
                pIS     = is;
                nFlags  = WRAP_CLOSE | WRAP_DELETE;
                return res;
            }
            is->close();
            delete is;
            return res;
        }

        status_t StreamDataReader::wrap(io::IInStream *is, size_t flags)
        {
            status_t res    = initial_read(is);
            if (res == STATUS_OK)
            {
                pIS     = is;
                nFlags  = flags;
            }
            return res;
        }

        status_t StreamDataReader::close()
        {
            status_t res = STATUS_OK;
            if (pIS != NULL)
            {
                if (nFlags & WRAP_CLOSE)
                    res = pIS->close();
                if (nFlags & WRAP_DELETE)
                    delete pIS;
                pIS     = NULL;
            }

            nFlags      = 0;
            nToken      = -1;
            enToken     = -STATUS_CLOSED;
            nVersion    = -STATUS_CLOSED;

            return res;
        }

        status_t StreamDataReader::initial_read(io::IInStream *is)
        {
            // Read stream header
            obj_stream_hdr_t hdr;
            status_t res = is->read_block(&hdr, sizeof(hdr));
            if (res != STATUS_OK)
                return (res == STATUS_EOF) ? STATUS_BAD_FORMAT : res;
            if (BE_TO_CPU(hdr.magic) != JAVA_STREAM_MAGIC)
                return STATUS_BAD_FORMAT;

            nVersion    = BE_TO_CPU(hdr.version);
            nToken      = -STATUS_UNSPECIFIED;

            return STATUS_OK;
        }

        ssize_t StreamDataReader::get_token(bool force)
        {
            if (pIS == NULL)
                return - STATUS_CLOSED;

            if (force)
            {
                ssize_t token   = pIS->read_byte();
                switch (token)
                {
                    #define JDEC(a, b) case a: enToken = b; break;
                    JDEC(TC_NULL, JST_NULL)
                    JDEC(TC_REFERENCE, JST_REFERENCE)
                    JDEC(TC_CLASS, JST_CLASS)
                    JDEC(TC_CLASSDESC, JST_CLASS_DESC)
                    JDEC(TC_PROXYCLASSDESC, JST_PROXY_CLASS_DESC)
                    JDEC(TC_STRING, JST_STRING)
                    JDEC(TC_LONGSTRING, JST_STRING)
                    JDEC(TC_ARRAY, JST_ARRAY)
                    JDEC(TC_ENUM, JST_ENUM)
                    JDEC(TC_OBJECT, JST_OBJECT)
                    JDEC(TC_EXCEPTION, JST_EXCEPTION)
                    JDEC(TC_BLOCKDATA, JST_BLOCK_DATA)
                    JDEC(TC_BLOCKDATALONG, JST_BLOCK_DATA)
                    JDEC(TC_RESET, JST_RESET)
                    #undef JDEC
                    default:
                        if (token >= 0)
                            token       = -STATUS_CORRUPTED;
                        enToken     = nToken;
                        break;
                }
                nToken      = token;
            }

            return nToken;
        }

        status_t StreamDataReader::lookup_token()
        {
            if (enToken >= 0)
                return STATUS_OK;
            if (pIS == NULL)
                return STATUS_CLOSED;
            ssize_t res = get_token(true);
            return (res >= 0) ? STATUS_OK : -res;
        }

#define STREAM_READ_IMPL(type_t) \
        status_t StreamDataReader::read_simple(type_t *dst) \
        { \
            type_t tmp; \
            if (pIS == NULL) \
                return STATUS_CLOSED; \
            status_t res = pIS->read_block(&tmp, sizeof(tmp)); \
            if ((res == STATUS_OK) && (dst != NULL)) \
                *dst    = BE_TO_CPU(tmp); \
            nToken      = -STATUS_UNSPECIFIED; \
            enToken     = -STATUS_UNSPECIFIED; \
            return res; \
        }

        STREAM_READ_IMPL(uint8_t)
        STREAM_READ_IMPL(int8_t)
        STREAM_READ_IMPL(uint16_t)
        STREAM_READ_IMPL(int16_t)
        STREAM_READ_IMPL(uint32_t)
        STREAM_READ_IMPL(int32_t)
        STREAM_READ_IMPL(uint64_t)
        STREAM_READ_IMPL(int64_t)
        STREAM_READ_IMPL(float)
        STREAM_READ_IMPL(double)

#undef STREAM_READ_IMPL

        status_t StreamDataReader::set_error(status_t res)
        {
            if (res == STATUS_EOF)
            {
                nToken      = -STATUS_EOF;
                enToken     = -STATUS_EOF;
            }
            else
            {
                nToken      = -STATUS_UNSPECIFIED;
                enToken     = -STATUS_UNSPECIFIED;
            }
            return res;
        }

        status_t StreamDataReader::read_string(size_t *handle, LSPString *dst)
        {
            status_t res = lookup_token();
            if (res != STATUS_OK)
                return res;

            // Fetch size of string
            size_t bytes;
            switch (nToken)
            {
                case TC_STRING:
                {
                    uint16_t slen = 0;
                    if ((res = read_simple(&slen)) != STATUS_OK)
                        return STATUS_CORRUPTED;
                    bytes       = slen;
                    break;
                }
                case TC_LONGSTRING:
                {
                    uint32_t slen = 0;
                    if ((res = read_simple(&slen)) != STATUS_OK)
                        return STATUS_CORRUPTED;
                    bytes       = slen;
                    break;
                }
                default:
                    return STATUS_BAD_TYPE;
            }

            size_t href     = (nHandle++) - JAVA_BASE_WIRE_HANDLE;

            // Need just skip?
            if (dst == NULL)
            {
                wssize_t skipped = pIS->skip(bytes);
                if (skipped < 0)
                    return status_t(-skipped);
                if (handle != NULL)
                    *handle     = href;

                return (skipped == wssize_t(bytes)) ? STATUS_OK : STATUS_CORRUPTED;
            }

            // Allocate temporary buffer for string data
            char *buf = reinterpret_cast<char *>(::malloc(bytes));
            if (buf == NULL)
                return STATUS_NO_MEM;

            res = pIS->read_block(buf, bytes);
            if (res == STATUS_OK)
            {
                // Prepare string for read
                LSPString str;
                if (str.set_utf8(buf, bytes))
                {
                    dst->swap(&str);
                    if (handle != NULL)
                        *handle     = href;
                }
                else
                    res = STATUS_CORRUPTED;
            }

            ::free(buf);
            return res;
        }

        status_t StreamDataReader::read_block(void **dst, size_t *size)
        {
            status_t res = lookup_token();
            if (res != STATUS_OK)
                return res;

            // Fetch size of data
            size_t bytes;
            switch (nToken)
            {
                case TC_BLOCKDATA:
                {
                    uint8_t xb = 0;
                    if ((res = read_simple(&xb)) != STATUS_OK)
                        return STATUS_CORRUPTED;
                    bytes       = xb;
                    break;
                }
                case TC_BLOCKDATALONG:
                {
                    uint32_t xb = 0;
                    if ((res = read_simple(&xb)) != STATUS_OK)
                        return STATUS_CORRUPTED;
                    bytes       = xb;
                    break;
                }
                default:
                    return STATUS_BAD_TYPE;
            }

            // Need to skip the block?
            if (dst == NULL)
            {
                wssize_t skipped = pIS->skip(bytes);
                if (skipped < 0)
                    return -skipped;
                else if (skipped != wssize_t(bytes))
                    return STATUS_CORRUPTED;
                if (size != NULL)
                    *size   = bytes;
                return STATUS_OK;
            }

            // Allocate temporary buffer for binary data
            char *buf = reinterpret_cast<char *>(::malloc(bytes));
            if (buf == NULL)
                return STATUS_NO_MEM;

            res = pIS->read_block(buf, bytes);
            if (res == STATUS_OK)
            {
                *dst    = buf;
                if (size != NULL)
                    *size   = bytes;
            }

            return res;
        }

        status_t StreamDataReader::read_reset()
        {
            status_t res = lookup_token();
            if (res != STATUS_OK)
                return res;

            if (nToken != TC_RESET)
                return STATUS_BAD_TYPE;

            nToken      = -STATUS_UNSPECIFIED;
            enToken     = -STATUS_UNSPECIFIED;
            nHandle     = JAVA_BASE_WIRE_HANDLE;

            return STATUS_OK;
        }

        status_t StreamDataReader::read_null()
        {
            status_t res = lookup_token();
            if (res != STATUS_OK)
                return res;

            if (nToken != TC_NULL)
                return STATUS_BAD_TYPE;

            nToken  = -STATUS_UNSPECIFIED;
            enToken = -STATUS_UNSPECIFIED;

            return STATUS_OK;
        }
    } /* namespace java */
} /* namespace lsp */
