/*
 * mime.cpp
 *
 *  Created on: 27 авг. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/helpers/mime.h>
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
            status_t res = seq.wrap(src, false);
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
            io::InSequence seq;
            status_t res = seq.wrap(is, WRAP_NONE, charset);
            if (res != STATUS_OK)
                return res;
            res = fetch_text_uri_list_item(dst, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            return seq.close();
        }

        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset)
        {
            io::InMemoryStream is;
            is.wrap(const_cast<void *>(src), bytes);
            status_t res = fetch_text_uri_list_item(dst, protocol, &is, charset);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            return is.close();
        }

        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, io::IInSequence *is)
        {
            // TODO
            return STATUS_OK;
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, const LSPString *src)
        {
            io::InStringSequence seq;
            status_t res = seq.wrap(src, false);
            if (res != STATUS_OK)
                return res;
            res = fetch_text_x_moz_url_item(dst, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            return seq.close();
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset)
        {
            io::InSequence seq;
            status_t res = seq.wrap(is, WRAP_NONE, charset);
            if (res != STATUS_OK)
                return res;
            res = fetch_text_x_moz_url_item(dst, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            return seq.close();
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset)
        {
            io::InMemoryStream is;
            is.wrap(const_cast<void *>(src), bytes);
            status_t res = fetch_text_x_moz_url_item(dst, protocol, &is, charset);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            return is.close();
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, io::IInSequence *is)
        {
            // TODO
            return STATUS_OK;
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, const LSPString *src)
        {
            io::InStringSequence seq;
            status_t res = seq.wrap(src, false);
            if (res != STATUS_OK)
                return res;
            res = fetch_application_x_kde4_urilist_item(dst, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            return seq.close();
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset)
        {
            io::InSequence seq;
            status_t res = seq.wrap(is, WRAP_NONE, charset);
            if (res != STATUS_OK)
                return res;
            res = fetch_application_x_kde4_urilist_item(dst, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            return seq.close();
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset)
        {
            io::InMemoryStream is;
            is.wrap(const_cast<void *>(src), bytes);
            status_t res = fetch_application_x_kde4_urilist_item(dst, protocol, &is, charset);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            return is.close();
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, io::IInSequence *is)
        {
            // TODO
            return STATUS_OK;
        }

    }
}


