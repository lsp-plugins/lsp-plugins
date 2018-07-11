/*
 * Text.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <ui/graphics.h>
#include <core/alloc.h>

#include <math.h>
#include <malloc.h>


namespace lsp
{
    Text::Text(plugin_ui *ui): IGraphObject(ui, W_TEXT)
    {
        nCoords             = 2;
        vCoords             = new coord_t[2];
        vCoords[0].pPort    = NULL;
        vCoords[0].fValue   = 0.0;
        vCoords[1].pPort    = NULL;
        vCoords[1].fValue   = 0.0;
        sText               = NULL;
        fHAlign             = 0.0;
        fVAlign             = 0.0;
        fSize               = 10.0;
        nCenter             = 0;

        sColor.set(ui->theme(), C_GRAPH_TEXT);
    }

    Text::~Text()
    {
        drop_coordinates();

        if (sText != NULL)
        {
            lsp_free(sText);
            sText = NULL;
        }
    }

    void Text::draw(IGraphCanvas *cv)
    {
        if ((pGraph == NULL) || (sText == NULL) || (vCoords == NULL) || (!bVisible))
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
            if (!axis->apply(cv, &x, &y, &vCoords[i].fValue, 1))
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
                    lsp_free(sText);
                    sText = NULL;
                }
                sText = lsp_strdup(value);
                break;

            default:
                IWidget::set(att, value);
                break;
        }
    }

    void Text::drop_coordinates()
    {
        if (vCoords != NULL)
        {
            for (size_t i=0; i<nCoords; ++i)
            {
                if (vCoords[i].pPort != NULL)
                    vCoords[i].pPort->unbind(this);
            }

            delete [] vCoords;
            vCoords = NULL;
            nCoords = 0;
        }
    }

    bool Text::read_coordinates(const char *value)
    {
        drop_coordinates();

        // Calculate number of items
        size_t items = 1;
        char port_name[16];
        for (const char *p = value; *p != '\0'; ++p)
        {
            if (*p == ';')
                items++;
        }

        vCoords = new coord_t[items];
        if (vCoords == NULL)
            return false;
        nCoords = 0;

        char *end = const_cast<char *>(value);

        // Read the string representing coordinates
        while (true)
        {
            if (*end == ':')
            {
                char *next      = strchr(++end, ';');
                if (next == NULL)
                    for (next = end; *next != '\0'; ++next) /* nothing */ ;

                // Read port name
                size_t nchars   = next - end;
                if (nchars >= (sizeof(port_name)/sizeof(char)))
                {
                    drop_coordinates();
                    return false;
                }
                memcpy(port_name, end, nchars*sizeof(char));
                port_name[nchars] = '\0';

                // Bind port
                IUIPort *p      = pUI->port(port_name);
                if (p != NULL)
                    p->bind(this);

                vCoords[nCoords].fValue = 0;
                vCoords[nCoords].pPort  = p;
                nCoords ++;

                // Update pointer
                end             = next;
            }
            else
            {
                errno           = 0;
                float v         = strtof(end, &end);
                if (errno != 0)
                {
                    drop_coordinates();
                    return false;
                }

                vCoords[nCoords].fValue = v;
                vCoords[nCoords].pPort  = NULL;
                nCoords ++;
            }

            if ((*end) == '\0')
                break;
            if ((*(end++)) != ';')
            {
                drop_coordinates();
                return false;
            }
        }

        return true;
    }

    void Text::notify(IUIPort *port)
    {
        IGraphObject::notify(port);

        for (size_t i=0; i<nCoords; ++i)
        {
            if (vCoords[i].pPort == port)
                vCoords[i].fValue   = port->getValue();
        }
    }

} /* namespace lsp */
