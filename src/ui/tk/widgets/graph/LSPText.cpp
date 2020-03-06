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

        LSPText::LSPText(LSPDisplay *dpy):
            LSPGraphItem(dpy),
            sText(this),
            sFont(dpy, this)
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
            sText.bind();

            status_t result = LSPGraphItem::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_GRAPH_TEXT, sFont.color());
            vCoords             = reinterpret_cast<coord_t *>(malloc(2 * sizeof(coord_t)));
            if (vCoords == NULL)
                return STATUS_NO_MEM;
            nCoords             = 2;
            for (size_t i=0; i<nCoords; ++i)
            {
                vCoords[i].nBasis   = i;
                vCoords[i].fCoord   = 0.0f;
            }

            sFont.init();
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

            coord_t *ptr = (vCoords != NULL) ?
                    reinterpret_cast<coord_t *>(realloc(vCoords, sizeof(coord_t) * axes)) :
                    reinterpret_cast<coord_t *>(malloc(sizeof(coord_t) * axes));
            if (ptr == NULL)
                return STATUS_NO_MEM;
            for (size_t i=nCoords; i<axes; ++i)
            {
                ptr[i].nBasis   = i;
                ptr[i].fCoord   = 0.0f;
            }

            vCoords     = ptr;
            nCoords     = axes;
            return STATUS_OK;
        }

        status_t LSPText::set_coord(size_t axis, float value)
        {
            if ((axis < 0) || (axis >= nCoords))
                return STATUS_OVERFLOW;
            if (vCoords[axis].fCoord == value)
                return STATUS_OK;
            vCoords[axis].fCoord = value;
            query_draw();

            return STATUS_OK;
        }

        status_t LSPText::set_basis(size_t axis, size_t value)
        {
            if ((axis < 0) || (axis >= nCoords))
                return STATUS_OVERFLOW;
            if (vCoords[axis].nBasis == value)
                return STATUS_OK;
            vCoords[axis].nBasis = value;
            query_draw();

            return STATUS_OK;
        }

        float LSPText::get_coord(size_t axis) const
        {
            return ((axis < 0) || (axis >= nCoords)) ? vCoords[axis].fCoord : 0.0f;
        }

        size_t LSPText::get_basis(size_t axis) const
        {
            return ((axis < 0) || (axis >= nCoords)) ? vCoords[axis].nBasis : 0;
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
            if (vCoords == NULL)
                return;

            LSPString text;
            sText.format(&text);
            if (text.is_empty())
                return;

            LSPGraph *cv = graph();
            if (cv == NULL)
                return;

            // Get palette
            Color font_color(sFont.raw_color());
            font_color.scale_lightness(brightness());

            // Get center
            float x = 0.0f, y = 0.0f;
            cv->center(nCenter, &x, &y);

            // Apply all axis
            for (size_t i=0; i<nCoords; ++i)
            {
                coord_t *coord = &vCoords[i];
                // Get axis
                LSPAxis *axis = cv->axis(coord->nBasis);
                if (axis == NULL)
                    return;
                // Apply changes
                if (!axis->apply(&x, &y, &coord->fCoord, 1))
                    return;
            }

            // Now we are ready to output text
            font_parameters_t fp;
            text_parameters_t tp;

            sFont.get_parameters(s, &fp);
            sFont.get_multiline_text_parameters(s, &tp, &text);

            // Center point
            ssize_t n_lines = 1 + text.count('\n');
            ssize_t ty      = y - fp.Height * n_lines * (fVAlign + 1.0f)*0.5f - fp.Descent;
            ssize_t tw      = tp.Width;

            // Estimate text size
            ssize_t last = 0, curr = 0, tail = 0, len = text.length();

            while (curr < len)
            {
                // Get next line indexes
                curr    = text.index_of(last, '\n');
                if (curr < 0)
                {
                    curr        = len;
                    tail        = len;
                }
                else
                {
                    tail        = curr;
                    if ((tail > last) && (text.at(tail-1) == '\r'))
                        --tail;
                }

                // Calculate text location
                sFont.get_text_parameters(s, &tp, &text, last, tail);
                ssize_t tx  = x + (tw - tp.Width*0.5f)*(fHAlign - 1.0f) + fHAlign*2.0f;
                ty         += fp.Height;
                sFont.draw(s, tx, ty, font_color, &text, last, tail);

                last    = curr + 1;
            }
        }

    } /* namespace tk */
} /* namespace lsp */
