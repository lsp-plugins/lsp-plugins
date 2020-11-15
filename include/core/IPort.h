/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 22 окт. 2015 г.
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

#ifndef _CORE_IPORT_H_
#define _CORE_IPORT_H_

#include <metadata/metadata.h>

namespace lsp
{
    enum port_flags_t
    {
        PF_NONE             = 0,            // No flags
        PF_STATE_RESTORE    = 1 << 1,       // Port's state restore
        PF_STATE_IMPORT     = 1 << 2,       // Port's state import
        PF_PRESET_IMPORT    = 1 << 3        // Port's preset import
    };

    class IPort
    {
        protected:
            const port_t       *pMetadata;

        public:
            explicit IPort(const port_t *meta);
            virtual ~IPort();

        public:
            /** Get port value
             *
             * @return port value or default metadata value if not connected/initialized
             */
            virtual float getValue();

            /** Set port value
             *
             * @param value value to set
             */
            virtual void setValue(float value);

            /** Get port buffer, may be NULL if buffer write is not required
             *
             */
            virtual void *getBuffer();

            /** Pre-process port state before processor execution
             * @param samples number of estimated samples to process
             * @return true if port value has been externally modified
             */
            virtual bool pre_process(size_t samples);

            /** Post-process port state after processor execution
             * @param samples number of samples processed by plugin
             */
            virtual void post_process(size_t samples);

        public:
            /** Get port metadata
             *
             * @return port metadata
             */
            inline const port_t *metadata() const { return pMetadata; };

            /** Get buffer casted to specified type
             *
             * @return buffer casted to specified type
             */
            template <class T> inline T *getBuffer()
            {
                return reinterpret_cast<T *>(getBuffer());
            }
    };

} /* namespace lsp */

#endif /* _CORE_IPORT_H_ */
