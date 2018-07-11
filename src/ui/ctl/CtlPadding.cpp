/*
 * CtlPadding.cpp
 *
 *  Created on: 12 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        CtlPadding::CtlPadding()
        {
            pPadding        = NULL;
            vAttributes[0]  = -1;
            vAttributes[1]  = -1;
            vAttributes[2]  = -1;
            vAttributes[3]  = -1;
            vAttributes[4]  = -1;
        }

        CtlPadding::~CtlPadding()
        {
            pPadding        = NULL;
        }

        void CtlPadding::init(LSPPadding *padding, ssize_t l, ssize_t r, ssize_t t, ssize_t b, ssize_t c)
        {
            pPadding        = padding;
            vAttributes[0]  = l;
            vAttributes[1]  = r;
            vAttributes[2]  = t;
            vAttributes[3]  = b;
            vAttributes[4]  = c;
        }

        bool CtlPadding::set(widget_attribute_t att, const char *value)
        {
            bool set = false;
            if (att == vAttributes[0])
                PARSE_INT(value, pPadding->set_left(__); set = true );
            if (att == vAttributes[1])
                PARSE_INT(value, pPadding->set_right(__); set = true );
            if (att == vAttributes[2])
                PARSE_INT(value, pPadding->set_top(__); set = true );
            if (att == vAttributes[3])
                PARSE_INT(value, pPadding->set_bottom(__); set = true );
            if (att == vAttributes[4])
                PARSE_INT(value, pPadding->set_all(__); set = true );

            return set;
        }
    } /* namespace ctl */
} /* namespace lsp */
