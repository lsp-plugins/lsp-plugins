/*
 * IUIPort.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_IUIPORT_H_
#define _UI_IUIPORT_H_

#include <data/cvector.h>

namespace lsp
{
    class IUIPort;

    class IUIPortListener
    {
        public:
            IUIPortListener();
            virtual ~IUIPortListener();

        public:
            virtual void notify(IUIPort *port);
    };

    class IUIPort
    {
        protected:
            cvector<IUIPortListener> vListeners;

        protected:
            const port_t       *pMetadata;

        public:
            IUIPort(const port_t *meta);
            virtual ~IUIPort();

        public:
            /** Add listener to the port
             *
             * @param listener that listens port changes
             */
            void    bind(IUIPortListener *listener);

            /** Unbind listener
             *
             * @param listener listener to unbind
             */
            void    unbind(IUIPortListener *listener);

            /** Unbind all controls
             *
             */
            void    unbindAll();

            /** Write some data to port
             *
             * @param buffer data to write to port
             * @param size size of data
             */
            virtual void write(const void *buffer, size_t size);

            /** Get data from port
             *
             * @return associated buffer (may be NULL)
             */
            virtual void *getBuffer();

            /** Get single float value
             *
             * @param dfl default value if there is no data
             * @return single float value
             */
            virtual float getValue();

            /** Set single float value
             *
             * @param value value to set
             */
            virtual void setValue(float value);

            /** Notify all that port data has been changed
             *
             */
            virtual void notifyAll();

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
            template <class T>
                inline T *getBuffer()
                {
                    return reinterpret_cast<T *>(getBuffer());
                }
    };

} /* namespace lsp */

#endif /* _UI_IUIPORT_H_ */
