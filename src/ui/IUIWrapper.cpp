/*
 * IUIWrapper.cpp
 *
 *  Created on: 25 апр. 2016 г.
 *      Author: sadko
 */

#include <ui/IUIWrapper.h>

namespace lsp
{
    
    IUIWrapper::IUIWrapper()
    {
    }
    
    IUIWrapper::~IUIWrapper()
    {
    }

    void IUIWrapper::ui_activated()
    {
    }

    void IUIWrapper::ui_deactivated()
    {
    }

    KVTStorage *IUIWrapper::kvt_lock()
    {
        return NULL;
    }

    KVTStorage *IUIWrapper::kvt_trylock()
    {
        return NULL;
    }

    bool IUIWrapper::kvt_release()
    {
        return false;
    }

} /* namespace lsp */
