/*
 * CtlRegistry.h
 *
 *  Created on: 23 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLREGISTRY_H_
#define UI_CTL_CTLREGISTRY_H_

#include <core/KVTStorage.h>

namespace lsp
{
    namespace ctl
    {
        class CtlWidget;
        class CtlPort;
        class CtlKvtListener;

        class CtlRegistry
        {
            protected:
                cvector<CtlWidget>      vControls;

            private:
                CtlRegistry & operator = (const CtlRegistry &);

            public:
                explicit CtlRegistry();
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

                /**
                 * Resolve widget by it's unique identifier
                 * @param uid unique widget identifier
                 * @return pointer to widget or NULL
                 */
                virtual LSPWidget *resolve(const char *uid);

                /**
                 * Lock the KVT storage
                 * @return pointer to KVT storage or NULL
                 */
                virtual KVTStorage *kvt_lock();

                /**
                 * Try to lock the KVT storage
                 * @return pointer to KVT storage or NULL if not locked/not supported
                 */
                virtual KVTStorage *kvt_trylock();

                /**
                 * Release the KVT storage
                 */
                virtual void kvt_release();

                /**
                 * Signal write to the KVT storage
                 * @param storage KVT storage
                 * @param id parameter identifier
                 * @param value parameter value
                 */
                virtual void kvt_write(KVTStorage *storage, const char *id, const kvt_param_t *value);

                /**
                 * Add KVT listener
                 * @param listener listener to add
                 * @return status of operation
                 */
                virtual status_t add_kvt_listener(CtlKvtListener *listener);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_CTL_CTLREGISTRY_H_ */
