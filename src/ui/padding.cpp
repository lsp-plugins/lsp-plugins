/*
 * Padding.cpp
 *
 *  Created on: 30 мая 2016 г.
 *      Author: sadko
 */

#include <ui/ui.h>

namespace lsp
{
    bool Padding::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_PAD_LEFT:
                PARSE_INT(value, nLeft = __);
                return true;

            case A_PAD_RIGHT:
                PARSE_INT(value, nRight = __);
                return true;

            case A_PAD_TOP:
                PARSE_INT(value, nTop = __);
                return true;

            case A_PAD_BOTTOM:
                PARSE_INT(value, nBottom = __);
                return true;

            default:
                return false;
        }

        return false;
    }
}
