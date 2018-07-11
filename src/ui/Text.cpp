/*
 * Text.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <ui/graphics.h>

#include <math.h>
#include <malloc.h>


namespace lsp
{
    Text::Text(plugin_ui *ui): IGraphObject(ui, W_TEXT)
    {
        nCoords     = 2;
        vCoords     = new float[2];
        vCoords[0]  = 0.0;
        vCoords[1]  = 0.0;
        sText       = NULL;
        fHAlign     = 0.0;
        fVAlign     = 0.0;
        fSize       = 10.0;
        nCenter     = 0;

        sColor.set(ui->theme(), C_GRAPH_TEXT);
    }

    Text::~Text()
    {
        if (vCoords != NULL)
        {
            delete [] vCoords;
            vCoords     = NULL;
        }
        if (sText != NULL)
        {
            free(sText);
            sText = NULL;
        }
    }

    void Text::draw(IGraphCanvas *cv)
    {
        if ((pGraph == NULL) || (sText == NULL) || (vCoords == NULL))
            return;

        // Get center
        float x = 0.0f, y = 0.0f;
        cv->center(nCenter, &x, &y);

        // Apply all axis
        for (size_t i=0; i<nCoords; ++i)
        {
            // Get axis
            Axis *axis = pGraph->axis(i);
            if (axis == NULL)
                return;
            // Apply changes
            if (!axis->apply(cv, &x, &y, &vCoords[i], 1))
                return;
        }

        // Now we are ready to output text
        cv->set_color(sColor);
        cv->out_text(x, y, fHAlign, fVAlign, fSize, sText);
    }

    void Text::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_COORD:
                read_coordinates(value);
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_HALIGN:
                PARSE_FLOAT(value, fHAlign = __);
                break;
            case A_VALIGN:
                PARSE_FLOAT(value, fVAlign = __);
                break;
            case A_CENTER:
                PARSE_INT(value, nCenter = __);
                break;
            case A_SIZE:
                PARSE_FLOAT(value, fSize = __);
                break;
            case A_TEXT:
                if (sText != NULL)
                {
                    free(sText);
                    sText = NULL;
                }
                sText = strdup(value);
                break;

            default:
                IWidget::set(att, value);
                break;
        }
    }

    bool Text::read_coordinates(const char *value)
    {
        size_t coords = 0, capacity = 10;
        float *buf = new float[10];
        if (buf == NULL)
            return false;
        char *end = const_cast<char *>(value);

        // Read the string representing coordinates
        while (true)
        {
            errno           = 0;
            float v         = strtof(end, &end);
            if (errno != 0)
            {
                delete [] buf;
                return false;
            }

            if (coords >= capacity)
            {
                float *n_buf = new float[capacity + 10];
                if (n_buf == NULL)
                    return false;
                for(size_t i=0; i<capacity; ++i)
                    n_buf[i] = buf[i];
                delete [] buf;
                buf         = n_buf;
                capacity   += 10;
            }
            buf[coords++] = v;

            if ((*end) == '\0')
                break;
            if ((*(end++)) != ';')
            {
                delete [] buf;
                return false;
            }
        }

        // Drop previously used buffers
        if (vCoords != NULL)
            delete [] vCoords;

        // Store new buffers
        vCoords     = buf;
        nCoords     = coords;
        return true;
    }

} /* namespace lsp */
