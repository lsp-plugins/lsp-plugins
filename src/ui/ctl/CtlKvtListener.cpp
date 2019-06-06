/*
 * CtlKvtListener.cpp
 *
 *  Created on: 6 июн. 2019 г.
 *      Author: sadko
 */

#include <ui/ctl/CtlKvtListener.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlKvtListener::CtlKvtListener()
        {
        }
        
        CtlKvtListener::~CtlKvtListener()
        {
        }
    
        bool CtlKvtListener::changed(KVTStorage *kvt, const char *id, const kvt_param_t *value)
        {
            return false;
        }
    } /* namespace ctl */
} /* namespace lsp */
