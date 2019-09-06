/*
 * RoomEQWizard.cpp
 *
 *  Created on: 6 сент. 2019 г.
 *      Author: sadko
 */

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
            cfg->vMaj           = major;
            cfg->vMin           = minor;
            cfg->sHeader        = xhdr;
            cfg->sNotes         = xnotes;
            cfg->nFilters       = filters;

            return cfg;
        }

        status_t load_object_stream(java::ObjectStream *os, config_t **dst)
        {
            status_t res;
            LSPString hdr, notes;
            java::RawArray *array;
            java::int_t major, minor;

            // Read the REW data in Java serialization format
            if ((res = os->read_string(&hdr)) != STATUS_OK)
                return res;
            if ((res = os->read_int(&major)) != STATUS_OK)
                return res;
            if ((res = os->read_int(&minor)) != STATUS_OK)
                return res;
            if ((res = os->read_string(&notes)) != STATUS_OK)
                return res;
            if ((res = os->read_array(&array)) != STATUS_OK)
                return res;

            // Now we are ready to allocate the data
            config_t *cfg   = build_config(&hdr, &notes, major, minor, array->length());
            if (cfg == NULL)
                return STATUS_NO_MEM;

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
            status_t res = is->sink(&os);
            if (res == STATUS_OK)
                res = load(os.data(), os.size(), dst);

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


