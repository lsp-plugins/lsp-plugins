/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 23 июн. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef UI_CTL_CTLPORT_H_
#define UI_CTL_CTLPORT_H_

#include <core/IPort.h>

namespace lsp
{
    namespace ctl
    {
        class CtlPort
        {
            protected:
                const port_t       *pMetadata;
                cvector<CtlPortListener> vListeners;

            public:
                explicit CtlPort(const port_t *meta);
                virtual ~CtlPort();

            public:
                /** Add listener to the port
                 *
                 * @param listener that listens port changes
                 */
                void    bind(CtlPortListener *listener);

                /** Unbind listener
                 *
                 * @param listener listener to unbind
                 */
                void    unbind(CtlPortListener *listener);

                /** Unbind all controls
                 *
                 */
                void    unbind_all();

                /** Write some data to port
                 *
                 * @param buffer data to write to port
                 * @param size size of data
                 */
                virtual void write(const void *buffer, size_t size);

                /** Write some data to port
                 *
                 * @param buffer data to write to port
                 * @param size size of data
                 * @param flags additional control flags
                 */
                virtual void write(const void *buffer, size_t size, size_t flags);

                /** Get data from port
                 *
                 * @return associated buffer (may be NULL)
                 */
                virtual void *get_buffer();

                /** Get single float value
                 *
                 * @return single float value
                 */
                virtual float get_value();

                /** Get single default float value
                 *
                 * @return default float value
                 */
                virtual float get_default_value();

                /** Set single float value
                 *
                 * @param value value to set
                 */
                virtual void set_value(float value);

                /** Set single float value
                 *
                 * @param flags additional control flags
                 */
                virtual void set_value(float value, size_t flags);

                /** Notify all that port data has been changed
                 *
                 */
                virtual void notify_all();

                /** Notify all that port metadata has been changed
                 *
                 */
                virtual void sync_metadata();

             public:
                /** Get port metadata
                 *
                 * @return port metadata
                 */
                inline const port_t *metadata() const { return pMetadata; };

                virtual const char *id() const;

                /** Get buffer casted to specified type
                 *
                 * @return buffer casted to specified type
                 */
                template <class T>
                    inline T *get_buffer()
                    {
                        return reinterpret_cast<T *>(get_buffer());
                    }
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLPORT_H_ */
