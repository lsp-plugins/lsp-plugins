/*
 * CtlValuePort.h
 *
 *  Created on: 12 мая 2018 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLVALUEPORT_H_
#define UI_CTL_CTLVALUEPORT_H_

namespace lsp
{
    namespace ctl
    {
        class CtlValuePort: public CtlPort
        {
            private:
                float   fValue;
                float   fPending;

            public:
                explicit CtlValuePort(const port_t *meta);
                virtual ~CtlValuePort();

            public:
                void commitValue(float value);

                void sync();

            public:
                virtual float get_value();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* INCLUDE_UI_CTL_CTLVALUEPORT_H_ */
