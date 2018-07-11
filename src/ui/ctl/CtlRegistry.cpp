/*
 * CtlPortSource.cpp
 *
 *  Created on: 23 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlRegistry::CtlRegistry()
        {
        }
        
        CtlRegistry::~CtlRegistry()
        {
            destroy();
        }

        void CtlRegistry::destroy()
        {
            size_t n = vControls.size();
            for (size_t i=0; i<n; ++i)
            {
                CtlWidget *w = vControls.at(i);
                if (w != NULL)
                {
                    w->destroy();
                    delete w;
                }
            }
            vControls.flush();
        }

        CtlPort *CtlRegistry::port(const char *name)
        {
            return NULL;
        }

        status_t CtlRegistry::add_widget(CtlWidget *widget)
        {
            return (vControls.add(widget)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t CtlRegistry::remove_widget(CtlWidget *widget)
        {
            return (vControls.remove(widget, true)) ? STATUS_OK : STATUS_NOT_FOUND;
        }
    
    } /* namespace tk */
} /* namespace lsp */
