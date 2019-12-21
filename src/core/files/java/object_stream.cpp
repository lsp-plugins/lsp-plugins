/*
 * object_stream.cpp
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#include <dsp/endian.h>
#include <core/stdlib/string.h>
#include <core/io/InFileStream.h>
#include <core/io/InMemoryStream.h>

#include <core/files/java/defs.h>
#include <core/files/java/object_stream.h>

namespace lsp
{
    namespace java
    {
        /*
        typedef struct context_t
        {
            io::IInStream          *is;
            IObjectStreamHandler   *handler;
            size_t                  version;
        };

        status_t    read_object(context_t *ctx, bool unshared);

        status_t    parse_object_stream(const char *file, IObjectStreamHandler *handler)
        {
            io::InFileStream is;
            status_t res = is.open(file);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = parse_object_stream(&is, handler);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }

            return is.close();
        }

        status_t    parse_object_stream(const LSPString *file, IObjectStreamHandler *handler)
        {
            io::InFileStream is;
            status_t res = is.open(file);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = parse_object_stream(&is, handler);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }

            return is.close();
        }

        status_t    parse_object_stream(const io::Path *file, IObjectStreamHandler *handler)
        {
            io::InFileStream is;
            status_t res = is.open(file);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = parse_object_stream(&is, handler);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }

            return is.close();
        }

        status_t    parse_object_stream(const void *buf, size_t count, IObjectStreamHandler *handler)
        {
            io::InMemoryStream is(buf, count);
            status_t res = parse_object_stream(&is, handler);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }

            return is.close();
        }

        inline ssize_t read_byte(context_t *ctx)
        {
            return ctx->is->read_byte();
        }

        inline status_t read_block(context_t *ctx, void *dst, size_t count)
        {
            return ctx->is->read_block(dst, count);
        }

        template <typename type_t>
            inline status_t read_primitive(context_t *ctx, type_t *dst)
            {
                type_t tmp;
                status_t res = ctx->is->read_block(&tmp, sizeof(tmp));
                if (res == STATUS_OK)
                    *dst    = BE_TO_CPU(tmp);
                return res;
            }

        status_t read_reference(context_t *ctx, bool unshared)
        {
            int32_t pass_handle;
            status_t res = ctx->is->read_block(&pass_handle, sizeof(pass_handle));
            if (res != STATUS_OK)
                return res;
            pass_handle = BE_TO_CPU(pass_handle) - JAVA_BASE_WIRE_HANDLE;
            if (pass_handle < 0)
                return STATUS_CORRUPTED;

            return ctx->handler->handle_reference(pass_handle);
        }

        status_t read_class(context_t *ctx, bool unshared)
        {
            return STATUS_OK; // TODO
        }

        status_t read_class_desc(context_t *ctx, bool unshared)
        {
            return STATUS_OK; // TODO
        }

        status_t read_java_utf8(context_t *ctx, wsize_t bytes, bool unshared)
        {
            // Allocate temporary buffer for string data
            char *buf = reinterpret_cast<char *>(::malloc(bytes));
            if (buf == NULL)
                return STATUS_NO_MEM;

            // Prepare string for read
            LSPString str;
            status_t res = (str.set_utf8(buf, bytes)) ? STATUS_OK : STATUS_NO_MEM;
            ::free(buf);

            return (res != STATUS_OK) ? res : ctx->handler->handle_string(&str, unshared);
        }

        status_t read_string(context_t *ctx, bool unshared)
        {
            uint16_t len;
            status_t res = read_primitive(ctx, &len);
            return (res != STATUS_OK) ? res : read_java_utf8(ctx, len, unshared);
        }

        status_t read_long_string(context_t *ctx, bool unshared)
        {
            uint64_t len;
            status_t res = read_primitive(ctx, &len);
            return (res != STATUS_OK) ? res : read_java_utf8(ctx, len, unshared);
        }

        status_t read_block_data(context_t *ctx)
        {
            ssize_t size    = read_byte(ctx);
            if (size < 0)
                return -size;
            uint8_t *buf    = reinterpret_cast<uint8_t *>(::malloc(size));
            if (buf == NULL)
                return STATUS_NO_MEM;
            status_t res    = read_block(ctx, buf, size);
            if (res == STATUS_OK)
                res    = ctx->handler->handle_block_data(buf, size);
            ::free(buf);
            return res;
        }

        status_t read_block_data_long(context_t *ctx)
        {
            uint32_t size;
            status_t res    = read_primitive(ctx, &size);
            if (res != STATUS_OK)
                return STATUS_CORRUPTED;
            uint8_t *buf    = reinterpret_cast<uint8_t *>(::malloc(size));
            if (buf == NULL)
                return STATUS_NO_MEM;
            res     = read_block(ctx, buf, size);
            if (res == STATUS_OK)
                res    = ctx->handler->handle_block_data(buf, size);
            ::free(buf);
            return res;
        }

        status_t read_array(context_t *ctx, bool unshared)
        {
            return STATUS_OK; // TODO
        }

        status_t read_enum(context_t *ctx, bool unshared)
        {
            return STATUS_OK; // TODO
        }

        status_t read_ordinary_object(context_t *ctx, bool unshared)
        {
            return STATUS_OK; // TODO
        }

        status_t read_fatal_exception(context_t *ctx)
        {
            return STATUS_OK; // TODO
        }

        status_t read_object(context_t *ctx, bool unshared)
        {
            // Read byte
            ssize_t byte = read_byte(ctx);
            if (byte < 0)
                return -byte;

            // Analyze token
            switch (byte)
            {
                case TC_NULL:
                    return ctx->handler->handle_null();
                case TC_REFERENCE:
                    return read_reference(ctx, unshared);
                case TC_CLASS:
                    return read_class(ctx, unshared);
                case TC_CLASSDESC:
                case TC_PROXYCLASSDESC:
                    return read_class_desc(ctx, unshared);
                case TC_STRING:
                    return read_string(ctx, unshared);
                case TC_LONGSTRING:
                    return read_long_string(ctx, unshared);
                case TC_ARRAY:
                    return read_array(ctx, unshared);
                case TC_ENUM:
                    return read_enum(ctx, unshared);
                case TC_OBJECT:
                    return read_ordinary_object(ctx, unshared);
                case TC_EXCEPTION:
                    return read_fatal_exception(ctx);
                case TC_ENDBLOCKDATA:
                    return ctx->handler->handle_end_block_data();

                default:
                    break;
            }

            return STATUS_CORRUPTED;
        }

        status_t parse_object_stream(io::IInStream *is, IObjectStreamHandler *handler)
        {
            // Initialize context
            context_t ctx;
            ::bzero(&ctx, sizeof(ctx));
            ctx.is      = is;
            ctx.handler = handler;

            // Read stream header
            obj_stream_hdr_t hdr;
            status_t res = ctx.is->read_block(&hdr, sizeof(hdr));
            if (res != STATUS_OK)
                return (res == STATUS_EOF) ? STATUS_BAD_FORMAT : res;
            if (BE_TO_CPU(hdr.magic) != JAVA_STREAM_MAGIC)
                return STATUS_BAD_FORMAT;
            ctx.version    = BE_TO_CPU(hdr.version);
            ctx.handler->handle_start(ctx.version);

            do
            {
                // Lookup next tag
                ssize_t byte = read_byte(&ctx);
                if (byte < 0)
                {
                    res     = -byte;
                    if (byte == -STATUS_EOF)
                        res     = ctx.handler->handle_end();
                    break;
                }

                // Analyze token
                switch (byte)
                {
                    case TC_NULL:
                        res = ctx.handler->handle_null();
                        break;
                    case TC_REFERENCE:
                        res = read_reference(&ctx, false);
                        break;
                    case TC_CLASS:
                        res = read_class(&ctx, false);
                        break;
                    case TC_CLASSDESC:
                    case TC_PROXYCLASSDESC:
                        res = read_class_desc(&ctx, false);
                        break;
                    case TC_STRING:
                        res = read_string(&ctx, false);
                        break;
                    case TC_LONGSTRING:
                        res = read_long_string(&ctx, false);
                        break;
                    case TC_ARRAY:
                        res = read_array(&ctx, false);
                        break;
                    case TC_ENUM:
                        res = read_enum(&ctx, false);
                        break;
                    case TC_OBJECT:
                        res = read_ordinary_object(&ctx, false);
                        break;
                    case TC_EXCEPTION:
                        res = read_fatal_exception(&ctx);
                        break;
                    case TC_BLOCKDATA:
                        res = read_block_data(&ctx);
                        break;
                    case TC_BLOCKDATALONG:
                        res = read_block_data_long(&ctx);
                        break;
                    case TC_RESET:
                        res = ctx.handler->handle_reset();
                        break;
                    default:
                        res = STATUS_CORRUPTED;
                        break;
                }
            } while (res == STATUS_OK);

            return ctx.handler->close(res);
        }
        */
    }
}


