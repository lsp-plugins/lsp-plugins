/*
 * Tokenizer.cpp
 *
 *  Created on: 14 окт. 2019 г.
 *      Author: sadko
 */

#include <wctype.h>
#include <math.h>
#include <core/files/json/Tokenizer.h>

namespace lsp
{
    namespace json
    {
        static const char *ecma_reserved[] =
        {
            "break",
            "case",
            "catch",
            "continue",
            "debugger",
            "default",
            "delete",
            "instanceof",
            "else",
            "finally",
            "for",
            "function",
            "if",
            "in",
            "typeof",
            "new",
            "return",
            "switch",
            "this",
            "throw",
            "try",
            "var",
            "void",
            "while",
            "with"
        };
        
        Tokenizer::Tokenizer(io::IInSequence *in)
        {
            pIn         = in;
            cCurrent    = -1;
            enToken     = JT_UNKNOWN;
            nError      = STATUS_OK;
            fValue      = 0;
            iValue      = 0;
            nUnget      = 0;
            vPending    = NULL;
            nPending    = 0;
            nCapacity   = 0;
        }
        
        Tokenizer::~Tokenizer()
        {
            pIn         = NULL;
            if (vPending != NULL)
            {
                ::free(vPending);
                vPending = NULL;
            }
            nPending    = 0;
            nCapacity   = 0;
        }

        token_t Tokenizer::set_error(status_t code)
        {
            nError          = code;
            return enToken  = JT_ERROR;
        }

        lsp_swchar_t Tokenizer::skip_whitespace()
        {
            if (cCurrent < 0)
                cCurrent = pIn->read();

            while (true)
            {
                // Skip whitespace
                if (::iswspace(cCurrent) || (::iswblank(cCurrent)))
                    cCurrent = pIn->read();
                else
                    return cCurrent;
            }
        }

        lsp_swchar_t Tokenizer::lookup()
        {
            if (cCurrent < 0)
                cCurrent = pIn->read();
            return cCurrent;
        }

        token_t Tokenizer::commit(token_t token)
        {
            if (cCurrent < 0)
                return set_error(STATUS_BAD_STATE);

            if (!sValue.append(cCurrent))
                return set_error(STATUS_NO_MEM);

            enToken     = token;
            cCurrent      = -1;
            return token;
        }

        lsp_swchar_t Tokenizer::commit_lookup(token_t token)
        {
            token_t x   = commit(token);
            return (x != JT_ERROR) ? lookup() : -1;
        }

        bool Tokenizer::is_identifier_start(lsp_wchar_t ch)
        {
            if (::iswlower(ch))
                return true;
            if (::iswupper(ch))
                return true;
            return (ch == '_') || (ch == '$') || (ch == '\\');
        }

        bool Tokenizer::is_identifier(lsp_wchar_t ch)
        {
            if (::iswlower(ch))
                return true;
            if (::iswupper(ch))
                return true;
            if (::iswdigit(ch))
                return true;
            return (ch == '_') || (ch == '$') || (ch == '\\');
        }

        bool Tokenizer::is_reserved_word(const LSPString *text)
        {
            size_t first = 0, last = sizeof(ecma_reserved) / sizeof (const char *) - 1;
            while (first <= last)
            {
                ssize_t center = (first + last) >> 1;
                int cmp = text->compare_to_ascii(ecma_reserved[center]);
                if (cmp < 0)
                    last = center - 1;
                else if (cmp > 0)
                    first = center + 1;
                else
                    return true;
            }

            return false;
        }

        bool Tokenizer::parse_digit(int *digit, lsp_wchar_t ch, int radix)
        {
            int res;
            if ((ch >= '0') && (ch <= '9'))
                res = ch - '0';
            else if ((ch >= 'a') && (ch <= 'f'))
                res = ch - 'a' + 10;
            else if ((ch >= 'A') && (ch <= 'F'))
                res = ch - 'A' + 10;
            else if (ch == '_')
                res = -1;
            else
                return false;

            if (res >= radix)
                return false;

            *digit  = res;
            return true;
        }

        status_t Tokenizer::add_pending_character(lsp_utf16_t ch)
        {
            if (nPending >= nCapacity)
            {
                size_t cap  = ((nPending + 0x10) & ~0xf) + nCapacity;
                lsp_utf16_t *buf = reinterpret_cast<lsp_utf16_t *>(::realloc(vPending, sizeof(lsp_utf16_t) * cap));
                if (buf == NULL)
                    return STATUS_NO_MEM;
                vPending    = buf;
            }

            vPending[nPending++] = ch;
            return STATUS_OK;
        }

        status_t Tokenizer::commit_pending_characters()
        {
            // Are there any pending characters?
            if (nPending <= 0)
                return STATUS_OK;

            // Initialize temporary string
            LSPString tmp;
            bool res = tmp.set_utf16(vPending, nPending);
            nPending = 0;
            if (!res)
                return STATUS_BAD_FORMAT;

            // Append string to result
            return (sValue.append(&tmp)) ? STATUS_OK : STATUS_NO_MEM;
        }

        token_t Tokenizer::parse_string(token_t type)
        {
            sValue.clear();
            skip(type);

            status_t res;
            bool protector  = false;

            // Parse string
            while (true)
            {
                // Read character
                lsp_swchar_t c = lookup();
                if (c < 0)
                    return set_error(-c);

                if (protector)
                {
                    protector = false;

                    if ((c == 'u') || (c == 'U'))
                    {
                        token_t tok = parse_unicode_escape_sequence(type);
                        if (tok == JT_ERROR)
                            return tok;
                    }
                    else
                    {
                        // Commit previously pending unicode characters
                        if ((res = commit_pending_characters()) != STATUS_OK)
                            return set_error(res);

                        switch (c)
                        {
                            // Escaped characters: ' " \ b f n r t v
                            case 'b': c = '\b'; break;
                            case 'f': c = '\f'; break;
                            case 'n': c = '\n'; break;
                            case 'r': c = '\r'; break;
                            case 't': c = '\t'; break;
                            case 'v': c = '\v'; break;
                            default: // Any other characters just omit the protector character in ECMA script
                                break;
                        }

                        if (!sValue.append(c))
                            return set_error(STATUS_NO_MEM);
                    }
                }
                else
                {
                    // Commit previously pending unicode characters
                    if ((res = commit_pending_characters()) != STATUS_OK)
                        return set_error(res);

                    switch (c)
                    {
                        case '\\':
                            protector   = true;
                            break;
                        case '\"':
                            if (type == JT_DQ_STRING)
                                return skip(type);
                            if (!sValue.append(c))
                                return set_error(STATUS_NO_MEM);
                            break;
                        case '\'':
                            if (type == JT_SQ_STRING)
                                return skip(type);
                            if (!sValue.append(c))
                                return set_error(STATUS_NO_MEM);
                            break;
                        default:
                            if (!sValue.append(cCurrent))
                                return set_error(STATUS_NO_MEM);
                            break;
                    }
                }
            }

            return enToken = type;
        }

        token_t Tokenizer::parse_unicode_escape_sequence(token_t type)
        {
            // Seek for 'u' or 'U'
            lsp_swchar_t c = lookup();
            if (c < 0)
            {
                if (c != -STATUS_EOF)
                    return set_error(-c);
                break;
            }
            else if ((c != 'u') && (c != 'U'))
                return set_error(STATUS_BAD_TOKEN);
            skip(type);

            // Read 4 mandatory digits
            int digit = 0;
            lsp_utf16_t cp = 0;
            for (size_t i=0; i<4; ++i)
            {
                c = lookup();
                if (c < 0)
                    return set_error(-c);

                skip(type);
                if (!parse_digit(&digit, c, 16))
                    return set_error(STATUS_BAD_TOKEN);

                cp = (cp << 16) + digit;
            }

            // All is fine, store, truncate value and return result
            status_t res = add_pending_character(cp);
            return (res != STATUS_OK) ? set_error(res) : enToken = type;
        }

        token_t Tokenizer::parse_identifier()
        {
            // Commit identifier's start character
            status_t res;
            token_t tok = commit(JT_IDENTIFIER);
            if (tok == JT_ERROR)
                return tok;

            while (true)
            {
                // Read character
                lsp_swchar_t c = lookup();
                if (c < 0)
                {
                    if (c != -STATUS_EOF)
                        return set_error(-c);

                    // Commit pending characters first
                    res = commit_pending_characters();
                    if (res != STATUS_OK)
                        return set_error(res);
                    break;
                }

                // Check that character is an identifier
                if (!is_identifier(c))
                {
                    if (c != '\\')
                        break;

                    skip(JT_IDENTIFIER);
                    tok     = parse_unicode_escape_sequence(JT_IDENTIFIER);
                }
                else
                {
                    // Commit pending unicode characters first
                    res = commit_pending_characters();
                    if (res != STATUS_OK)
                        return set_error(res);

                    // Now, commit pending character
                    tok = commit(JT_IDENTIFIER);
                }

                if (tok == JT_ERROR)
                    return tok;
            }

            // Analyze identifier
            if (sValue.equals_ascii("true"))
                return enToken = JT_TRUE;
            else if (sValue.equals_ascii("false"))
                return enToken = JT_FALSE;
            else if (sValue.equals_ascii("null"))
                return enToken = JT_NULL;
            else if (sValue.equals_ascii("NaN"))
            {
                fValue  = NAN;
                return enToken = JT_DOUBLE;
            }
            else if (sValue.equals_ascii("Infinity"))
            {
                fValue  = INFINITY;
                return enToken = JT_DOUBLE;
            }
            else if (is_reserved_word(&sValue))
                return enToken = JT_RESERVED;

            return enToken = JT_IDENTIFIER;
        }

        token_t Tokenizer::parse_single_line_comment()
        {
            // Reset the length of string literal
            sValue.set_length(0);
            skip(JT_SL_COMMENT);

            while (true)
            {
                // Read character
                lsp_swchar_t c = lookup();
                if (c < 0)
                    return (c == -STATUS_EOF) ? JT_SL_COMMENT : set_error(-c);

                // Analyze character
                switch (c) {
                    case '\n':
                        return skip(JT_SL_COMMENT);
                    default:
                    {
                        token_t res = commit(JT_SL_COMMENT);
                        if (res == JT_ERROR)
                            return res;
                        break;
                    }
                }
            }

            return JT_UNKNOWN;
        }

        token_t Tokenizer::parse_multiline_comment()
        {
            // Reset the length of string literal
            sValue.set_length(0);
            skip(JT_SL_COMMENT);

            bool asterisk = false;
            while (true)
            {
                // Read character
                lsp_swchar_t c = lookup();
                if (c < 0)
                    return set_error(-c);

                // Check state
                if (asterisk)
                {
                    if (c == '/')
                    {
                        // Remove last stored character (asterisk)
                        sValue.remove_last();
                        return commit(JT_ML_COMMENT);
                    }
                }

                // Append current character to the comment
                token_t res = commit(JT_ML_COMMENT);
                if (res == JT_ERROR)
                    return res;

                // Last character was asterisk?
                if (c == '*')
                    asterisk = true;
            }

            return JT_UNKNOWN;
        }

        token_t Tokenizer::parse_number()
        {
            // TODO
            return JT_UNKNOWN;
        }

        token_t Tokenizer::get_token(bool get)
        {
            // Pre-checks
            if (!get)
                return enToken;
            else if (nUnget > 0)
            {
                --nUnget;
                return enToken;
            }

            // Skip whitespaces
            lsp_swchar_t c = skip_whitespace();
            if (c < 0)
            {
                enToken = (c == -STATUS_EOF) ? JT_EOF : JT_ERROR;
                nError  = -c;
                return enToken;
            }

            sValue.set_length(0);

            switch (c)
            {
                case '[': return commit(JT_LQ_BRACE);
                case ']': return commit(JT_RQ_BRACE);
                case '{': return commit(JT_LC_BRACE);
                case '}': return commit(JT_RC_BRACE);
                case ':': return commit(JT_COLON);
                case ',': return commit(JT_COMMA);
                case '\'': return parse_string(JT_SQ_STRING);
                case '\"': return parse_string(JT_DQ_STRING);
                case '/':
                    c = commit_lookup(JT_UNKNOWN);
                    if (c == '/')
                        return parse_single_line_comment();
                    else if (c == '*')
                        return parse_multiline_comment();
                    commit(JT_UNKNOWN);
                    break;

                default:
                    if (is_identifier_start(c))
                        return parse_identifier();
                    return parse_number();
            }

            return enToken = JT_UNKNOWN;
        }
    
    } /* namespace json */
} /* namespace lsp */
