/*
 * PortAlias.cpp
 *
 *  Created on: 20 апр. 2016 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <core/alloc.h>

namespace lsp
{
    
    PortAlias::PortAlias(plugin_ui *ui): IWidget(ui, W_PORT)
    {
        sID     = NULL;
        sAlias  = NULL;
    }
    
    PortAlias::~PortAlias()
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

    void PortAlias::set(widget_attribute_t att, const char *value)
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
                IWidget::set(att, value);
                break;
        }
    }

} /* namespace lsp */
