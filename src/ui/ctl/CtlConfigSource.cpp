/*
 * CtlConfigSource.cpp
 *
 *  Created on: 22 июн. 2018 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        CtlConfigSource::CtlConfigSource()
        {
            nPortID = 0;
        }
        
        CtlConfigSource::~CtlConfigSource()
        {
        }

        status_t CtlConfigSource::set_comment(const char *comment)
        {
            if (!sComment.set_native(comment))
                return STATUS_NO_MEM;
            return STATUS_OK;
        }

        status_t CtlConfigSource::set_comment(const LSPString *comment)
        {
            if (!sComment.set(comment))
                return STATUS_NO_MEM;
            return STATUS_OK;
        }

        status_t CtlConfigSource::get_head_comment(LSPString *comment)
        {
            return (comment->set(&sComment)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t CtlConfigSource::get_parameter(LSPString *name, LSPString *value, LSPString *comment, int *flags)
        {
            port_ref_t *ref = vPorts.get(nPortID++);
            if (ref == NULL)
                return STATUS_EOF;

            LSP_STATUS_ASSERT(format_port_value(ref->pPort, name, value, comment, flags));
            if (!name->set(&ref->sName))
                return STATUS_NO_MEM;

            return STATUS_OK;
        }
    } /* namespace ctl */
} /* namespace lsp */
