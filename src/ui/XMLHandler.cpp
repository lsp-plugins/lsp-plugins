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

    const XML_Char *XMLHandler::findAttribute(const XML_Char **atts, const XML_Char *name)
    {
        while (*atts != NULL)
        {
            const XML_Char *a_name      = *atts++;
            const XML_Char *a_value     = *atts++;
            if ((a_name != NULL) && (a_value != NULL))
            {
                if (!strcmp(a_name, name))
                    return a_value;
            }
        }

        return NULL;
    }

    XMLHandler *XMLHandler::startElement(const XML_Char *name, const XML_Char **atts)
    {
        return NULL;
    }

    void XMLHandler::endElement(const XML_Char *name)
    {
    }

    void XMLHandler::quit()
    {
    }

    void XMLHandler::completed(XMLHandler *child)
    {
    }

} /* namespace lsp */
