/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 28 сент. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
