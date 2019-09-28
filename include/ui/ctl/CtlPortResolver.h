/*
 * CtlPortResolver.h
 *
 *  Created on: 28 сент. 2019 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLPORTRESOLVER_H_
#define UI_CTL_CTLPORTRESOLVER_H_

#include <core/calc/Resolver.h>

namespace lsp
{
    namespace ctl
    {
        
        class CtlPortResolver: public calc::Resolver
        {
            private:
                CtlPortResolver & operator = (const CtlPortResolver &);

            protected:
                CtlRegistry *pRegistry;

            public:
                explicit CtlPortResolver();
                virtual ~CtlPortResolver();

            public:
                void init(CtlRegistry *registry);

                virtual status_t on_resolved(const LSPString *name, CtlPort *p);

                virtual status_t on_resolved(const char *name, CtlPort *p);

                virtual status_t resolve(calc::value_t *value, const char *name, size_t num_indexes = 0, const ssize_t *indexes = NULL);

                virtual status_t resolve(calc::value_t *value, const LSPString *name, size_t num_indexes = 0, const ssize_t *indexes = NULL);
        };
    
    } /* namespace java */
} /* namespace lsp */

#endif /* UI_CTL_CTLPORTRESOLVER_H_ */
