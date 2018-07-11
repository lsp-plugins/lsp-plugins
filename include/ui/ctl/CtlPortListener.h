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
                CtlPortListener();
                virtual ~CtlPortListener();

            public:
                virtual void notify(CtlPort *port);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_CTL_CTLPORTLISTENER_H_ */
