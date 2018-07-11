/*
 * IPort.h
 *
 *  Created on: 22 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _CORE_IPORT_H_
#define _CORE_IPORT_H_

#include <core/metadata.h>

namespace lsp
{

    class IPort
    {
        protected:
            const port_t       *pMetadata;

        public:
            IPort(const port_t *meta);
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

            virtual const char *getPath();

            const port_t *metadata() const;
    };

} /* namespace lsp */

#endif /* _CORE_IPORT_H_ */
