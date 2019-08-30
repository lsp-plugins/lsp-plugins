/*
 * ObjectStream.cpp
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#include <dsp/endian.h>
#include <core/stdlib/string.h>
#include <core/io/InFileStream.h>
#include <core/io/InMemoryStream.h>

#include <core/files/java/defs.h>
#include <core/files/java/ObjectStream.h>

namespace lsp
{
    namespace java
    {
        
        ObjectStream::ObjectStream(Handles *handles)
        {
            pIS             = NULL;
            nFlags          = 0;
            nToken          = -STATUS_CLOSED;
            enToken         = JST_UNDEFINED;
            nDepth          = 0;
            nVersion        = -STATUS_CLOSED;
            pHandles        = handles;

            sBlock.data     = NULL;
            sBlock.size     = 0;
            sBlock.offset   = 0;
            sBlock.unread   = 0;
            sBlock.enabled  = true;
        }
        
        ObjectStream::~ObjectStream()
        {
            do_close();
        }

        status_t ObjectStream::do_close()
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

            if (sBlock.data != NULL)
                ::free(sBlock.data);
            sBlock.data     = NULL;
            sBlock.size     = 0;
            sBlock.offset   = 0;
            sBlock.unread   = 0;
            sBlock.enabled  = true;

            nFlags          = 0;
            nToken          = -STATUS_UNSPECIFIED;
            enToken         = JST_UNDEFINED;
            nVersion        = -STATUS_UNSPECIFIED;

            return res;
        }

        status_t ObjectStream::open(const char *file)
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

        status_t ObjectStream::open(const LSPString *file)
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

        status_t ObjectStream::open(const io::Path *file)
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

        status_t ObjectStream::wrap(const void *buf, size_t count)
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

        status_t ObjectStream::wrap(void *buf, size_t count, lsp_memdrop_t drop)
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

        status_t ObjectStream::wrap(io::IInStream *is, size_t flags)
        {
            status_t res    = initial_read(is);
            if (res == STATUS_OK)
            {
                pIS     = is;
                nFlags  = flags;
            }
            return res;
        }

        status_t ObjectStream::close()
        {
            return do_close();
        }

        status_t ObjectStream::initial_read(io::IInStream *is)
        {
            // Read stream header
            obj_stream_hdr_t hdr;
            status_t res = is->read_fully(&hdr, sizeof(hdr));
            if (res != sizeof(hdr))
                return ((res >= 0) || (res == STATUS_EOF)) ? STATUS_CORRUPTED : res;
            if (BE_TO_CPU(hdr.magic) != JAVA_STREAM_MAGIC)
                return STATUS_CORRUPTED;
            uint8_t *block  = reinterpret_cast<uint8_t *>(::malloc(JAVA_MAX_BLOCK_SIZE));
            if (block == NULL)
                return STATUS_NO_MEM;

            nVersion    = BE_TO_CPU(hdr.version);
            nToken      = -STATUS_UNSPECIFIED;
            enToken     = JST_UNDEFINED;
            sBlock.data = block;

            return STATUS_OK;
        }

        status_t ObjectStream::fill_block()
        {
            while (sBlock.offset >= sBlock.size) // Is there data in a block
            {
                // Is there unread data in a block?
                if (sBlock.unread > 0)
                {
                    ssize_t amount  = (sBlock.unread <= JAVA_MAX_BLOCK_SIZE) ? sBlock.unread : JAVA_MAX_BLOCK_SIZE;
                    ssize_t read    = pIS->read_fully(sBlock.data, amount);
                    if (read != amount)
                        return (read >= 0) ? STATUS_CORRUPTED : -read;

                    sBlock.size     = read;
                    sBlock.offset   = 0;
                    sBlock.unread  -= amount;

                    return STATUS_OK;
                }

                // Fetch next token
                status_t res = lookup_token();
                if (res <= 0)
                    return res;

                switch (res)
                {
                    case TC_RESET: // Handle stream reset
                        nToken      = -STATUS_UNSPECIFIED;
                        enToken     = JST_UNDEFINED;
                        pHandles->clear();
                        break;

                    case TC_BLOCKDATA:
                    {
                        uint8_t blen;
                        res             = pIS->read_fully(&blen, sizeof(blen));
                        if (res != sizeof(blen))
                            return (res < 0) ? res : -STATUS_CORRUPTED;
                        sBlock.unread   = blen;
                        break;
                    }
                    case TC_BLOCKDATALONG:
                    {
                        int32_t blen;
                        res             = pIS->read_fully(&blen, sizeof(blen));
                        if (res != sizeof(blen))
                            return (res < 0) ? res : -STATUS_CORRUPTED;
                        sBlock.unread   = blen;
                        break;
                    }
                    default:
                        return STATUS_CORRUPTED;
                }
            }

            return STATUS_OK;
        }

        status_t ObjectStream::read_fully(void *dst, size_t count)
        {
            if (pIS == NULL)
                return STATUS_CLOSED;

            if (!sBlock.enabled)
            {
                ssize_t bytes = pIS->read_fully(dst, count);
                if (bytes < 0)
                    return -bytes;
                return (size_t(bytes) == count) ? STATUS_OK : STATUS_CORRUPTED;
            }

            // Read data from block
            uint8_t *buf = reinterpret_cast<uint8_t *>(dst);
            while (count > 0)
            {
                // Fetch block
                status_t res    = fill_block();
                if (res != STATUS_OK)
                    return res;

                // Copy data from block
                size_t avail = sBlock.size - sBlock.offset;
                if (avail > count)
                    avail       = count;
                ::memcpy(buf, &sBlock.data[sBlock.offset], avail);
                sBlock.offset  += avail;
                count          -= avail;
            }

            return STATUS_OK;
        }

        ssize_t ObjectStream::get_token()
        {
            // Read and parse token
            ssize_t token   = pIS->read_byte();
            switch (token)
            {
                #define JDEC(a, b) case a: nToken = a; enToken = b; break;
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
                    nToken      = (token < 0) ? token : -STATUS_CORRUPTED;
                    enToken     = JST_UNDEFINED;
                    break;
            }

            return nToken;
        }

        status_t ObjectStream::current_token()
        {
            if (pIS == NULL)
                return - STATUS_CLOSED;

            // Try to get token
            if (enToken == JST_UNDEFINED)
            {
                if (sBlock.enabled)
                {
                    if ((sBlock.unread > 0) || (sBlock.offset < sBlock.size))
                        return JST_BLOCK_DATA;
                }
                get_token();
            }

            // Return valid token or error code stored in nToken
            return (enToken != JST_UNDEFINED) ? enToken : nToken;
        }

        status_t ObjectStream::lookup_token()
        {
            if (pIS == NULL)
                return - STATUS_CLOSED;

            // Try to get token
            if (enToken == JST_UNDEFINED)
            {
                // Protect from reading token from inside of the TC_BLOCKDATA
                if (sBlock.enabled)
                {
                    if ((sBlock.unread > 0) || (sBlock.offset < sBlock.size))
                        return STATUS_BAD_STATE;
                }
                get_token();
            }

            // Return valid token or error code stored in nToken
            return nToken;
        }

#define STREAM_READ_IMPL(name, type_t) \
        status_t ObjectStream::read_ ## name(type_t *dst) \
        { \
            type_t tmp; \
            status_t res =  read_fully(&tmp, sizeof(tmp)); \
            if ((res == STATUS_OK) && (dst != NULL)) \
                *dst    = BE_TO_CPU(tmp); \
            nToken      = -STATUS_UNSPECIFIED; \
            enToken     = JST_UNDEFINED; \
            return res; \
        }

        STREAM_READ_IMPL(byte, uint8_t)
        STREAM_READ_IMPL(byte, int8_t)
        STREAM_READ_IMPL(short, uint16_t)
        STREAM_READ_IMPL(short, int16_t)
        STREAM_READ_IMPL(int, uint32_t)
        STREAM_READ_IMPL(int, int32_t)
        STREAM_READ_IMPL(long, uint64_t)
        STREAM_READ_IMPL(long, int64_t)
        STREAM_READ_IMPL(float, float)
        STREAM_READ_IMPL(double, double)

#undef STREAM_READ_IMPL

        status_t ObjectStream::set_block_mode(bool enabled, bool *old)
        {
            if (sBlock.enabled == enabled)
                return STATUS_OK;
            if (enabled)
            {
                sBlock.offset   = 0;
                sBlock.size     = 0;
                sBlock.unread   = 0;
            }
            else if ((sBlock.offset < sBlock.size) || (sBlock.unread > 0))
                return STATUS_BAD_STATE;

            if (old != NULL)
                *old            = sBlock.enabled;
            sBlock.enabled  = enabled;
            return STATUS_OK;
        }

        status_t ObjectStream::read_handle(Object **dst)
        {
            status_t res = lookup_token();
            if (res != STATUS_OK)
                return res;
            else if (nToken != TC_REFERENCE)
                return STATUS_BAD_TYPE;

            uint32_t handle = 0;
            res = read_int(&handle);
            if (res != STATUS_OK)
                return res;
            else if (handle < JAVA_BASE_WIRE_HANDLE)
                return STATUS_CORRUPTED;
            Object *obj = pHandles->get(handle - JAVA_BASE_WIRE_HANDLE);
            if (obj == NULL)
                return STATUS_CORRUPTED;

            if (dst != NULL)
                *dst    = obj;
            else
                obj->release();

            return STATUS_OK;
        }

        status_t ObjectStream::read_utf(LSPString *dst, size_t bytes)
        {
            // Allocate temporary buffer for string data
            char *buf = reinterpret_cast<char *>(::malloc(bytes));
            if (buf == NULL)
                return STATUS_NO_MEM;

            // Read string data
            status_t res = read_fully(buf, bytes);
            if (res != STATUS_OK)
            {
                ::free(buf);
                return res;
            }

            // Create string and return
            LSPString tmp;
            res = (tmp.set_utf8(buf, bytes)) ? STATUS_OK : STATUS_NO_MEM;
            ::free(buf);
            if ((res == STATUS_OK) && (dst != NULL))
                dst->swap(&tmp);
            return res;
        }

        status_t ObjectStream::read_utf(LSPString *dst)
        {
            // Read length
            uint16_t bytes  = 0;
            status_t res    = read_short(&bytes);
            return (res == STATUS_OK) ? read_utf(dst, bytes) : STATUS_CORRUPTED;
        }

        status_t ObjectStream::read_string_internal(String **dst)
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
                    if ((res = read_short(&slen)) != STATUS_OK)
                        return STATUS_CORRUPTED;
                    bytes       = slen;
                    break;
                }
                case TC_LONGSTRING:
                {
                    uint32_t slen = 0;
                    if ((res = read_int(&slen)) != STATUS_OK)
                        return STATUS_CORRUPTED;
                    bytes       = slen;
                    break;
                }
                default:
                    return STATUS_CORRUPTED;
            }

            // Allocate handle reference
            String *str = new String(pHandles->new_handle());
            if (str == NULL)
                return STATUS_NO_MEM;

            // Read string
            res = read_utf(str->string(), bytes);
            if (res == STATUS_OK)
                pHandles->put(str);

            str->release();
            return res;
        }

        status_t ObjectStream::handle_reset()
        {
            if (nDepth > 0)
                return STATUS_CORRUPTED;

            nToken      = -STATUS_UNSPECIFIED;
            enToken     = JST_UNDEFINED;
            pHandles->clear();
            return STATUS_OK;
        }

        status_t ObjectStream::read_null()
        {
            status_t res = lookup_token();
            if (res != STATUS_OK)
                return res;

            if (nToken != TC_NULL)
                return STATUS_BAD_TYPE;

            nToken  = -STATUS_UNSPECIFIED;
            enToken = JST_UNDEFINED;

            return STATUS_OK;
        }

        status_t ObjectStream::read_class_descriptor(ClassDescriptor **dst)
        {
            status_t res = lookup_token();
            if (res != STATUS_OK)
                return res;

            switch (nToken)
            {
                case TC_NULL:
                    if (dst != NULL)
                        *dst        = NULL;
                    nToken      = -STATUS_UNSPECIFIED;
                    enToken     = JST_UNDEFINED;
                    return STATUS_OK;
                case TC_REFERENCE:
                    // TODO
                    return STATUS_CORRUPTED;

            }

            return STATUS_OK;
        }

        status_t ObjectStream::read_string(String **dst)
        {
            size_t bytes;
            bool old_mode = false;

            status_t res = set_block_mode(false, &old_mode);
            if (res != STATUS_OK)
                return res;

            while (true)
            {
                ssize_t token = lookup_token();
                if (token < 0)
                    return -token;

                switch (token)
                {
                    case TC_RESET: // Stream reset
                    {
                        if ((res = handle_reset()) != STATUS_OK)
                            return res;
                        continue;
                    }
                    case TC_NULL: // Null reference, valid to be string
                    {
                        nToken      = -STATUS_UNSPECIFIED;
                        enToken     = JST_UNDEFINED;

                        if (dst != NULL)
                            *dst        = NULL;
                        set_block_mode(old_mode, NULL);
                        return STATUS_OK;
                    }
                    case TC_REFERENCE: // Some reference to object, required to be string
                    {
                        Object *obj     = NULL;
                        status_t res    = read_handle(&obj);
                        if (res != STATUS_OK)
                            return res;
                        else if (!obj->instanceof(CLASSNAME_STRING))
                            return STATUS_BAD_TYPE;

                        if (dst != NULL)
                            *dst    = static_cast<String *>(obj);
                        else
                            obj->release();
                        set_block_mode(old_mode, NULL);
                        return STATUS_OK;
                    }
                    case TC_STRING: // String with 16-bit length
                    {
                        uint16_t slen = 0;
                        if ((res = read_short(&slen)) != STATUS_OK)
                            return STATUS_CORRUPTED;
                        bytes       = slen;
                        break;
                    }
                    case TC_LONGSTRING: // String with 32-bit length
                    {
                        uint32_t slen = 0;
                        if ((res = read_int(&slen)) != STATUS_OK)
                            return STATUS_CORRUPTED;
                        bytes       = slen;
                        break;
                    }
                    default:
                        return STATUS_BAD_STATE;
                }

                // Allocate handle reference
                String *str = new String(pHandles->new_handle());
                if (str == NULL)
                    return STATUS_NO_MEM;

                // Read string
                res = read_utf(str->string(), bytes);
                if (res == STATUS_OK)
                    res = pHandles->put(str);

                if ((res == STATUS_OK) && (dst != NULL))
                {
                    str->acquire();
                    *dst        = str;
                }

                // Reset current token
                nToken      = -STATUS_UNSPECIFIED;
                enToken     = JST_UNDEFINED;

                set_block_mode(old_mode, NULL);
                str->release();
                return res;
            }
        }
    } /* namespace java */
} /* namespace lsp */
