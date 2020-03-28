/*
 * parse.cpp
 *
 *  Created on: 23 дек. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        typedef struct file_format_t
        {
            const char *id;
            const char *filter;
            const char *text;
            const char *ext;
            size_t flags;
        } file_format_t;

        static const file_format_t file_formats[] =
        {
            { "wav", "*.wav", "files.audio.wave", ".wav", LSPFileMask::NONE },
            { "lspc", "*.lspc", "files.config.lspc", ".lspc", LSPFileMask::NONE },
            { "cfg", "*.cfg", "files.config.lsp", ".cfg", LSPFileMask::NONE },
            { "audio", "*.wav", "files.audio.supported", ".wav", LSPFileMask::NONE },
            { "audio_lspc", "*.wav|*.lspc", "files.audio.audio_lspc", ".wav", LSPFileMask::NONE },
            { "obj3d", "*.obj", "files.3d.wavefont", ".obj", LSPFileMask::NONE },
            { "all", "*", "files.all", "", LSPFileMask::NONE },
            { NULL, NULL, NULL, 0 }
        };

        void add_format(LSPFileFilter *flt, const char *variable, size_t n)
        {
            for (const file_format_t *f = file_formats; f->id != NULL; ++f)
            {
                if (!strncasecmp(f->id, variable, n))
                {
                    LSPFileFilterItem ffi;
                    ffi.pattern()->set(f->filter, f->flags);
                    ffi.title()->set(f->text);
                    ffi.set_extension(f->ext);

                    flt->add(&ffi);
                    return;
                }
            }
        }

        bool parse_file_formats(const char *variable, LSPFileFilter *flt)
        {
            status_t res = flt->clear();
            if (res != STATUS_OK)
                return res;

            while (true)
            {
                while (*variable == ' ')
                    variable ++;
                if (*variable == '\0')
                    break;

                const char *s = strchr(variable, ',');
                const char *end = (s == NULL) ? strchr(variable, '\0') : s;
                while ((end > variable) && (end[-1] == ' '))
                    --end;

                if (end > variable)
                    add_format(flt, variable, end - variable);

                if (s == NULL)
                    break;
                variable = s + 1;
            }

            return true;
        }

        bool set_port_value(CtlPort *up, const char *value, size_t flags)
        {
            if (up == NULL)
                return false;

            // Get metadata
            const port_t *p = up->metadata();
            if (p == NULL)
                return false;

            // Check that it's a control port
            if (!IS_IN_PORT(p))
                return false;

            // Apply changes
            switch (p->role)
            {
                case R_PORT_SET:
                case R_CONTROL:
                {
                    if (is_discrete_unit(p->unit))
                    {
                        if (p->unit == U_BOOL)
                        {
                            PARSE_BOOL(value, up->set_value(__, flags); );
                        }
                        else
                        {
                            PARSE_INT(value, up->set_value(__, flags); );
                        }
                    }
                    else
                    {
                        PARSE_FLOAT(value, up->set_value(__, flags); );
                    }
                    break;
                }
                case R_PATH:
                {
                    size_t len      = ::strlen(value);
                    up->write(value, len, flags);
                    break;
                }
                default:
                    return false;
            }
            return true;
        }

        status_t format_port_value(CtlPort *up, LSPString *name, LSPString *value, LSPString *comment, int *flags)
        {
            // Get metadata
            const port_t *p    = up->metadata();
            if (p == NULL)
                return STATUS_OK;

            switch (p->role)
            {
                case R_PORT_SET:
                case R_CONTROL:
                {
                    // Serialize meta information
                    const char *unit = encode_unit(p->unit);
                    if (unit != NULL)
                        LSP_BOOL_ASSERT(comment->fmt_append_utf8("%s [%s]", p->name, unit), STATUS_NO_MEM)
                    else if (p->unit == U_BOOL)
                        LSP_BOOL_ASSERT(comment->fmt_append_utf8("%s [boolean]", p->name), STATUS_NO_MEM)
                    else
                        LSP_BOOL_ASSERT(comment->append_utf8(p->name), STATUS_NO_MEM);

                    if ((p->flags & (F_LOWER | F_UPPER)) || (p->unit == U_ENUM) || (p->unit == U_BOOL))
                    {
                        if (is_discrete_unit(p->unit) || (p->flags & F_INT))
                        {
                            if (p->unit != U_BOOL)
                            {
                                if (p->unit == U_ENUM)
                                {
                                    int value       = p->min + list_size(p->items) - 1;
                                    LSP_BOOL_ASSERT(comment->fmt_append_utf8(": %d..%d", int(p->min), int(value)), STATUS_NO_MEM);
                                }
                                else
                                    LSP_BOOL_ASSERT(comment->fmt_append_utf8(": %d..%d", int(p->min), int(p->max)), STATUS_NO_MEM);
                            }
                            else
                                LSP_BOOL_ASSERT(comment->append_utf8(": true/false"), STATUS_NO_MEM);
                        }
                        else if (!(p->flags & F_EXT))
                        {
                            LSP_BOOL_ASSERT(comment->fmt_append_utf8(": %.8f..%.8f", p->min, p->max), STATUS_NO_MEM);
                        }
                        else
                        {
                            LSP_BOOL_ASSERT(comment->fmt_append_utf8(": %.12f..%.12f", p->min, p->max), STATUS_NO_MEM);
                        }
                    }

                    // Describe enum
                    if ((p->unit == U_ENUM) && (p->items != NULL))
                    {
                        int value   = p->min;
                        for (const port_item_t *item = p->items; item->text != NULL; ++item)
                            LSP_BOOL_ASSERT(comment->fmt_append_utf8("\n  %d: %s", value++, item->text), STATUS_NO_MEM);
                    }

                    // Serialize name
                    LSP_BOOL_ASSERT(name->append_utf8(p->id), STATUS_NO_MEM);

                    // Serialize value
                    float v = up->get_value();
                    if (is_discrete_unit(p->unit) || (p->flags & F_INT))
                    {
                        if (p->unit == U_BOOL)
                            LSP_BOOL_ASSERT(value->append_utf8((v >= 0.5f) ? "true" : "false"), STATUS_NO_MEM)
                        else
                            LSP_BOOL_ASSERT(value->fmt_utf8("%d", int(v)), STATUS_NO_MEM);
                    }
                    else if (!(p->flags & F_EXT))
                    {
                        LSP_BOOL_ASSERT(value->fmt_utf8("%.8f", v), STATUS_NO_MEM);
                    }
                    else
                    {
                        LSP_BOOL_ASSERT(value->fmt_utf8("%.12f", v), STATUS_NO_MEM);
                    }

                    // No flags
                    *flags = 0;
                    break;
                }
                case R_PATH:
                {
                    LSP_BOOL_ASSERT(comment->fmt_append_utf8("%s [pathname]", p->name), STATUS_NO_MEM);
                    LSP_BOOL_ASSERT(name->append_utf8(p->id), STATUS_NO_MEM);

                    const char *path    = up->get_buffer<const char>();
                    if (value != NULL)
                        LSP_BOOL_ASSERT(value->append_utf8(path), STATUS_NO_MEM)
                    else
                        LSP_BOOL_ASSERT(value->append_utf8(""), STATUS_NO_MEM);

                    // No flags
                    *flags = config::SF_QUOTED;
                    break;
                }
                default:
                    return STATUS_BAD_TYPE;
            }
            return STATUS_OK;
        }
    }
}

