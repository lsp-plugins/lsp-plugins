/*
 * LSPUrlSink.cpp
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <ui/tk/helpers/mime.h>

namespace lsp
{
    namespace tk
    {
        LSPUrlSink::LSPUrlSink()
        {
            sProtocol   = NULL;
            pOS         = NULL;
            nCtype      = -1;
        }
        
        LSPUrlSink::LSPUrlSink(const char *protocol)
        {
            sProtocol   = ::strdup(protocol);
            pOS         = NULL;
            nCtype      = -1;
        }
        
        LSPUrlSink::~LSPUrlSink()
        {
            if (pOS != NULL)
            {
                pOS->close();
                delete pOS;
                pOS     = NULL;
            }
            if (sProtocol != NULL)
            {
                ::free(sProtocol);
                sProtocol = NULL;
            }
        }

        const char * const LSPUrlSink::acceptMime[] =
        {
            "text/uri-list",
            "text/x-moz-url",
            "application/x-kde4-urilist",
            "text/plain",
            NULL
        };

        ssize_t LSPUrlSink::get_mime_index(const char *mime)
        {
            ssize_t idx = 0;
            for (const char * const *p = acceptMime; *p != NULL; ++p, ++idx)
            {
                if (!::strcasecmp(*p, mime))
                    return idx;
            }
            return -1;
        }

        ssize_t LSPUrlSink::select_mime_type(const char * const *mime_types)
        {
            for (const char * const *p = acceptMime; *p != NULL; ++p)
            {
                ssize_t idx = 0;
                for (const char * const *q = mime_types; *q != NULL; ++q, ++idx)
                {
                    if (!::strcasecmp(*p, *q))
                        return idx;
                }
            }
            return -1;
        }

        ssize_t LSPUrlSink::open(const char * const *mime_types)
        {
            if (pOS != NULL)
                return -STATUS_BAD_STATE;

            ssize_t idx = select_mime_type(mime_types);
            if (idx < 0)
                return -STATUS_UNSUPPORTED_FORMAT;
            ssize_t ctype = get_mime_index(mime_types[idx]);
            if (ctype < 0)
                return -STATUS_UNSUPPORTED_FORMAT;

            pOS         = new io::OutMemoryStream();
            if (pOS == NULL)
                return -STATUS_NO_MEM;

            nCtype      = ctype;
            return idx; // Return the index from mime_types array
        }

        status_t LSPUrlSink::write(const void *buf, size_t count)
        {
            if (pOS == NULL)
                return STATUS_CLOSED;
            return pOS->write(buf, count);
        }

        status_t LSPUrlSink::close(status_t code)
        {
            if (pOS == NULL)
                return STATUS_OK;
            pOS->close();

            // Commit the content
            const uint8_t *raw_data = pOS->data();
            size_t raw_size = pOS->size();

            status_t res = STATUS_NOT_FOUND;
            LSPString data;

            if ((raw_data != NULL) && (raw_size > 0))
            {
                lsp_dumpb("Content dump", raw_data, raw_size);

                switch (nCtype)
                {
                    case TEXT_URI_LIST:
                        res = fetch_text_uri_list_item(&data, sProtocol, raw_data, raw_size, "UTF-8");
                        break;
                    case TEXT_X_MOZ_URL:
                        res = fetch_text_uri_list_item(&data, sProtocol, raw_data, raw_size, __IF_LEBE("UTF-16LE", "UTF-16BE"));
                        break;
                    case APPLICATION_X_KDE4_URILIST:
                        res = fetch_text_uri_list_item(&data, sProtocol, raw_data, raw_size, "UTF-8");
                        break;
                    case TEXT_PLAIN:
                        if (data.set_native(reinterpret_cast<const char *>(raw_data), raw_size))
                            res = STATUS_OK;
                        break;
                }

                // Remove file:// prefix
                if (data.ends_with('\n'))
                    data.remove_last();
                if (data.ends_with('\r'))
                    data.remove_last();
            }
            else
                lsp_trace("Empty content");

            // Drop allocated data
            pOS->drop();
            delete pOS;
            pOS     = NULL;
            nCtype  = -1;

            // Need to update file name?
            if (res == STATUS_OK)
                res     = commit_url(&data);

            return STATUS_OK;
        }

        status_t LSPUrlSink::commit_url(const LSPString *url)
        {
            return STATUS_OK;
        }

        status_t LSPUrlSink::set_protocol(const char *protocol)
        {
            if (protocol != NULL)
            {
                char *proto = ::strdup(protocol);
                if (proto == NULL)
                    return STATUS_NO_MEM;
                if (sProtocol != NULL)
                    ::free(sProtocol);
                sProtocol   = proto;
            }
            else if (sProtocol != NULL)
            {
                ::free(sProtocol);
                sProtocol = NULL;
            }
            return STATUS_OK;
        }

    
    } /* namespace tk */
} /* namespace lsp */
