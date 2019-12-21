/*
 * mime.cpp
 *
 *  Created on: 27 авг. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/helpers/mime.h>
#include <core/files/url.h>
#include <core/io/InStringSequence.h>
#include <core/io/InSequence.h>
#include <core/io/InMemoryStream.h>

namespace lsp
{
    namespace tk
    {
        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, const LSPString *src)
        {
            LSPString tmp;
            io::InStringSequence seq;
            status_t res = seq.wrap(src);
            if (res != STATUS_OK)
                return res;
            res = fetch_text_uri_list_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset)
        {
            LSPString tmp;
            io::InSequence seq;
            status_t res = seq.wrap(is, WRAP_NONE, charset);
            if (res != STATUS_OK)
                return res;
            res = fetch_text_uri_list_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset)
        {
            LSPString tmp;
            io::InMemoryStream is;
            is.wrap(const_cast<void *>(src), bytes);
            status_t res = fetch_text_uri_list_item(&tmp, protocol, &is, charset);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = is.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, io::IInSequence *is)
        {
            LSPString line;
            while (true)
            {
                status_t res = is->read_line(&line, true);
                if (res == STATUS_EOF)
                    return STATUS_NOT_FOUND;
                if (line.starts_with('#'))
                    continue;
                if (protocol == NULL)
                    break;
                if (line.starts_with_ascii(protocol))
                    break;
            }

            // URL-decode the string
            line.swap(dst);
            return STATUS_OK;
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, const LSPString *src)
        {
            LSPString tmp;
            io::InStringSequence seq;
            status_t res = seq.wrap(src);
            if (res != STATUS_OK)
                return res;
            res = fetch_text_x_moz_url_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset)
        {
            LSPString tmp;
            io::InSequence seq;
            status_t res = seq.wrap(is, WRAP_NONE, charset);
            if (res != STATUS_OK)
                return res;
            res = fetch_text_x_moz_url_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset)
        {
            LSPString tmp;
            io::InMemoryStream is;
            is.wrap(const_cast<void *>(src), bytes);
            status_t res = fetch_text_x_moz_url_item(&tmp, protocol, &is, charset);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = is.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, io::IInSequence *is)
        {
            LSPString line;
            size_t index = 0;
            while (true)
            {
                status_t res = is->read_line(&line, true);
                if (res == STATUS_EOF)
                    return STATUS_NOT_FOUND;
                if (index & 1) // Skip odd lines
                    continue;
                if (protocol == NULL)
                    break;
                if (line.starts_with_ascii(protocol))
                    break;
            }

            line.swap(dst);
            return STATUS_OK;
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, const LSPString *src)
        {
            LSPString tmp;
            io::InStringSequence seq;
            status_t res = seq.wrap(src);
            if (res != STATUS_OK)
                return res;
            res = fetch_application_x_kde4_urilist_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset)
        {
            LSPString tmp;
            io::InSequence seq;
            status_t res = seq.wrap(is, WRAP_NONE, charset);
            if (res != STATUS_OK)
                return res;
            res = fetch_application_x_kde4_urilist_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset)
        {
            LSPString tmp;
            io::InMemoryStream is;
            is.wrap(const_cast<void *>(src), bytes);
            status_t res = fetch_application_x_kde4_urilist_item(&tmp, protocol, &is, charset);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = is.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, io::IInSequence *is)
        {
            LSPString line;
            while (true)
            {
                status_t res = is->read_line(&line, true);
                if (res == STATUS_EOF)
                    return STATUS_NOT_FOUND;
                if (line.starts_with('#'))
                    continue;
                if (protocol == NULL)
                    break;
                if (line.starts_with_ascii(protocol))
                    break;
            }

            line.swap(dst);
            return STATUS_OK;
        }

    }
}


