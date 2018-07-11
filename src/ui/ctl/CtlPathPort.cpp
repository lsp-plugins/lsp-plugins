/*
 * CtlPathPort.cpp
 *
 *  Created on: 13 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <ui/ui.h>
#include <ui/plugin_ui.h>

namespace lsp
{
    namespace ctl
    {
        CtlPathPort::CtlPathPort(const port_t *meta, plugin_ui *ui): CtlPort(meta)
        {
            sPath[0]    = '\0';
            pUI         = ui;
        }

        CtlPathPort::~CtlPathPort()
        {
            sPath[0]    = '\0';
            pUI         = NULL;
        }

        void CtlPathPort::write(const void* buffer, size_t size)
        {
            // Check that attribute didn't change
            if ((size == strlen(sPath)) && (memcmp(sPath, buffer, size) == 0))
                return;

            if ((buffer != NULL) && (size > 0))
            {
                size_t copy     = (size >= PATH_MAX) ? PATH_MAX-1 : size;
                memcpy(sPath, buffer, size);
                sPath[copy]     = '\0';
            }
            else
                sPath[0]        = '\0';

            // Save config (if possible)
            if (pUI != NULL)
                pUI->save_global_config();
        }

        void *CtlPathPort::get_buffer()
        {
            return sPath;
        }
    
    } /* namespace ctl */
} /* namespace lsp */
