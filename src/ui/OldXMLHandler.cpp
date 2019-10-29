/*
 * OldXMLHandler.cpp
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <string.h>

namespace lsp
{
    OldXMLHandler::~OldXMLHandler()
    {
    }

    void OldXMLHandler::enter()
    {
    }

    const char *OldXMLHandler::findAttribute(const char **atts, const char *name)
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

    OldXMLHandler *OldXMLHandler::startElement(const char *name, const char **atts)
    {
        return NULL;
    }

    void OldXMLHandler::endElement(const char *name)
    {
    }

    void OldXMLHandler::quit()
    {
    }

    void OldXMLHandler::completed(OldXMLHandler *child)
    {
    }

} /* namespace lsp */
