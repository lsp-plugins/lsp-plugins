/*
 * test.h
 *
 *  Created on: 22 авг. 2018 г.
 *      Author: vsadovnikov
 */

#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <data/cvector.h>

#ifdef LSP_TESTING
    #define TEST_EXPORT(ptr)        ::test::Test::mark_supported(ptr)
    #define TEST_SUPPORTED(ptr)     ::test::Test::check_supported(ptr)
#else
    #define TEST_EXPORT(ptr)
    #define TEST_SUPPORTED(ptr)     false
#endif /* LSP_TESTING */

namespace test
{
    class Test
    {
        private:
            static lsp::cvector<void> support;
            static void     __mark_supported(const void *x);
            static bool     __check_supported(const void *ptr);

        protected:
            const char     *__test_group;
            const char     *__test_name;
            mutable char   *__full_name;
            bool            __verbose;

        public:
            inline const char *name() const     { return __test_name; }
            inline const char *group() const    { return __test_group; }
            const char *full_name() const;

        public:
            explicit Test(const char *group, const char *name);
            virtual ~Test();

        public:
            inline void set_verbose(bool verbose)      { __verbose = verbose; }

            virtual void execute(int argc, const char **argv) = 0;

            virtual bool ignore() const;

            virtual Test *next_test() const = 0;

        public:
            template <typename T>
                static inline void     mark_supported(T x)
                {
                    __mark_supported(reinterpret_cast<const void *>(x));
                }

            template <typename T>
                static bool     check_supported(T x)
                {
                    return __check_supported(reinterpret_cast<const void *>(x));
                }

    };
}

#endif /* TEST_TEST_H_ */
