/*
 * XMLParser.cpp
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <string.h>

namespace lsp
{
    XMLHandler::~XMLHandler()
    {
    }

    void XMLHandler::enter()
    {
    }

    const char *XMLHandler::findAttribute(const char **atts, const char *name)
    {
        while (*atts != NULL)
        {
            const char *a_name      = *atts++;
            const char *a_value     = *atts++;
            if ((a_name != NULL) && (a_value != NULL))
            {
                if (!strcmp(a_name, name))
                    return a_value;
            }
        }

        return NULL;
    }

    XMLHandler *XMLHandler::startElement(const char *name, const char **atts)
    {
        return NULL;
    }

    void XMLHandler::endElement(const char *name)
    {
    }

    void XMLHandler::quit()
    {
    }

    void XMLHandler::completed(XMLHandler *child)
    {
    }

} /* namespace lsp */
