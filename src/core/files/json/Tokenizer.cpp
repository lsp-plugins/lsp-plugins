/*
 * Tokenizer.cpp
 *
 *  Created on: 14 окт. 2019 г.
 *      Author: sadko
 */

#include <wctype.h>
#include <core/files/json/Tokenizer.h>

namespace lsp
{
    namespace json
    {
        
        Tokenizer::Tokenizer(io::IInSequence *in)
        {
            pIn         = in;
            cCurrent    = -1;
            enToken     = JT_UNKNOWN;
            nError      = STATUS_OK;
            fValue      = 0;
            iValue      = 0;
            nUnget      = 0;
        }
        
        Tokenizer::~Tokenizer()
        {
            pIn         = NULL;
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

        token_t Tokenizer::parse_string(token_t type)
        {
            // TODO
            return JT_UNKNOWN;
        }

        token_t Tokenizer::parse_identifier()
        {
            // TODO
            return JT_UNKNOWN;
        }

        token_t Tokenizer::parse_single_line_comment()
        {
            // TODO
            return JT_UNKNOWN;
        }

        token_t Tokenizer::parse_multiline_comment()
        {
            // TODO
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
