/*
 * IUIPort.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <stddef.h>
#include <ui/ui.h>
#include <math.h>

namespace lsp
{
    IUIPortListener::IUIPortListener()
    {
    }

    IUIPortListener::~IUIPortListener()
    {
    }

    void IUIPortListener::notify(IUIPort *port)
    {
    }

    IUIPort::IUIPort(const port_t *meta)
    {
        pMetadata       = meta;
    }

    IUIPort::~IUIPort()
    {
        unbindAll();
    }

    void IUIPort::bind(IUIPortListener *listener)
    {
        vListeners.add(listener);
    }

    void IUIPort::unbind(IUIPortListener *listener)
    {
        vListeners.remove(listener, true);
    }

    void IUIPort::unbindAll()
    {
        vListeners.clear();
    }

    void IUIPort::write(const void* buffer, size_t size)
    {
        // This is a stub interface function
    }

    void *IUIPort::getBuffer()
    {
        return NULL;
    }

    float IUIPort::getValue()
    {
        return NAN;
    }

    void IUIPort::setValue(float value)
    {
    }

    void IUIPort::notifyAll()
    {
//        lsp_trace("id=%s", (pMetadata != NULL) ? pMetadata->id : NULL);
        size_t count = vListeners.size();
        for (size_t i=0; i<count; ++i)
            vListeners[i]->notify(this);
    }

} /* namespace lsp */

