/*
 * CtlControlPort.h
 *
 *  Created on: 11 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLCONTROLPORT_H_
#define UI_CTL_CTLCONTROLPORT_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlControlPort: public CtlPort
        {
            private:
                float       fValue;
                plugin_ui  *pUI;

            public:
                explicit CtlControlPort(const port_t *meta, plugin_ui *ui);
                virtual ~CtlControlPort();

            public:
                virtual float get_value();

                virtual void set_value(float value);

        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLCONTROLPORT_H_ */
