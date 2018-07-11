/*
 * CtlValuePort.cpp
 *
 *  Created on: 12 мая 2018 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlValuePort::CtlValuePort(const port_t *meta): CtlPort(meta)
        {
            fValue      = meta->start;
            fPending    = meta->start;
        }
        
        CtlValuePort::~CtlValuePort()
        {
        }

        void CtlValuePort::commitValue(float value)
        {
            fPending = value;
        }

        void CtlValuePort::sync()
        {
            if (fValue == fPending)
                return;
            fValue = fPending;
            notify_all();
        }

        float CtlValuePort::get_value()
        {
            return fValue;
        }
    
    } /* namespace ctl */
} /* namespace lsp */
