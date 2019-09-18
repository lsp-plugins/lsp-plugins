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
            cCurrent    = -1;
            enToken     = TT_UNKNOWN;
            nError      = STATUS_OK;
            fValue      = 0;
            iValue      = 0;
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
            size_t idx      = 0;
            char c          = sValue.char_at(idx++);
            token_t res     = enToken;

            switch (c)
            {
                // Alpha
                case 'a': case 'A': // TT_AND, TT_ADD
                    c = sValue.char_at(idx++);
                    if ((c == 'n') || (c == 'N'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'd') || (c == 'D'))           // AND
                            res         = TT_AND;
                    }
                    else if ((c == 'd') || (c == 'D'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'd') || (c == 'D'))           // ADD
                            res         = TT_ADDSYM;
                    }
                    break;

                case 'b': case 'B': // TT_BAND, TT_BNOT, TT_BOR, TT_BXOR
                    c = sValue.char_at(idx++);
                    if ((c == 'a') || (c == 'A'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'n') || (c == 'N'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'd') || (c == 'D'))       // BAND
                                res         = TT_BAND;
                        }
                    }
                    else if ((c == 'n') || (c == 'N'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'o') || (c == 'O'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 't') || (c == 'T'))       // BNOT
                                res         = TT_BNOT;
                        }
                    }
                    else if ((c == 'o') || (c == 'O'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'r') || (c == 'R'))           // BOR
                            res         = TT_BOR;
                    }
                    else if ((c == 'x') || (c == 'X'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'o') || (c == 'O'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'r') || (c == 'R'))       // BXOR
                                res         = TT_BXOR;
                        }
                    }
                    break;

                case 'c': case 'C': // TT_CMP
                    c = sValue.char_at(idx++);
                    if ((c == 'm') || (c == 'M'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'p') || (c == 'P'))
                            res         = TT_CMP;               // CMP
                    }
                    break;

                case 'd': case 'D': // TT_DIV, TT_DB
                    c = sValue.char_at(idx++);
                    if ((c == 'i') || (c == 'I'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'v') || (c == 'V'))           // DIV
                            res         = TT_DIV;
                    }
                    else if ((c == 'b') || (c == 'B'))          // DB
                        res         = TT_DB;
                    break;

                case 'e': case 'E': // TT_EQ, TT_EX
                    c = sValue.char_at(idx++);
                    if ((c == 'q') || (c == 'Q'))               // EQ
                        res         = TT_EQ;
                    else if ((c == 'x') || (c == 'X'))          // EX
                        res         = TT_EX;
                    break;

                case 'f': case 'F': // FALSE, FMOD
                    c = sValue.char_at(idx++);
                    if ((c == 'a') || (c == 'A'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'l') || (c == 'L'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 's') || (c == 'S'))
                            {
                                c = sValue.char_at(idx++);
                                if ((c == 'e') || (c == 'E'))   // FALSE
                                    res         = TT_FALSE;
                            }
                        }
                    }
                    else if ((c == 'm') || (c == 'M'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'o') || (c == 'O'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'd') || (c == 'D'))       // FMOD
                                res         = TT_FMOD;
                        }
                    }
                    break;

                case 'g': case 'G': // TT_GREATER, TT_GREATER_EQ
                    c = sValue.char_at(idx++);
                    if ((c == 't') || (c == 'T'))               // GT
                        res         = TT_GREATER;
                    else if ((c == 'e') || (c == 'E'))          // GE
                        res         = TT_GREATER_EQ;
                    break;

                case 'i': case 'I': // TT_IADD, TT_ISUB, TT_IMUL, TT_IDIV, TT_IMOD
                    c = sValue.char_at(idx++);
                    if ((c == 'a') || (c == 'A'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'd') || (c == 'D'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'd') || (c == 'D'))       // IADD
                                res         = TT_IADD;
                        }
                    }
                    else if ((c == 'c') || (c == 'C'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'm') || (c == 'M'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'p') || (c == 'P'))       // ICMP
                                res         = TT_CMP;
                        }
                    }
                    else if ((c == 'd') || (c == 'D'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'i') || (c == 'I'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'v') || (c == 'V'))       // IDIV
                                res         = TT_IDIV;
                        }
                    }
                    else if ((c == 'e') || (c == 'E'))          // IE
                    {
                        res         = TT_IEQ;
                        c           = (idx < sValue.length()) ?
                                      sValue.char_at(idx++) : -1;
                        if ((c == 'q') || (c == 'Q'))           // IEQ
                            res         = TT_IEQ;
                    }
                    else if ((c == 'g') || (c == 'G'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 't') || (c == 'T'))           // IGT
                            res         = TT_IGREATER;
                        else if ((c == 'e') || (c == 'T'))      // IGE
                            res         = TT_IGREATER_EQ;
                    }
                    else if ((c == 'l') || (c == 'L'))
                    {
                        c = sValue.char_at(idx++);              // ILT
                        if ((c == 't') || (c == 'T'))
                            res         = TT_ILESS;
                        else if ((c == 'e') || (c == 'T'))      // ILE
                            res         = TT_ILESS_EQ;
                    }
                    else if ((c == 'm') || (c == 'M'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'o') || (c == 'O'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'd') || (c == 'D'))       // IMOD
                                res         = TT_IMOD;
                        }
                        else if ((c == 'u') || (c == 'U'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'l') || (c == 'L'))       // IMUL
                                res         = TT_IMUL;
                        }
                    }
                    else if ((c == 'n') || (c == 'N'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'e') || (c == 'E'))
                            res         = TT_INOT_EQ;           // INE
                        else if ((c == 'g') || (c == 'G'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 't') || (c == 'T'))       // INGT
                                res         = TT_ILESS_EQ;
                            else if ((c == 'e') || (c == 'T'))  // INGE
                                res         = TT_ILESS;
                        }
                        else if ((c == 'l') || (c == 'L'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 't') || (c == 'T'))       // INLT
                                res         = TT_IGREATER_EQ;
                            else if ((c == 'e') || (c == 'T'))  // INLE
                                res         = TT_IGREATER;
                        }
                    }
                    else if ((c == 's') || (c == 'S'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'u') || (c == 'U'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'b') || (c == 'B'))       // ISUB
                                res         = TT_ISUB;
                        }
                    }
                    break;

                case 'l': case 'L': // TT_LESS, TT_LESS_EQ
                    c = sValue.char_at(idx++);
                    if ((c == 't') || (c == 'T'))               // LT
                        res         = TT_LESS;
                    else if ((c == 'e') || (c == 'T'))          // LE
                        res         = TT_LESS_EQ;
                    break;

                case 'm': case 'M': // TT_MUL, TT_MOD
                    c = sValue.char_at(idx++);
                    if ((c == 'o') || (c == 'O'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'd') || (c == 'D'))           // MOD
                            res         = TT_IMOD;
                    }
                    else if ((c == 'u') || (c == 'U'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'l') || (c == 'L'))           // MUL
                            res         = TT_MUL;
                    }
                    break;

                case 'n': case 'N': // TT_NOT, TT_LESS, TT_GREATER, TT_LESS_EQ, TT_GREATER_EQ, TT_NOT_EQ
                    c = sValue.char_at(idx++);
                    if ((c == 'o') || (c == 'O'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 't') || (c == 'T'))           // NOT
                            res         = TT_NOT;
                    }
                    else if ((c == 'g') || (c == 'G'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 't') || (c == 'T'))           // NGT
                            res         = TT_LESS_EQ;
                        else if ((c == 'e') || (c == 'T'))      // NGE
                            res         = TT_LESS;
                    }
                    else if ((c == 'l') || (c == 'L'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 't') || (c == 'T'))           // NLT
                            res         = TT_GREATER_EQ;
                        else if ((c == 'e') || (c == 'T'))      // NLE
                            res         = TT_GREATER;
                    }
                    else if ((c == 'e') || (c == 'E'))          // NE
                        res         = TT_NOT_EQ;
                    else if ((c == 'u') || (c == 'U'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'l') || (c == 'L'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'l') || (c == 'L'))       // NULL
                                res         = TT_NULL;
                        }
                    }

                    break;

                case 'o': case 'O': // TT_OR
                    c = sValue.char_at(idx++);
                    if ((c == 'r') || (c == 'R'))               // OR
                        res         = TT_OR;
                    break;

                case 'p': case 'P': // TT_POW
                    c = sValue.char_at(idx++);
                    if ((c == 'o') || (c == 'O'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'w') || (c == 'W'))           // POW
                            res         = TT_POW;
                    }
                    break;

                case 's': case 'S': // TT_SUB
                    c = sValue.char_at(idx++);
                    if ((c == 'u') || (c == 'U'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'b') || (c == 'B'))           // SUB
                            res         = TT_SUBSYM;
                    }
                    break;

                case 't': case 'T': // TRUE
                    c = sValue.char_at(idx++);
                    if ((c == 'r') || (c == 'R'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'u') || (c == 'U'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'e') || (c == 'E'))       // TRUE
                                res         = TT_TRUE;
                        }
                    }
                    break;

                case 'u': case 'U': // UNDEF
                    c = sValue.char_at(idx++);
                    if ((c == 'n') || (c == 'N'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'd') || (c == 'D'))
                        {
                            c = sValue.char_at(idx++);
                            if ((c == 'e') || (c == 'E'))
                            {
                                c = sValue.char_at(idx++);
                                if ((c == 'f') || (c == 'F'))
                                    res         = TT_UNDEF;
                            }
                        }
                    }
                    break;

                case 'x': case 'X': // TT_XOR
                    c = sValue.char_at(idx++);
                    if ((c == 'o') || (c == 'O'))
                    {
                        c = sValue.char_at(idx++);
                        if ((c == 'r') || (c == 'R'))       // XOR
                            res         = TT_XOR;
                    }
                    break;
            }

            // Replace token with new decoded value
            if (idx == sValue.length())
                enToken     = res;

            return enToken;
        }

        token_t Tokenizer::get_token(size_t flags)
        {
            // Pre-checks
            if (!(flags & TF_GET))
                return enToken;

            // Skip whitespaces
            lsp_swchar_t c = skip_whitespace();
            if (c < 0)
            {
                enToken = (c == -STATUS_EOF) ? TT_EOF : TT_ERROR;
                nError  = -c;
                return enToken;
            }

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
