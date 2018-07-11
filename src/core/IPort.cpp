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

    bool IPort::pre_process(size_t samples)
    {
        return false;
    }

    void IPort::post_process(size_t samples)
    {
    }

    const char *IPort::getPath()
    {
        return NULL;
    }

    const port_t *IPort::metadata() const
    {
        return pMetadata;
    }

} /* namespace lsp */
