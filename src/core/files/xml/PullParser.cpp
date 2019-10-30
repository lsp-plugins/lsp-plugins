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
            nStates     = 0;

            nUngetch    = 0;
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
            nStates         = 0;
            enVersion       = XML_VERSION_1_0;
            sVersion.truncate();
            sEncoding.truncate();
            sDoctype.truncate();
            sPublic.truncate();
            sSystem.truncate();
            nFlags          = 0;
            nUngetch        = 0;

            return STATUS_OK;
        }

        status_t PullParser::close()
        {
            status_t res = STATUS_OK;

            // Drop unnecessary resources
            nUngetch        = 0;
            sVersion.truncate();
            sEncoding.truncate();
            sName.truncate();
            sValue.truncate();
            sDoctype.truncate();
            sPublic.truncate();
            sSystem.truncate();
            nFlags          = 0;

            // Remove all tag hierarchy
            drop_list(&vTags);
            drop_list(&vAtts);

            // Release input sequence
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

        lsp_swchar_t PullParser::getch()
        {
            return (nUngetch > 0) ? vUngetch[--nUngetch] : pIn->read();
        }

        void PullParser::ungetch(lsp_swchar_t ch)
        {
            vUngetch[nUngetch++] = ch;
        }

        void PullParser::push_state(parse_state_t override)
        {
            vStates[nStates++]  = nState;
            nState              = override;
        }

        void PullParser::pop_state()
        {
            nState  = vStates[--nStates];
        }

        void PullParser::drop_list(cvector<LSPString> *list)
        {
            for (size_t i=0, n=list->size(); i<n; ++i)
            {
                LSPString *s = list->at(i);
                if (s != NULL)
                    delete s;
            }
            list->flush();
        }

        status_t PullParser::check_duplicate_attribute()
        {
            // Is item present in list?
            for (size_t i=0, n=vAtts.size(); i<n; ++i)
            {
                LSPString *s = vAtts.at(i);
                if ((s != NULL) && (s->equals(&sName)))
                    return STATUS_CORRUPTED;
            }

            // Add to list
            LSPString *copy = sName.clone();
            if (copy == NULL)
                return STATUS_NO_MEM;
            if (!vAtts.add(copy))
            {
                delete copy;
                return STATUS_NO_MEM;
            }

            return STATUS_OK;
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
            if (!(is_name_first(c)))
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
            } while (is_name_next(c));

            // Return back last character and return OK status
            ungetch(c);
            return STATUS_OK;
        }

        status_t PullParser::read_attribute_value(lsp_swchar_t qc)
        {
            lsp_swchar_t c;
            status_t res;

            while (true)
            {
                // Read character
                if ((c = getch()) < 0)
                {
                    pop_state();
                    return -c;
                }
                else if (c == qc)
                    break;

                // Reference?
                if (c == '&')
                {
                    // Read and append reference (if possible) to the string value
                    if ((res = read_entity_reference(&sValue)) != STATUS_OK)
                    {
                        pop_state();
                        return res;
                    }

                    // Need to query reference?
                    if (nState != PS_READ_REFERENCE)
                        continue;
                    return STATUS_OK; // Query for reference, do not need to pop_state()
                }

                // Append current character
                if (!sValue.append(c))
                {
                    pop_state();
                    return STATUS_NO_MEM;
                }
            }

            pop_state();
            nToken = XT_ATTRIBUTE;
            return STATUS_OK;
        }

        status_t PullParser::read_version()
        {
            // Get quote character
            lsp_swchar_t qc = getch();
            if ((qc != '\'') && (qc != '\"'))
                return (qc < 0) ? -qc : STATUS_CORRUPTED;

            // Version should be '1.x'
            lsp_swchar_t c;
            if ((c = getch()) != '1')
                return (c < 0) ? -c : STATUS_CORRUPTED;
            if ((c = getch()) != '.')
                return (c < 0) ? -c : STATUS_CORRUPTED;

            // Read integer value
            size_t v=0, k=0;
            while ((c = getch()) != qc)
            {
                if (v >= 0x1000000) // Prevent from integer overflow
                    return STATUS_CORRUPTED;

                if ((c >= '0') && (c <= '9'))
                    v = v * 10 + (c - '0');
                else
                    return (c < 0) ? -c : STATUS_CORRUPTED;
                ++k;
            }

            // Validate number of digits
            if (k <= 0)
                return STATUS_CORRUPTED;

            // Update version text
            if (!sVersion.fmt_ascii("1.%d", int(v)))
                return STATUS_NO_MEM;

            enVersion = (v >= 1) ? XML_VERSION_1_1 : XML_VERSION_1_0;
            nFlags |= XF_VERSION;

            return STATUS_OK;
        }

        status_t PullParser::read_encoding()
        {
            sEncoding.clear();

            // Get quote character
            lsp_swchar_t qc = getch();
            if ((qc != '\'') && (qc != '\"'))
                return (qc < 0) ? -qc : STATUS_CORRUPTED;

            // Read encoding char
            lsp_swchar_t c = getch();
            if (!is_encoding_first(c))
                return STATUS_BAD_FORMAT;
            if (!sEncoding.append(c))
                return STATUS_NO_MEM;

            // Check the remained characters
            while ((c = getch()) != qc)
            {
                if (!is_encoding_next(c))
                    return (c < 0) ? -c : STATUS_CORRUPTED;
                if (!sEncoding.append(c))
                    return STATUS_NO_MEM;
            }

            nFlags |= XF_ENCODING;

            return STATUS_OK;
        }

        status_t PullParser::read_standalone()
        {
            LSPString tmp;

            // Get quote character
            lsp_swchar_t qc = getch();
            if ((qc != '\'') && (qc != '\"'))
                return (qc < 0) ? -qc : STATUS_CORRUPTED;

            // Read quoted string
            lsp_swchar_t c;
            while ((c = getch()) != qc)
            {
                if (tmp.length() >= 3)
                    return STATUS_CORRUPTED;
                if (!tmp.append(c))
                    return STATUS_NO_MEM;
            }

            // Compare string with possible value
            if (tmp.equals_ascii("yes"))
                nFlags |= XF_STANDALONE;
            else if (tmp.equals_ascii("no"))
                nFlags &= ~XF_STANDALONE;
            else
                return STATUS_CORRUPTED;

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
                    return (flags & F_VERSION) ? read_start_document() : STATUS_CORRUPTED;
                }

                // At least one space is mandatory
                if (!skipped)
                    return STATUS_CORRUPTED;

                // Read attribute name
                ungetch(c);
                if ((res = read_name(&name)) != STATUS_OK)
                    return res;

                // Required '=' sign
                skip_spaces();
                if ((c = getch()) != '=')
                    return (c < 0) ? -c : STATUS_CORRUPTED;

                // Check attribute type
                size_t flag = 0;
                if (name.equals_ascii("version"))
                {
                    flag = F_VERSION;
                    if ((res = read_version()) != STATUS_OK)
                        return res;
                }
                else if (name.equals_ascii("encoding"))
                {
                    flag = F_ENCODING;
                    if ((res = read_encoding()) != STATUS_OK)
                        return res;
                }
                else if (name.equals_ascii("standalone"))
                {
                    flag = F_STANDALONE;
                    if ((res = read_standalone()) != STATUS_OK)
                        return res;
                }

                // Check that attribute is at proper place
                if (flag <= flags)
                    return STATUS_CORRUPTED;
                flags |= flag;
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
                    // End of comment?
                    if ((xc = getch()) == '-')
                    {
                        // Next character should be '>'
                        if ((xc = getch()) != '>')
                            return (xc < 0) ? -xc : STATUS_CORRUPTED;

                        nToken      = XT_COMMENT;
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
            lsp_swchar_t c;
            skip_spaces(); // Skip spaces

            sValue.clear();
            while (true)
            {
                // Fetch new character
                if ((c = getch()) < 0)
                    return -c;

                // PI end?
                if (c == '>')
                {
                    ssize_t pos = sValue.length() - 1;
                    if ((pos >= 0) && (sValue.char_at(pos) == '?'))
                        break;
                }

                // No, simple character
                if (!sValue.append(c))
                    return STATUS_NO_MEM;
            }

            // Remove last character which is '?'
            sValue.set_length(sValue.length() - 1);

            nToken = XT_PROCESSING_INSTRUCTION;
            return STATUS_OK;
        }

        status_t PullParser::read_system_literal(LSPString *dst)
        {
            LSPString tmp;

            // Get quote character
            lsp_swchar_t qc = getch();
            if ((qc != '\'') && (qc != '\"'))
                return (qc < 0) ? -qc : STATUS_CORRUPTED;

            // Read quoted string
            lsp_swchar_t c;
            while ((c = getch()) != qc)
            {
                if (!tmp.append(c))
                    return STATUS_NO_MEM;
            }

            dst->swap(&tmp);
            return STATUS_OK;
        }

        status_t PullParser::read_pubid_literal(LSPString *dst)
        {
            LSPString tmp;

            // Get quote character
            lsp_swchar_t qc = getch();
            if ((qc != '\'') && (qc != '\"'))
                return (qc < 0) ? -qc : STATUS_CORRUPTED;

            // Read quoted string
            lsp_swchar_t c;
            while ((c = getch()) != qc)
            {
                if ((!is_pubid_char(c)) || (c == qc))
                    return STATUS_CORRUPTED;
                if (!tmp.append(c))
                    return STATUS_NO_MEM;
            }

            dst->swap(&tmp);
            return STATUS_OK;
        }

        status_t PullParser::read_doctype()
        {
            status_t res;
            lsp_swchar_t c;
            LSPString x;

            // Duplicate DOCTYPE?
            if (nFlags & XF_DOCTYPE)
                return STATUS_CORRUPTED;

            // Space is required
            if (!skip_spaces())
                return STATUS_CORRUPTED;
            if ((res = read_name(&sDoctype)) != STATUS_OK)
                return res;

            // Watch next token
            nFlags |= XF_DOCTYPE;
            bool skip = skip_spaces();
            if ((c = getch()) < 0)
                return -c;

            // ExternalID is present?
            if (c == 'P')
            {
                if (!skip)
                    return STATUS_CORRUPTED;
                if ((res = read_text("UBLIC")) != STATUS_OK)
                    return res;
                if (!skip_spaces())
                    return STATUS_CORRUPTED;
                if ((res = read_pubid_literal(&sPublic)) != STATUS_OK)
                    return res;
                nFlags |= XF_DOCTYPE_PUB;
                if (!skip_spaces())
                    return STATUS_CORRUPTED;
                if ((res = read_system_literal(&sSystem)) != STATUS_OK)
                    return res;
                nFlags |= XF_DOCTYPE_SYS;

                // Skip spaces and get next token
                skip_spaces();
                if ((c = getch()) < 0)
                    return -c;
            }
            else if (c == 'S')
            {
                if (!skip)
                    return STATUS_CORRUPTED;
                if ((res = read_text("YSTEM")) != STATUS_OK)
                    return res;
                if (!skip_spaces())
                    return STATUS_CORRUPTED;
                if ((res = read_system_literal(&sSystem)) != STATUS_OK)
                    return res;
                nFlags |= XF_DOCTYPE_SYS;

                // Skip spaces and get next token
                skip_spaces();
                if ((c = getch()) < 0)
                    return -c;
            }

            // intSubset?
            if (c == '[')
            {
                // TODO: currently we don't support DOCTYPE definition with built-in doctypes
                return STATUS_NOT_IMPLEMENTED;
            }

            // End of Doctype?
            nToken      = XT_DTD;
            return (c == '>') ? STATUS_OK : STATUS_CORRUPTED;
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
                    return (nFlags & XF_HEADER) ? read_end_document() : read_start_document();
                return (c < 0) ? -c : STATUS_CORRUPTED;
            }

            // Get the following character
            if ((c = getch()) < 0)
                return -c;

            // Processing instruction?
            if (c == '?')
                return read_processing_instruction();
            else if (!(nFlags & XF_HEADER))
            {
                ungetch(c);
                ungetch('<');
                return read_start_document();
            }

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

        status_t PullParser::read_cdata()
        {
            lsp_swchar_t c;

            sValue.clear();

            while (true)
            {
                // Get next character
                if ((c = getch()) < 0)
                    return -c;

                // CDATA end?
                if (c == '>')
                {
                    ssize_t pos = sValue.length() - 2;
                    if (
                        (pos >= 0) &&
                        (sValue.char_at(pos) == ']') &&
                        (sValue.char_at(pos+1) == ']')
                    )
                        break;
                }

                // No, simple character
                if (!sValue.append(c))
                    return STATUS_NO_MEM;
            }

            // Remove last two characters which are ']]'
            sValue.set_length(sValue.length() - 2);

            nToken      = XT_CDATA;
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
            drop_list(&vAtts);
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
            else if (!sName.equals(name))
            {
                delete name;
                return STATUS_CORRUPTED;
            }
            delete name;

            // Update state
            drop_list(&vAtts);
            nToken = XT_END_ELEMENT;
            nState = (vTags.size() > 0) ? PS_READ_ELEMENT_DATA : PS_READ_MISC;
            return STATUS_OK;
        }

        status_t PullParser::read_entity_reference(LSPString *cdata)
        {
            lsp_swchar_t c, code = 0;
            status_t res;

            // Get character
            if ((c = getch()) < 0)
                return -c;

            // Entity reference ?
            if (c != '#')
            {
                ungetch(c);

                // Read entity name
                if ((res = read_name(&sRefName)) != STATUS_OK)
                    return res;

                if (sRefName.equals_ascii("amp"))
                    code    = '&';
                else if (sRefName.equals_ascii("gt"))
                    code    = '>';
                else if (sRefName.equals_ascii("lt"))
                    code    = '<';
                else if (sRefName.equals_ascii("apos"))
                    code    = '\'';
                else if (sRefName.equals_ascii("quot"))
                    code    = '\"';

                // Get next character which should be ';'
                if ((c = getch()) < 0)
                    return -c;
            }
            else
            {
                // Get next character
                if ((c = getch()) < 0)
                    return -c;

                // Hexadecimal character?
                if (c == 'x')
                {
                    // Read hex digit
                    while ((c = getch()) >= 0)
                    {
                        // Protect from integer overflow
                        if (code >= 0x1000000)
                            return STATUS_CORRUPTED;

                        // Decode hex character
                        if ((c >= '0') && (c <= '9'))
                            code = (code << 4) | (c - '0');
                        else if ((c >= 'a') && (c <= 'f'))
                            code = (code << 4) | (c - 'a' + 10);
                        else if ((c >= 'A') && (c <= 'F'))
                            code = (code << 4) | (c - 'A' + 10);
                        else
                            break;
                    }
                }
                else // Decimal character?
                {
                    do
                    {
                        // Protect from integer overflow
                        if (code >= 0x1000000)
                            return STATUS_CORRUPTED;

                        // Decode decimal character
                        if ((c >= '0') && (c <= '9'))
                            code = (code * 10) + (c - '0');
                        else
                            break;
                    } while ((c = getch()) >= 0);
                }

                // Validate character
                if (!is_valid_char(code, enVersion))
                    return STATUS_CORRUPTED;
            }

            // Current character should be ';'
            if (c != ';')
                return STATUS_CORRUPTED;
            else if (code == 0)
            {
                push_state(PS_READ_REFERENCE);
                nToken      = XT_ENTITY_RESOLVE;
                return STATUS_OK;
            }

            // Append fetched character to the character data and exit
            return (cdata->append(code)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t PullParser::read_characters()
        {
            lsp_swchar_t c;
            status_t res;

            while (true)
            {
                // Get next character
                if ((c = getch()) < 0)
                {
                    pop_state();
                    return -c;
                }

                // Start of tag?
                if (c == '<')
                {
                    ungetch(c);
                    break;
                }

                // Reference?
                if (c == '&')
                {
                    // Read and append reference (if possible) to the string value
                    if ((res = read_entity_reference(&sValue)) != STATUS_OK)
                    {
                        pop_state();
                        return res;
                    }

                    // Need to query reference?
                    if (nState != PS_READ_REFERENCE)
                        continue;
                    return STATUS_OK; // Query for reference, do not need to pop_state()
                }

                // CDATA end?
                if (c == '>')
                {
                    ssize_t pos = sValue.length() - 2;
                    if (
                        (pos >= 0) &&
                        (sValue.char_at(pos) == ']') &&
                        (sValue.char_at(pos+1) == ']')
                    )
                    {
                        pop_state();
                        return STATUS_CORRUPTED;
                    }
                }

                // No, simple character
                if (!sValue.append(c))
                {
                    pop_state();
                    return STATUS_NO_MEM;
                }
            }

            // Ensure that there is character data
            pop_state();

            if (sValue.length() <= 0)
                return STATUS_CORRUPTED;

            nToken      = XT_CHARACTERS;
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
            if (c != '<')
            {
                ungetch(c);
                sValue.clear();
                push_state(PS_READ_CHARACTERS);
                return read_characters();
            }

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

                return read_tag_close(false);
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
            return read_tag_open();
        }

        status_t PullParser::read_tag_attribute()
        {
            lsp_swchar_t c;
            status_t res;

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

            // Read attribute name
            ungetch(c);
            if ((res = read_name(&sName)) != STATUS_OK)
                return res;
            else if (check_duplicate_attribute())
                return STATUS_CORRUPTED;

            skip_spaces(); // Spaces are optional
            if ((c = getch()) != '=')
                return STATUS_CORRUPTED;

            skip_spaces(); // Spaces are optional
            c = getch(); // Get quote character
            if ((c != '\'') && (c != '\"'))
                return (c < 0) ? -c : STATUS_CORRUPTED;

            // Read quoted value
            sValue.clear();
            push_state((c == '\'') ? PS_READ_SQ_ATTRIBUTE : PS_READ_DQ_ATTRIBUTE);
            return read_attribute_value(c);
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

                case PS_READ_REFERENCE:
                    nToken          = XT_ENTITY_RESOLVE;
                    return STATUS_OK;

                case PS_READ_CHARACTERS:
                    return read_characters();

                case PS_READ_SQ_ATTRIBUTE:
                    return read_attribute_value('\'');

                case PS_READ_DQ_ATTRIBUTE:
                    return read_attribute_value('\"');

                default:
                    break;
            }
            return STATUS_CORRUPTED;
        }

        status_t PullParser::set_value(const LSPString *value)
        {
            if (pIn == NULL)
                return STATUS_BAD_STATE;
            else if (value == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (nState != PS_READ_REFERENCE)
                return STATUS_BAD_STATE;

            // Append value with entity content
            if (!sValue.append(value))
                return STATUS_NO_MEM;

            pop_state();
            return STATUS_OK;
        }

        status_t PullParser::resolve_entity(const char *value, const char *charset)
        {
            LSPString tmp;
            if (!tmp.set_native(value, charset))
                return STATUS_NO_MEM;
            return resolve_entity(value);
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

        const LSPString *PullParser::name() const
        {
            if (pIn == NULL)
                return NULL;

            switch (nToken)
            {
                case XT_ATTRIBUTE:
                case XT_PROCESSING_INSTRUCTION:
                case XT_START_ELEMENT:
                case XT_END_ELEMENT:
                    return &sName;
                case XT_ENTITY_RESOLVE:
                    return &sRefName;
                default:
                    break;
            }
            return NULL;
        }

        const LSPString *PullParser::value() const
        {
            if (pIn == NULL)
                return NULL;

            switch (nToken)
            {
                case XT_ATTRIBUTE:
                case XT_CDATA:
                case XT_CHARACTERS:
                case XT_COMMENT:
                case XT_PROCESSING_INSTRUCTION:
                    return &sValue;
                default:
                    break;
            }
            return NULL;
        }

    } /* namespace xml */
} /* namespace lsp */
