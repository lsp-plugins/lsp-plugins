/*
 * IUIPort.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_IUIPORT_H_
#define _UI_IUIPORT_H_

namespace lsp
{
    class IUIPort
    {
        private:
            IWidget           **pvWidgets;
            size_t              nWidgets;
            size_t              nCapacity;

        protected:
            const port_t       *pMetadata;

        public:
            IUIPort(const port_t *meta);
            virtual ~IUIPort();

        public:
            inline const port_t *metadata() const { return pMetadata; }

            /** Add control to the port
             *
             * @param ctl control to add
             */
            void    bind(IWidget *widget);

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
    };

} /* namespace lsp */

#endif /* _UI_IUIPORT_H_ */
