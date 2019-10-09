/*
 * CtlPort.cpp
 *
 *  Created on: 23 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        CtlPort::CtlPort(const port_t *meta)
        {
            pMetadata       = meta;
        }
        
        CtlPort::~CtlPort()
        {
            vListeners.flush();
        }

        void CtlPort::bind(CtlPortListener *listener)
        {
            if (vListeners.index_of(listener) >= 0)
                return;
            vListeners.add(listener);
        }

        void CtlPort::unbind(CtlPortListener *listener)
        {
            vListeners.remove(listener, true);
        }

        void CtlPort::unbind_all()
        {
            vListeners.flush();
        }

        void CtlPort::write(const void *buffer, size_t size)
        {
        }

        void CtlPort::write(const void *buffer, size_t size, size_t flags)
        {
            write(buffer, size);
        }

        void *CtlPort::get_buffer()
        {
            return NULL;
        }

        float CtlPort::get_value()
        {
            return NAN;
        }

        float CtlPort::get_default_value()
        {
            return (pMetadata != NULL) ? pMetadata->start : NAN;
        }

        void CtlPort::set_value(float value)
        {
        }

        void CtlPort::set_value(float value, size_t flags)
        {
            set_value(value);
        }

        const char *CtlPort::id() const
        {
            return (pMetadata != NULL) ? pMetadata->id : NULL;
        }

        void CtlPort::notify_all()
        {
            // Prevent from modifying list of listeners at the sync stage
            cvector<CtlPortListener> listeners;
            if (!listeners.copy_from(&vListeners))
                return;

            // Call notify() for all listeners in the list
            size_t count = listeners.size();
            for (size_t i=0; i<count; ++i)
                listeners.at(i)->notify(this);
        }

        void CtlPort::sync_metadata()
        {
            // Prevent from modifying list of listeners at the sync stage
            cvector<CtlPortListener> listeners;
            if (!listeners.copy_from(&vListeners))
                return;

            // Call sync_metadata() for all listeners in the list
            size_t count = listeners.size();
            for (size_t i=0; i<count; ++i)
                listeners.at(i)->sync_metadata(this);
        }
    
    } /* namespace tk */
} /* namespace lsp */
