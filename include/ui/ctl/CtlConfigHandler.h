/*
 * CtlConfigHandler.h
 *
 *  Created on: 26 июн. 2018 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLCONFIGHANDLER_H_
#define UI_CTL_CTLCONFIGHANDLER_H_

#include <ui/ctl/CtlConfigHandler.h>
#include <core/files/config/IConfigHandler.h>

namespace lsp
{
    namespace ctl
    {
        class CtlConfigHandler: public config::IConfigHandler, public CtlPortHandler
        {
            public:
                explicit CtlConfigHandler();
                virtual ~CtlConfigHandler();

            public:
                virtual status_t handle_parameter(const LSPString *name, const LSPString *value, size_t flags);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLCONFIGHANDLER_H_ */
