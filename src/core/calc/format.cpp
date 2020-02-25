/*
 * format.cpp
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: sadko
 */

#include <core/calc/format.h>
#include <core/calc/Tokenizer.h>
#include <core/io/InStringSequence.h>
#include <core/io/OutStringSequence.h>

namespace lsp
{
    namespace calc
    {
        status_t format(io::IOutSequence *out, const char *fmt, Resolver *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::InStringSequence xfmt;
            status_t res = xfmt.wrap(fmt);
            if (res != STATUS_OK)
            {
                xfmt.close();
                return res;
            }

            res = format(out, &xfmt, r);
            if (res != STATUS_OK)
            {
                xfmt.close();
                return res;
            }

            return xfmt.close();
        }

        status_t format(io::IOutSequence *out, const LSPString *fmt, Resolver *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::InStringSequence xfmt;
            status_t res = xfmt.wrap(fmt);
            if (res != STATUS_OK)
            {
                xfmt.close();
                return res;
            }

            res = format(out, &xfmt, r);
            if (res != STATUS_OK)
            {
                xfmt.close();
                return res;
            }

            return xfmt.close();
        }

        status_t format(LSPString *out, io::IInSequence *fmt, Resolver *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence xout;
            status_t res = xout.wrap(out, false);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            res = format(&xout, fmt, r);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            return xout.close();
        }

        status_t format(LSPString *out, const char *fmt, Resolver *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence xout;
            status_t res = xout.wrap(out, false);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            res = format(&xout, fmt, r);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            return xout.close();
        }

        status_t format(LSPString *out, const LSPString *fmt, Resolver *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence xout;
            status_t res = xout.wrap(out, false);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            res = format(&xout, fmt, r);
            if (res != STATUS_OK)
            {
                xout.close();
                return res;
            }

            return xout.close();
        }

        enum align_type_t
        {
            AL_NONE,            // No alignment
            AL_LEFT,            // '<' - pad formatted value from right
            AL_RIGHT,           // '>' - pad formatted value from left
            AL_MIDDLE,          // '|' - pad formatted value from left and right
            AL_FROM_LEFT,       // '>|' - pad formatted value from left and right, prefer right alignment
            AL_FROM_RIGHT,      // '|<' - pad formatted value from left and right, prefer left alignment
            AL_TO_LEFT,         // '<|' - take 1/4 from left, 3/4 from right
            AL_TO_RIGHT,        // '|>' - take 1/4 from right, 3/4 from left
        };

        enum fmt_flags_t
        {
            F_NAME      = 1 << 0,
            F_INDEX     = 1 << 1,
            F_TYPE      = 1 << 2,
            F_WIDTH     = 1 << 3,
            F_FRAC      = 1 << 4,
            F_SIGN      = 1 << 5,
            F_LPAD      = 1 << 6,
            F_RPAD      = 1 << 7
        };

        typedef struct fmt_spec_t
        {
            LSPString       buf;        // Buffer to store full specifier
            LSPString       name;       // Name of the parameter
            size_t          index;      // Index of parameter
            size_t          flags;      // Format flags
            lsp_wchar_t     lpad;       // Padding left
            lsp_wchar_t     rpad;       // Padding right
            align_type_t    align;      // Alignment
            lsp_wchar_t     type;       // Type
            size_t          width;      // Width
            size_t          frac;       // Fraction
        } fmt_spec_t;

        void init_spec(fmt_spec_t *spec, size_t index)
        {
            spec->buf.clear();
            spec->name.clear();
            spec->index = index;
            spec->flags = 0;
            spec->lpad  = ' ';
            spec->rpad  = ' ';
            spec->align = AL_NONE;
            spec->type  = 0;
            spec->width = 0;
            spec->frac  = 0;
        }

        status_t read_specifier(io::IOutSequence *out, io::IInSequence *fmt, fmt_spec_t *spec)
        {
            status_t res = STATUS_OK;

            // Read format specifier as string
            while (true)
            {
                // Read next character
                lsp_swchar_t c = fmt->read();
                if (c < 0)
                {
                    if (c != STATUS_EOF)
                        return -c;

                    // Format specifier has been interrupted
                    if ((res = out->write('{')) != STATUS_OK)
                        return res;
                    if ((res = out->write(&spec->buf)) != STATUS_OK)
                        return res;
                    return STATUS_BAD_FORMAT;
                }

                // End of format specifier?
                if (c == '}')
                    break;

                // Append character and repeat
                spec->buf.append(c);
            }

            // now parse format specifier
            size_t len = spec->buf.length();
            for (size_t i=0; i<len; )
            {
                lsp_wchar_t c = spec->buf.char_at(i++);

                switch (c)
                {
                    case '@': // Name
                        // Prevent from duplicate definitions
                        if (spec->flags & (F_NAME | F_INDEX))
                        {
                            res = STATUS_BAD_FORMAT;
                            break;
                        }

                        // Check first character
                        c = (i < len) ? spec->buf.char_at(i++) : 0;
                        if (Tokenizer::is_identifier_first(c))
                        {
                            spec->flags    |= F_NAME;
                            if (!spec->name.append(c))
                            {
                                res = STATUS_NO_MEM;
                                break;
                            }
                        }

                        // Check other characters
                        while (i < len)
                        {
                            c = spec->buf.char_at(i);
                            if (!Tokenizer::is_identifier_next(c))
                                break;
                            else if (!spec->name.append(c))
                            {
                                res = STATUS_NO_MEM;
                                break;
                            }
                        }
                        break;
                    case '[': // Index
                        // Prevent from duplicate definitions
                        if (spec->flags & (F_NAME | F_INDEX))
                        {
                            res = STATUS_BAD_FORMAT;
                            break;
                        }

                        // Read decimal index
                        while (i < len)
                        {
                            c = spec->buf.char_at(i++);
                            if ((c >= '0') && (c <= '9'))
                            {
                                spec->index = spec->index * 10 + (c - '0');
                                spec->flags |= F_INDEX;
                            }
                            else
                                break;
                        }

                        // Require final ']'
                        if ((spec->flags & F_INDEX) && (i < len))
                        {
                            c = spec->buf.char_at(i++);
                            if (c != ']')
                                res = STATUS_BAD_FORMAT;
                        }
                        else
                            res = STATUS_BAD_FORMAT;

                        break;

                    case '^': // Left padding
                        if ((!(spec->flags & F_LPAD)) && (i < len))
                        {
                            spec->flags |= F_LPAD;
                            spec->lpad = spec->buf.char_at(i++);
                        }
                        else
                            res = STATUS_BAD_FORMAT;
                        break;

                    case '$': // Right padding
                        if ((!(spec->flags & F_RPAD)) && (i < len))
                        {
                            spec->flags |= F_RPAD;
                            spec->rpad = spec->buf.char_at(i++);
                        }
                        else
                            res = STATUS_BAD_FORMAT;
                        break;

                    case '>': // Right alignment
                        if (spec->align == AL_NONE)
                        {
                            spec->align = AL_RIGHT;
                            if ((i < len) && (spec->buf.char_at(i) == '|')) // '>|'
                            {
                                spec->align = AL_FROM_LEFT;
                                ++i;
                            }
                        }
                        else
                            res = STATUS_BAD_FORMAT;
                        break;

                    case '<': // Left alignment
                        if (spec->align == AL_NONE)
                        {
                            spec->align = AL_LEFT;
                            if ((i < len) && (spec->buf.char_at(i) == '|')) // '<|'
                            {
                                spec->align = AL_TO_LEFT;
                                ++i;
                            }
                        }
                        else
                            res = STATUS_BAD_FORMAT;
                        break;

                    case '|': // Middle alignment
                        if (spec->align == AL_NONE)
                        {
                            spec->align = AL_MIDDLE;
                            if (i < len)
                            {
                                // Lookup additional modes
                                c = spec->buf.char_at(i++);
                                if (c == '<') // '|<'
                                    spec->align     = AL_FROM_RIGHT;
                                else if (c == '>') // '|>'
                                    spec->align     = AL_TO_RIGHT;
                                else if (c != 0)
                                    res             = STATUS_BAD_FORMAT;
                            }
                        }
                        else
                            res = STATUS_BAD_FORMAT;
                        break;

                    case '%': // Type specifier
                        // Prevent from duplicate definition
                        if (spec->flags & F_TYPE)
                        {
                            res = STATUS_BAD_FORMAT;
                            break;
                        }
                        spec->flags |= F_TYPE;

                        // Lookup for sign
                        if ((i < len) && (spec->buf.char_at(i) == '+'))
                        {
                            spec->flags |= F_SIGN;
                            ++i;
                        }

                        // Read width
                        while (i < len)
                        {
                            c = spec->buf.char_at(i);
                            if ((c >= '0') && (c <= '9'))
                            {
                                spec->flags |= F_WIDTH;
                                spec->width = spec->width * 10 + (c - '0');
                                ++i;
                            }
                            else
                                break;
                        }

                        // Lookup for dot
                        if ((i < len) && (spec->buf.char_at(i) == '.'))
                        {
                            // Read fraction
                            while ((++i) < len)
                            {
                                c = spec->buf.char_at(i);
                                if ((c >= '0') && (c <= '9'))
                                {
                                    spec->flags |= F_FRAC;
                                    spec->frac = spec->frac * 10 + (c - '0');
                                }
                                else
                                    break;
                            }
                        }

                        // Read format character
                        if (i < len)
                        {
                            res = STATUS_BAD_FORMAT;
                            break;
                        }

                        c = spec->buf.char_at(i++);
                        switch (c)
                        {
                            case 'i': case 'd': // decimals
                            case 'b': case 'o': case 'x': case 'X': // octals, binaries, hexadecimals
                            case 'f': case 'F': case 'e': case 'E': // floating-points
                                spec->type = c;
                                break;
                            case 't': // boolean, bOOLEAN
                                c = (i < len) ? spec->buf.char_at(i) : 0;
                                if (c == 't')
                                {
                                    ++i;
                                    spec->type = 't';
                                }
                                else if (c == 'T')
                                {
                                    ++i;
                                    spec->type = 'x';
                                }
                                else
                                    spec->type = 't';
                                break;

                            case 'T': // BOOLEAN, Boolean
                                c = (i < len) ? spec->buf.char_at(i) : 0;
                                if (c == 't')
                                {
                                    ++i;
                                    spec->type = 'X';
                                }
                                else if (c == 'T')
                                {
                                    ++i;
                                    spec->type = 'T';
                                }
                                else
                                    spec->type = 'T';
                                break;

                            case 's': case 'S': // string, capitalized string, lowered string, first-capitalized string
                        }

                        break;

                    default:
                        res = STATUS_BAD_FORMAT;
                        break;
                }

                // Check result
                if (res == STATUS_BAD_FORMAT)
                {
                    if ((res = out->write('{')) != STATUS_OK)
                        return res;
                    if ((res = out->write(&spec->buf)) != STATUS_OK)
                        return res;
                    if ((res = out->write('}')) != STATUS_OK)
                        return res;
                }
                if (res != STATUS_OK)
                    break;
            }

            return res;
        }

        status_t emit_parameter(io::IOutSequence *out, fmt_spec_t *spec, Resolver *r)
        {
            return STATUS_OK;
        }

        status_t format(io::IOutSequence *out, io::IInSequence *fmt, Resolver *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            status_t res;
            size_t index = 0;
            bool protector = false;
            fmt_spec_t spec;
            init_spec(&spec, index);

            while (true)
            {
                // Read character
                lsp_swchar_t c = fmt->read();
                if (c < 0)
                    return (c != STATUS_EOF) ? -c : STATUS_OK;

                switch (c)
                {
                    case '\\':
                        if (protector)
                        {
                            if ((res = out->write('\\')) != STATUS_OK)
                                return res;
                        }
                        protector = !protector;
                        break;

                    case '{':
                        if (protector)
                        {
                            if ((res = out->write('{')) != STATUS_OK)
                                return res;
                        }
                        else
                        {
                            // Read specifier and format the value
                            if ((res = read_specifier(out, fmt, &spec)) != STATUS_OK)
                                return res;
                            if ((res = emit_parameter(out, &spec, r)) != STATUS_OK)
                                return res;

                            // Reset specifier
                            if (!(spec.flags & (F_NAME | F_INDEX)))
                                ++index;
                            init_spec(&spec, index);
                        }
                        break;

                    default:
                        if ((res = out->write(c)) != STATUS_OK)
                            return res;
                        break;
                } // c
            }
        }
    }
}

