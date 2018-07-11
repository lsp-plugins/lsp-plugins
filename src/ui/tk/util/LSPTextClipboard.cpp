/*
 * LSPTextClipboard.cpp
 *
 *  Created on: 17 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <ctype.h>

namespace lsp
{
    namespace tk
    {
        LSPTextClipboard::InputStream::InputStream(char *data, size_t total)
        {
            pData       = data;
            nPosition   = 0;
            nTotal      = total;
        }

        LSPTextClipboard::InputStream::~InputStream()
        {
            close();
        }

        wssize_t LSPTextClipboard::InputStream::avail()
        {
            if (pData == NULL)
                return - (nError = STATUS_CLOSED);
            nError = STATUS_OK;
            return nTotal - nPosition;
        }

        wssize_t LSPTextClipboard::InputStream::position()
        {
            if (pData == NULL)
                return - (nError = STATUS_CLOSED);
            nError = STATUS_OK;
            return nPosition;
        }

        ssize_t LSPTextClipboard::InputStream::read(void *dst, size_t count)
        {
            if (pData == NULL)
                return - (nError = STATUS_CLOSED);
            nError = STATUS_OK;

            size_t can_read = nTotal - nPosition;
            if (count > can_read)
                count   = can_read;

            memcpy(dst, &pData[nPosition], count);
            nPosition      += count;

            return count;
        }

        wssize_t LSPTextClipboard::InputStream::seek(wsize_t position)
        {
            if (pData == NULL)
                return - (nError = STATUS_CLOSED);
            nError = STATUS_OK;
            return nPosition = (position <= nTotal) ? position : nTotal;
        }

        status_t LSPTextClipboard::InputStream::close()
        {
            if (pData == NULL)
                return - (nError = STATUS_CLOSED);

            delete [] pData;
            pData       = NULL;
            nPosition   = 0;
            nTotal      = 0;
            return nError = STATUS_OK;
        };

        const char *LSPTextClipboard::x_targets[] =
        {
            "UTF8_STRING",
            "text/plain",
            "text/plain;charset=utf-8",
            "text/plain;charset=UTF-16LE",
            "text/plain;charset=UTF-16BE",
            "text/plain;charset=US-ASCII"
        };

        LSPTextClipboard::LSPTextClipboard()
        {
        }


        LSPTextClipboard::~LSPTextClipboard()
        {
        }
        
        IInputStream *LSPTextClipboard::read(const char *ctype)
        {
            if (ctype == NULL)
            {
                nError = STATUS_BAD_ARGUMENTS;
                return NULL;
            }

            const char *data = NULL;
            if (!strcasecmp(ctype, "utf8_string"))
                data = sString.get_utf8();
            else if (!strcmp(ctype, "text/plain"))
                data = sString.get_utf8();
            else if (strstr(ctype, "text/plain;") == ctype)
            {
                ctype   += 11;
                while (isspace(*ctype))
                    ++ctype;
                if (strstr(ctype, "charset=") == ctype)
                {
                    char charset[128];
                    size_t i=0;
                    for (; i<sizeof(charset)-1; ++i)
                    {
                        if (((*ctype) == '\0')  || ((*ctype) == ';') || (isspace(*ctype)))
                            break;
                        charset[i] = *(ctype++);
                    }
                    charset[i] = '\0';

                    if ((!strcasecmp(charset, "UTF8")) ||
                        (!strcmp(charset, "UTF-8")))
                        data = sString.get_utf8();
                    else if (
                        (!strcasecmp(charset, "US-ASCII")) ||
                        (!strcmp(charset, "ASCII")))
                        data = sString.get_ascii();
                    data = sString.get_native(charset);
                }
                else
                    data = sString.get_utf8();
            }

            if (data == NULL)
            {
                nError = STATUS_BAD_FORMAT;
                return NULL;
            }

            size_t size = sString.temporal_size();
            char *buffer = new char[size];
            if (buffer == NULL)
            {
                nError = STATUS_NO_MEM;
                return NULL;
            }

            memcpy(buffer, data, size);
            InputStream *is = new InputStream(buffer, size);
            if (is == NULL)
            {
                delete [] buffer;
                nError = STATUS_NO_MEM;
                return NULL;
            }
            return is;
        }

        status_t LSPTextClipboard::update_text(const LSPString *text)
        {
            if (!sString.set(text))
                return STATUS_NO_MEM;
            return STATUS_OK;
        }

        status_t LSPTextClipboard::update_text(const LSPString *text, ssize_t first)
        {
            if (!sString.set(text, first))
                return STATUS_NO_MEM;
            return STATUS_OK;
        }

        status_t LSPTextClipboard::update_text(const LSPString *text, ssize_t first, ssize_t last)
        {
            if (!sString.set(text, first, last))
                return STATUS_NO_MEM;
            return STATUS_OK;
        }

        size_t LSPTextClipboard::targets()
        {
            return sizeof(x_targets) / sizeof (const char *);
        }

        const char *LSPTextClipboard::target(size_t i)
        {
            return (i < (sizeof(x_targets) / sizeof (const char *))) ? x_targets[i] : NULL;
        }

    } /* namespace tk */
} /* namespace lsp */
