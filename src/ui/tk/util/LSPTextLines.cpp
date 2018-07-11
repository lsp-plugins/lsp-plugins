/*
 * LSPTextLines.cpp
 *
 *  Created on: 26 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        LSPTextLines::LSPTextLines(LSPWidget *widget)
        {
            pWidget     = widget;
            sText       = NULL;
            sLines      = NULL;
        }

        LSPTextLines::~LSPTextLines()
        {
            flush();
        }

        void LSPTextLines::flush()
        {
            if (sText != NULL)
            {
                free(sText);
                sText = NULL;
            }
            if (sLines != NULL)
            {
                free(sLines);
                sLines = NULL;
            }
            vLines.flush();
        }

        status_t LSPTextLines::get_text(LSPString *dst) const
        {
            LSPString tmp, tmp2;
            LSPTextLines *_this = const_cast<LSPTextLines *>(this);

            size_t n = vLines.size();
            // Estimate text size
            for (size_t i=0; i<n; ++i)
            {
                if (i > 0)
                {
                    if (!tmp.append('\n'))
                        return STATUS_NO_MEM;
                }

                if (!tmp2.set_native(_this->vLines.at(i)))
                    return STATUS_NO_MEM;

                if (!tmp.append(&tmp2))
                    return STATUS_NO_MEM;
            }

            tmp.swap(dst);
            return STATUS_OK;
        }

        status_t LSPTextLines::set_text(const char *src)
        {
            if (src == sText)
                return STATUS_OK;
            else if (src == NULL)
            {
                flush();
                if (pWidget != NULL)
                    pWidget->query_resize();
                return STATUS_OK;
            }
            else if (sText != NULL)
            {
                // Prevent from duplicate values
                if (!strcmp(sText, src))
                    return STATUS_OK;
            }

            // Allocate strings
            cvector<char> vlines;
            char *text = strdup(src);
            if (text == NULL)
                return STATUS_NO_MEM;
            char *lines = strdup(src);
            if (lines == NULL)
            {
                free(text);
                return STATUS_NO_MEM;
            }

            // Now we are ready to parse lines
            char *p = lines;
            while (true)
            {
                // Try to add line to list of lines
                if (!vlines.add(p))
                {
                    free(text);
                    free(lines);
                    vlines.flush();
                    return STATUS_NO_MEM;
                }

                // Iterate to next separator
                p = strchr(p, '\n');
                if (p == NULL)
                    break;
                // Replace line termination by end of string
                *(p++) = '\0';
                if (*p == '\r') // Analyze extra character
                    p++;
            }

            // Now we are ready to store new values
            flush();

            sText       = text;
            sLines      = lines;
            vLines.take_from(&vlines);

            // Query for resize
            if (pWidget != NULL)
                pWidget->query_resize();

            return STATUS_OK;
        }

        void LSPTextLines::calc_text_params(ISurface *s, Font *f, ssize_t *w, ssize_t *h)
        {
            font_parameters_t fp;
            text_parameters_t tp;

            *w = 0;
            *h = 0;

            s->get_font_parameters(*f, &fp);

            size_t count = vLines.size();
            if (count > 0)
            {
                // Estimate text size
                for (size_t i=0; i<count; ++i)
                {
                    char *l = vLines.at(i);
                    if (!s->get_text_parameters(*f, &tp, l))
                        continue;

                    (*h)  += fp.Height;
                    if ((*w) < tp.Width)
                        *w = tp.Width;
                }
            }
            else
                *h      = fp.Height;
        }

        void LSPTextLines::calc_text_params(ISurface *s, LSPFont *f, ssize_t *w, ssize_t *h)
        {
            font_parameters_t fp;
            text_parameters_t tp;

            *w = 0;
            *h = 0;

            f->get_parameters(s, &fp);

            size_t count = vLines.size();
            if (count > 0)
            {
                // Estimate text size
                for (size_t i=0; i<count; ++i)
                {
                    char *l = vLines.at(i);
                    if (!f->get_text_parameters(s, &tp, l))
                        continue;

                    (*h)  += fp.Height;
                    if ((*w) < tp.Width)
                        *w = tp.Width;
                }
            }
            else
                *h      = fp.Height;
        }
    } /* namespace ctl */
} /* namespace lsp */
