/*
 * TextConfigHandler.cpp
 *
 *  Created on: 13 июл. 2019 г.
 *      Author: sadko
 */

#include <errno.h>
#include <dsp/dsp.h>
#include <core/parse.h>
#include <core/files/config/TextConfigHandler.h>

namespace lsp
{
    namespace config
    {
        TextConfigHandler::TextConfigHandler()
        {
        }
        
        TextConfigHandler::~TextConfigHandler()
        {
        }

        status_t TextConfigHandler::handle_parameter(const char *name, const char *value, size_t flags)
        {
            // Check if parameter is regular
            if (name[0] != '/')
                return handle_regular_parameter(name, value, flags); // Just call regular parameter callback

            // Parse KVT parameter
            kvt_param_t p;

            switch (flags & SF_TYPE_MASK)
            {
                case SF_TYPE_I32:
                    PARSE_INT(value, { p.i32 = __; p.type = KVT_INT32; } );
                    break;
                case SF_TYPE_U32:
                    PARSE_UINT(value, { p.u32 = __; p.type = KVT_UINT32; } );
                    break;
                case SF_TYPE_I64:
                    PARSE_LLINT(value, { p.i64 = __; p.type = KVT_INT64; } );
                    break;
                case SF_TYPE_U64:
                    PARSE_ULLINT(value, { p.u64 = __; p.type = KVT_UINT64; } );
                    break;
                case SF_TYPE_F64:
                    PARSE_DOUBLE(value, { p.f64 = __; p.type = KVT_FLOAT64; } );
                    break;
                case SF_TYPE_STR:
                    p.str       = ::strdup(value);
                    if (p.str == NULL)
                        return STATUS_NO_MEM;
                    p.type      = KVT_STRING;
                    break;
                case SF_TYPE_BLOB:
                {
                    // Get content type
                    const char *split = ::strchr(value, ':');
                    if (split == NULL)
                        return STATUS_BAD_FORMAT;

                    char *ctype = NULL;
                    size_t clen = (++split) - value;

                    if (clen > 0)
                    {
                        ctype = ::strndup(value, clen);
                        if (ctype == NULL)
                            return STATUS_NO_MEM;
                        ctype[clen-1]   = '\0';
                    }
                    p.blob.ctype    = ctype;

                    // Get content size
                    errno = 0;
                    char *base64 = NULL;
                    p.blob.size = size_t(::strtoull(split, &base64, 10));
                    if ((errno != 0) || (*(base64++) != ':'))
                    {
                        if (ctype != NULL)
                            ::free(ctype);
                        return STATUS_BAD_FORMAT;
                    }

                    // Decode content
                    size_t src_left = ::strlen(base64); // Size of base64-block
                    p.blob.data     = NULL;
                    if (src_left > 0)
                    {
                        // Allocate memory
                        size_t dst_left = 0x10 + ((src_left * 3) / 4);
                        void *blob      = ::malloc(dst_left);
                        if (blob == NULL)
                        {
                            if (ctype != NULL)
                                ::free(ctype);
                            return STATUS_NO_MEM;
                        }

                        // Decode
                        size_t n = dsp::base64_dec(blob, &dst_left, base64, &src_left);
                        if ((n != p.blob.size) || (src_left != 0))
                        {
                            ::free(ctype);
                            ::free(blob);
                            return STATUS_BAD_FORMAT;
                        }
                        p.blob.data = blob;
                    }
                    else if (p.blob.size != 0)
                    {
                        ::free(ctype);
                        return STATUS_BAD_FORMAT;
                    }

                    break;
                }

                case SF_TYPE_NATIVE:
                case SF_TYPE_F32:
                default:
                    PARSE_FLOAT(value, { p.f32 = __; p.type = KVT_FLOAT32; } );
                    break;
            }

            if (p.type == KVT_ANY)
                return STATUS_BAD_FORMAT;

            // Call KVT parameter callback
            status_t res = handle_kvt_parameter(name, &p, flags);

            // Destroy allocated data for KVT parameter
            if (p.type == KVT_STRING)
            {
                if (p.str != NULL)
                    ::free(const_cast<char *>(p.str));
            }
            else if (p.type == KVT_BLOB)
            {
                if (p.blob.ctype != NULL)
                    ::free(const_cast<char *>(p.blob.ctype));
                if (p.blob.data != NULL)
                    ::free(const_cast<void *>(p.blob.data));
            }

            return res;
        }

        status_t TextConfigHandler::handle_regular_parameter(const char *name, const char *value, size_t flags)
        {
            return STATUS_OK;
        }

        status_t TextConfigHandler::handle_kvt_parameter(const char *name, const kvt_param_t *param, size_t flags)
        {
            return STATUS_OK;
        }
    }

} /* namespace lsp */
