/*
 * CtlPortAlias.cpp
 *
 *  Created on: 23 окт. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        CtlPortAlias::CtlPortAlias(CtlRegistry *src): CtlWidget(src, NULL)
        {
            sID     = NULL;
            sAlias  = NULL;
        }

        CtlPortAlias::~CtlPortAlias()
        {
            if (sID != NULL)
            {
                lsp_free(sID);
                sID     = NULL;
            }
            if (sAlias != NULL)
            {
                lsp_free(sAlias);
                sAlias  = NULL;
            }
        }
    
        void CtlPortAlias::set(widget_attribute_t att, const char *value)
        {
            switch (att)
            {
                case A_ID:
                    if (sID != NULL)
                        lsp_free(sID);
                    sID     = lsp_strdup(value);
                    break;
                case A_VALUE:
                    if (sAlias != NULL)
                        lsp_free(sAlias);
                    sAlias  = lsp_strdup(value);
                    break;
                default:
                    CtlWidget::set(att, value);
                    break;
            }
        }
    } /* namespace ctl */
} /* namespace lsp */
