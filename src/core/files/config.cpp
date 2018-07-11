/*
 * ConfigFile.cpp
 *
 *  Created on: 11 июн. 2018 г.
 *      Author: sadko
 */

#include <core/files/config.h>
#include <core/debug.h>
#include <stdio.h>
#include <locale.h>

#include <core/io/FileReader.h>
#include <core/io/FileWriter.h>
#include <core/io/StringReader.h>
#include <core/io/StringWriter.h>
#include <core/io/InputStreamReader.h>

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
                    (ch == '_'))
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
                        (ch == '_'))
                {
                    LSP_BOOL_ASSERT(key->append(ch), STATUS_NO_MEM)
                }
                else
                    return STATUS_BAD_FORMAT;
            }

            return STATUS_OK;
        }

        static status_t read_value(const LSPString *line, LSPString *value, size_t &off)
        {
            size_t len      = line->length();

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
                        if (protector)
                            ch = (ch == 'n') ? '\n' : '\t';

                        LSP_BOOL_ASSERT(value->append(ch), STATUS_NO_MEM);
                        protector   = false;
                        break;

                    default:
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

        static status_t parse_line(const LSPString *line, LSPString *key, LSPString *value)
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

            // Fetch the value value
            res             = read_value(line, value, off);
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

        static status_t serialize_comment(io::Writer *os, const LSPString *comment)
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

        static status_t serialize_value(io::Writer *os, const LSPString *value, int flags)
        {
            size_t n = value->length();
            if (n > 0)
            {
                LSPString escaped;
                escaped.reserve(value->length() + 32);

                for (size_t i=0; i<n; ++i)
                {
                    lsp_wchar_t ch = value->at(i);

                    switch (ch)
                    {
                        case '\"':
                        case '\\':
                        case '\n':
                        case '\t':
                            if (!escaped.append('\\'))
                                return STATUS_NO_MEM;
                            break;
                        default:
                            break;
                    }

                    if (!escaped.append(ch))
                        return STATUS_NO_MEM;
                }

                // Emit parameter
                if (flags & SF_QUOTED)
                    LSP_STATUS_ASSERT(os->write('\"'));
                LSP_STATUS_ASSERT(os->write(&escaped));
                if (flags & SF_QUOTED)
                    LSP_STATUS_ASSERT(os->write('\"'));
            }
            else if (flags & SF_QUOTED)
                LSP_STATUS_ASSERT(os->write_ascii("\"\""));

            return STATUS_OK;
        }

        status_t save(io::Writer *os, IConfigSource *s, bool comments)
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
            io::FileWriter fos;
            status_t res = fos.attach(fd);
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
            io::FileWriter fos;
            status_t res = fos.open(path);
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

        status_t load(io::Reader *is, IConfigHandler *h)
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

                // Parse the line
                result = parse_line(&line, &key, &value);
                if (result != STATUS_OK)
                    break;

                if (!key.is_empty())
                {
                    lsp_trace("Configuration: %s = %s", key.get_native(), value.get_native());
                    result = h->handle_parameter(&key, &value);
                }
            }

            return result;
        }

        status_t load(io::IInputStream *is, IConfigHandler *h)
        {
            io::InputStreamReader isr;
            status_t res = isr.attach(is);
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
            io::FileReader fis;
            status_t res = fis.attach(fd);
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
            io::FileReader fis;
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
            io::StringWriter sos(cfg);
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
            io::StringReader sis(cfg, false);
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
