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
        typedef struct bareword_t {
            const char     *text;
            token_t         token;
        } bareword_t;

        static const bareword_t barewords[] =
        {
            { "add",        TT_ADDSYM       },
            { "and",        TT_AND          },
            { "band",       TT_BAND         },
            { "bnot",       TT_BNOT         },
            { "bool",       TT_BOOL         },
            { "bor",        TT_BOR          },
            { "bxor",       TT_BXOR         },
            { "cmp",        TT_CMP          },
            { "db",         TT_DB           },
            { "div",        TT_DIV          },
            { "eq",         TT_EQ           },
            { "ex",         TT_EX           },
            { "false",      TT_FALSE        },
            { "float",      TT_FLOAT        },
            { "fmod",       TT_FMOD         },
            { "fp",         TT_FLOAT        },
            { "ge",         TT_GREATER_EQ   },
            { "gt",         TT_GREATER      },
            { "iadd",       TT_IADD         },
            { "icmp",       TT_ICMP         },
            { "idiv",       TT_IDIV         },
            { "ie",         TT_IEQ          },
            { "ieq",        TT_IEQ          },
            { "ige",        TT_IGREATER_EQ  },
            { "igt",        TT_IGREATER     },
            { "ile",        TT_ILESS_EQ     },
            { "ilt",        TT_ILESS        },
            { "imod",       TT_IMOD         },
            { "imul",       TT_IMUL         },
            { "ine",        TT_INOT_EQ      },
            { "inge",       TT_ILESS        },
            { "ingt",       TT_ILESS_EQ     },
            { "inle",       TT_IGREATER     },
            { "inlt",       TT_IGREATER_EQ  },
            { "int",        TT_INT          },
            { "isub",       TT_ISUB         },
            { "lc",         TT_SLWR         },
            { "le",         TT_LESS_EQ      },
            { "lt",         TT_LESS         },
            { "mod",        TT_IMOD         },
            { "mul",        TT_MUL          },
            { "ne",         TT_NOT_EQ       },
            { "nge",        TT_LESS         },
            { "ngt",        TT_LESS_EQ      },
            { "nle",        TT_GREATER      },
            { "nlt",        TT_GREATER_EQ   },
            { "not",        TT_NOT          },
            { "null",       TT_NULL         },
            { "or",         TT_OR           },
            { "pow",        TT_POW          },
            { "sc",         TT_SCAT         },
            { "scat",       TT_SCAT         },
            { "sl",         TT_SLEN         },
            { "slen",       TT_SLEN         },
            { "slwr",       TT_SLWR         },
            { "sr",         TT_SREP         },
            { "srep",       TT_SREP         },
            { "srev",       TT_SREV         },
            { "str",        TT_STR          },
            { "sub",        TT_SUBSYM       },
            { "supr",       TT_SUPR         },
            { "true",       TT_TRUE         },
            { "uc",         TT_SUPR         },
            { "undef",      TT_UNDEF        },
            { "xor",        TT_XOR          }
        };
        
        Tokenizer::Tokenizer(io::IInSequence *in)
        {
            pIn         = in;
            cCurrent    = -1;
            enToken     = TT_UNKNOWN;
            nError      = STATUS_OK;
            fValue      = 0;
            iValue      = 0;
            nUnget      = 0;
        }
        
        Tokenizer::~Tokenizer()
        {
            pIn         = NULL;
        }

        lsp_swchar_t Tokenizer::lookup()
        {
            if (cCurrent < 0)
                cCurrent = pIn->read();
            return cCurrent;
        }

        token_t Tokenizer::set_error(status_t code)
        {
            nError          = code;
            return enToken  = TT_ERROR;
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
            return (x != TT_ERROR) ? lookup() : -1;
        }

        lsp_swchar_t Tokenizer::skip_whitespace()
        {
            if (cCurrent < 0)
                cCurrent = pIn->read();

            while (true)
            {
                // Skip whitespace
                switch (cCurrent)
                {
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\r':
                        cCurrent = pIn->read();
                        continue;
                    default:
                        return cCurrent;
                }
            }
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

        token_t Tokenizer::lookup_identifier(token_t type)
        {
            if (cCurrent < 0)
                cCurrent = pIn->read();

            if (!is_identifier_first(cCurrent))
                return enToken;

            sValue.clear();
            do
            {
                // Append character
                if (!sValue.append(cCurrent))
                    return set_error(STATUS_NO_MEM);

                // Read next character
                if ((cCurrent = pIn->read()) < 0)
                {
                    if (cCurrent == -STATUS_EOF)
                        break;
                    return set_error(-cCurrent);
                }
            } while (is_identifier_next(cCurrent));

            return enToken = type;
        }

        token_t Tokenizer::lookup_string()
        {
            sValue.clear();

            while (cCurrent == '\'')
            {
                bool protector  = false;
                bool scan       = true;

                // Parse string
                do
                {
                    // Read character
                    if ((cCurrent = pIn->read()) < 0)
                        return set_error(-cCurrent);

                    if (protector)
                    {
                        switch (cCurrent)
                        {
                            case '\\':
                                if (!sValue.append('\\'))
                                    return set_error(STATUS_NO_MEM);
                                break;
                            case 'n':
                                if (!sValue.append('\n'))
                                    return set_error(STATUS_NO_MEM);
                                break;
                            case 't':
                                if (!sValue.append('\t'))
                                    return set_error(STATUS_NO_MEM);
                                break;
                            case 'r':
                                if (!sValue.append('\r'))
                                    return set_error(STATUS_NO_MEM);
                                break;
                            case '\'':
                                if (!sValue.append('\''))
                                    return set_error(STATUS_NO_MEM);
                                break;
                            default:
                                if (!sValue.append('\\'))
                                    return set_error(STATUS_NO_MEM);
                                if (!sValue.append(cCurrent))
                                    return set_error(STATUS_NO_MEM);
                                break;
                        }
                        protector = false;
                    }
                    else
                    {
                        switch (cCurrent)
                        {
                            case '\\':
                                protector   = true;
                                break;
                            case '\'':
                                scan        = false;
                                cCurrent    = -1;
                                break;
                            default:
                                if (!sValue.append(cCurrent))
                                    return set_error(STATUS_NO_MEM);
                                break;
                        }
                    }
                } while (scan);

                // Skip whitespace after string
                skip_whitespace();
            }

            return enToken = TT_STRING;
        }

        token_t Tokenizer::lookup_number()
        {
            enum flags_t
            {
                F_NEGATIVE      = 1 << 0,
                F_SIGN          = 1 << 1,
                F_INT           = 1 << 2,
                F_DOT           = 1 << 3,
                F_FRAC          = 1 << 4,
                F_EXP           = 1 << 5,
                F_ESIGN         = 1 << 6,
                F_ENEGATIVE     = 1 << 7
            };

            lsp_swchar_t c  = lookup();
            size_t flags    = 0;
            int radix       = 10;
            int digit       = 0;
            ssize_t ivalue  = 0;
            double ifrac    = 0;
            double ifpow    = 1.0;
            double rradix   = 0.1;
            ssize_t iexp    = 0;

            // Has sign?
            if (c == '-')
            {
                flags      |= F_NEGATIVE | F_SIGN;
                c           = commit_lookup(TT_UNKNOWN);
            }
            else if (c == '+')
            {
                flags      |= F_SIGN;
                c           = commit_lookup(TT_UNKNOWN);
            }

            // Has prefix ?
            if (c == '0')
            {
                c           = commit_lookup(TT_UNKNOWN);
                switch (c)
                {
                    case 'b': // binary
                    case 'B':
                        radix       = 2;
                        rradix      = 0.5;
                        c           = commit_lookup(TT_UNKNOWN);
                        break;
                    case 'o': // octal
                    case 'O':
                        radix       = 8;
                        rradix      = 0.125;
                        c           = commit_lookup(TT_UNKNOWN);
                        break;
                    case 'd': // decimal
                    case 'D':
                        radix       = 10;
                        rradix      = 0.1;
                        c           = commit_lookup(TT_UNKNOWN);
                        break;
                    case 'x': // hexadecimal
                    case 'X':
                        radix       = 16;
                        rradix      = 0.0625;
                        c           = commit_lookup(TT_UNKNOWN);
                        break;
                    default:
                        flags      |= F_INT;
                        break;
                }
            }

            // Read the integer part
            while (parse_digit(&digit, c, radix))
            {
                if (digit < 0) // Skip separator
                {
                    if (!(flags & F_INT)) // Allow separators only after the first digit
                        return commit(TT_UNKNOWN);
                }
                else
                {
                    ivalue      = ivalue*radix + digit;
                    flags      |= F_INT;
                }
                c           = commit_lookup(TT_IVALUE);
            }

            // Has a fraction part?
            if (c == '.')
            {
                flags      |= F_DOT;
                c           = commit_lookup(TT_FVALUE);

                while (parse_digit(&digit, c, radix))
                {
                    if (digit < 0) // Skip separator
                    {
                        if (!(flags & F_FRAC)) // Allow separators only after the first digit
                            return commit(TT_UNKNOWN);
                    }
                    else
                    {
                        ifpow      *= rradix;
                        ifrac      += digit * ifpow;
                        flags      |= F_FRAC;
                    }
                    c           = commit_lookup(TT_FVALUE);
                }
            }

            // Is there at least INT or FRAC part defined?
            if ((flags & (F_INT | F_FRAC)) == 0)
            {
                if ((flags & (F_SIGN | F_DOT)) == F_SIGN)
                    return enToken = (flags & F_NEGATIVE) ? TT_SUB : TT_ADD;
                return enToken = TT_UNKNOWN;
            }

            // Has an exponent part?
            if ((c == 'e') || (c == 'E') || (c == 'p') || (c == 'P'))
            {
                c           = commit_lookup(TT_FVALUE);

                // Has sign?
                if (c == '-')
                {
                    flags      |= F_ENEGATIVE | F_ESIGN;
                    c           = commit_lookup(TT_UNKNOWN);
                }
                else if (c == '+')
                {
                    flags      |= F_ESIGN;
                    c           = commit_lookup(TT_UNKNOWN);
                }

                // Parse exponent
                while (parse_digit(&digit, c, radix))
                {
                    if (digit < 0) // Skip separator
                    {
                        if (!(flags & F_EXP)) // Allow separators only after the first digit
                            return commit(TT_UNKNOWN);
                    }
                    else
                    {
                        iexp        = iexp*radix + digit;
                        flags      |= F_EXP;
                    }
                    c           = commit_lookup(TT_FVALUE);
                }

                // Analyze post-condition: if exponent sign is defined,
                // the exponent value also should be defined
                if ((flags & (F_ESIGN | F_EXP)) == F_ESIGN)
                    return enToken  = TT_UNKNOWN;
                else if (flags & F_ENEGATIVE)
                    iexp        = -iexp;
            }

            // Now analyze parsing state
            if ((flags & (F_INT | F_FRAC | F_EXP | F_DOT)) == F_INT)
            {
                iValue      = (flags & F_NEGATIVE) ? -ivalue : ivalue;
                return enToken = TT_IVALUE;
            }

            // Form the floating-point value
            double fv       = (double(ivalue) + ifrac) * pow(radix, iexp);
            fValue          = (flags & F_NEGATIVE) ? -fv : fv;
            return enToken  = TT_FVALUE;
        }

        token_t Tokenizer::decode_bareword()
        {
            const char *text = sValue.get_utf8();
            ssize_t first = 0, last = sizeof(barewords)/sizeof(bareword_t) - 1;
            while (first <= last)
            {
                ssize_t center = (first + last) >> 1;
                const bareword_t *bw = &barewords[center];
                int cmp = ::strcasecmp(text, bw->text);

                if (cmp < 0)
                    last = center - 1;
                else if (cmp > 0)
                    first = center + 1;
                else
                {
                    enToken = bw->token;
                    break;
                }
            }

            return enToken;
        }

        token_t Tokenizer::get_token(size_t flags)
        {
            // Pre-checks
            if (!(flags & TF_GET))
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
                enToken = (c == -STATUS_EOF) ? TT_EOF : TT_ERROR;
                nError  = -c;
                return enToken;
            }

            sValue.set_length(0);

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
                case '{': // TT_LCBRACE
                    return commit(TT_LCBRACE);
                case '}': // TT_RCBRACE
                    return commit(TT_RCBRACE);
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
                    return commit(TT_IMOD);
                case '~': // TT_BNOT
                    return commit(TT_BNOT);
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
                    return lookup_identifier(TT_IDENTIFIER);
                }

                case '\'': // TT_STRING
                    return lookup_string();

                // Defaults
                default: // TT_VALUE
                {
                    // Test character
                    if (flags & TF_XSIGN)
                    {
                        if (c == '+')
                            return commit(TT_ADD);
                        else if (c == '-')
                            return commit(TT_SUB);
                    }

                    if (!is_identifier_first(c))
                        return lookup_number();

                    if (lookup_identifier(TT_BAREWORD) != TT_BAREWORD)
                        return enToken;

                    return decode_bareword();
                }
            }

            return enToken  = TT_UNKNOWN;
        }

    } /* namespace calc */
} /* namespace lsp */
