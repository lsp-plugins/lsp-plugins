/*
 * Serializer.cpp
 *
 *  Created on: 17 окт. 2019 г.
 *      Author: sadko
 */

#include <core/io/File.h>
#include <core/io/OutFileStream.h>
#include <core/io/OutStringSequence.h>
#include <core/io/OutSequence.h>
#include <core/files/json/Tokenizer.h>
#include <core/files/json/Serializer.h>
#include <core/stdlib/math.h>

namespace lsp
{
    namespace json
    {
        
        Serializer::Serializer()
        {
            pOut            = NULL;
            nWFlags         = 0;
            sState.mode     = WRITE_ROOT;
            sState.flags    = 0;
            sState.ident    = 0;
        }
        
        Serializer::~Serializer()
        {
            close();
        }

        status_t Serializer::open(const char *path, const serial_flags_t *settings, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutFileStream *ofs = new io::OutFileStream();
            if (ofs == NULL)
                return STATUS_NO_MEM;
            status_t res = ofs->open(path, io::File::FM_WRITE | io::File::FM_TRUNC | io::File::FM_CREATE);
            if (res == STATUS_OK)
            {
                res     = wrap(ofs, settings, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ofs->close();
            }
            delete ofs;

            return res;
        }

        status_t Serializer::open(const LSPString *path, const serial_flags_t *settings, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutFileStream *ofs = new io::OutFileStream();
            if (ofs == NULL)
                return STATUS_NO_MEM;
            status_t res = ofs->open(path, io::File::FM_WRITE | io::File::FM_TRUNC | io::File::FM_CREATE);
            if (res == STATUS_OK)
            {
                res     = wrap(ofs, settings, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ofs->close();
            }
            delete ofs;

            return res;
        }

        status_t Serializer::open(const io::Path *path, const serial_flags_t *settings, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutFileStream *ofs = new io::OutFileStream();
            if (ofs == NULL)
                return STATUS_NO_MEM;
            status_t res = ofs->open(path, io::File::FM_WRITE | io::File::FM_TRUNC | io::File::FM_CREATE);
            if (res == STATUS_OK)
            {
                res     = wrap(ofs, settings, WRAP_CLOSE | WRAP_DELETE, charset);
                if (res == STATUS_OK)
                    return res;
                ofs->close();
            }
            delete ofs;

            return res;
        }

        void Serializer::copy_settings(const serial_flags_t *flags)
        {
            if (flags != NULL)
            {
                sSettings       = *flags;
                return;
            }

            sSettings.version           = JSON_LEGACY;
            sSettings.identifiers       = false;
            sSettings.ident             = ' ';
            sSettings.padding           = 0;
            sSettings.separator         = false;
            sSettings.multiline         = false;
        }

        status_t Serializer::wrap(LSPString *str, const serial_flags_t *settings)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (str == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutStringSequence *seq = new io::OutStringSequence(str, false);
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = wrap(seq, settings, WRAP_CLOSE | WRAP_DELETE);
            if (res == STATUS_OK)
                return res;

            seq->close();
            delete seq;

            return res;
        }

        status_t Serializer::wrap(io::IOutStream *os, const serial_flags_t *settings, size_t flags, const char *charset)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (os == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::OutSequence *seq = new io::OutSequence();
            if (seq == NULL)
                return STATUS_NO_MEM;

            status_t res = seq->wrap(os, flags, charset);
            if (res == STATUS_OK)
            {
                if ((res = wrap(seq, settings, WRAP_CLOSE | WRAP_DELETE)) == STATUS_OK)
                    return res;
                seq->close();
            }

            delete seq;
            return res;
        }

        status_t Serializer::wrap(io::IOutSequence *seq, const serial_flags_t *settings, size_t flags)
        {
            if (pOut != NULL)
                return STATUS_BAD_STATE;
            else if (seq == NULL)
                return STATUS_BAD_ARGUMENTS;

            pOut            = seq;
            nWFlags         = flags;
            sState.mode     = WRITE_ROOT;
            sState.flags    = 0;
            copy_settings(settings);

            return STATUS_OK;
        }

        status_t Serializer::close()
        {
            status_t res = STATUS_OK;

            // Close handles
            if (pOut != NULL)
            {
                if (nWFlags & WRAP_CLOSE)
                {
                    status_t xres = pOut->close();
                    if (res == STATUS_OK)
                        res = xres;
                }

                if (nWFlags & WRAP_DELETE)
                    delete pOut;

                pOut = NULL;
            }

            sStack.flush();

            return res;
        }

        status_t Serializer::push_state(pmode_t mode)
        {
            if (!sStack.add(&sState))
                return STATUS_NO_MEM;

            sState.mode     = mode;
            sState.flags    = 0;
            sState.ident   += sSettings.padding;
            return STATUS_OK;
        }

        status_t Serializer::pop_state()
        {
            state_t *st = sStack.last();
            if (st == NULL)
                return STATUS_BAD_STATE;

            sState          = *st;
            return (sStack.remove_last()) ? STATUS_OK : STATUS_BAD_STATE;
        }

        status_t Serializer::write_comma()
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            switch (sState.mode)
            {
                case WRITE_ROOT:
                    return STATUS_INVALID_VALUE;
                case WRITE_ARRAY:
                case WRITE_OBJECT:
                    if ((sState.flags & (SF_VALUE | SF_COMMA)) != SF_VALUE)
                        return STATUS_INVALID_VALUE;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }

            sState.flags   |= SF_COMMA;
            return pOut->write(',');
        }

        status_t Serializer::writeln()
        {
            if (!sSettings.multiline)
                return STATUS_OK;

            sState.flags   &= ~SF_CONTENT;
            status_t res = pOut->write('\n');
            if (res != STATUS_OK)
                return res;
            for (size_t i=0, n=sState.ident; i<n; ++i)
            {
                if ((res = pOut->write(sSettings.ident)) != STATUS_OK)
                    break;
            }

            return res;
        }

        char Serializer::hex(int x)
        {
            return ((x &= 0x0f) < 10) ? (x + '0') : (x + 'A' - 10);
        }

        status_t Serializer::write(const event_t *event)
        {
            if (event == NULL)
                return STATUS_BAD_ARGUMENTS;

            switch (event->type)
            {
                case JE_OBJECT_START:   return start_object();
                case JE_OBJECT_END:     return end_object();
                case JE_ARRAY_START:    return start_array();
                case JE_ARRAY_END:      return end_array();
                case JE_PROPERTY:       return write_property(&event->sValue);
                case JE_STRING:         return write_string(&event->sValue);
                case JE_INTEGER:        return write_int(event->iValue);
                case JE_DOUBLE:         return write_double(event->fValue);
                case JE_BOOL:           return write_bool(event->bValue);
                case JE_NULL:           return write_null();
                default:                break;
            }

            return STATUS_BAD_ARGUMENTS;
        }

        status_t Serializer::emit_comma()
        {
            if ((sState.flags & (SF_VALUE | SF_COMMA)) != SF_VALUE)
                return STATUS_OK;
            sState.flags |= SF_CONTENT;
            return pOut->write(',');
        }

        status_t Serializer::emit_separator()
        {
            if ((!sSettings.separator) || (!(sState.flags & SF_CONTENT)))
                return STATUS_OK;
            return pOut->write(' ');
        }

        status_t Serializer::write_raw(const char *buf, int len)
        {
            status_t res = STATUS_OK;

            switch (sState.mode)
            {
                case WRITE_ROOT:
                    if (sState.flags & SF_VALUE)
                        return STATUS_INVALID_VALUE;
                    break;
                case WRITE_ARRAY:
                    if ((res = emit_comma()) == STATUS_OK)
                        res = writeln();
                    break;
                case WRITE_OBJECT:
                    if (!(sState.flags & SF_PROPERTY))
                        return STATUS_INVALID_VALUE;
                    sState.flags &= ~SF_PROPERTY;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }
            if (res == STATUS_OK)
                res = emit_separator();

            sState.flags   &= ~SF_COMMA;
            sState.flags   |= SF_VALUE | SF_CONTENT;

            return (res == STATUS_OK) ? pOut->write_ascii(buf, len) : res;
        }

        status_t Serializer::write_int(ssize_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            char buf[0x20];
            int len = ::snprintf(buf, sizeof(buf), "%lld", (long long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_hex(ssize_t value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            else if (sSettings.version < JSON_VERSION5)
                return STATUS_INVALID_VALUE;

            char buf[0x20];
            int len = (value < 0) ?
                    ::snprintf(buf, sizeof(buf), "-0x%llx", (long long)(-value)) :
                    ::snprintf(buf, sizeof(buf), "0x%llx", (long long)value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_double(double value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            if (isnan(value))
                return write_raw("NaN", 3);
            else if (isinf(value))
                return (value < 0.0) ? write_raw("-Infinity", 9) : write_raw("Infinity", 8);

            char buf[0x20];
            int len = ::snprintf(buf, sizeof(buf), "%f", value);
            return (len < int(sizeof(buf))) ? write_raw(buf, len) : STATUS_OVERFLOW;
        }

        status_t Serializer::write_double(double value, const char *fmt)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            if (isnan(value))
                return write_raw("NaN", 3);
            else if (isinf(value))
                return (value < 0.0) ? write_raw("-Infinity", 9) : write_raw("Infinity", 8);

            char *buf = NULL;
            int len = ::asprintf(&buf, fmt, value);
            if (buf == NULL)
                return STATUS_NO_MEM;

            status_t res = (len >= 0) ? write_raw(buf, len) : STATUS_NO_DATA;
            ::free(buf);
            return res;
        }

        status_t Serializer::write_bool(bool value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            return (value) ? write_raw("true", 4) : write_raw("false", 5);
        }

        status_t Serializer::write_null()
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            return write_raw("null", 4);
        }

        status_t Serializer::write_string(const char *value, const char *charset)
        {
            if (value == NULL)
                return write_null();

            LSPString tmp;
            return (tmp.set_native(value, charset)) ? write_string(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_string(const char *value)
        {
            if (value == NULL)
                return write_null();

            LSPString tmp;
            return (tmp.set_utf8(value)) ? write_string(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_string(const LSPString *value)
        {
            if (value == NULL)
                return write_null();
            else if (pOut == NULL)
                return STATUS_BAD_STATE;

            // Analyze state
            status_t res = STATUS_OK;

            switch (sState.mode)
            {
                case WRITE_ROOT:
                    if (sState.flags & SF_VALUE)
                        return STATUS_INVALID_VALUE;
                    break;
                case WRITE_ARRAY:
                    if ((res = emit_comma()) == STATUS_OK)
                        res = writeln();
                    break;
                case WRITE_OBJECT:
                    if (!(sState.flags & SF_PROPERTY))
                        return STATUS_INVALID_VALUE;
                    sState.flags &= ~SF_PROPERTY;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }
            if (res == STATUS_OK)
                res = emit_separator();

            sState.flags   &= ~SF_COMMA;
            sState.flags   |= SF_VALUE | SF_CONTENT;

            return (res == STATUS_OK) ? write_literal(value) : res;
        }

        status_t Serializer::write_literal(const LSPString *value)
        {
            status_t res;
            size_t last = 0, curr = 0, bl = 1;
            char xb[0x10];
            xb[0] = '\\';

            // Output start quote
            sState.flags |= SF_CONTENT;
            if ((res = pOut->write('\"')) != STATUS_OK)
                return res;

            for (size_t n=value->length(); curr<n; ++curr)
            {
                lsp_wchar_t ch = value->char_at(curr);

                switch (ch)
                {
                    case '\0': xb[bl++] = '0'; break;
                    case '\b': xb[bl++] = 'b'; break;
                    case '\f': xb[bl++] = 'f'; break;
                    case '\r': xb[bl++] = 'r'; break;
                    case '\t': xb[bl++] = 't'; break;
                    case '\v': xb[bl++] = 'v'; break;
                    case '\n': xb[bl++] = 'n'; break;
                    case '\\': xb[bl++] = '\\'; break;
                    case '\"': xb[bl++] = '\"'; break;
                    default:
                        if (ch < 0x20)
                        {
                            xb[bl++] = 'x';
                            xb[bl++] = hex(ch >> 4);
                            xb[bl++] = hex(ch);
                        }
                        break;
                }

                // Is there extra escape sequence?
                if (bl > 1)
                {
                    // Write string directly to output
                    if (last < curr)
                    {
                        if ((res = pOut->write(value, last, curr)) != STATUS_OK)
                            return res;
                    }

                    // Append escape sequence and reset extra buffer size
                    if ((res = pOut->write_ascii(xb, bl)) != STATUS_OK)
                        return res;

                    last = curr+1; // Put buffer pointer to the next character
                    bl = 1;
                }
            }

            // Need to emit extra data?
            if (last < curr)
            {
                if ((res = pOut->write(value, last, curr)) != STATUS_OK)
                    return res;
            }

            // Output end quote
            return pOut->write('\"');
        }

        status_t Serializer::write_comment(const char *value, const char *charset)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString tmp;
            return (tmp.set_native(value, charset)) ? write_comment(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_comment(const char *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString tmp;
            return (tmp.set_utf8(value)) ? write_comment(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_comment(const LSPString *value)
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;
            else if (value == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (sSettings.version < JSON_VERSION5)
                return STATUS_INVALID_VALUE;

            // Comment can be placed everywhere, we don't need to analyze state
            status_t res;
            lsp_swchar_t prev = -1;
            size_t last = 0, curr = 0, bl = 4;
            char xb[0x10];
            xb[0] = '\\';
            xb[1] = 'U';
            xb[2] = '0';
            xb[3] = '0';

            // Output start quote
            if ((res = emit_separator()) != STATUS_OK)
                return res;

            sState.flags |= SF_CONTENT;
            if ((res = pOut->write_ascii("/*", 2)) != STATUS_OK)
                return res;

            for (size_t n=value->length(); curr<n; ++curr)
            {
                lsp_wchar_t ch = value->char_at(curr);

                switch (ch)
                {
                    case '\n':
                    case '\r':
                        if (sSettings.multiline)
                            break;
                        xb[bl++]    = hex(ch >> 4);
                        xb[bl++]    = hex(ch);
                        break;
                    case '/':
                        if (prev != '*')
                            break;
                        // Asterisk code in unicode
                        xb[bl++]    = '2';
                        xb[bl++]    = 'F';
                        break;
                    case '*':
                        if (prev != '/')
                            break;
                        // Asterisk code in unicode
                        xb[bl++]    = '2';
                        xb[bl++]    = 'A';
                        break;
                    default:
                        if (ch < 0x20)
                        {
                            xb[bl++] = hex(ch >> 4);
                            xb[bl++] = hex(ch);
                        }
                        break;
                }

                // Is there extra escape sequence?
                if (bl > 4)
                {
                    // Write string directly to output
                    if (last < curr)
                    {
                        if ((res = pOut->write(value, last, curr)) != STATUS_OK)
                            return res;
                    }

                    // Append escape sequence and reset extra buffer size
                    if ((res = pOut->write_ascii(xb, bl)) != STATUS_OK)
                        return res;
                    last = curr+1; // Put buffer pointer to the next character
                    bl = 4;
                }

                // Store previous character
                prev = ch;
            }

            // Need to emit extra data?
            if (last < curr)
            {
                if ((res = pOut->write(value, last, curr)) != STATUS_OK)
                    return res;
            }

            // Output end quote
            return pOut->write_ascii("*/", 2);
        }

        status_t Serializer::write_property(const char *name, const char *charset)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString tmp;
            return (tmp.set_native(name, charset)) ? write_property(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_property(const char *name)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString tmp;
            return (tmp.set_utf8(name)) ? write_property(&tmp) : STATUS_NO_MEM;
        }

        status_t Serializer::write_property(const LSPString *name)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (pOut == NULL)
                return STATUS_BAD_STATE;

            status_t res;

            switch (sState.mode)
            {
                case WRITE_OBJECT:
                    if (sState.flags & SF_PROPERTY)
                        return STATUS_INVALID_VALUE;
                    if ((res = emit_comma()) == STATUS_OK)
                        res = writeln();
                    if (res != STATUS_OK)
                        return res;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }

            sState.flags   &= ~SF_COMMA;
            sState.flags   |= SF_PROPERTY | SF_CONTENT;

            // Do we need to prefer identifiers instead of property name?
            if ((sSettings.identifiers) && (sSettings.version >= JSON_VERSION5))
                res = (Tokenizer::is_valid_identifier(name)) ? pOut->write(name) : write_literal(name);
            else
                res = write_literal(name);

            // Write queued comment, colon and spacing
            return (res == STATUS_OK) ? pOut->write(':') : res;
        }

        status_t Serializer::start_object()
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            status_t res = STATUS_OK;
            switch (sState.mode)
            {
                case WRITE_ROOT:
                    if (sState.flags & SF_VALUE)
                        return STATUS_INVALID_VALUE;
                    break;
                case WRITE_ARRAY:
                    if ((res = emit_comma()) == STATUS_OK)
                        res = writeln();
                    break;
                case WRITE_OBJECT:
                    if (!(sState.flags & SF_PROPERTY))
                        return STATUS_INVALID_VALUE;
                    sState.flags &= ~SF_PROPERTY;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }
            if (res == STATUS_OK)
                res = emit_separator();

            sState.flags   |= SF_VALUE | SF_CONTENT;
            if (res == STATUS_OK)
                res = pOut->write('{');
            return (res == STATUS_OK) ? push_state(WRITE_OBJECT) : res;
        }

        status_t Serializer::end_object()
        {
            if ((pOut == NULL) || (sState.mode != WRITE_OBJECT) || (sState.flags & SF_PROPERTY))
                return STATUS_BAD_STATE;
            else if ((sState.flags & SF_COMMA) && (sSettings.version < JSON_VERSION5))
                return STATUS_INVALID_VALUE;

            bool data = sState.flags & SF_VALUE;
            status_t res = pop_state();
            if ((res == STATUS_OK) && (data))
                res = writeln();

            sState.flags   &= ~SF_COMMA;
            sState.flags   |= SF_CONTENT;
            return (res == STATUS_OK) ? pOut->write('}') : res;
        }

        status_t Serializer::start_array()
        {
            if (pOut == NULL)
                return STATUS_BAD_STATE;

            status_t res = STATUS_OK;
            switch (sState.mode)
            {
                case WRITE_ROOT:
                    if (sState.flags & SF_VALUE)
                        return STATUS_INVALID_VALUE;
                    break;
                case WRITE_ARRAY:
                    if ((res = emit_comma()) == STATUS_OK)
                        res = writeln();
                    break;
                case WRITE_OBJECT:
                    if (!(sState.flags & SF_PROPERTY))
                        return STATUS_INVALID_VALUE;
                    sState.flags &= ~SF_PROPERTY;
                    break;
                default:
                    return STATUS_BAD_STATE;
            }
            if (res == STATUS_OK)
                res = emit_separator();

            sState.flags   |= SF_VALUE | SF_CONTENT;
            if (res == STATUS_OK)
                res = pOut->write('[');
            return (res == STATUS_OK) ? push_state(WRITE_ARRAY) : res;
        }

        status_t Serializer::end_array()
        {
            if ((pOut == NULL) || (sState.mode != WRITE_ARRAY))
                return STATUS_BAD_STATE;
            else if ((sState.flags & SF_COMMA) && (sSettings.version < JSON_VERSION5))
                return STATUS_INVALID_VALUE;

            bool data = sState.flags & SF_VALUE;
            status_t res = pop_state();
            if ((res == STATUS_OK) && (data))
                res = writeln();

            sState.flags   &= ~SF_COMMA;
            sState.flags   |= SF_CONTENT;
            return (res == STATUS_OK) ? pOut->write(']') : res;
        }

    } /* namespace json */
} /* namespace lsp */
