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
    IUIPort::IUIPort(const port_t *meta)
    {
        pvWidgets       = NULL;
        nWidgets        = 0;
        nCapacity       = 0;
        pMetadata       = meta;
    }

    IUIPort::~IUIPort()
    {
        unbindAll();
        if (pvWidgets != NULL)
            delete [] pvWidgets;
    }

    void IUIPort::bind(IWidget *widget)
    {
        if (nWidgets >= nCapacity)
        {
            IWidget **new_list = new IWidget *[nCapacity + 16];
            if (new_list == NULL)
                return;

            nCapacity      += 16;
            if (pvWidgets != NULL)
            {
                for (size_t i=0; i<nWidgets; ++i)
                    new_list[i]     = pvWidgets[i];
                delete  [] pvWidgets;
            }
            pvWidgets      = new_list;
        }

        pvWidgets[nWidgets++] = widget;
    }

    void IUIPort::unbindAll()
    {
        nWidgets = 0;
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
        for (size_t i=0; i<nWidgets; ++i)
            pvWidgets[i]->notify(this);
    }

} /* namespace lsp */

