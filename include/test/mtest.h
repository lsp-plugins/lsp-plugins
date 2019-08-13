/*
 * mtest.h
 *
 *  Created on: 24 авг. 2018 г.
 *      Author: sadko
 */

#ifndef TEST_MTEST_H_
#define TEST_MTEST_H_

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <test/test.h>

#define MTEST_BEGIN(group, name) \
        namespace mtest { \
        namespace { \
            \
            using namespace ::test; \
            \
            class mtest_ ## name: public ManualTest { \
                public: \
                    typedef mtest_ ## name test_type_t; \
                \
                public: \
                    explicit mtest_ ## name() : ManualTest(group, #name) {} \
                    \
                    virtual ~mtest_ ## name() {}

#define MTEST_MAIN \
        virtual void execute(int argc, const char **argv)

#define MTEST_SUPPORTED(ptr)        TEST_SUPPORTED(ptr)

#define MTEST_FAIL_MSG(message, ...) {  \
            fprintf(stderr, "Manual test '%s.%s' has failed at file %s, line %d with message: \n  " message  "\n", \
                    __test_group, __test_name, __FILE__, __LINE__, ## __VA_ARGS__); \
            exit(1); \
        }

#define MTEST_FAIL() {\
            fprintf(stderr, "Manual test '%s.%s' has failed at file %s, line %d\n", \
                    __test_group, __test_name, __FILE__, __LINE__); \
            exit(1); \
        }

#define MTEST_FAIL_SILENT()     exit(5);

#define MTEST_ASSERT(code) \
        if (!(code)) { \
            fprintf(stderr, "Manual test '%s.%s' assertion has failed at file %s, line %d:\n  %s\n", \
                    __test_group, __test_name, __FILE__, __LINE__, # code); \
            exit(2); \
        }

#define MTEST_ASSERT_MSG(code, message, ...) \
        if (!(code)) { \
            fprintf(stderr, "Manual test '%s.%s' assertion has failed at file %s, line %d:\n  %s\n  " message "\n", \
                    __test_group, __test_name, __FILE__, __LINE__, # code, ## __VA_ARGS__); \
            exit(2); \
        }

#define MTEST_END \
        } manual_test;  /* mtest class */ \
        } /* anonymous namespace */ \
        } /* namespace mtest */

namespace test
{
    class ManualTest: public Test
    {
        private:
            friend ManualTest *mtest_init();

        private:
            static ManualTest        *__root;
            ManualTest               *__next;

        public:
            explicit ManualTest(const char *group, const char *name);
            virtual ~ManualTest();

        public:
            inline ManualTest *next()               { return __next; }

            virtual Test *next_test() const         { return const_cast<ManualTest *>(__next); };
    };


    /**
     * Initialize set of performance tests (validate duplicates, etc)
     * @return valid set of performance tests
     */
    ManualTest *mtest_init();
}

#endif /* TEST_MTEST_H_ */
