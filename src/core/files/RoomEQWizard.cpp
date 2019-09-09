/*
 * RoomEQWizard.cpp
 *
 *  Created on: 6 сент. 2019 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/io/InFileStream.h>
#include <core/io/OutMemoryStream.h>
#include <core/io/InMemoryStream.h>
#include <core/io/InSequence.h>

#include <core/files/RoomEQWizard.h>
#include <core/files/java/ObjectStream.h>

#include <dsp/endian.h>

#include <data/cstorage.h>

namespace lsp
{
    namespace room_ew
    {
        const char *charsets[] =
        {
            "UTF-8",
            "UTF-16LE",
            "UTF-16BE",
            NULL
        };

        config_t *build_config(const LSPString *eq, const LSPString *notes,
                int32_t major, int32_t minor, size_t filters)
        {
            const char *peq     = eq->get_utf8();
            if (peq == NULL)
                return NULL;
            const char *pnotes  = notes->get_utf8();
            if (pnotes == NULL)
                return NULL;

            size_t  n_hdr       = ::strlen(peq) + 1;
            size_t  n_notes     = ::strlen(pnotes) + 1;
            size_t  n_strings   = ALIGN_SIZE(n_hdr + n_notes, DEFAULT_ALIGN);
            size_t  n_cfg       = ALIGN_SIZE(sizeof(config_t), DEFAULT_ALIGN);
            size_t  n_filters   = ALIGN_SIZE(sizeof(filter_t) * filters, DEFAULT_ALIGN);
            size_t  to_alloc    = n_strings + n_cfg + n_filters;

            uint8_t *ptr        = reinterpret_cast<uint8_t *>(::malloc(to_alloc));
            if (ptr == NULL)
                return NULL;
            ::bzero(ptr, to_alloc);

            config_t *cfg       = reinterpret_cast<config_t *>(ptr);
            ptr                += n_cfg;

            char *xeq          = reinterpret_cast<char *>(ptr);
            char *xnotes        = &xeq[n_hdr];
            ptr                += n_strings;
            ::memcpy(xeq, peq, n_hdr);
            ::memcpy(xnotes, pnotes, n_hdr);

            cfg->vFilters       = reinterpret_cast<filter_t *>(ptr);
            ptr                += n_filters;

            // Fill the basic configuration
            cfg->nVerMaj        = major;
            cfg->nVerMin        = minor;
            cfg->sEqType        = xeq;
            cfg->sNotes         = xnotes;
            cfg->nFilters       = filters;

            return cfg;
        }

        filter_type_t decode_filter_type(const char *type)
        {
            #define DECODE(text, ftype) \
                if (!::strcasecmp(type, text)) return ftype;

            DECODE("PK", PK);
            DECODE("MODAL", MODAL);
            DECODE("LP", LP);
            DECODE("HP", HP);
            DECODE("LPQ", LPQ);
            DECODE("HPQ", HPQ);
            DECODE("LS", LS);
            DECODE("HS", HS);
            DECODE("LS6", LS6);
            DECODE("HS6", HS6);
            DECODE("LS12", LS12);
            DECODE("HS12", HS12);
            DECODE("NO", NO);
            DECODE("AP", AP);

            #undef DECODE

            return NONE;
        }

        status_t load_object_stream(java::ObjectStream *os, config_t **dst)
        {
            status_t res;
            LSPString eq, notes, xeq;
            java::RawArray *filters;
            java::int_t major=0, minor=0, stub=0;

            // Read the REW data in Java serialization format
            if ((res = os->read_string(&eq)) != STATUS_OK)
                return res;
            if (!xeq.set_ascii("Equaliser:"))
                return STATUS_NO_MEM;
            ssize_t idx = eq.index_of(&xeq);
            if (idx >= 0)
                eq.remove(0, idx + xeq.length());
            lsp_trace("equalizer: %s", eq.get_utf8());
            if ((res = os->read_int(&major)) != STATUS_OK)
                return res;
            if ((res = os->read_int(&minor)) != STATUS_OK)
                return res;
            lsp_trace("version: %d.%d", int(major), int(minor));
            if ((res = os->read_string(&notes)) != STATUS_OK)
                return res;
            if (notes.starts_with_ascii("Notes:"))
                notes.remove(0, 6);
            lsp_trace("notes: %s", notes.get_utf8());

            if ((res = os->read_int(&stub)) != STATUS_OK) // Don't know what this number actually means currently
                return res;
            lsp_trace("stub: %d", int(stub));
            if ((res = os->read_array(&filters)) != STATUS_OK)
                return res;

            // Now we are ready to allocate the data
            config_t *cfg   = build_config(&eq, &notes, major, minor, filters->length());
            if (cfg == NULL)
                return STATUS_NO_MEM;

            // Read the entire data
            const java::Object **vsf = filters->get_objects();
            filter_t *vdf = cfg->vFilters;

            for (size_t i=0, n=filters->length(); i<n; ++i)
            {
                const java::Object *sf  = vsf[i];
                filter_t *df            = &vdf[i];

                java::double_t Q, fc, gain;
                java::bool_t enabled;
                const char *ftype;

                // Read the filter parameters
                if ((res = sf->get_double("Q", &Q)) != STATUS_OK)
                    break;
                if ((res = sf->get_double("fc", &fc)) != STATUS_OK)
                    break;
                if ((res = sf->get_double("gain", &gain)) != STATUS_OK)
                    break;
                if ((res = sf->get_bool("enabled", &enabled)) != STATUS_OK)
                    break;
                if ((res = sf->get_enum("filterType", &ftype)) != STATUS_OK)
                    break;

                lsp_trace(
                        "Filter %d: %s %s Fc %.0f Hz Gain %.1f dB Q %.7f",
                        int(i+1), (enabled) ? "ON" : "OFF", ftype,
                        double(fc), double(gain), double(Q)
                    );

//                lsp_trace(
//                        "check_filter(&vf[idx++], %s, room_ew::%s, %.2f, %.2f, %.7f);",
//                        (enabled) ? "true" : "false", ftype,
//                        double(fc), double(gain), double(Q)
//                    );

                // Fill the state of filter
                df->Q           = Q;
                df->fc          = fc;
                df->gain        = gain;
                df->enabled     = enabled;
                df->filterType  = decode_filter_type(ftype);
            }

            if ((res != STATUS_OK) || (dst == NULL))
                ::free(cfg);
            else if (res == STATUS_OK)
                *dst    = cfg;

            return STATUS_OK;
        }

        status_t load_java(io::IInStream *is, config_t **dst)
        {
            // Open file
            java::Handles handles;
            java::ObjectStream os(&handles);

            status_t res = os.wrap(is, WRAP_NONE);
            if (res == STATUS_OK)
                res     = load_object_stream(&os, dst);

            if (res == STATUS_OK)
                return os.close();

            os.close();
            return res;
        }

        status_t skip_spaces(const LSPString *s, size_t *offset)
        {
            size_t len = s->length();
            while (*offset < len)
            {
                switch (s->char_at(*offset))
                {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\r':
                        ++(*offset);
                        break;
                    default:
                        return STATUS_OK;
                }
            }

            return STATUS_OK;
        }

        status_t parse_decimal(ssize_t *dst, const LSPString *s, size_t *offset)
        {
            // TODO
            return STATUS_OK;
        }

        status_t parse_filter_settings(filter_t *f, const LSPString *s, size_t *offset)
        {
            // TODO
            return STATUS_OK;
        }

        status_t parse_text_config(io::IInSequence *is, config_t **dst)
        {
            LSPString s;
            status_t res;

            // Read header
            if ((res = is->read_line(&s, true)) != STATUS_OK)
                return res;
            if (!s.equals_ascii("Filter Settings file"))
                return STATUS_UNSUPPORTED_FORMAT;

            // Read lines
            LSPString notes, eq;
            ssize_t major=0, minor=0;
            size_t offset = 0;
            cstorage<filter_t> vfilters;

            while ((res = is->read_line(&s, true)) == STATUS_OK)
            {
                if (s.starts_with_ascii("Room EQ V"))
                {
                    offset = 9;
                    if ((res = parse_decimal(&major, &s, &offset)) != STATUS_OK)
                        return res;
                    if ((offset >= s.length()) || (s.char_at(offset) != '.'))
                        return STATUS_BAD_FORMAT;
                    ++offset;
                    if ((res = parse_decimal(&minor, &s, &offset)) != STATUS_OK)
                        return res;
                }
                else if (s.starts_with_ascii("Notes:"))
                {
                    if (!notes.set(&s, 6))
                        return STATUS_NO_MEM;
                }
                else if ((s.starts_with_ascii("Equaliser:")) || (s.starts_with_ascii("Equalizer:")))
                {
                    offset = 10;
                    if ((res = skip_spaces(&s, &offset)) != STATUS_OK)
                        return res;
                    if (!eq.set(&s, offset))
                        return STATUS_NO_MEM;
                }
                else if (s.starts_with_ascii("Filter "))
                {
                    offset = 7;

                    // Find filter definition
                    size_t len = s.length();
                    while (offset < len)
                        if (s.char_at(offset++) == ':')
                            break;

                    // Allocate filter
                    filter_t *f = vfilters.add();
                    if (f == NULL)
                        return STATUS_NO_MEM;

                    // Parse filter settings
                    if ((res = parse_filter_settings(f, &s, &offset)) != STATUS_OK)
                        return res;
                }
            }

            // Analyze current status
            if (res == STATUS_EOF)
                res = STATUS_OK;
            else if (res != STATUS_OK)
                return res;

            // Now we are ready to allocate the data
            config_t *cfg   = build_config(&eq, &notes, major, minor, vfilters.size());
            if (cfg == NULL)
                return STATUS_NO_MEM;

            // Copy filter data
            ::memcpy(cfg->vFilters, vfilters.get_array(), sizeof(filter_t) * vfilters.size());

            return STATUS_OK;
        }

        status_t load_text_file(io::IInStream *is, config_t **dst, const char *charset)
        {
            io::InSequence cs;
            status_t res = cs.wrap(is, WRAP_NONE, charset);
            if (res != STATUS_OK)
            {
                cs.close();
                return res;
            }

            res = parse_text_config(&cs, dst);
            if (res == STATUS_OK)
                return cs.close();

            cs.close();
            return res;
        }

        status_t load_text(io::IInStream *is, config_t **dst)
        {
            // Read UTF-16 signature (if present)
            uint16_t signature;
            status_t res = is->read_block(&signature, sizeof(signature));
            if (res != STATUS_OK)
                return (res == STATUS_EOF) ? STATUS_BAD_FORMAT : res;

            signature = BE_TO_CPU(signature);
            if (signature == 0xfeff) // UTF-16BE
            {
                if ((res = load_text_file(is, dst, "UTF-16BE")) == STATUS_OK)
                    return res;
            }
            else if (signature == 0xfffe) // UTF-16LE
            {
                if ((res = load_text_file(is, dst, "UTF-16LE")) == STATUS_OK)
                    return res;
            }

            // Try to load unicode character sets
            for (const char **cset=charsets; *cset != NULL; ++cset)
            {
                if ((res = is->seek(0)) != STATUS_OK)
                    return res;
                if ((res = load_text_file(is, dst, *cset)) == STATUS_OK)
                    return res;
            }

            if ((res = is->seek(0)) != STATUS_OK)
                return res;
            return load_text_file(is, dst, NULL);
        }


        status_t load(const char *path, config_t **dst)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString tmp;
            if (!tmp.set_utf8(path))
                return STATUS_NO_MEM;

            return load(&tmp, dst);
        }

        status_t load(const LSPString *path, config_t **dst)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InFileStream ifs;
            status_t res = ifs.open(path);
            if (res != STATUS_OK)
            {
                ifs.close();
                return res;
            }

            res = load(&ifs, dst);
            if (res != STATUS_OK)
            {
                ifs.close();
                return res;
            }

            return ifs.close();
        }

        status_t load(const io::Path *path, config_t **dst)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return load(path->as_string(), dst);
        }

        status_t load(FILE *fd, config_t **dst)
        {
            if (fd == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InFileStream ifs;
            status_t res = ifs.wrap(fd, false);
            if (res != STATUS_OK)
            {
                ifs.close();
                return res;
            }
            res = load(&ifs, dst);
            if (res != STATUS_OK)
            {
                ifs.close();
                return res;
            }

            return ifs.close();
        }

        status_t load(io::File *fd, config_t **dst)
        {
            if (fd == NULL)
                return STATUS_BAD_ARGUMENTS;

            io::InFileStream ifs;
            status_t res = ifs.wrap(fd, false);
            if (res != STATUS_OK)
            {
                ifs.close();
                return res;
            }
            res = load(&ifs, dst);
            if (res != STATUS_OK)
            {
                ifs.close();
                return res;
            }

            return ifs.close();
        }

        status_t load(io::IInStream *is, config_t **dst)
        {
            if (is == NULL)
                return STATUS_BAD_ARGUMENTS;

            // We need to sink file to memory to detect the file format (java or text)
            io::OutMemoryStream os;
            wssize_t bytes = is->sink(&os);
            if (bytes < 0)
            {
                os.close();
                return -bytes;
            }

            // Load the file
            status_t res = load(os.data(), os.size(), dst);
            if (res != STATUS_OK)
            {
                os.close();
                return res;
            }

            return os.close();
        }


        status_t load(const void *data, size_t size, config_t **dst)
        {
            if ((data == NULL) || (dst == NULL))
                return STATUS_BAD_ARGUMENTS;

            io::InMemoryStream is;
            is.wrap(data, size);

            // Try to load java format
            status_t res = load_java(&is, dst);
            if (res == STATUS_OK)
                return is.close();
            else if (res != STATUS_BAD_FORMAT)
            {
                is.close();
                return res;
            }

            // Try to load plain text format
            is.seek(0);
            res = load_text(&is, dst);
            if (res == STATUS_OK)
                return is.close();

            is.close();
            return res;
        }
    }
}


