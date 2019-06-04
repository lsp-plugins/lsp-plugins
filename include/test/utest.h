/*
 * utest.h
 *
 *  Created on: 19 авг. 2018 г.
 *      Author: sadko
 */

#ifndef TEST_UTEST_H_
#define TEST_UTEST_H_

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <test/test.h>

#define UTEST_BEGIN(group, name) \
        namespace utest { \
        namespace { \
            \
            using namespace ::test; \
            \
            class utest_ ## name: public UnitTest { \
                public: \
                    typedef utest_ ## name test_type_t;\
                \
                public: \
                    \
                    _Pragma("GCC diagnostic push") \
                    _Pragma("GCC diagnostic ignored \"-Wuninitialized\"") \
                    explicit utest_ ## name() : UnitTest(group, #name) {} \
                    _Pragma("GCC diagnostic pop") \
                    \
                    virtual ~utest_ ## name() {}

#define UTEST_TIMELIMIT(seconds) \
        virtual double time_limit() const { return double(seconds); }

#define UTEST_IGNORE \
        virtual bool ignore() const { return true; }

#define UTEST_MAIN \
        virtual void execute(int argc, const char **argv)

#define UTEST_INIT \
        virtual void init()

#define UTEST_DESTROY \
        virtual void destroy()

#define UTEST_SUPPORTED(ptr)        TEST_SUPPORTED(ptr)

#define UTEST_FAIL_MSG(message, ...) {  \
            ::fprintf(stderr, "Unit test '%s.%s' has failed at file %s, line %d with message: \n  " message  "\n", \
                    __test_group, __test_name, __FILE__, __LINE__, ## __VA_ARGS__); \
            ::exit(1); \
        }

#define UTEST_FAIL() {\
            ::fprintf(stderr, "Unit test '%s.%s' has failed at file %s, line %d\n", \
                    __test_group, __test_name, __FILE__, __LINE__); \
            ::exit(1); \
        }

#define UTEST_ASSERT(code) \
        if (!(code)) { \
            ::fprintf(stderr, "Unit test '%s.%s' assertion has failed at file %s, line %d:\n  %s\n", \
                    __test_group, __test_name, __FILE__, __LINE__, # code); \
            ::exit(2); \
        }

#define UTEST_ASSERT_MSG(code, message, ...) \
        if (!(code)) { \
            ::fprintf(stderr, "Unit test '%s.%s' assertion has failed at file %s, line %d:\n  %s\n  " message "\n", \
                    __test_group, __test_name, __FILE__, __LINE__, # code, ## __VA_ARGS__); \
            ::exit(2); \
        }

#define UTEST_FOREACH(var, ...)    \
        const size_t ___sizes[] = { __VA_ARGS__, 0 }; \
        for (size_t ___i=0, var=___sizes[0]; ___i<(sizeof(___sizes)/sizeof(size_t) - 1); var=___sizes[++___i])

#define UTEST_END \
        } unit_test;  /* utest class */ \
        } /* anonymous namespace */ \
        } /* namespace utest */

namespace test
{
    class UnitTest: public Test
    {
        private:
            friend UnitTest *utest_init();

        private:
            static UnitTest        *__root;
            UnitTest               *__next;

        public:
            explicit UnitTest(const char *group, const char *name);
            virtual ~UnitTest();

        public:
            inline UnitTest *next()                 { return __next; }

            virtual Test *next_test() const         { return const_cast<UnitTest *>(__next); };

        public:
            virtual double time_limit() const;
    };


    /**
     * Initialize set of performance tests (validate duplicates, etc)
     * @return valid set of performance tests
     */
    UnitTest *utest_init();
}

#endif /* TEST_UTEST_H_ */
