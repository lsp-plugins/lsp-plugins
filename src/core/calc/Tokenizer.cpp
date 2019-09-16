/*
 * Tokenizer.cpp
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: sadko
 */

#include <core/calc/Tokenizer.h>
#include <core/io/charset.h>

namespace lsp
{
    namespace calc
    {
        
        Tokenizer::Tokenizer(io::IInSequence *in)
        {
            pIn         = in;
            cUnget      = -1;
            enToken     = TT_UNKNOWN;
            nError      = STATUS_OK;
        }
        
        Tokenizer::~Tokenizer()
        {
            pIn         = NULL;
        }

        lsp_swchar_t Tokenizer::lookup()
        {
            if (cUnget >= 0)
                return cUnget;

            return cUnget = pIn->read();
        }

        token_t Tokenizer::commit(token_t token)
        {
            if (cUnget < 0)
            {
                nError          = STATUS_BAD_STATE;
                return enToken  = TT_ERROR;
            }

            if (!sValue.append(lsp_wchar_t(cUnget)))
            {
                nError          = STATUS_NO_MEM;
                return enToken  = TT_ERROR;
            }

            enToken     = token;
            cUnget      = -1;
            return token;
        }

        lsp_swchar_t Tokenizer::commit_lookup(token_t token)
        {
            token_t x   = commit(token);
            return (x != TT_ERROR) ? lookup() : -1;
        }

        void Tokenizer::lookup_identifier()
        {
            // TODO
        }

        bool Tokenizer::is_identifier_first(lsp_wchar_t ch)
        {
            if ((ch >= 'a') && (ch <= 'z'))
                return true;
            if ((ch >= 'A') && (ch <= 'Z'))
                return true;
            return ch == '_';
        }

        bool Tokenizer::is_identifier_next(lsp_wchar_t ch)
        {
            if ((ch >= 'a') && (ch <= 'z'))
                return true;
            if ((ch >= 'A') && (ch <= 'Z'))
                return true;
            if ((ch >= '0') && (ch <= '9'))
                return true;
            return ch == '_';
        }

        token_t Tokenizer::commit_word(lsp_wchar_t ch)
        {
            if ((enToken != TT_UNKNOWN) && (is_identifier_first(ch)))
                commit(TT_UNKNOWN);
            return enToken;
        }

        token_t Tokenizer::get_token(size_t flags)
        {
            // Pre-checks
            if (!(flags & TF_GET))
                return enToken;

            // Skip whitespaces
            lsp_swchar_t c;
            sValue.set_length(0);

            while (true)
            {
                // Lookup character
                c = lookup();

                // Skip whitespace
                if ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
                    continue;
                else if (c < 0)
                {
                    enToken = (c == -STATUS_EOF) ? TT_EOF : TT_ERROR;
                    nError  = -c;
                    return enToken;
                }
                else
                    break; // OK
            }

            // Now analyze first character
            switch (c)
            {
                // Non-alpha
                case '(': // TT_LBRACE
                    return commit(TT_LBRACE);
                case ')': // TT_RBRACE
                    return commit(TT_RBRACE);
                case '[': // TT_LQBRACE
                    return commit(TT_LQBRACE);
                case ']': // TT_RQBRACE
                    return commit(TT_RQBRACE);
                case '*': // TT_MUL, TT_POW
                {
                    commit(TT_MUL);
                    if ((c = lookup()) == '*') // Accept both variants: ^ and ^^
                        commit(TT_POW);
                    return enToken;
                }
                case '/': // TT_DIV
                    return commit(TT_DIV);
                case '%': // TT_MOD
                    return commit(TT_MOD);
                case '?': // TT_QUESTION
                    return commit(TT_QUESTION);
                case '&': // TT_AND
                    c = commit_lookup(TT_AND);
                    if (c == '&') // Accept both variants: & and &&
                        commit(TT_AND);
                    return enToken;

                case '|': // TT_OR
                    c = commit_lookup(TT_OR);
                    if (c == '|') // Accept both variants: | and ||
                        commit(TT_OR);
                    return enToken;

                case '!': // TT_NOT, TT_NOT_EQ
                    c = commit_lookup(TT_NOT);
                    if (c == '=') // Check for !=
                        commit(TT_NOT_EQ);
                    return enToken;

                case '^': // TT_XOR
                    c = commit_lookup(TT_XOR);
                    if (c == '^') // Accept both variants: ^ and ^^
                        commit(TT_XOR);
                    return enToken;

                case '<': // TT_LESS, TT_LESS_EQ, TT_NOT_EQ, TT_CMP
                    c = commit_lookup(TT_LESS);
                    if (c == '=') // <=
                    {
                        c = commit_lookup(TT_LESS_EQ);
                        if (c == '>')
                            commit(TT_CMP);
                    }
                    else if (c == '>') // <>
                        commit(TT_NOT_EQ);

                    return enToken;

                case '>': // TT_GREATER, TT_GREATER_EQ
                    c = commit_lookup(TT_GREATER);
                    if (c == '=') // Check for >=
                        commit(TT_GREATER_EQ);
                    return enToken;

                case '=': // TT_EQ
                    c = commit_lookup(TT_EQ);
                    if (c == '=') // Accept both variants: = and ==
                        commit(TT_EQ);
                    return enToken;

                case ';': // TT_SEMICOLON
                    return commit(TT_SEMICOLON);

                case ':': // TT_COLON, TT_IDENTIFIER
                {
                    c = commit(TT_COLON);
                    lookup_identifier();
                    return enToken;
                }

                // Alpha
                case 'a': case 'A': // TT_AND, TT_ADD
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'n') || (c == 'N'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'd') || (c == 'D'))           // AND
                            c = commit_lookup(TT_AND);
                    }
                    else if ((c == 'd') || (c == 'D'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'd') || (c == 'D'))           // ADD
                            c = commit_lookup(TT_ADD);
                    }
                    return commit_word(c);
                }

                case 'b': case 'B': // TT_BAND, TT_BNOT, TT_BOR, TT_BXOR
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'a') || (c == 'A'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'n') || (c == 'N'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 'd') || (c == 'D'))       // BAND
                                c = commit_lookup(TT_BAND);
                        }
                    }
                    else if ((c == 'n') || (c == 'N'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'o') || (c == 'O'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 't') || (c == 'T'))       // BNOT
                                c = commit_lookup(TT_BNOT);
                        }
                    }
                    else if ((c == 'o') || (c == 'O'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'r') || (c == 'R'))           // BOR
                            c = commit_lookup(TT_BOR);
                    }
                    else if ((c == 'x') || (c == 'X'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'o') || (c == 'O'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 'r') || (c == 'R'))       // BXOR
                                c = commit_lookup(TT_BXOR);
                        }
                    }
                    return commit_word(c);
                }

                case 'c': case 'C': // TT_CMP
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'm') || (c == 'M'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'p') || (c == 'P'))
                            c = commit_lookup(TT_CMP);          // CMP
                    }
                    return commit_word(c);
                }

                case 'd': case 'D': // TT_DIV
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'i') || (c == 'I'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'v') || (c == 'V'))           // DIV
                            c = commit_lookup(TT_DIV);
                    }
                    return commit_word(c);
                }

                case 'e': case 'E': // TT_EQ, TT_EX
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'q') || (c == 'Q'))               // EQ
                        c = commit_lookup(TT_EQ);
                    else if ((c == 'x') || (c == 'X'))          // EX
                        c = commit_lookup(TT_EX);
                    if ((enToken != TT_UNKNOWN) && (is_identifier_first(c)))
                        commit(TT_UNKNOWN);
                    return enToken;
                }

                case 'f': case 'F': // FALSE
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'a') && (c == 'A'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'l') && (c == 'L'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 's') && (c == 'S'))
                            {
                                c = commit_lookup(TT_UNKNOWN);
                                if ((c == 'e') && (c == 'E'))   // FALSE
                                    c = commit_lookup(TT_FALSE);
                            }
                        }
                    }
                    return commit_word(c);
                }

                case 'g': case 'G': // TT_GREATER, TT_GREATER_EQ
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 't') || (c == 'T'))               // GT
                        c = commit_lookup(TT_GREATER);
                    else if ((c == 'e') || (c == 'E'))          // GE
                        c = commit_lookup(TT_GREATER_EQ);
                    return commit_word(c);
                }

                case 'i': case 'I': // TT_IADD, TT_ISUB, TT_IMUL, TT_IDIV, TT_IMOD
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'a') || (c == 'A'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'd') || (c == 'D'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 'd') || (c == 'D'))
                                c = commit_lookup(TT_IADD);     // IADD
                        }
                    }
                    else if ((c == 'd') || (c == 'D'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'i') || (c == 'I'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 'V') || (c == 'V'))
                                c = commit_lookup(TT_IDIV);     // IDIV
                        }
                    }
                    else if ((c == 'e') || (c == 'E'))
                    {
                        c = commit_lookup(TT_IEQ);              // IE
                        if ((c == 'q') || (c == 'Q'))
                            c = commit_lookup(TT_IEQ);          // IEQ
                    }
                    else if ((c == 'g') || (c == 'G'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 't') || (c == 'T'))
                            c = commit_lookup(TT_IGREATER);     // IGT
                        else if ((c == 'e') || (c == 'T'))
                            c = commit_lookup(TT_IGREATER_EQ);  // IGE
                    }
                    else if ((c == 'l') || (c == 'L'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 't') || (c == 'T'))
                            c = commit_lookup(TT_ILESS);        // ILT
                        else if ((c == 'e') || (c == 'T'))
                            c = commit_lookup(TT_LESS_EQ);      // ILE
                    }
                    else if ((c == 'm') || (c == 'M'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'o') || (c == 'O'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 'd') || (c == 'D'))
                                c = commit_lookup(TT_MOD);      // IMOD
                        }
                        else if ((c == 'u') || (c == 'U'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 'l') || (c == 'L'))       // IMUL
                                c = commit_lookup(TT_MOD);
                        }
                    }
                    else if ((c == 'n') || (c == 'N'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'e') || (c == 'E'))
                            c = commit_lookup(TT_INOT_EQ);      // INE
                        else if ((c == 'g') || (c == 'G'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 't') || (c == 'T'))
                                c = commit_lookup(TT_ILESS_EQ); // INGT
                            else if ((c == 'e') || (c == 'T'))
                                c = commit_lookup(TT_ILESS);    // INGE
                        }
                        else if ((c == 'l') || (c == 'L'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 't') || (c == 'T'))
                                c = commit_lookup(TT_IGREATER_EQ); // INLT
                            else if ((c == 'e') || (c == 'T'))
                                c = commit_lookup(TT_IGREATER);    // INLE
                        }
                    }
                    else if ((c == 's') || (c == 'S'))
                    {
                        if ((c == 'u') || (c == 'U'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 'b') || (c == 'B'))
                                c = commit_lookup(TT_ISUB);     // ISUB
                        }
                    }
                    return commit_word(c);
                }

                case 'l': case 'L': // TT_LESS, TT_LESS_EQ
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 't') || (c == 'T'))
                        c = commit_lookup(TT_ILESS);        // ILT
                    else if ((c == 'e') || (c == 'T'))
                        c = commit_lookup(TT_ILESS_EQ);     // ILE
                    return commit_word(c);
                }

                case 'm': case 'M': // TT_MUL, TT_MOD
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'o') || (c == 'O'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'd') || (c == 'D'))
                            c = commit_lookup(TT_MOD);      // MOD
                    }
                    else if ((c == 'u') || (c == 'U'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'l') || (c == 'L'))       // MUL
                            c = commit_lookup(TT_MOD);
                    }
                    return commit_word(c);
                }

                case 'n': case 'N': // TT_NOT, TT_LESS, TT_GREATER, TT_LESS_EQ, TT_GREATER_EQ, TT_NOT_EQ
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'o') || (c == 'O'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 't') || (c == 'T'))           // NOT
                            c = commit_lookup(TT_NOT);
                    }
                    else if ((c == 'g') || (c == 'G'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 't') || (c == 'T'))           // NGT
                            c = commit_lookup(TT_LESS_EQ);
                        else if ((c == 'e') || (c == 'T'))      // NGE
                            c = commit_lookup(TT_LESS);
                    }
                    else if ((c == 'l') || (c == 'L'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 't') || (c == 'T'))           // NLT
                            c = commit_lookup(TT_GREATER_EQ);
                        else if ((c == 'e') || (c == 'T'))      // NLE
                            c = commit_lookup(TT_GREATER);
                    }
                    else if ((c == 'e') || (c == 'E'))          // NE
                        c = commit_lookup(TT_NOT_EQ);

                    return commit_word(c);
                }

                case 'o': case 'O': // TT_OR
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'r') || (c == 'R'))           // OR
                        c = commit_lookup(TT_OR);
                    return commit_word(c);
                }

                case 'p': case 'P': // TT_POW
                {
                    if ((c == 'o') || (c == 'O'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'w') || (c == 'W'))
                            c = commit_lookup(TT_POW);     // POW
                    }
                    return commit_word(c);
                }

                case 's': case 'S': // TT_SUB
                {
                    if ((c == 'u') || (c == 'U'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'b') || (c == 'B'))
                            c = commit_lookup(TT_SUB);     // SUB
                    }
                    return commit_word(c);
                }

                case 't': case 'T': // TRUE
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'r') && (c == 'R'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'u') && (c == 'U'))
                        {
                            c = commit_lookup(TT_UNKNOWN);
                            if ((c == 'e') && (c == 'E'))   // TRUE
                                c = commit_lookup(TT_FALSE);
                        }
                    }
                    return commit_word(c);
                }

                case 'x': case 'X': // TT_XOR
                {
                    c = commit_lookup(TT_UNKNOWN);
                    if ((c == 'o') || (c == 'O'))
                    {
                        c = commit_lookup(TT_UNKNOWN);
                        if ((c == 'r') || (c == 'R'))       // XOR
                            c = commit_lookup(TT_XOR);
                    }
                    return commit_word(c);
                }

                // Defaults
                default: // TT_VALUE
                {
                    // Lookup character
                    c = commit_lookup(TT_UNKNOWN);
                    if (flags & TF_XSIGN)
                    {
                        if (c == '+')
                            return commit(TT_ADD);
                        else if (c == '-')
                            return commit(TT_SUB);
                    }

                    // TODO:
                    // Try to parse float value
                    char *ep        = NULL;
                    errno           = 0;
                    t->fValue       = strtof(p, &ep);
                    if (errno != 0)
                    {
                        if (c == '+')
                            t->enType   = TT_ADD;
                        else if (c == '-')
                            t->enType   = TT_SUB;
                        else
                            t->enType    = TT_UNKNOWN;
                        return t->enType;
                    }

                    t->pStr     = ep;

                    // Lookup for the extra measurements
                    if (ep != NULL)
                    {
                        // Skip spaces
                        while (*ep == ' ')
                            ep++;
                        // Check data
                        if (((ep[0] == 'd') || (ep[0] == 'D')) &&
                            ((ep[1] == 'b') || (ep[1] == 'B')))
                        {
                            t->pStr     = &ep[2];
                            t->fValue   = expf(t->fValue * M_LN10 * 0.05);
                        }
                    }

                    return t->enType    = TT_VALUE;
                }
            }

            return t->enType    = TT_UNKNOWN;
        }

    } /* namespace calc */
} /* namespace lsp */
