/*
 * CtlConfigSource.h
 *
 *  Created on: 22 июн. 2018 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLCONFIGSOURCE_H_
#define UI_CTL_CTLCONFIGSOURCE_H_

#include <ui/ctl/CtlConfigHandler.h>
#include <core/files/config/IConfigSource.h>

namespace lsp
{
    namespace ctl
    {
        class CtlConfigSource: public config::IConfigSource, public CtlPortHandler
        {
            protected:
                LSPString           sComment;

            public:
                explicit CtlConfigSource();
                virtual ~CtlConfigSource();

            public:
                status_t set_comment(const char *comment);
                status_t set_comment(const LSPString *comment);

            public:
                virtual status_t get_head_comment(LSPString *comment);

                virtual status_t get_parameter(LSPString *name, LSPString *value, LSPString *comment, int *flags);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLCONFIGSOURCE_H_ */
