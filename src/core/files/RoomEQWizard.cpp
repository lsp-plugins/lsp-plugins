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

#include <core/files/RoomEQWizard.h>
#include <core/files/java/ObjectStream.h>

namespace lsp
{
    namespace room_ew
    {
        config_t *build_config(const LSPString *hdr, const LSPString *notes,
                int32_t major, int32_t minor, size_t filters)
        {
            const char *phdr    = hdr->get_utf8();
            if (phdr == NULL)
                return NULL;
            const char *pnotes  = notes->get_utf8();
            if (pnotes == NULL)
                return NULL;

            size_t  n_hdr       = ::strlen(phdr) + 1;
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

            char *xhdr          = reinterpret_cast<char *>(ptr);
            char *xnotes        = &xhdr[n_hdr];
            ptr                += n_strings;
            ::memcpy(xhdr, phdr, n_hdr);
            ::memcpy(xnotes, pnotes, n_hdr);

            cfg->vFilters       = reinterpret_cast<filter_t *>(ptr);
            ptr                += n_filters;

            // Fill the basic configuration
            cfg->nVerMaj        = major;
            cfg->nVerMin        = minor;
            cfg->sEqType        = xhdr;
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
            LSPString hdr, notes, eq;
            java::RawArray *filters;
            java::int_t major, minor, stub;

            // Read the REW data in Java serialization format
            if ((res = os->read_string(&hdr)) != STATUS_OK)
                return res;
            if (!eq.set_ascii("Equaliser:"))
                return STATUS_NO_MEM;
            ssize_t idx = hdr.index_of(&eq);
            if (idx >= 0)
                hdr.remove(0, idx + eq.length());
            lsp_trace("equalizer: %s", hdr.get_utf8());
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
            config_t *cfg   = build_config(&hdr, &notes, major, minor, filters->length());
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

        status_t load_text(io::IInStream *is, config_t **dst)
        {
            return STATUS_OK;
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


