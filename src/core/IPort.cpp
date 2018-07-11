/*
 * IPort.cpp
 *
 *  Created on: 22 окт. 2015 г.
 *      Author: sadko
 */

#include <math.h>
#include <stddef.h>

#include <core/IPort.h>

namespace lsp
{
    
    IPort::IPort(const port_t *meta)
    {
        pMetadata       = meta;
    }
    
    IPort::~IPort()
    {
    }

    float IPort::getValue()
    {
        return (pMetadata != NULL) ? pMetadata->start : NAN;
    }

    void IPort::setValue(float value)
    {
    }

    void *IPort::getBuffer()
    {
        return NULL;
    }

    void IPort::pre_process()
    {
    }

    void IPort::post_process()
    {
    }

    bool IPort::changed()
    {
        return false;
    }

    const char *IPort::getPath()
    {
        return NULL;
    }

    void IPort::update()
    {
    }

    const port_t *IPort::metadata() const
    {
        return pMetadata;
    }

} /* namespace lsp */
