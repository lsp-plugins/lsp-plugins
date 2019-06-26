/*
 * CtlPathPort.h
 *
 *  Created on: 13 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLPATHPORT_H_
#define UI_CTL_CTLPATHPORT_H_

namespace lsp
{
    namespace ctl
    {
        class CtlPathPort: public CtlPort
        {
            protected:
                char            sPath[PATH_MAX];
                plugin_ui      *pUI;

            public:
                explicit CtlPathPort(const port_t *meta, plugin_ui *ui);
                virtual ~CtlPathPort();

            public:
                virtual void write(const void* buffer, size_t size);

                virtual void *get_buffer();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLPATHPORT_H_ */
