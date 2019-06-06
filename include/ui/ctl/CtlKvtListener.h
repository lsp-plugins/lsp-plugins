/*
 * CtlKvtListener.h
 *
 *  Created on: 6 июн. 2019 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLKVTLISTENER_H_
#define UI_CTL_CTLKVTLISTENER_H_

#include <core/KVTStorage.h>

namespace lsp
{
    namespace ctl
    {
        class CtlKvtListener
        {
            public:
                explicit CtlKvtListener();
                virtual ~CtlKvtListener();

            public:
                /**
                 * Get listener name
                 * @return listener name or NULL
                 */
                virtual const char *name();

                /**
                 * Handle change of the KVT paramter
                 * @param kvt KVT storage
                 * @param id KVT parameter full path identifier
                 * @param value actual KVT parameter value
                 * @return true if listener processed the message, false if ignored
                 */
                virtual bool changed(KVTStorage *kvt, const char *id, const kvt_param_t *value);

                /**
                 * Check that parameter name matches
                 * @param id parameter identifier
                 * @return true if parameter name matches
                 */
                virtual bool match(const char *id);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLKVTLISTENER_H_ */
