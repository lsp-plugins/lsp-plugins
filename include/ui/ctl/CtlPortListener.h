/*
 * CtlPortListener.h
 *
 *  Created on: 23 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLPORTLISTENER_H_
#define UI_CTL_CTLPORTLISTENER_H_

namespace lsp
{
    namespace ctl
    {
        class CtlPort;

        class CtlPortListener
        {
            public:
                explicit CtlPortListener();
                virtual ~CtlPortListener();

            public:
                /**
                 * Is called when the port value has been changed
                 * @param port port that caused the change
                 */
                virtual void notify(CtlPort *port);

                /**
                 * Is called when the metadata of port has been changed
                 * @param port port that caused the change
                 */
                virtual void sync_metadata(CtlPort *port);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_CTL_CTLPORTLISTENER_H_ */
