/*
 * CtlRegistry.h
 *
 *  Created on: 23 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLREGISTRY_H_
#define UI_CTL_CTLREGISTRY_H_

namespace lsp
{
    namespace ctl
    {
        class CtlWidget;
        class CtlPort;

        class CtlRegistry
        {
            protected:
                cvector<CtlWidget>      vControls;

            public:
                CtlRegistry();
                virtual ~CtlRegistry();

                virtual void destroy();

            public:
                /** Get port by it's name
                 *
                 */
                virtual CtlPort *port(const char *name);

                /** Add widget to main controller
                 *
                 * @param widget widget to add
                 * @return status of operation
                 */
                status_t add_widget(CtlWidget *widget);

                /** Remove widget from main controller
                 *
                 * @param widget widget to remove
                 * @return status of operation
                 */
                status_t remove_widget(CtlWidget *widget);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_CTL_CTLREGISTRY_H_ */
