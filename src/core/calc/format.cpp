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
#include <core/stdlib/stdio.h>

namespace lsp
{
    namespace calc
    {
        status_t format(io::IOutSequence *out, const char *fmt, const Parameters *r)
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

        status_t format(io::IOutSequence *out, const LSPString *fmt, const Parameters *r)
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

        status_t format(LSPString *out, io::IInSequence *fmt, const Parameters *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence xout;
            out->set_length(0);
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

        status_t format(LSPString *out, const char *fmt, const Parameters *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence xout;
            out->set_length(0);
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

        status_t format(LSPString *out, const LSPString *fmt, const Parameters *r)
        {
            if ((out == NULL) || (fmt == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence xout;
            out->set_length(0);
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
            lsp_swchar_t c;
            status_t res = STATUS_OK;

            // Read format specifier as string
            while (true)
            {
                // Read next character
                c = fmt->read();
                if (c < 0)
                {
                    if (c != -STATUS_EOF)
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
                c = spec->buf.char_at(i++);

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
                            // Append character
                            spec->flags    |= F_NAME;
                            if (!spec->name.append(c))
                            {
                                res = STATUS_NO_MEM;
                                break;
                            }

                            // Check other characters
                            for (; i < len; ++i)
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
                        }
                        else
                            res = STATUS_BAD_FORMAT;

                        break;
                    case '[': // Index
                        // Prevent from duplicate definitions
                        if (spec->flags & (F_NAME | F_INDEX))
                        {
                            res = STATUS_BAD_FORMAT;
                            break;
                        }

                        // Read decimal index
                        spec->index = 0;
                        for ( ; i < len; ++i)
                        {
                            c = spec->buf.char_at(i);
                            if ((c < '0') || (c > '9'))
                                break;

                            spec->index = spec->index * 10 + (c - '0');
                            spec->flags |= F_INDEX;
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
                                c = spec->buf.char_at(i);
                                if (c == '<') // '|<'
                                {
                                    spec->align     = AL_FROM_RIGHT;
                                    ++i;
                                }
                                else if (c == '>') // '|>'
                                {
                                    spec->align     = AL_TO_RIGHT;
                                    ++i;
                                }
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
                        for ( ; i < len; ++i)
                        {
                            c = spec->buf.char_at(i);
                            if ((c < '0') || (c > '9'))
                                break;

                            spec->flags |= F_WIDTH;
                            spec->width = spec->width * 10 + (c - '0');
                        }

                        // Lookup for dot
                        if ((i < len) && (spec->buf.char_at(i) == '.'))
                        {
                            // Read fraction
                            while ((++i) < len)
                            {
                                c = spec->buf.char_at(i);
                                if ((c < '0') || (c > '9'))
                                    break;

                                spec->flags |= F_FRAC;
                                spec->frac = spec->frac * 10 + (c - '0');
                            }

                            // Check that fraction part is present
                            if (!(spec->flags & F_FRAC))
                            {
                                res = STATUS_BAD_FORMAT;
                                break;
                            }
                        }

                        // Read type specifier
                        if (i >= len)
                        {
                            res = STATUS_BAD_FORMAT;
                            break;
                        }

                        c = spec->buf.char_at(i++);
                        switch (c)
                        {
                            case 'i': case 'd': case 'u': // decimals
                            case 'b': case 'o': case 'x': case 'X': // octals, binaries, hexadecimals
                            case 'f': case 'F': case 'e': case 'E': // floating-points
                            case 's': // string
                                spec->type = c;
                                break;
                            case 'l': // boolean, bOOLEAN
                                c = (i < len) ? spec->buf.char_at(i) : 0;
                                if (c == 'l')
                                {
                                    ++i;
                                    spec->type = 'l';
                                }
                                else if (c == 'L')
                                {
                                    ++i;
                                    spec->type = 'z';
                                }
                                else
                                    spec->type = 'l';
                                break;

                            case 'L': // BOOLEAN, Boolean
                                c = (i < len) ? spec->buf.char_at(i) : 0;
                                if (c == 'l')
                                {
                                    ++i;
                                    spec->type = 'Z';
                                }
                                else if (c == 'L')
                                {
                                    ++i;
                                    spec->type = 'L';
                                }
                                else
                                    spec->type = 'L';
                                break;

                            case 't': // text, tEXT
                                c = (i < len) ? spec->buf.char_at(i) : 0;
                                if (c == 't')
                                {
                                    ++i;
                                    spec->type = 't';
                                }
                                else if (c == 'T')
                                {
                                    ++i;
                                    spec->type = 'y';
                                }
                                else
                                    spec->type = 't';
                                break;
                            case 'T': // TEXT, Text
                                c = (i < len) ? spec->buf.char_at(i) : 0;
                                if (c == 't')
                                {
                                    ++i;
                                    spec->type = 'Y';
                                }
                                else if (c == 'T')
                                {
                                    ++i;
                                    spec->type = 'T';
                                }
                                else
                                    spec->type = 'T';
                                break;
                            default:
                                // No type specifier, just width
                                --i;
                                break;
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
                    res = STATUS_BAD_FORMAT;
                    break;
                }
                else if (res != STATUS_OK)
                    break;
            }

            return res;
        }

        status_t check_specials(fmt_spec_t *spec, value_t *v)
        {
            if (v->type == VT_NULL)
                return (spec->buf.set_ascii("<null>")) ? STATUS_SKIP : STATUS_NO_MEM;
            else if (v->type == VT_UNDEF)
                return (spec->buf.set_ascii("<undef>")) ? STATUS_SKIP : STATUS_NO_MEM;
            return STATUS_OK;
        }

        status_t int_to_dec(fmt_spec_t *spec, value_t *v)
        {
            status_t res = check_specials(spec, v);
            if (res != STATUS_OK)
                return (res == STATUS_SKIP) ? STATUS_OK : res;

            ssize_t x = v->v_int;
            do
            {
                lsp_swchar_t rem = x % 10;
                if (!spec->buf.append(lsp_wchar_t((rem >= 0) ? '0' + rem : '0' - rem)))
                    return STATUS_NO_MEM;
                x /= 10;
            } while (x);

            if (v->v_int < 0)
                res = (spec->buf.append('-')) ? STATUS_OK : STATUS_NO_MEM;
            else if (spec->flags & F_SIGN)
                res = (spec->buf.append('+')) ? STATUS_OK : STATUS_NO_MEM;

            if (res != STATUS_OK)
                return res;

            spec->buf.reverse();
            return STATUS_OK;
        }

        status_t uint_to_dec(fmt_spec_t *spec, value_t *v)
        {
            status_t res = check_specials(spec, v);
            if (res != STATUS_OK)
                return (res == STATUS_SKIP) ? STATUS_OK : res;

            size_t x = v->v_int;
            do
            {
                if (!spec->buf.append(lsp_wchar_t((x % 10) + '0')))
                    return STATUS_NO_MEM;
                x /= 10;
            } while (x);

            if (res != STATUS_OK)
                return res;

            spec->buf.reverse();
            return STATUS_OK;
        }

        status_t int_to_bin(fmt_spec_t *spec, value_t *v)
        {
            status_t res = check_specials(spec, v);
            if (res != STATUS_OK)
                return (res == STATUS_SKIP) ? STATUS_OK : res;

            size_t x = v->v_int;
            do
            {
                if (!spec->buf.append(lsp_wchar_t((x & 1) + '0')))
                    return STATUS_NO_MEM;
                x >>= 1;
            } while (x);

            if (res != STATUS_OK)
                return res;

            spec->buf.reverse();
            return STATUS_OK;
        }

        status_t int_to_oct(fmt_spec_t *spec, value_t *v)
        {
            status_t res = check_specials(spec, v);
            if (res != STATUS_OK)
                return (res == STATUS_SKIP) ? STATUS_OK : res;

            size_t x = v->v_int;
            do
            {
                if (!spec->buf.append(lsp_wchar_t((x & 0x7) + '0')))
                    return STATUS_NO_MEM;
                x >>= 3;
            } while (x);

            if (res != STATUS_OK)
                return res;

            spec->buf.reverse();
            return STATUS_OK;
        }

        static const char *hex_table = "0123456789abcdef0123456789ABCDEF";

        status_t int_to_hex(fmt_spec_t *spec, value_t *v)
        {
            status_t res = check_specials(spec, v);
            if (res != STATUS_OK)
                return (res == STATUS_SKIP) ? STATUS_OK : res;

            const char *table = (spec->type == 'X') ? &hex_table[16] : hex_table;
            size_t x = v->v_int;
            do
            {
                if (!spec->buf.append(table[x & 0xf]))
                    return STATUS_NO_MEM;
                x >>= 4;
            } while (x);

            if (res != STATUS_OK)
                return res;

            spec->buf.reverse();
            return STATUS_OK;
        }

        status_t float_to_str(fmt_spec_t *spec, value_t *v)
        {
            status_t res = check_specials(spec, v);
            if (res != STATUS_OK)
                return (res == STATUS_SKIP) ? STATUS_OK : res;

            if (isnan(v->v_float))
                res = (spec->buf.set_ascii("nan")) ? STATUS_OK : STATUS_NO_MEM;
            else if (isinf(v->v_float))
            {
                if (v->v_float < 0)
                {
                    v->v_float = INFINITY;
                    res = (spec->buf.set_ascii("-inf")) ? STATUS_OK : STATUS_NO_MEM;
                }
                else if (spec->flags & F_SIGN)
                    res = (spec->buf.set_ascii("+inf")) ? STATUS_OK : STATUS_NO_MEM;
                else
                    res = (spec->buf.set_ascii("inf")) ? STATUS_OK : STATUS_NO_MEM;
            }
            else
            {
                char fmt[64];
                if (spec->flags & F_FRAC)
                    ::snprintf(fmt, sizeof(fmt), "%%.%d%c", int(spec->frac), char(spec->type));
                else
                    ::snprintf(fmt, sizeof(fmt), "%%.6%c", char(spec->type));

                fmt[63] = '\0';
                res = spec->buf.fmt_ascii(fmt, v->v_float) ? STATUS_OK : STATUS_NO_MEM;
                if ((res == STATUS_OK) && (spec->flags & F_SIGN) && (v->v_float > 0))
                    res = (spec->buf.prepend(lsp_wchar_t('+'))) ? STATUS_OK : STATUS_NO_MEM;
            }

            return res;
        }

        status_t text_to_str(fmt_spec_t *spec, value_t *v)
        {
            status_t res = check_specials(spec, v);
            if (res != STATUS_OK)
                return (res == STATUS_SKIP) ? STATUS_OK : res;

            if (!spec->buf.set(v->v_str))
                return STATUS_NO_MEM;

            // Additional modifications?
            switch (spec->type)
            {
                case 't': spec->buf.tolower(); break;
                case 'T': spec->buf.toupper(); break;
                case 'y':
                    if (spec->buf.length() > 0)
                        spec->buf.tolower(0, 1);
                    if (spec->buf.length() > 1)
                        spec->buf.toupper(1);
                    break;
                case 'Y':
                    if (spec->buf.length() > 0)
                        spec->buf.toupper(0, 1);
                    if (spec->buf.length() > 1)
                        spec->buf.tolower(1);
                    break;
            }

            return STATUS_OK;
        }

        status_t bool_to_str(fmt_spec_t *spec, value_t *v)
        {
            status_t res = check_specials(spec, v);
            if (res != STATUS_OK)
                return (res == STATUS_SKIP) ? STATUS_OK : res;

            bool success = true;
            switch (spec->type)
            {
                case 'l': success = spec->buf.set_ascii((v->v_bool) ? "true" : "false"); break;
                case 'L': success = spec->buf.set_ascii((v->v_bool) ? "TRUE" : "FALSE"); break;
                case 'z': success = spec->buf.set_ascii((v->v_bool) ? "tRUE" : "fALSE"); break;
                case 'Z': success = spec->buf.set_ascii((v->v_bool) ? "True" : "False"); break;
            }
            return (success) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t emit_parameter(io::IOutSequence *out, fmt_spec_t *spec, const Parameters *r)
        {
            value_t v;
            init_value(&v);
            status_t res = STATUS_OK;

            // Fetch the value if resolver is present
            if (r != NULL)
            {
                if (spec->flags & F_NAME)
                    res = r->get(&spec->name, &v);
                else
                    res = r->get(spec->index, &v);

                if (res != STATUS_OK)
                {
                    destroy_value(&v);
                    return res;
                }
            }

            // Cast the value's type
            spec->buf.clear();

            switch (spec->type)
            {
                case 'i': case 'd':
                    res = cast_value(&v, VT_INT);
                    if (res == STATUS_OK)
                        res = int_to_dec(spec, &v);
                    break;
                case 'u':
                    res = cast_value(&v, VT_INT);
                    if (res == STATUS_OK)
                        res = uint_to_dec(spec, &v);
                    break;
                case 'b':
                    res = cast_value(&v, VT_INT);
                    if (res == STATUS_OK)
                        res = int_to_bin(spec, &v);
                    break;
                case 'o':
                    res = cast_value(&v, VT_INT);
                    if (res == STATUS_OK)
                        res = int_to_oct(spec, &v);
                    break;
                case 'x': case 'X':
                    res = cast_value(&v, VT_INT);
                    if (res == STATUS_OK)
                        res = int_to_hex(spec, &v);
                    break;
                case 'e': case 'E':
                case 'f': case 'F':
                    res = cast_value(&v, VT_FLOAT);
                    if (res == STATUS_OK)
                        res = float_to_str(spec, &v);
                    break;
                case 's': case 't': case 'T': case 'y': case 'Y':
                    res = cast_value(&v, VT_STRING);
                    if (res == STATUS_OK)
                        res = text_to_str(spec, &v);
                    break;
                case 'l': case 'L': case 'z': case 'Z':
                    res = cast_value(&v, VT_BOOL);
                    if (res == STATUS_OK)
                        res = bool_to_str(spec, &v);
                    break;
                default:
                    res = cast_value(&v, VT_STRING);
                    if (res == STATUS_OK)
                        res = text_to_str(spec, &v);
                    break;
            }

            if (res != STATUS_OK)
            {
                destroy_value(&v);
                return res;
            }

            // Compute padding
            ssize_t lpad = 0, rpad = 0, pad = spec->width - spec->buf.length();
            if ((spec->flags & F_WIDTH) && (pad > 0))
            {
                switch (spec->align)
                {
                    case AL_LEFT:
                        rpad = pad;
                        break;
                    case AL_RIGHT:
                        lpad = pad;
                        break;
                    case AL_MIDDLE:
                    case AL_FROM_LEFT:
                        lpad = pad >> 1;
                        rpad = pad - lpad;
                        break;
                    case AL_FROM_RIGHT:
                        rpad = pad >> 1;
                        lpad = pad - rpad;
                        break;
                    case AL_TO_LEFT:
                        lpad = pad >> 2;
                        rpad = pad - lpad;
                        break;
                    case AL_TO_RIGHT:
                        rpad = pad >> 2;
                        lpad = pad - rpad;
                        break;
                    default:
                        break;
                }
            }

            // Emit value
            while (lpad--)
            {
                if ((res = out->write(spec->lpad)) != STATUS_OK)
                {
                    destroy_value(&v);
                    return res;
                }
            }
            if ((res = out->write(&spec->buf)) != STATUS_OK)
            {
                destroy_value(&v);
                return res;
            }
            while (rpad--)
            {
                if ((res = out->write(spec->rpad)) != STATUS_OK)
                {
                    destroy_value(&v);
                    return res;
                }
            }

            destroy_value(&v);
            return res;
        }

        status_t format(io::IOutSequence *out, io::IInSequence *fmt, const Parameters *r)
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
                    return (c != -STATUS_EOF) ? -c : STATUS_OK;

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
                            protector = false;
                        }
                        else
                        {
                            // Read specifier and format the value
                            res = read_specifier(out, fmt, &spec);
                            if (res == STATUS_OK)
                            {
                                if ((res = emit_parameter(out, &spec, r)) != STATUS_OK)
                                    return res;

                                // Reset specifier
                                if (!(spec.flags & (F_NAME | F_INDEX)))
                                    ++index;
                            }
                            else if (res != STATUS_BAD_FORMAT)
                                return res;
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

