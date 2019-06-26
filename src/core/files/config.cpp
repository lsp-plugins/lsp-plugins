/*
 * ConfigFile.cpp
 *
 *  Created on: 11 июн. 2018 г.
 *      Author: sadko
 */

#include <core/files/config.h>
#include <core/debug.h>
#include <core/io/IInSequence.h>
#include <stdio.h>
#include <locale.h>

#include <core/io/InSequence.h>
#include <core/io/InStringSequence.h>
#include <core/io/OutSequence.h>
#include <core/io/OutStringSequence.h>

namespace lsp
{
    namespace config
    {
        static bool validate_key(const LSPString *key)
        {
            for (size_t i=0, n=key->length(); i<n; ++i)
            {
                lsp_wchar_t ch = key->at(i);
                if (((ch >= 'a') && (ch <= 'z')) ||
                    ((ch >= 'A') && (ch <= 'Z')) ||
                    ((ch >= '0') && (ch <= '9')) ||
                    (ch == '_') || (ch == '/'))
                    continue;

                return false;
            }

            return true;
        }

        static status_t read_key(const LSPString *line, LSPString *key, size_t &off)
        {
            size_t len      = line->length();

            while (off < len)
            {
                lsp_wchar_t ch = line->at(off++);

                if ((ch == ' ') || (ch == '\t'))
                {
                    if (key->is_empty())
                        continue;

                    while (off < len)
                    {
                        ch = line->at(off);
                        if ((ch != ' ') && (ch != '\t'))
                            break;
                        off ++;
                    }

                    return STATUS_OK;
                }
                else if (ch == '=') // Assignment occurred?
                {
                    if (key->is_empty())
                        return STATUS_BAD_FORMAT;
                    off --;
                    break;
                }
                else if (ch == '#')
                {
                    off --;
                    break;
                }
                else if (((ch >= 'a') && (ch <= 'z')) ||
                        ((ch >= 'A') && (ch <= 'Z')) ||
                        ((ch >= '0') && (ch <= '9')) ||
                        (ch == '_') || (ch == '/'))
                {
                    LSP_BOOL_ASSERT(key->append(ch), STATUS_NO_MEM)
                }
                else
                    return STATUS_BAD_FORMAT;
            }

            // Validate that key should start with '/' or not contain any '/' character
            if (key->index_of('/') > 0)
                return STATUS_BAD_FORMAT;

            return STATUS_OK;
        }

        static status_t check_type(const LSPString *line, size_t &off, size_t &flags)
        {
            size_t last = off + 4;
            if (last > line->length())
                last    = line->length();

            const char *utf8 = line->get_utf8(off - 1, last);
            if (::strstr(utf8, "i32:") == utf8)
            {
                flags  |= SF_TYPE_I32;
                off += 3;
                return STATUS_SKIP;
            }
            else if (::strstr(utf8, "u32:") == utf8)
            {
                flags  |= SF_TYPE_U32;
                off += 3;
                return STATUS_SKIP;
            }
            else if (::strstr(utf8, "i64:") == utf8)
            {
                flags  |= SF_TYPE_I64;
                off += 3;
                return STATUS_SKIP;
            }
            else if (::strstr(utf8, "u64:") == utf8)
            {
                flags  |= SF_TYPE_U64;
                off += 3;
                return STATUS_SKIP;
            }
            else if (::strstr(utf8, "f32:") == utf8)
            {
                flags  |= SF_TYPE_F32;
                off += 3;
                return STATUS_SKIP;
            }
            else if (::strstr(utf8, "f64:") == utf8)
            {
                flags  |= SF_TYPE_F64;
                off += 3;
                return STATUS_SKIP;
            }
            else if (::strstr(utf8, "str:") == utf8)
            {
                flags  |= SF_TYPE_STR;
                off += 3;
                return STATUS_SKIP;
            }
            else if (::strstr(utf8, "blob:") == utf8)
            {
                flags  |= SF_TYPE_BLOB;
                off += 4;
                return STATUS_SKIP;
            }

            return STATUS_OK;
        }

        static status_t read_value(const LSPString *line, LSPString *value, size_t &off, size_t &flags)
        {
            size_t len      = line->length();

            flags           = 0;
            bool type_set   = false;
            bool quoted     = false;
            bool protector  = false;

            while (off < len)
            {
                lsp_wchar_t ch = line->at(off++);

                switch (ch)
                {
                    case '\"':
                        if (protector)
                        {
                            LSP_BOOL_ASSERT(value->append(ch), STATUS_NO_MEM);
                            protector   = false;
                        }
                        else if (!quoted)
                        {
                            if (!value->is_empty())
                                return STATUS_BAD_FORMAT;
                            quoted      = true;
                            flags      |= SF_QUOTED;
                        }
                        else
                            return STATUS_OK;
                        break;

                    case '\\':
                        if (protector)
                            LSP_BOOL_ASSERT(value->append(ch), STATUS_NO_MEM);

                        protector   = !protector;
                        break;

                    case ' ':
                    case '\t':
                    case '#':
                        if (quoted)
                        {
                            if (protector)
                            {
                                LSP_BOOL_ASSERT(value->append('\\'), STATUS_NO_MEM);
                                protector   = false;
                            }
                            LSP_BOOL_ASSERT(value->append(ch), STATUS_NO_MEM);
                        }
                        else if (protector)
                        {
                            LSP_BOOL_ASSERT(value->append(ch), STATUS_NO_MEM);
                            protector   = false;
                        }
                        else if (ch == '#')
                        {
                            off --;
                            return STATUS_OK;
                        }
                        else if (!value->is_empty())
                            return STATUS_OK;
                        break;

                    case '\n':
                        if (quoted)
                            return STATUS_BAD_FORMAT;
                        return STATUS_OK;

                    case 'n':
                    case 't':
                        if (!type_set)
                        {
                            status_t res = check_type(line, off, flags);
                            type_set = true;
                            if (res == STATUS_SKIP)
                                break;
                            else if (res != STATUS_OK)
                                return res;
                        }

                        if (protector)
                            ch = (ch == 'n') ? '\n' : '\t';

                        LSP_BOOL_ASSERT(value->append(ch), STATUS_NO_MEM);
                        protector   = false;
                        break;

                    default:
                        // Check type prefix
                        if (!type_set)
                        {
                            status_t res = check_type(line, off, flags);
                            type_set = true;
                            if (res == STATUS_SKIP)
                                break;
                            else if (res != STATUS_OK)
                                return res;
                        }

                        if (protector)
                        {
                            LSP_BOOL_ASSERT(value->append('\\'), STATUS_NO_MEM);
                            protector   = false;
                        }
                        LSP_BOOL_ASSERT(value->append(ch), STATUS_NO_MEM);
                        break;
                }
            }

            if ((quoted) || (protector))
                return STATUS_BAD_FORMAT;

            return STATUS_OK;
        }

        static status_t parse_line(const LSPString *line, LSPString *key, LSPString *value, size_t *flags)
        {
            size_t len      = line->length();
            if (len <= 0)
                return STATUS_OK;
            size_t off      = 0;

            // Fetch the key value
            status_t res    = read_key(line, key, off);
            if (res != STATUS_OK) // Error while parsing line
                return res;

            if (off >= len)
            {
                if (key->is_empty())
                    return STATUS_OK;
                return (value->set(key)) ? STATUS_OK : STATUS_NO_MEM;
            }

            // Analyze character
            lsp_wchar_t ch = line->at(off++);
            if (ch == '#') // Skip comments
            {
                if (key->is_empty())
                    return STATUS_OK;
                return (value->set(key)) ? STATUS_OK : STATUS_NO_MEM;
            }
            else if ((key->is_empty()) || (ch != '='))
                return STATUS_BAD_FORMAT;

            // Fetch the value's value
            res             = read_value(line, value, off, *flags);
            if (res != STATUS_OK)
                return res;

            // Ensure that line ends correctly
            while (off < len)
            {
                switch (line->at(off++))
                {
                    case '#':
                        return STATUS_OK;
                    case ' ':
                    case '\t':
                        break;
                    default:
                        return STATUS_BAD_FORMAT;
                }
            }

            return STATUS_OK;
        }

        static status_t serialize_comment(io::IOutSequence *os, const LSPString *comment)
        {
            size_t first = 0;
            while (true)
            {
                os->write_ascii("# ");
                ssize_t split = comment->index_of(first, '\n');
                if (split < 0)
                {
                    os->write(comment, first);
                    os->write('\n');
                    return STATUS_OK;
                }
                else
                {
                    os->write(comment, first, split);
                    os->write('\n');
                    first = split + 1;
                }
            }
        }

        static status_t serialize_value(io::IOutSequence *os, const LSPString *value, int flags)
        {
            // Write type prefix if type specified
            const char *prefix = 0;

            switch (flags & SF_TYPE_MASK)
            {
                case SF_TYPE_I32: prefix = "i32:"; break;
                case SF_TYPE_U32: prefix = "u32:"; break;
                case SF_TYPE_I64: prefix = "i64:"; break;
                case SF_TYPE_U64: prefix = "u64:"; break;
                case SF_TYPE_F32: prefix = "f32:"; break;
                case SF_TYPE_F64: prefix = "f64:"; break;
                case SF_TYPE_STR: prefix = "str:"; flags |= SF_QUOTED; break;
                case SF_TYPE_BLOB: prefix = "blob:"; flags |= SF_QUOTED; break;
                default:
                    break;
            }

            if (prefix != NULL)
                LSP_STATUS_ASSERT(os->write_ascii(prefix));

            size_t n = value->length();

            if (n > 0)
            {
                // Emit quote
                if (flags & SF_QUOTED)
                    LSP_STATUS_ASSERT(os->write('\"'));

                for (size_t i=0; i<n; ++i)
                {
                    lsp_wchar_t ch = value->at(i);

                    switch (ch)
                    {
                        case '\"':
                        case '\\':
                        case '\n':
                        case '\t':
                            LSP_STATUS_ASSERT(os->write('\\'));
                            break;
                        default:
                            break;
                    }
                    LSP_STATUS_ASSERT(os->write(ch));
                }

                // Emit quote
                if (flags & SF_QUOTED)
                    LSP_STATUS_ASSERT(os->write('\"'));
            }
            else if (flags & SF_QUOTED)
                LSP_STATUS_ASSERT(os->write_ascii("\"\""));

            return STATUS_OK;
        }

        status_t save(io::IOutSequence *os, IConfigSource *s, bool comments)
        {
            LSPString key, value, comment;
            status_t res;

            // Change locale
            char *saved_locale  = setlocale(1, "C");

            // Print the comment
            if (comments)
            {
                LSP_STATUS_ASSERT(os->write_ascii("#-------------------------------------------------------------------------------\n"));

                res = s->get_head_comment(&comment);
                if (res == STATUS_OK)
                {
                    LSP_STATUS_ASSERT(os->write_ascii("#\n"));
                    LSP_STATUS_ASSERT(serialize_comment(os, &comment));
                    LSP_STATUS_ASSERT(os->write_ascii("#-------------------------------------------------------------------------------\n"));
                }
                else if ((res != STATUS_NO_DATA) && (res != STATUS_NOT_FOUND))
                {
                    setlocale(1, saved_locale);
                    return res;
                }
            }

            while (true)
            {
                // Get next parameter
                int flags = 0;
                key.clear();
                value.clear();
                comment.clear();

                res = s->get_parameter(&key, &value, &comment, &flags);
                if ((res == STATUS_NO_DATA) || (res == STATUS_NOT_FOUND) || (res == STATUS_EOF))
                    break;
                else if (res != STATUS_OK)
                {
                    setlocale(1, saved_locale);
                    return res;
                }

                if (comments)
                {
                    LSP_STATUS_ASSERT(os->write('\n'));

                    // Serialize comment (if present)
                    if (!comment.is_empty())
                        serialize_comment(os, &comment);
                }

                // Validate and serialize key
                if (!validate_key(&key))
                {
                    setlocale(1, saved_locale);
                    return STATUS_BAD_FORMAT;
                }

                LSP_STATUS_ASSERT(os->write(&key));
                LSP_STATUS_ASSERT(os->write_ascii(" = "));

                // Serialize value
                res = serialize_value(os, &value, flags);
                if (res != STATUS_OK)
                {
                    setlocale(1, saved_locale);
                    return res;
                }
                LSP_STATUS_ASSERT(os->write('\n'));
            }

            if (comments)
                LSP_STATUS_ASSERT(os->write_ascii("\n#-------------------------------------------------------------------------------\n"));

            // Return locale
            setlocale(1, saved_locale);

            return STATUS_OK;
        }


        status_t save(FILE *fd, IConfigSource *s, bool comments)
        {
            io::OutSequence fos;
            status_t res = fos.wrap(fd, WRAP_NONE);
            if (res != STATUS_OK)
            {
                fos.close();
                return res;
            }

            res = save(&fos, s, comments);
            if (res != STATUS_OK)
            {
                fos.close();
                return res;
            }

            return fos.close();
        }

        status_t save(io::File *fd, IConfigSource *s, bool comments)
        {
            io::OutSequence fos;
            status_t res = fos.wrap(fd, WRAP_NONE);
            if (res != STATUS_OK)
            {
                fos.close();
                return res;
            }

            res = save(&fos, s, comments);
            if (res != STATUS_OK)
            {
                fos.close();
                return res;
            }

            return fos.close();
        }

        status_t save(io::IOutStream *os, IConfigSource *s, bool comments)
        {
            io::OutSequence fos;
            status_t res = fos.wrap(os, WRAP_NONE);
            if (res != STATUS_OK)
            {
                fos.close();
                return res;
            }

            res = save(&fos, s, comments);
            if (res != STATUS_OK)
            {
                fos.close();
                return res;
            }

            return fos.close();
        }

        status_t save(const char *path, IConfigSource *s, bool comments)
        {
            io::OutSequence fos;
            status_t res = fos.open(path, io::File::FM_CREATE | io::File::FM_TRUNC);
            if (res != STATUS_OK)
            {
                fos.close();
                return res;
            }

            res = save(&fos, s, comments);
            if (res != STATUS_OK)
            {
                fos.close();
                return res;
            }

            return fos.close();
        }

        status_t load(io::IInSequence *is, IConfigHandler *h)
        {
            status_t result = STATUS_OK;
            LSPString line, key, value;

            while (result == STATUS_OK)
            {
                // Clear buffer
                line.clear();
                key.clear();
                value.clear();

                // Read line from file
                result      = is->read_line(&line, true);
                if (result != STATUS_OK)
                {
                    if (result == STATUS_EOF)
                        result = STATUS_OK;
                    break;
                }
                //lsp_trace("Config line: %s", line.get_native());

                // Parse the line
                size_t flags = 0;
                result = parse_line(&line, &key, &value, &flags);
                if (result != STATUS_OK)
                    break;

                if (!key.is_empty())
                {
                    lsp_trace("Configuration: %s = %s (flags=0x%x)", key.get_native(), value.get_native(), int(flags));
                    result = h->handle_parameter(&key, &value, flags);
                }
            }

            return result;
        }

        status_t load(io::IInStream *is, IConfigHandler *h)
        {
            io::InSequence isr;
            status_t res = isr.wrap(is, false);
            if (res != STATUS_OK)
            {
                isr.close();
                return res;
            }

            res = load(&isr, h);
            if (res != STATUS_OK)
            {
                isr.close();
                return res;
            }

            return isr.close();
        }

        status_t load(io::File *fd, IConfigHandler *h)
        {
            io::InSequence isr;
            status_t res = isr.wrap(fd, false);
            if (res != STATUS_OK)
            {
                isr.close();
                return res;
            }

            res = load(&isr, h);
            if (res != STATUS_OK)
            {
                isr.close();
                return res;
            }

            return isr.close();
        }

        status_t load(FILE *fd, IConfigHandler *h)
        {
            io::InSequence fis;
            status_t res = fis.wrap(fd, WRAP_NONE);
            if (res != STATUS_OK)
            {
                fis.close();
                return res;
            }

            res = load(&fis, h);
            if (res != STATUS_OK)
            {
                fis.close();
                return res;
            }

            return fis.close();
        }

        status_t load(const char *path, IConfigHandler *h)
        {
            io::InSequence fis;
            status_t res = fis.open(path);
            if (res != STATUS_OK)
            {
                fis.close();
                return res;
            }

            res = load(&fis, h);
            if (res != STATUS_OK)
            {
                fis.close();
                return res;
            }

            return fis.close();
        }

        status_t serialize(LSPString *cfg, IConfigSource *s, bool comments)
        {
            io::OutStringSequence sos(cfg);
            status_t res = save(&sos, s, comments);
            if (res != STATUS_OK)
            {
                sos.close();
                return res;
            }

            return sos.close();
        }

        status_t deserialize(const LSPString *cfg, IConfigHandler *h)
        {
            io::InStringSequence sis(cfg, false);
            status_t res = load(&sis, h);
            if (res != STATUS_OK)
            {
                sis.close();
                return res;
            }

            return sis.close();
        }
    }
} /* namespace lsp */
