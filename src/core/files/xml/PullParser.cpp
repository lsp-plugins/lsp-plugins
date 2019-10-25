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
            nToken      = -STATUS_NO_DATA;
            nState      = PS_READ_MISC;
            enVersion   = XML_VERSION_1_0;
            nFlags      = 0;

            cLast       = -1;
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
            nToken          = -STATUS_NO_DATA;
            nState          = PS_READ_MISC;
            enVersion       = XML_VERSION_1_0;
            sVersion.truncate();
            sEncoding.truncate();
            nFlags          = 0;
            cLast           = -1;

            return STATUS_OK;
        }

        status_t PullParser::close()
        {
            status_t res = STATUS_OK;

            cLast           = -1;
            sVersion.truncate();
            sEncoding.truncate();
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

        bool PullParser::is_encoding_first(lsp_swchar_t c)
        {
            if ((c >= 'a') && (c <= 'z'))
                return true;
            if ((c >= 'A') && (c <= 'Z'))
                return true;
            return false;
        }

        bool PullParser::is_encoding_char(lsp_swchar_t c)
        {
            if (is_encoding_first(c))
                return true;
            if ((c >= '0') && (c <= '9'))
                return true;

            return ((c == '_') || (c == '.') || (c == '-'));
        }

        lsp_swchar_t PullParser::getch()
        {
            if (cLast < 0)
                return pIn->read();

            lsp_swchar_t c = cLast;
            cLast = -1;
            return c;
        }

        void PullParser::ungetch(lsp_swchar_t ch)
        {
            cLast = ch;
        }

        bool PullParser::skip_spaces()
        {
            bool skipped = false;

            while (true)
            {
                // Read next character
                lsp_swchar_t c = getch();
                if (!is_whitespace(c))
                {
                    ungetch(c);
                    break;
                }
                skipped = true;
            }

            return skipped;
        }

        status_t PullParser::read_text(const char *text)
        {
            lsp_swchar_t c;
            for ( ; *text != '\0'; ++text)
            {
                if ((c = getch()) != *text)
                    return (c < 0) ? -c : STATUS_CORRUPTED;
            }
            return STATUS_OK;
        }

        status_t PullParser::read_name(LSPString *name)
        {
            // Get first character
            lsp_swchar_t c = getch();
            if (!(is_name_start(c)))
                return (c < 0) ? -c : STATUS_CORRUPTED;

            // Read name
            name->clear();
            do
            {
                // Append current character
                if (!name->append(c))
                    return STATUS_NO_MEM;

                // Get next character
                c = getch();
            } while (is_name_char(c));

            // Return back last character and return OK status
            ungetch(c);
            return STATUS_OK;
        }

        status_t PullParser::read_value(LSPString *value)
        {
            // Get first character
            lsp_swchar_t c = getch();
            if ((c != '\'') && (c != '\"'))
                return (c < 0) ? -c : STATUS_CORRUPTED;

            // Read quoted value
            value->clear();

            while (true)
            {
                lsp_swchar_t xc = getch();
                if (xc == c)
                    return STATUS_OK;
                else if (xc < 0)
                    return -xc;

                // Append current character
                if (!value->append(xc))
                    return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        status_t PullParser::read_attribute(LSPString *name, LSPString *value)
        {
            status_t res;
            lsp_swchar_t c;

            if ((res = read_name(name)) != STATUS_OK)
                return res;

            skip_spaces(); // Spaces are optional
            if ((c = getch()) != '=')
                return STATUS_CORRUPTED;

            skip_spaces(); // Spaces are optional
            if ((res = read_value(value)) != STATUS_OK)
                return res;

            return res;
        }

        status_t PullParser::parse_version(const LSPString *text)
        {
            if (!text->starts_with_ascii("1."))
                return STATUS_BAD_FORMAT;

            ssize_t n = text->length();
            if (n <= 0)
                return STATUS_CORRUPTED;

            ssize_t v = 0;
            for (size_t i=2; i<n; ++i)
            {
                lsp_wchar_t c = text->char_at(i);
                if ((c >= '0') && (c <= '9'))
                    v = v * 10 + (c - '0');
                else
                    return STATUS_CORRUPTED;
            }

            enVersion = (v >= 1) ? XML_VERSION_1_1 : XML_VERSION_1_0;
            if (!sVersion.set(text))
                return STATUS_NO_MEM;
            nFlags |= XF_VERSION;

            return STATUS_OK;
        }

        status_t PullParser::parse_encoding(const LSPString *text)
        {
            // Estimate length
            size_t n = text->length();
            if (n <= 0)
                return STATUS_BAD_FORMAT;

            // Check the first character
            if (!is_encoding_first(text->first()))
                return STATUS_BAD_FORMAT;

            // Check the remained characters
            for (size_t i=1; i<n; ++i)
                if (!is_encoding_char(text->char_at(i)))
                    return STATUS_BAD_FORMAT;

            if (!sEncoding.set(text))
                return STATUS_NO_MEM;

            nFlags |= XF_ENCODING;

            return STATUS_OK;
        }

        status_t PullParser::parse_standalone(const LSPString *text)
        {
            if (text->equals_ascii("yes"))
                nFlags |= XF_STANDALONE;
            else if (text->equals_ascii("no"))
                nFlags &= ~XF_STANDALONE;
            else
                return STATUS_BAD_FORMAT;

            return STATUS_OK;
        }

        status_t PullParser::read_header()
        {
            status_t res;
            lsp_swchar_t c;

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
                // Skip spaces and read next character
                bool skipped = skip_spaces();
                if ((c = getch()) < 0)
                    return -c;

                if (c == '?') // end of header?
                {
                    // Read next character
                    if ((c = getch()) != '>')
                        return (c < 0) ? -c : STATUS_CORRUPTED;
                    return read_start_document();
                }

                // At least one space is mandatory
                if (!skipped)
                    return STATUS_CORRUPTED;

                // Read attribute name
                ungetch(c);
                if ((res = read_attribute(&name, &value)) != STATUS_OK)
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

                // Check that attribute is at proper place
                if (flag <= flags)
                    return STATUS_CORRUPTED;
            }
        }

        status_t PullParser::read_comment()
        {
            lsp_swchar_t c, xc;
            sValue.clear();

            while (true)
            {
                // Fetch new character
                if ((c = getch()) < 0)
                    return -c;

                // Going to end of comment?
                if (c == '-')
                {
                    // Get next character
                    if ((xc = getch()) < 0)
                        return -xc;

                    // End of comment?
                    if (xc == '-')
                    {
                        // Next character should be '>'
                        if ((xc = getch()) != '>')
                            return (xc < 0) ? -xc : STATUS_CORRUPTED;
                        return STATUS_OK;
                    }

                    // Return character back
                    ungetch(xc);
                }

                if (!sValue.append(c))
                    return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        status_t PullParser::read_processing_instruction()
        {
            status_t res;

            // Read processing instruction name
            if ((res = read_name(&sName)) != STATUS_OK)
                return res;

            if (sName.equals_ascii_nocase("xml"))
            {
                if (nFlags & XF_HEADER)
                    return STATUS_CORRUPTED; // XML processing instruction is prohibited
                return read_header();
            }

            // Read processing instruction value
            lsp_swchar_t p = -1, c; // Previous character is undefined

            while (true)
            {
                // Fetch new character
                if ((c = getch()) < 0)
                    return -c;

                // Check character
                if ((c == '>') && (p == '?'))
                {
                    // Fully fetched comment
                    nToken = XT_PROCESSING_INSTRUCTION;
                    return STATUS_OK;
                }
                else
                {
                    if ((p == '-') && (!sValue.append(p)))
                        return STATUS_NO_MEM;
                    if (!sValue.append(c))
                        return STATUS_NO_MEM;
                }

                // Save last read chatacter
                p = c;
            }

            return STATUS_OK;
        }

        status_t PullParser::read_doctype()
        {
            // TODO
            return STATUS_OK;
        }

        status_t PullParser::read_start_document()
        {
            nToken  = XT_START_DOCUMENT;
            nFlags |= XF_HEADER;
            return STATUS_OK;
        }

        status_t PullParser::read_end_document()
        {
            nToken  = XT_END_DOCUMENT;
            nState  = PS_END_DOCUMENT;
            return STATUS_OK;
        }

        status_t PullParser::read_misc()
        {
            status_t res;
            lsp_swchar_t c;

            // Skip whitespace
            if (!(nFlags & XF_HEADER))
            {
                if (skip_spaces())
                    return read_start_document();
            }
            else
                skip_spaces();

            // Next character should be '<'
            if ((c = getch()) != '<')
            {
                if (c == -STATUS_EOF)
                    return read_end_document();
                return (c < 0) ? -c : STATUS_CORRUPTED;
            }

            // Get the following character
            if ((c = getch()) < 0)
                return -c;

            // Processing instruction?
            if (c == '?')
                return read_processing_instruction();
            else if (!(nFlags & XF_HEADER))
                return read_start_document();

            // Comment or Doctype?
            if (c == '!')
            {
                // Get next character
                if ((c = getch()) < 0)
                    return -c;

                if (c == '-') // Comment?
                {
                    // '<!--' should be parsed
                    if ((c = getch()) != '-')
                        return (c < 0) ? -c : STATUS_CORRUPTED;
                    return read_comment();
                }

                if (c == 'D') // Doctype?
                {
                    // 'DOCTYPE' should be parsed
                    if ((res = read_text("OCTYPE")) != STATUS_OK)
                        return res;
                    return read_doctype();
                }

                return STATUS_CORRUPTED;
            }

            // We already have root tag?
            if (nFlags & XF_ROOT)
                return STATUS_CORRUPTED;
            nFlags |= XF_ROOT;  // Now we already have root tag defined

            // Return character and read root tag name
            ungetch(c);
            return read_tag_open();
        }

        status_t PullParser::preprocess_value(LSPString *value)
        {
            // TODO
            return STATUS_OK;
        }

        status_t PullParser::read_cdata()
        {
            status_t res;



            // TODO
            return STATUS_OK;
        }

        status_t PullParser::read_tag_open()
        {
            status_t res;
            if ((res = read_name(&sName)) != STATUS_OK)
                return res;

            // Add tag to stack
            LSPString *tag = sName.clone();
            if (tag == NULL)
                return STATUS_NO_MEM;
            else if (!vTags.push(tag))
            {
                delete tag;
                return STATUS_NO_MEM;
            }

            // Change state
            nToken  = XT_START_ELEMENT;
            nState  = PS_READ_ATTRIBUTES;
            return STATUS_OK;
        }

        status_t PullParser::read_tag_close(bool copy)
        {
            // Get last tag name
            LSPString *name = NULL;
            if (!vTags.pop(&name))
                return STATUS_CORRUPTED;

            if (copy)
                sName.swap(name);
            else if ((!sName.equals(name)))
                return STATUS_CORRUPTED;

            delete name;

            // Update state
            nToken = XT_END_ELEMENT;
            nState = (vTags.size() > 0) ? PS_READ_ELEMENT_DATA : PS_READ_MISC;
            return STATUS_OK;
        }

        status_t PullParser::read_tag_content()
        {
            lsp_swchar_t c;
            status_t res;

            // Read character
            if ((c = getch()) < 0)
                return -c;

            // Tag? Processing instruction? End of tag? Comment? CDATA?
            if (c == '<')
            {
                // Get next character
                if ((c = getch()) < 0)
                    return -c;

                // Read tag name
                if (c == '/') // End of tag ?
                {
                    // Read tag name
                    if ((res = read_name(&sName)) != STATUS_OK)
                        return res;

                    // '>' is required
                    skip_spaces();
                    if ((c = getch()) != '>')
                        return (c < 0) ? -c : STATUS_CORRUPTED;

                    return read_tag_close(true);
                }
                else if (c == '?') // Processing instruction ?
                    return read_processing_instruction();
                else if (c == '!') // Comment? CDATA?
                {
                    // Get next character
                    if ((c = getch()) < 0)
                        return -c;

                    // CDATA?
                    if (c == '[')
                    {
                        // Lookup CDATA start
                        if ((res = read_text("CDATA[")) != STATUS_OK)
                            return res;
                        return read_cdata();
                    }

                    // Comment?
                    if (c == '-')
                    {
                        // Next character is required to be '-'
                        if ((c = getch()) != '-')
                            return (c < 0) ? -c : STATUS_CORRUPTED;
                        return read_comment();
                    }

                    // No match
                    return STATUS_CORRUPTED;
                }

                // Just open tag name?
                ungetch(c);
                if ((res = read_name(&sName)) != STATUS_OK)
                    return res;

                nToken      = XT_START_ELEMENT;
                nState      = PS_READ_ATTRIBUTES;
                return STATUS_OK;
            }

            // Entity reference?
            if (c == '&')
            {
            }

            // Character reference?
            if (c == '%')
            {
            }

            // Character data
            sValue.clear();

            // TODO: read character data


            return STATUS_OK;
        }

        status_t PullParser::read_tag_attribute()
        {
            status_t res;
            lsp_swchar_t c;

            // Ignore set of spaces if they are present
            bool skipped = skip_spaces();
            if ((c = getch()) < 0)
                return -c;

            // End of tag header?
            if (c == '>')
            {
                nState = PS_READ_ELEMENT_DATA;
                return read_tag_content();
            }

            // End of tag?
            if (c == '/')
            {
                // Required character
                if ((c = getch()) != '>')
                    return (c < 0) ? -c : STATUS_CORRUPTED;

                return read_tag_close(true);
            }

            // Try to read attribute and preprocess it's value
            if (!skipped) // At least one space is mandatory
                return STATUS_CORRUPTED;
            ungetch(c);
            if ((res = read_attribute(&sName, &sValue)) != STATUS_OK)
                return res;
            if ((res = preprocess_value(&sValue)) != STATUS_OK)
                return res;

            nToken = XT_ATTRIBUTE;
            return STATUS_OK;
        }

        status_t PullParser::read_token()
        {
            if (pIn == NULL)
                return STATUS_BAD_STATE;

            switch (nState)
            {
                case PS_END_DOCUMENT:
                    nToken          = XT_END_DOCUMENT;
                    return STATUS_EOF;

                case PS_READ_MISC:
                    return read_misc();

                case PS_READ_ATTRIBUTES:
                    return read_tag_attribute();

                case PS_READ_ELEMENT_DATA:
                    return read_tag_content();

                default:
                    break;
            }
            return STATUS_CORRUPTED;
        }

        status_t PullParser::read_next()
        {
            status_t res = read_token();
            return (res == STATUS_OK) ? nToken : -res;
        }

        status_t PullParser::get_current()
        {
            return nToken;
        }
    
    } /* namespace xml */
} /* namespace lsp */
