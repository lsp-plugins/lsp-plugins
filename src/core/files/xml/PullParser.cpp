/*
 * PullParser.cpp
 *
 *  Created on: 24 окт. 2019 г.
 *      Author: sadko
 */

#include <core/io/InStringSequence.h>
#include <core/io/InSequence.h>
#include <core/io/InFileStream.h>
#include <core/files/xml/PullParser.h>
#include <ctype.h>
#include <wctype.h>

namespace lsp
{
    namespace xml
    {
        
        PullParser::PullParser()
        {
            pIn         = NULL;
            nWFlags     = 0;
            nToken      = XT_START_DOCUMENT;
            nState      = PS_READ_PROLOG;
            enVersion   = XML_VERSION_1_0;
            nFlags      = 0;

            vBuffer     = NULL;
            nBufHead    = 0;
            nBufTail    = 0;
            nBufSize    = 0;
            nBufCap     = 0;
        }
        
        PullParser::~PullParser()
        {
            close();
        }

        status_t PullParser::open(const char *path, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InFileStream *ifs = new io::InFileStream();
            if (ifs == NULL)
                return STATUS_NO_MEM;
            status_t res = ifs->open(path);
            if (res == STATUS_OK)
            {
                res     = wrap(ifs, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ifs->close();
            }
            delete ifs;

            return res;
        }

        status_t PullParser::open(const LSPString *path, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InFileStream *ifs = new io::InFileStream();
            if (ifs == NULL)
                return STATUS_NO_MEM;
            status_t res = ifs->open(path);
            if (res == STATUS_OK)
            {
                res     = wrap(ifs, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ifs->close();
            }
            delete ifs;

            return res;
        }

        status_t PullParser::open(const io::Path *path, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InFileStream *ifs = new io::InFileStream();
            if (ifs == NULL)
                return STATUS_NO_MEM;
            status_t res = ifs->open(path);
            if (res == STATUS_OK)
            {
                res     = wrap(ifs, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ifs->close();
            }
            delete ifs;

            return res;
        }

        status_t PullParser::wrap(const char *str, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (str == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InStringSequence *seq = new io::InStringSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(str, charset);
            if (res == STATUS_OK)
            {
                if ((res = wrap(seq, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t PullParser::wrap(const LSPString *str)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (str == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InStringSequence *seq = new io::InStringSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(str);
            if (res == STATUS_OK)
            {
                if ((res = wrap(seq, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t PullParser::wrap(io::IInStream *is, size_t flags, const char *charset)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (is == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InSequence *seq = new io::InSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(is, flags, charset);
            if (res == STATUS_OK)
            {
                if ((res = wrap(seq, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t PullParser::wrap(io::IInSequence *seq, size_t flags)
        {
            if (pIn != NULL)
                return STATUS_BAD_STATE;
            else if (seq == NULL)
                return STATUS_BAD_ARGUMENTS;

            pIn             = seq;
            nWFlags         = flags;
            nToken          = XT_START_DOCUMENT;
            nState          = PS_READ_PROLOG;
            enVersion       = XML_VERSION_1_0;
            sVersion.truncate();
            sCharset.truncate();
            nFlags          = 0;

            nBufHead        = 0;
            nBufTail        = 0;
            nBufSize        = 0;

            return STATUS_OK;
        }

        status_t PullParser::close()
        {
            status_t res = STATUS_OK;

            if (vBuffer != NULL)
            {
                ::free(vBuffer);
                vBuffer     = NULL;
            }
            nBufHead    = 0;
            nBufTail    = 0;
            nBufSize    = 0;
            nBufCap     = 0;

            sVersion.truncate();
            sCharset.truncate();
            nFlags          = 0;

            if (pIn != NULL)
            {
                if (nWFlags & WRAP_CLOSE)
                {
                    if (res == STATUS_OK)
                        res = pIn->close();
                    else
                        pIn->close();
                }

                if (nWFlags & WRAP_DELETE)
                    delete pIn;

                pIn     = NULL;
            }

            return res;
        }

        bool PullParser::is_valid_char(lsp_swchar_t c)
        {
            if (enVersion == XML_VERSION_1_0)
            {
                if ((c >= 0x20) && (c <= 0xd7ff))
                    return true;
                if ((c == 0x9) || (c == 0xa) || (c == 0xd))
                    return true;
            }
            else
            {
                if ((c >= 1) && (c <= 0xd7ff))
                    return true;
            }
            if ((c >= 0xe000) && (c <= 0xfffd))
                return true;
            return (c >= 0x10000) && (c <= 0x10ffff);
        }

        bool PullParser::is_name_start(lsp_swchar_t c)
        {
            if ((c >= 'a') && (c <= 'z'))
                return true;
            if ((c >= 'A') && (c <= 'Z'))
                return true;
            if ((c == ':') || (c == '_'))
                return true;
            if ((c >= 0xc0) && (c <= 0xd6))
                return true;
            if ((c >= 0xd8) && (c <= 0xf6))
                return true;
            if ((c >= 0xf8) && (c <= 0x2ff))
                return true;
            if ((c >= 0x370) && (c <= 0x37d))
                return true;
            if ((c >= 0x37f) && (c <= 0x1fff))
                return true;
            if ((c >= 0x200c) && (c <= 0x200d))
                return true;
            if ((c >= 0x2070) && (c <= 0x218f))
                return true;
            if ((c >= 0x2c00) && (c <= 0x2fef))
                return true;
            if ((c >= 0x3001) && (c <= 0xd7ff))
                return true;
            if ((c >= 0xf900) && (c <= 0xfdcf))
                return true;
            if ((c >= 0xfdf0) && (c <= 0xfffd))
                return true;
            if ((c >= 0x10000) && (c <= 0xeffff))
                return true;

            return false;
        }

        bool PullParser::is_name_char(lsp_swchar_t c)
        {
            if ((c >= '0') && (c <= '9'))
                return true;
            if ((c == '-') || (c == '.') || (c == 0xb7))
                return true;
            if (is_name_start(c))
                return true;
            if ((c >= 0x300) && (c <= 0x36f))
                return true;
            if ((c >= 0x203f) && (c <= 0x2040))
                return true;
            return false;
        }

        bool PullParser::is_whitespace(lsp_swchar_t c)
        {
            switch (c)
            {
                case 0x20:
                case 0x09:
                case 0x0d:
                case 0x0a:
                    return true;
            }
            return false;
        }

        bool PullParser::is_restricted_char(lsp_swchar_t c)
        {
            if (enVersion > XML_VERSION_1_0)
            {
                if ((c >= 0x01) && (c <= 0x08))
                    return true;
                if ((c >= 0x0b) && (c <= 0x0c))
                    return true;
                if ((c >= 0x0e) && (c <= 0x1f))
                    return true;
                if ((c >= 0x7f) && (c <= 0x84))
                    return true;
                if ((c >= 0x86) && (c <= 0x9f))
                    return true;
            }
            return false;
        }

        void PullParser::unget_all()
        {
            nBufTail    = nBufHead;
        }

        void PullParser::commit_all()
        {
            nBufHead    = nBufTail;
        }

        void PullParser::commit_unget()
        {
            nBufHead    = nBufTail-1;
        }

        lsp_swchar_t PullParser::get_char()
        {
            // Is there available data in buffer?
            if (nBufTail < nBufSize)
                return vBuffer[nBufTail++];

            // Release the rest of buffer
            if (nBufHead > 0)
            {
                ::memmove(vBuffer, &vBuffer[nBufHead], (nBufSize - nBufHead) * sizeof(lsp_wchar_t));
                nBufSize   -= nBufHead;
                nBufTail   -= nBufHead;
                nBufHead    = 0;
            }

            // Need to reserve space in buffer?
            if (nBufSize >= nBufCap)
            {
                size_t ncap = ((nBufSize + 0x1000) & (~0xffff)) + nBufCap;
                lsp_wchar_t *p = reinterpret_cast<lsp_wchar_t *>(::realloc(vBuffer, ncap * sizeof(lsp_wchar_t)));
                if (p == NULL)
                    return -STATUS_NO_MEM;
                vBuffer     = p;
                nBufCap     = ncap;
            }

            // Read character data
            ssize_t nread = pIn->read(&vBuffer[nBufSize], nBufCap - nBufSize);
            if (nread <= 0)
                return (nread < 0) ? nread : -STATUS_EOF;
            nBufSize       += nread;

            // Return the next character
            return vBuffer[nBufTail++];
        }

        status_t PullParser::lookup(const char *text)
        {
            for ( ; *text != '\0'; ++text)
            {
                lsp_swchar_t c = get_char();
                if (c < 0)
                {
                    unget_all();
                    return -c;
                }
                else if (c != *text)
                {
                    unget_all();
                    return STATUS_NOT_FOUND;
                }
            }
            commit_all();
            return STATUS_OK;
        }

        status_t PullParser::lookup_nocase(const char *text)
        {
            for ( ; *text != '\0'; ++text)
            {
                lsp_swchar_t c = get_char();
                if (c < 0)
                {
                    unget_all();
                    return -c;
                }
                else if (lsp_swchar_t(::towupper(c)) != lsp_swchar_t(::toupper(*text)))
                {
                    unget_all();
                    return STATUS_NOT_FOUND;
                }
            }
            commit_all();
            return STATUS_OK;
        }

        bool PullParser::skip_spaces()
        {
            bool skipped = false;
            while (true)
            {
                // Read next character
                lsp_swchar_t c = get_char();
                if (!is_whitespace(c))
                {
                    unget_all();
                    break;
                }

                commit_all();
                skipped = true;
            }

            return skipped;
        }

        status_t PullParser::read_assign()
        {
            // Read character
            lsp_swchar_t c = get_char();
            if (c != '=')
            {
                unget_all();
                return (c < 0) ? -c : STATUS_CORRUPTED;
            }

            commit_all();
            return STATUS_OK;
        }

        status_t PullParser::read_name(LSPString *name)
        {
            // Get first character
            lsp_swchar_t c = get_char();
            if (!(is_name_start(c)))
            {
                unget_all();
                return (c < 0) ? -c : STATUS_NOT_FOUND;
            }

            // Read name
            name->clear();
            do
            {
                // Append current character
                if (!name->append(c))
                    return STATUS_NO_MEM;

                // Get next character
                c = get_char();
            } while (is_name_char(c));

            commit_unget();
            return STATUS_OK;
        }

        status_t PullParser::read_value(LSPString *value)
        {
            // Get first character
            lsp_swchar_t c = get_char();
            if ((c != '\'') && (c != '\"'))
            {
                unget_all();
                return (c < 0) ? -c : STATUS_NOT_FOUND;
            }

            // Read quoted value
            value->clear();
            while (true)
            {
                lsp_swchar_t xc = get_char();
                if (xc < 0)
                    return -xc;
                else if (xc == c)
                    break;

                // Append current character
                if (!value->append(xc))
                    return STATUS_NO_MEM;
            }

            commit_all();
            return STATUS_OK;
        }

        status_t PullParser::read_attribute(LSPString *name, LSPString *value)
        {
            status_t res;

            if (!skip_spaces()) // Space is mandatory
                return STATUS_CORRUPTED;
            if ((res = read_name(name)) != STATUS_OK)
                return res;

            skip_spaces(); // Space is optional
            if ((res = read_assign()) != STATUS_OK)
                return res;

            skip_spaces(); // Space is optional
            if ((res = read_value(value)) != STATUS_OK)
                return res;

            return res;
        }

        status_t PullParser::parse_version(const LSPString *version)
        {
            // TODO
            return STATUS_OK;
        }

        status_t PullParser::parse_encoding(const LSPString *version)
        {
            // TODO
            return STATUS_OK;
        }

        status_t PullParser::parse_standalone(const LSPString *version)
        {
            // TODO
            return STATUS_OK;
        }

        status_t PullParser::read_prolog()
        {
            status_t res;

            // Lookup prolog
            if ((res = lookup_nocase("<?xml")) != STATUS_OK)
            {
                if (res != STATUS_NOT_FOUND)
                    return res;

                enVersion   = XML_VERSION_1_0;
                nToken      = XT_START_DOCUMENT;
                nState      = PS_READ_MISC;
                return STATUS_OK;
            }

            // Fetch optional attributes
            enum flags_t
            {
                F_VERSION       = 1 << 0,
                F_ENCODING      = 1 << 1,
                F_STANDALONE    = 1 << 2
            };

            size_t flags = 0;
            LSPString name, value;

            while (true)
            {
                // Fetch next attribute
                res = read_attribute(&name, &value);
                if (res == STATUS_NOT_FOUND)
                    break;
                else if (res != STATUS_OK)
                    return res;

                // Check attribute type
                size_t flag = 0;
                if (name.equals_ascii("version"))
                {
                    flag = F_VERSION;
                    if ((res = parse_version(&value)) != STATUS_OK)
                        return res;
                }
                else if (name.equals_ascii("encoding"))
                {
                    flag = F_ENCODING;
                    if ((res = parse_encoding(&value)) != STATUS_OK)
                        return res;
                }
                else if (name.equals_ascii("standalone"))
                {
                    flag = F_STANDALONE;
                    if ((res = parse_standalone(&value)) != STATUS_OK)
                        return res;
                }

                // Check that flag is at proper place
                if (flag <= flags)
                    return STATUS_CORRUPTED;
            }

            // Skip extra space and lookup for close
            skip_spaces();
            if ((res = lookup("?>")) != STATUS_OK)
                return (res != STATUS_NOT_FOUND) ? res : STATUS_CORRUPTED;

            // Change state
            nToken      = XT_START_DOCUMENT;
            nState      = PS_READ_MISC;

            return STATUS_OK;
        }

        status_t PullParser::read_comment()
        {
            // TODO
            return STATUS_OK;
        }

        status_t PullParser::read_processing_instruction()
        {
            // TODO
            return STATUS_OK;
        }

        status_t PullParser::read_misc()
        {
            status_t res;

            skip_spaces();

            // Check for comment
            if ((res = lookup("<!--")) == STATUS_OK)
                return read_comment();
            else if (res != STATUS_NOT_FOUND)
                return res;

            // Check for processing instruction
            if ((res = lookup("<?")) == STATUS_OK)
                return read_processing_instruction();
            else if (res != STATUS_NOT_FOUND)
                return res;

            return STATUS_CORRUPTED;
        }

        status_t PullParser::read_next()
        {
            if (pIn == NULL)
                return STATUS_BAD_STATE;

            switch (nState)
            {
                case PS_END_DOCUMENT:
                    nToken          = XT_END_DOCUMENT;
                    return -STATUS_EOF;

                case PS_READ_PROLOG:
                    return read_prolog();

                case PS_READ_MISC:
                    return read_misc();

                default:
                    break;
            }
            return STATUS_CORRUPTED;
        }

        status_t PullParser::get_current()
        {
            return nToken;
        }
    
    } /* namespace xml */
} /* namespace lsp */
