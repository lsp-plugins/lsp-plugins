/*
 * LSPText.cpp
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPText::metadata = { "LSPText", &LSPGraphItem::metadata };

        LSPText::LSPText(LSPDisplay *dpy): LSPGraphItem(dpy), sFont(dpy, this)
        {
            nCoords             = 0;
            vCoords             = NULL;
            fHAlign             = 0.0;
            fVAlign             = 0.0;
            nCenter             = 0;

            pClass          = &metadata;
        }

        LSPText::~LSPText()
        {
            do_destroy();
        }

        void LSPText::destroy()
        {
            LSPGraphItem::destroy();
            do_destroy();
        }

        void LSPText::do_destroy()
        {
            if (vCoords != NULL)
            {
                free(vCoords);
                vCoords = NULL;
            }
            nCoords = 0;
        }

        status_t LSPText::init()
        {
            status_t result = LSPGraphItem::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_GRAPH_TEXT, sFont.color());
            vCoords             = reinterpret_cast<float *>(malloc(2 * sizeof(float)));
            if (vCoords == NULL)
                return STATUS_NO_MEM;
            nCoords             = 2;
            for (size_t i=0; i<nCoords; ++i)
                vCoords[i]          = 0.0f;

            sFont.set_size(10.0f);

            return STATUS_OK;
        }

        status_t LSPText::set_axes(size_t axes)
        {
            if (nCoords == axes)
                return STATUS_OK;
            if (axes == 0)
            {
                if (vCoords != NULL)
                {
                    free(vCoords);
                    vCoords = NULL;
                }
                nCoords = 0;
                query_draw();
                return STATUS_OK;
            }

            float *ptr = (vCoords != NULL) ?
                    reinterpret_cast<float *>(realloc(vCoords, sizeof(float) * axes)) :
                    reinterpret_cast<float *>(malloc(sizeof(float) * axes));
            if (ptr == NULL)
                return STATUS_NO_MEM;
            for (size_t i=nCoords; i<axes; ++i)
                ptr[i]      = 0.0f;

            vCoords     = ptr;
            nCoords     = axes;
            return STATUS_OK;
        }

        status_t LSPText::set_coord(size_t axis, float value)
        {
            if ((axis < 0) || (axis >= nCoords))
                return STATUS_OVERFLOW;
            if (vCoords[axis] == value)
                return STATUS_OK;
            vCoords[axis] = value;
            query_draw();

            return STATUS_OK;
        }

        void LSPText::set_halign(float value)
        {
            if (fHAlign == value)
                return;
            fHAlign = value;
            query_draw();
        }

        void LSPText::set_valign(float value)
        {
            if (fVAlign == value)
                return;
            fVAlign = value;
            query_draw();
        }

        void LSPText::set_center(size_t value)
        {
            if (nCenter == value)
                return;
            nCenter = value;
            query_draw();
        }

        void LSPText::render(ISurface *s, bool force)
        {
            if ((sText.is_empty()) || (vCoords == NULL))
                return;

            LSPGraph *cv = graph();
            if (cv == NULL)
                return;

            // Get center
            float x = 0.0f, y = 0.0f;
            cv->center(nCenter, &x, &y);

            // Apply all axis
            for (size_t i=0; i<nCoords; ++i)
            {
                // Get axis
                LSPAxis *axis = cv->axis(i);
                if (axis == NULL)
                    return;
                // Apply changes
                if (!axis->apply(&x, &y, &vCoords[i], 1))
                    return;
            }

            // Now we are ready to output text
            font_parameters_t fp;
            text_parameters_t tp;

            sFont.get_parameters(s, &fp);
            sFont.get_multiline_text_parameters(s, &tp, &sText);

            // Center point
            ssize_t n_lines = 1 + sText.count('\n');
            ssize_t ty      = y - fp.Height * n_lines * (fVAlign + 1.0f)*0.5f - fp.Descent;
            ssize_t tw      = tp.Width;

            // Estimate text size
            ssize_t last = 0, curr = 0, tail = 0, len = sText.length();

            while (curr < len)
            {
                // Get next line indexes
                curr    = sText.index_of(last, '\n');
                if (curr < 0)
                {
                    curr        = len;
                    tail        = len;
                }
                else
                {
                    tail        = curr;
                    if ((tail > last) && (sText.at(tail-1) == '\r'))
                        --tail;
                }

                // Calculate text location
                sFont.get_text_parameters(s, &tp, &sText, last, tail);
                ssize_t tx  = x + (tw - tp.Width*0.5f)*(fHAlign - 1.0f) + fHAlign*2.0f;
                ty         += fp.Height;
                sFont.draw(s, tx, ty, &sText, last, tail);

                last    = curr + 1;
            }
        }

        status_t LSPText::set_text(const char *text)
        {
            if (!sText.set_native(text))
                return STATUS_NO_MEM;
            query_resize();
            return STATUS_OK;
        };

        status_t LSPText::set_text(const LSPString *text)
        {
            if (!sText.set(text))
                return STATUS_NO_MEM;
            query_resize();
            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */
