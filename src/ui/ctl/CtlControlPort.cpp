/*
 * CtlControlPort.cpp
 *
 *  Created on: 11 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <ui/ui.h>
#include <ui/plugin_ui.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlControlPort::CtlControlPort(const port_t *meta, plugin_ui *ui): CtlPort(meta)
        {
            fValue  = meta->start;
            pUI     = ui;
        }

        CtlControlPort::~CtlControlPort()
        {
            pUI     = NULL;
        }

        float CtlControlPort::get_value()
        {
            return fValue;
        }

        void CtlControlPort::set_value(float value)
        {
            float v  = limit_value(pMetadata, value);
            if (fValue != v)
            {
                fValue  = v;
                if (pUI != NULL)
                    pUI->save_global_config();
            }
        }
    } /* namespace ctl */
} /* namespace lsp */
