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

#define UTEST_BEGIN(group, name, time, iterations) \
        namespace test { \
        namespace ptest { \
        namespace name { \
            \
            using namespace ::test; \
            \
            class utest_ ## name: public UnitTest { \
                public: \
                    explicit ptest_ ## name() : UnitTest(group, #name) {} \
                    \
                    virtual ~ptest_ ## name() {}

#define UTEST_TIMELIMIT(value) \
        virtual double time_limit() const { return double(value); }

#define UTEST_IGNORE \
        virtual bool ignore() const { return true; }

#define UTEST_FAIL(code)    \
        fprintf(stderr, "Unit test '%s' group '%s' has failed at file %s, line %d", \
                __test_name, __test_group, __FILE__, __LINE__); \
        exit(1));

#define UTEST_ASSERT(code) \
        fprintf(stderr, "Unit test '%s' group '%s' assertion has failed at file %s, line %d:\n  %s", \
                __test_name, __test_group, __FILE__, __LINE__, # code); \
        exit(2));

#define UTEST_END \
        } unit_test;  /* utest class */ \
        } /* namespace name */ \
        } /* namespace utest */ \
        } /* namespace test */

namespace test
{
    class UnitTest
    {
        private:
            friend UnitTest *utest_init();

        private:
            static UnitTest        *__root;
            UnitTest               *__next;

        protected:
            const char *__test_group;
            const char *__test_name;

        public:
            explicit UnitTest(const char *group, const char *name);
            virtual ~UnitTest();

        public:
            inline const char *name() const     { return __test_name; }
            inline const char *group() const    { return __test_group; }
            inline UnitTest *next()             { return __next; }

        public:
            virtual void execute() = 0;

            virtual bool ignore() const;

            virtual double time_limit() const;
    };


    /**
     * Initialize set of performance tests (validate duplicates, etc)
     * @return valid set of performance tests
     */
    UnitTest *utest_init();
}

#endif /* TEST_UTEST_H_ */
