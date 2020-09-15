/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 07 окт. 2015 г.
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

#ifndef CORE_LIB_H_
#define CORE_LIB_H_

class StaticInitializer
{
    public:
        typedef void (*func_t)();

    private:
        func_t init;
        func_t fini;

    public:
        StaticInitializer(func_t f_init, func_t f_fini)
        {
            init = f_init;
            fini = f_fini;
            if (init != NULL)
                init();
        }

        ~StaticInitializer()
        {
            if (fini != NULL)
                fini();
        }
};

class StaticFinalizer
{
    public:
        typedef void (*func_t)();

    private:
        func_t fini;

    public:
        StaticFinalizer(func_t f_fini)
        {
            fini = f_fini;
        }

        ~StaticFinalizer()
        {
            if (fini != NULL)
                fini();
        }
};

#endif /* CORE_LIB_H_ */
