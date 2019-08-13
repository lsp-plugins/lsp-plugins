/*
 * CtlPortHandler.h
 *
 *  Created on: 27 июн. 2018 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLPORTHANDLER_H_
#define UI_CTL_CTLPORTHANDLER_H_

namespace lsp
{
    namespace ctl
    {
        class CtlPortHandler
        {
            protected:
                typedef struct port_ref_t {
                    LSPString       sName;
                    CtlPort        *pPort;
                } port_ref_t;

            protected:
                cvector<port_ref_t> vPorts;
                size_t              nPortID;

            public:
                explicit CtlPortHandler();
                virtual ~CtlPortHandler();

            public:
                status_t add_port(const char *name, CtlPort *port);
                status_t add_port(const LSPString *name, CtlPort *port);
                status_t add_port(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLPORTHANDLER_H_ */
