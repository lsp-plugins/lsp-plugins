/*
 * ObjectStream.cpp
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/stdlib/string.h>
#include <core/io/InFileStream.h>
#include <core/io/InMemoryStream.h>

#include <core/files/java/defs.h>
#include <core/files/java/wrappers.h>
#include <core/files/java/ObjectStream.h>

#include <dsp/endian.h>

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

            for (size_t i=0; i<JFT_TOTAL; ++i)
                vTypeStrings[i]     = NULL;
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

            for (size_t i=0; i<JFT_TOTAL; ++i)
            {
                if (vTypeStrings[i] != NULL)
                {
                    delete vTypeStrings[i];
                    vTypeStrings[i] = NULL;
                }
            }

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
                return ((res >= 0) || (res == STATUS_EOF)) ? STATUS_BAD_FORMAT : res;
            if (BE_TO_CPU(hdr.magic) != JAVA_STREAM_MAGIC)
                return STATUS_BAD_FORMAT;
            uint8_t *block  = reinterpret_cast<uint8_t *>(::malloc(JAVA_MAX_BLOCK_SIZE));
            if (block == NULL)
                return STATUS_NO_MEM;

            nVersion    = BE_TO_CPU(hdr.version);
            clear_token();
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
                        res     = parse_reset();
                        if (res != STATUS_OK)
                            return res;
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
                JDEC(TC_CLASSDESC, JST_CLASS_DESC)
                JDEC(TC_OBJECT, JST_OBJECT)
                JDEC(TC_STRING, JST_STRING)
                JDEC(TC_ARRAY, JST_ARRAY)
                JDEC(TC_CLASS, JST_CLASS)
                JDEC(TC_BLOCKDATA, JST_BLOCK_DATA)
                JDEC(TC_ENDBLOCKDATA, JST_END_BLOCK_DATA)
                JDEC(TC_RESET, JST_RESET)
                JDEC(TC_BLOCKDATALONG, JST_BLOCK_DATA)
                JDEC(TC_EXCEPTION, JST_EXCEPTION)
                JDEC(TC_LONGSTRING, JST_STRING)
                JDEC(TC_PROXYCLASSDESC, JST_PROXY_CLASS_DESC)
                JDEC(TC_ENUM, JST_ENUM)
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

        inline void ObjectStream::clear_token()
        {
            nToken      = -STATUS_UNSPECIFIED;
            enToken     = JST_UNDEFINED;
        }

        status_t ObjectStream::lookup_token()
        {
            // Check state
            if (pIS == NULL)
                return -STATUS_CLOSED;

            // Return valid token immediately if it is
            if (enToken != JST_UNDEFINED)
                return nToken;

            // Protect from reading token from inside of the TC_BLOCKDATA
            if (sBlock.enabled)
            {
                if ((sBlock.unread > 0) || (sBlock.offset < sBlock.size))
                    return -STATUS_BAD_STATE;
            }

            // Loop and process TC_RESET tokens
            while (true)
            {
                // Get token
                status_t res = get_token();
                if ((res < 0) || (res != TC_RESET))
                    return res;

                // Handle TC_RESET
                if (nDepth > 0)
                    return -STATUS_CORRUPTED;

                pHandles->clear();
                clear_token();
            }
        }

#define STREAM_READ_IMPL(name, type_t) \
        status_t ObjectStream::read_ ## name(type_t *dst) \
        { \
            type_t tmp; \
            status_t res =  read_fully(&tmp, sizeof(tmp)); \
            if ((res == STATUS_OK) && (dst != NULL)) \
                *dst    = BE_TO_CPU(tmp); \
            clear_token(); \
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
        STREAM_READ_IMPL(float, float_t)
        STREAM_READ_IMPL(double, double_t)
        STREAM_READ_IMPL(char, lsp_utf16_t)
        STREAM_READ_IMPL(bool, bool_t)

#undef STREAM_READ_IMPL

#define STREAM_READS_IMPL(name, type_t) \
        status_t ObjectStream::read_ ## name(type_t *dst, size_t count) \
        { \
            if (dst == NULL) return STATUS_BAD_ARGUMENTS; \
            status_t res =  read_fully(dst, sizeof(type_t) * count); \
            __IF_LE(if (res == STATUS_OK) byte_swap(dst, count)); \
            clear_token(); \
            return res; \
        }

        STREAM_READS_IMPL(bytes, uint8_t)
        STREAM_READS_IMPL(bytes, int8_t)
        STREAM_READS_IMPL(shorts, uint16_t)
        STREAM_READS_IMPL(shorts, int16_t)
        STREAM_READS_IMPL(ints, uint32_t)
        STREAM_READS_IMPL(ints, int32_t)
        STREAM_READS_IMPL(longs, uint64_t)
        STREAM_READS_IMPL(longs, int64_t)
        STREAM_READS_IMPL(floats, float_t)
        STREAM_READS_IMPL(doubles, double_t)
        STREAM_READS_IMPL(chars, lsp_utf16_t)
        STREAM_READS_IMPL(bools, bool_t)

#undef STREAM_READS_IMPL

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

        status_t ObjectStream::start_object(bool &mode)
        {
            status_t res = set_block_mode(false, &mode);
            if (res == STATUS_OK)
                ++nDepth;
            return res;
        }

        status_t ObjectStream::end_object(bool &mode, status_t res)
        {
            --nDepth;
            set_block_mode(mode, NULL);
            return res;
        }

        status_t ObjectStream::intern_type_string(String **dst, ftype_t type, char ptype)
        {
            if ((type < 0) || (type >= JFT_TOTAL))
                return STATUS_CORRUPTED;

            String *p = vTypeStrings[type];
            if (p == NULL)
            {
                p = new String();
                if (p == NULL)
                    return STATUS_NO_MEM;
                if (!p->string()->set(ptype))
                {
                    delete p;
                    return STATUS_NO_MEM;
                }
                vTypeStrings[type] = p;
            }

            if (dst != NULL)
                *dst = p;
            return STATUS_OK;
        }

        status_t ObjectStream::parse_reference(Object **dst, const char *type)
        {
            // Analyze token
            status_t res = lookup_token();
            if (res != TC_REFERENCE)
                return (res >= 0) ? STATUS_BAD_TYPE : -res;
            clear_token();

            // Read handle number and validate
            uint32_t handle = 0;
            res = read_int(&handle);
            if (res != STATUS_OK)
                return res;
            else if (handle < JAVA_BASE_WIRE_HANDLE)
                return STATUS_CORRUPTED;

            // Dereference handle
            Object *obj = pHandles->get(handle - JAVA_BASE_WIRE_HANDLE);
            if (obj == NULL)
                return STATUS_CORRUPTED;
            if ((type != NULL) && (!obj->instanceof(type)))
                return STATUS_BAD_TYPE;

            // Store handle
            if (dst != NULL)
                *dst    = obj;

            return STATUS_OK;
        }

        status_t ObjectStream::parse_utf(LSPString *dst, size_t bytes)
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
            return (res == STATUS_OK) ? parse_utf(dst, bytes) : STATUS_CORRUPTED;
        }

        status_t ObjectStream::parse_string(String **dst)
        {
            status_t res = lookup_token();
            if (res < 0)
                return -res;

            // Fetch size of string
            size_t bytes;
            switch (res)
            {
                case TC_STRING:
                {
                    uint16_t slen = 0;
                    if ((res = read_short(&slen)) != STATUS_OK) // This will clear token
                        return STATUS_CORRUPTED;
                    bytes       = slen;
                    break;
                }
                case TC_LONGSTRING:
                {
                    uint32_t slen = 0;
                    if ((res = read_int(&slen)) != STATUS_OK) // This will clear token
                        return STATUS_CORRUPTED;
                    bytes       = slen;
                    break;
                }
                default:
                    return STATUS_CORRUPTED;
            }

            // Allocate handle reference
            String *str = new String();
            if (str == NULL)
                return STATUS_NO_MEM;

            // Read string contents
            res = parse_utf(str->string(), bytes);
            if (res == STATUS_OK)
                pHandles->assign(str);

            // Need to return string?
            if (dst != NULL)
                *dst    = str;

            return res;
        }

        status_t ObjectStream::parse_reset()
        {
            if (nDepth > 0)
                return STATUS_CORRUPTED;

            pHandles->clear();
            clear_token();
            return STATUS_OK;
        }

        status_t ObjectStream::parse_null(Object **dst)
        {
            status_t token = lookup_token();
            if (token != TC_NULL)
                return (token >= 0) ? STATUS_CORRUPTED : -token;
            clear_token();

            if (dst != NULL)
                *dst        = NULL;

            return STATUS_OK;
        }

        status_t ObjectStream::parse_class_field(ObjectStreamField **dst)
        {
            // Allocate object
            ObjectStreamField *f = new ObjectStreamField();
            if (f == NULL)
                return STATUS_NO_MEM;

            status_t res;
            uint8_t tcode = 0;

            // Type code
            res = read_byte(&tcode);
            if (res == STATUS_OK)
            {
                f->enType   = decode_primitive_type(tcode);
                if (f->enType == JFT_UNKNOWN)
                    res         = STATUS_CORRUPTED;
            }

            // Name
            if (res == STATUS_OK)
            {
                res     = read_utf(&f->sName);
                if (res == STATUS_OK)
                {
                    f->sRawName     = f->sName.clone_utf8();
                    if (f->sRawName == NULL)
                        res = STATUS_NO_MEM;
                }
            }

            // Signature
            if (res == STATUS_OK)
            {
                res = (f->is_reference()) ?
                    read_string(&f->pSignature) :
                    intern_type_string(&f->pSignature, f->enType, tcode);
            }

            // Return result
            if ((res == STATUS_OK) && (dst != NULL))
                *dst    = f;

            return res;
        }

        status_t ObjectStream::skip_block_data()
        {
            if (!sBlock.enabled)
                return STATUS_OK;

            do
            {
                sBlock.offset   = sBlock.size;
                status_t res    = fill_block();
                if (res != STATUS_OK)
                    return res;
            } while (sBlock.unread > 0);

            return STATUS_OK;
        }

        status_t ObjectStream::skip_custom_data()
        {
            status_t res;

            while (true)
            {
                // Complete data block
                if (sBlock.enabled)
                {
                    res = skip_block_data();
                    if (res == STATUS_OK)
                        res = set_block_mode(false, NULL);
                    if (res != STATUS_OK)
                        return res;
                }

                // Read token
                res     = lookup_token();
                if (res < 0)
                    return -res;

                // Analyze token
                switch (res)
                {
                    case TC_BLOCKDATA:
                    case TC_BLOCKDATALONG:
                        res = set_block_mode(true, NULL);
                        if (res == STATUS_OK)
                            res = fill_block();
                        break;

                    case TC_ENDBLOCKDATA:
                        clear_token();
                        return STATUS_OK;

                    default:
                        res = read_object(NULL);
                        break;
                }

                // Analyze result
                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t ObjectStream::read_custom_data(void **dst, size_t *size)
        {
            status_t res = STATUS_OK;
            size_t capacity = 0;
            uint8_t *ptr = NULL;
            bool endblockdata = false;

            do
            {
                // Complete data block
                if (sBlock.enabled)
                {
                    // Read the block
                    size_t extra = sBlock.unread + sBlock.size - sBlock.offset;
                    uint8_t *xptr = reinterpret_cast<uint8_t *>(::realloc(ptr, capacity + extra));
                    if (xptr == NULL)
                        res     = STATUS_NO_MEM;
                    if (res == STATUS_OK)
                    {
                        // Copy data from block
                        size_t gap      = sBlock.size - sBlock.offset;
                        if (gap > 0)
                        {
                            ::memcpy(&xptr[capacity], &sBlock.data[sBlock.size], gap);
                            capacity       += gap;
                            sBlock.size     = sBlock.offset;
                        }

                        // Read the left data
                        if (sBlock.unread > 0)
                        {
                            res             = pIS->read_fully(&xptr[capacity], sBlock.unread);
                            if (res == ssize_t(sBlock.unread))
                            {
                                capacity       += res;
                                sBlock.unread   = 0;
                            }
                            else
                                res = STATUS_CORRUPTED;
                        }
                    }
                    if (res == STATUS_OK)
                        res = set_block_mode(false, NULL);
                }
                if (res != STATUS_OK)
                    break;

                // Read token
                if ((res = lookup_token()) < 0)
                {
                    res     = -res;
                    break;
                }

                // Analyze token
                switch (res)
                {
                    case TC_BLOCKDATA:
                    {
                        uint8_t blen;
                        res             = pIS->read_fully(&blen, sizeof(blen));
                        if (res != sizeof(blen))
                            res = (res < 0) ? -res : STATUS_CORRUPTED;
                        sBlock.enabled  = true;
                        sBlock.offset   = 0;
                        sBlock.size     = 0;
                        sBlock.unread   = blen;
                        res             = STATUS_OK;
                        break;
                    }

                    case TC_BLOCKDATALONG:
                    {
                        int32_t blen;
                        res             = pIS->read_fully(&blen, sizeof(blen));
                        if (res != sizeof(blen))
                            res = (res < 0) ? -res : STATUS_CORRUPTED;
                        sBlock.enabled  = true;
                        sBlock.offset   = 0;
                        sBlock.size     = 0;
                        sBlock.unread   = blen;
                        res             = STATUS_OK;
                        break;
                    }
                    case TC_ENDBLOCKDATA:
                        clear_token();
                        endblockdata    = true;
                        break;

                    default:
                        res             = STATUS_CORRUPTED;
                        break;
                }

                // Analyze result
                if (res != STATUS_OK)
                    break;
            } while (!endblockdata);

            // Return result
            if (res == STATUS_OK)
            {
                if (dst != NULL)
                    *dst    = ptr;
                else
                    ::free(ptr);
                if (size != NULL)
                    *size   = capacity;
            }
            else if (ptr != NULL)
                ::free(ptr);

            return res;
        }

        status_t ObjectStream::parse_class_descriptor(ObjectStreamClass **dst)
        {
            status_t res = lookup_token();
            if (res != TC_CLASSDESC)
                return (res >= 0) ? STATUS_CORRUPTED : -res;

            ObjectStreamClass *desc = new ObjectStreamClass();
            if (desc == NULL)
                return STATUS_CORRUPTED;

            // Class name, will do reset_token()
            res = read_utf(&desc->sName);
            if (res == STATUS_OK)
            {
                desc->pRawName = desc->sName.clone_utf8();
                res = ((desc->pRawName) != NULL) ? STATUS_OK : STATUS_NO_MEM;
            }
//            lsp_trace("Class name: %s", desc->sName.get_native());

            // Serial version UID
            if (res == STATUS_OK)
            {
                res = read_long(&desc->nSuid);
//                lsp_trace("Class suid: %lld", (long long)(desc->nSuid));
            }

            // Register handle
            if (res == STATUS_OK)
                pHandles->assign(desc);

            // Read & decode flags
            uint8_t flags = 0;
            if (res == STATUS_OK)
                res     = read_byte(&flags);
            if (res == STATUS_OK)
            {
//                lsp_trace("Class flags: 0x%x", int(flags));

                // Validate flags
                if ((flags & (SC_SERIALIZABLE | SC_EXTERNALIZABLE)) == (SC_SERIALIZABLE | SC_EXTERNALIZABLE))
                    res = STATUS_CORRUPTED;
                else if ((flags & SC_ENUM) && (desc->nSuid != 0))
                    res = STATUS_CORRUPTED;
            }
            if (res == STATUS_OK)
            {
                // Decode flags
                if (flags & SC_WRITE_METHOD)
                    desc->nFlags    |= JCF_WRITE_METHOD;
                if (flags & SC_BLOCK_DATA)
                    desc->nFlags    |= JCF_BLOCK_DATA;
                if (flags & SC_EXTERNALIZABLE)
                    desc->nFlags    |= JCF_EXTERNALIZABLE;
                if (flags & SC_SERIALIZABLE)
                    desc->nFlags    |= JCF_SERIALIZABLE;
                if (flags & SC_ENUM)
                    desc->nFlags    |= JCF_ENUM;
            }

            // Read fields
            uint16_t fields = 0;
            if (res == STATUS_OK)
                res     = read_short(&fields);
            if ((res == STATUS_OK) && (fields > 0))
            {
                desc->vFields = reinterpret_cast<ObjectStreamField **>(::malloc(fields * sizeof(ObjectStreamField *)));
                if (desc->vFields != NULL)
                {
                    // Clear the list
                    ObjectStreamField **fl = desc->vFields;
                    for (size_t i=0; i<fields; ++i)
                        fl[i]    = NULL;
                    desc->nFields = fields;

                    // Read, parse and validate each field
                    size_t base           = 0;
                    size_t num_obj_fields = 0;
                    ssize_t first_obj_idx = -1;

                    for (size_t i=0; i<fields; ++i)
                    {
                        ObjectStreamField *f = NULL;
                        if ((res = parse_class_field(&f)) != STATUS_OK)
                            break;
//                        lsp_trace("Class Field: %s, signature: %s, size=%d, offset=0x%x",
//                                f->name()->get_native(), f->signature()->get_native(),
//                                int(f->size_of()), int(prim_data_size));

                        // Determine field location
                        desc->vFields[i]    = f;
                        if (f->is_reference())
                        {
                            ++num_obj_fields;
                            if (first_obj_idx < 0)
                                first_obj_idx   = i;
                        }

                        // Store the offset of the field
                        f->nOffset      = f->aligned_offset(base);
                        base            = f->nOffset + f->size_of();
                    }

                    // Validate the final state
                    if ((first_obj_idx >= 0) && ((first_obj_idx + num_obj_fields) != fields))
                        res     = STATUS_CORRUPTED;

                    // Update object statistics
                    desc->nSizeOf   = base;
                }
                else
                    res     = STATUS_NO_MEM;
            }

            // Skip class annotations
            if (res == STATUS_OK)
                res     = skip_custom_data();

            // Read parent class
            if (res == STATUS_OK)
            {
                desc->pParent   = NULL;
                res             = read_class_descriptor(&desc->pParent);
            }

            // Generate slots
            if (res == STATUS_OK)
            {
                // Store slots in reverse order to the class hierarchy
                size_t slots = 0;
                for (ObjectStreamClass *curr = desc; curr != NULL; curr = curr->pParent)
                    ++slots;
                desc->vSlots    = reinterpret_cast<ObjectStreamClass **>(::malloc(slots * sizeof(ObjectStreamClass *)));
                desc->nSlots    = slots;
                if (desc->vSlots != NULL)
                {
                    for (ObjectStreamClass *curr = desc; curr != NULL; curr = curr->pParent)
                        desc->vSlots[--slots]   = curr;
                }
            }

            // Analyze result
            if ((res == STATUS_OK) && (dst != NULL))
                *dst    = desc;

            return res;
        }

        status_t ObjectStream::parse_proxy_class_descriptor(ObjectStreamClass **dst)
        {
            //TODO
            return STATUS_NOT_SUPPORTED;
        }

        status_t ObjectStream::parse_array(RawArray **dst)
        {
            // Fetch token
            ssize_t token   = lookup_token();
            if (token != TC_ARRAY)
                return (token >= 0) ? STATUS_CORRUPTED : -token;
            clear_token();

            // Read class descriptor
            ObjectStreamClass *desc = NULL;
            status_t res    = read_class_descriptor(&desc);
            if (res != STATUS_OK)
                return res;

            // Create array object
            RawArray *arr   = new RawArray(desc->raw_name());
            if (arr == NULL)
                return STATUS_NO_MEM;

            // Register array and allocate data
            res = pHandles->assign(arr);

            // Read length
            uint32_t len = 0;
            if (res == STATUS_OK)
                res     = read_int(&len);

            // Create array instance
            if (res == STATUS_OK)
                res     = arr->allocate(len);

            // Read elements
            if (res == STATUS_OK)
            {
                switch (arr->item_type())
                {
                    case JFT_BYTE:      res = read_bytes(arr->get<uint8_t>(), len); break;
                    case JFT_CHAR:      res = read_chars(arr->get<lsp_utf16_t>(), len); break;
                    case JFT_DOUBLE:    res = read_doubles(arr->get<double_t>(), len); break;
                    case JFT_FLOAT:     res = read_floats(arr->get<float_t>(), len); break;
                    case JFT_INTEGER:   res = read_ints(arr->get<uint32_t>(), len); break;
                    case JFT_LONG:      res = read_longs(arr->get<uint64_t>(), len); break;
                    case JFT_SHORT:     res = read_shorts(arr->get<uint16_t>(), len); break;
                    case JFT_BOOL:      res = read_bools(arr->get<bool_t>(), len); break;
                    case JFT_ARRAY:
                    case JFT_OBJECT:
                    {
                        Object **data       = arr->get<Object *>();
                        for (size_t i=0; i<len; ++i)
                            if ((res = read_object(&data[i])) != STATUS_OK)
                                break;
                        break;
                    }
                    default:            res = STATUS_BAD_TYPE;
                }
            }

            // Store result
            if ((res == STATUS_OK) && (dst != NULL))
                *dst    = arr;

            return res;
        }

        status_t ObjectStream::parse_enum(Enum **dst)
        {
            // Fetch token
            ssize_t token   = lookup_token();
            if (token != TC_ENUM)
                return (token >= 0) ? STATUS_CORRUPTED : -token;
            clear_token();

            // Read class descriptor
            ObjectStreamClass *desc = NULL;
            status_t res    = read_class_descriptor(&desc);
            if (res != STATUS_OK)
                return res;

            // Create enum object
            Enum *en        = new Enum();
            if (en == NULL)
                return STATUS_NO_MEM;
            en->pClass      = desc->pRawName;

            // Register enum and allocate data
            res = pHandles->assign(en);

            // Read enum constant
            if (res == STATUS_OK)
            {
                String *pstr = NULL;
                res = read_string(&pstr);
                if (res == STATUS_OK)
                    res = (en->sName.set(pstr->string())) ? STATUS_OK : STATUS_NO_MEM;
            }

            // Store result
            if ((res == STATUS_OK) && (dst != NULL))
                *dst    = en;

            return res;
        }

        status_t ObjectStream::parse_serial_data(Object *dst, ObjectStreamClass *desc)
        {
            // Initialize slots
            dst->vSlots     = reinterpret_cast<object_slot_t *>(::malloc(desc->nSlots * sizeof(object_slot_t)));
            if (dst->vSlots == NULL)
                return STATUS_NO_MEM;
            dst->nSlots     = desc->nSlots;

            // Estimate number of initial data for allocation
            size_t allocated = 0, offset = 0;
            for (size_t i=0, n=desc->nSlots; i<n; ++i)
            {
                ObjectStreamClass *cl = desc->vSlots[i];
//                if (cl->is_serializable()) // Skip serializable objects
//                    continue;
                allocated      += ALIGN_SIZE(cl->nSizeOf, MINIMUM_ALIGN);
            }

            // Allocate memory
            dst->vData      = reinterpret_cast<uint8_t *>(::malloc(allocated));
            if (dst->vData == NULL)
                return STATUS_NO_MEM;
            ::bzero(dst->vData, allocated);

            // Perform read of the object
            status_t res = STATUS_OK;
            for (size_t i=0, n=desc->nSlots; i<n; ++i)
            {
                ObjectStreamClass *cl   = desc->vSlots[i];
                object_slot_t *sl       = &dst->vSlots[i];

                sl->desc            = cl;
                sl->offset          = offset;
                sl->size            = 0;
                sl->__pad           = 0;

                if (cl->has_write_method())
                {
                    void *tmp = NULL;
                    size_t tail;

                    // Read object's serial data from underlying stream
                    res = read_custom_data(&tmp, &tail);
                    if (res != STATUS_OK)
                        break;
                    else if (tail <= 0)
                        continue;

                    // Align the object's data to the boundary and re-allocate segment
                    size_t space    = ALIGN_SIZE(tail, MINIMUM_ALIGN);
                    sl->size        = tail;
                    allocated      += space;
                    uint8_t *xdata  = reinterpret_cast<uint8_t *>(::realloc(dst->vData, space));
                    if (xdata == NULL)
                    {
                        res = STATUS_NO_MEM;
                        break;
                    }

                    // Copy data to the class structure
                    ::memcpy(&xdata[offset], tmp, tail);
                    ::free(tmp);
                    offset         += space;
                }
                else
                {
                    // Read serial data
                    prim_ptr_t xdata;
                    size_t space    = ALIGN_SIZE(cl->nSizeOf, MINIMUM_ALIGN);
                    sl->size        = cl->nSizeOf;

                    // Operate each field
                    for (size_t j=0, m=cl->nFields; j<m; ++j)
                    {
                        ObjectStreamField *f    = cl->vFields[j];
                        xdata.p_ubyte           = &dst->vData[f->offset()];
//                        lsp_trace("  reading field: %s", f->sName.get_native());
                        switch (f->type())
                        {
                            case JFT_BYTE:      res = read_byte(xdata.p_ubyte); break;
                            case JFT_CHAR:      res = read_char(xdata.p_char); break;
                            case JFT_DOUBLE:    res = read_double(xdata.p_double); break;
                            case JFT_FLOAT:     res = read_float(xdata.p_float); break;
                            case JFT_INTEGER:   res = read_int(xdata.p_uint); break;
                            case JFT_LONG:      res = read_long(xdata.p_ulong); break;
                            case JFT_SHORT:     res = read_short(xdata.p_ushort); break;
                            case JFT_BOOL:      res = read_bool(xdata.p_bool); break;
                            case JFT_ARRAY:
                            case JFT_OBJECT:
                            {
                                Object *obj = NULL;
                                res     = read_object(&obj);
                                if (res == STATUS_OK)
                                    *xdata.p_object     = obj;
                                break;
                            }
                            default:
                                res     = STATUS_CORRUPTED;
                                break;
                        }

                        if (res != STATUS_OK)
                            break;
                    }

                    offset         += space;
                }

                if (res != STATUS_OK)
                    break;
            }

            return res;
        }

        status_t ObjectStream::parse_external_data(Object *dst, ObjectStreamClass *desc)
        {
            // TODO
            return STATUS_NOT_IMPLEMENTED;
        }

        Object *ObjectStream::build_object(ObjectStreamClass *desc)
        {
            const char *raw_name = desc->raw_name();

            if (!strcmp(raw_name, Byte::CLASS_NAME))
                return new Byte();
            if (!strcmp(raw_name, Short::CLASS_NAME))
                return new Short();
            if (!strcmp(raw_name, Integer::CLASS_NAME))
                return new Integer();
            if (!strcmp(raw_name, Long::CLASS_NAME))
                return new Long();
            if (!strcmp(raw_name, Double::CLASS_NAME))
                return new Double();
            if (!strcmp(raw_name, Float::CLASS_NAME))
                return new Float();
            if (!strcmp(raw_name, Boolean::CLASS_NAME))
                return new Boolean();
            if (!strcmp(raw_name, Character::CLASS_NAME))
                return new Character();

            return new Object(desc->raw_name());
        }

        status_t ObjectStream::parse_ordinary_object(Object **dst)
        {
            // Fetch token
            ssize_t token   = lookup_token();
            if (token != TC_OBJECT)
                return (token >= 0) ? STATUS_CORRUPTED : -token;
            clear_token();

            // Read class descriptor
            ObjectStreamClass *desc = NULL;
            status_t res    = read_class_descriptor(&desc);
            if (res != STATUS_OK)
                return res;

            // Create object
            Object *obj     = build_object(desc);
            if (obj == NULL)
                return STATUS_NO_MEM;

            // Register object and allocate data
            res = pHandles->assign(obj);

            if (res == STATUS_OK)
            {
                res = (desc->is_externalizable()) ?
                        parse_external_data(obj, desc) :
                        parse_serial_data(obj, desc);
            }

            // Store result
            if ((res == STATUS_OK) && (dst != NULL))
                *dst    = obj;

            return res;
        }


        status_t ObjectStream::parse_object(Object **dst)
        {
            // Fetch token
            ssize_t token = lookup_token();
            if (token < 0)
                return token;

            // Start object mode
            bool mode = false;
            status_t res = start_object(mode);
            if (res != STATUS_OK)
                return res;

            obj_ptr_t ret(dst);
            switch (token)
            {
                case TC_NULL: // Null reference, valid to be string
                    return end_object(mode, parse_null(ret));

                case TC_REFERENCE: // Some reference to object, required to be string
                    return end_object(mode, parse_reference(ret, NULL));

                case TC_STRING: // String with 16-bit length
                case TC_LONGSTRING: // String with 32-bit length
                    return end_object(mode, parse_string(ret));

                case TC_ARRAY: // Array
                    return end_object(mode, parse_array(ret));

                case TC_OBJECT: // Object
                    return end_object(mode, parse_ordinary_object(ret));

                case TC_ENUM: // Enumeration
                    return end_object(mode, parse_enum(ret));

                case TC_CLASSDESC:
                case TC_PROXYCLASSDESC:
                    return end_object(mode, parse_class_descriptor(ret));

                default:
                    break;
            }

            return end_object(mode, STATUS_BAD_STATE);
        }

        status_t ObjectStream::read_object(Object **dst)
        {
            return parse_object(dst);
        }

        status_t ObjectStream::read_string(String **dst)
        {
            // Fetch token
            ssize_t token = lookup_token();
            if (token < 0)
                return token;

            // Start object mode
            bool mode = false;
            status_t res = start_object(mode);
            if (res != STATUS_OK)
                return res;

            obj_ptr_t ret(dst);
            switch (token)
            {
                case TC_NULL: // Null reference, valid to be string
                    return end_object(mode, parse_null(ret));

                case TC_REFERENCE: // Some reference to object, required to be string
                    return end_object(mode, parse_reference(ret, String::CLASS_NAME));

                case TC_STRING:
                case TC_LONGSTRING:
                    return end_object(mode, parse_string(ret));

                default:
                    break;
            }

            return end_object(mode, STATUS_BAD_STATE);
        }

        status_t ObjectStream::read_string(LSPString *dst)
        {
            String *str = NULL;
            status_t res = read_string(&str);
            if (res != STATUS_OK)
                return res;
            else if (str == NULL)
                return STATUS_NULL;

            if (dst != NULL)
            {
                if (!dst->set(str->string()))
                    return STATUS_NO_MEM;
            }
            return STATUS_OK;
        }

        status_t ObjectStream::read_array(RawArray **dst)
        {
            // Fetch token
            ssize_t token = lookup_token();
            if (token < 0)
                return token;

            // Start object mode
            bool mode = false;
            status_t res = start_object(mode);
            if (res != STATUS_OK)
                return res;

            obj_ptr_t ret(dst);
            switch (token)
            {
                case TC_NULL: // Null reference, valid to be string
                    return end_object(mode, parse_null(ret));

                case TC_REFERENCE: // Some reference to object, required to be string
                    return end_object(mode, parse_reference(ret, RawArray::CLASS_NAME));

                case TC_ARRAY:
                    return end_object(mode, parse_array(ret));

                default:
                    break;
            }

            return end_object(mode, STATUS_BAD_STATE);
        }

        status_t ObjectStream::read_enum(Enum **dst)
        {
            // Fetch token
            ssize_t token = lookup_token();
            if (token < 0)
                return token;

            // Start object mode
            bool mode = false;
            status_t res = start_object(mode);
            if (res != STATUS_OK)
                return res;

            obj_ptr_t ret(dst);
            switch (token)
            {
                case TC_NULL: // Null reference, valid to be string
                    return end_object(mode, parse_null(ret));

                case TC_REFERENCE: // Some reference to object, required to be string
                    return end_object(mode, parse_reference(ret, Enum::CLASS_NAME));

                case TC_ENUM:
                    return end_object(mode, parse_enum(ret));

                default:
                    break;
            }

            return end_object(mode, STATUS_BAD_STATE);
        }

        status_t ObjectStream::read_class_descriptor(ObjectStreamClass **dst)
        {
            // Fetch token
            ssize_t token = lookup_token();
            if (token < 0)
                return token;

            // Start object mode
            bool mode = false;
            status_t res = start_object(mode);
            if (res != STATUS_OK)
                return res;

            obj_ptr_t ret(dst);
            switch (token)
            {
                case TC_NULL: // Null reference, valid to be string
                    return end_object(mode, parse_null(ret));

                case TC_REFERENCE: // Some reference to object, required to be string
                    return end_object(mode, parse_reference(ret, ObjectStreamClass::CLASS_NAME));

                case TC_CLASSDESC:
                    return end_object(mode, parse_class_descriptor(ret));

                case TC_PROXYCLASSDESC:
                    return end_object(mode, parse_proxy_class_descriptor(ret));

                default:
                    break;
            }

            return end_object(mode, STATUS_BAD_STATE);
        }

    } /* namespace java */
} /* namespace lsp */
