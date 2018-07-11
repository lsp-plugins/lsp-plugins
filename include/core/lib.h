/*
 * lib.h
 *
 *  Created on: 07 окт. 2015 г.
 *      Author: sadko
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
