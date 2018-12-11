/*
 * ptest.h
 *
 *  Created on: 16 авг. 2018 г.
 *      Author: sadko
 */

#ifndef TEST_PTEST_H_
#define TEST_PTEST_H_

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <core/types.h>
#include <data/cstorage.h>
#include <test/test.h>

#define PTEST_BEGIN(group, name, time, iterations) \
        namespace ptest { \
        namespace { \
            \
            using namespace ::test; \
            \
            class ptest_ ## name: public PerformanceTest { \
                public: \
                    explicit ptest_ ## name() : PerformanceTest(group, #name, time, iterations) {} \
                    \
                    virtual ~ptest_ ## name() {}

#define PTEST_IGNORE \
        virtual bool ignore() const { return true; }

#define PTEST_MAIN \
        virtual void execute(int argc, const char **argv)

#define PTEST_SUPPORTED(ptr)        TEST_SUPPORTED(ptr)

#define PTEST_LOOP(__key, ...) { \
        double __start = clock(); \
        double __time = 0.0f; \
        wsize_t __iterations = 0; \
        \
        do { \
            for (size_t i=0; i<__test_iterations; ++i) { \
                __VA_ARGS__; \
            } \
            /* Calculate statistics */ \
            __iterations   += __test_iterations; \
            __time          = (clock() - __start) / CLOCKS_PER_SEC; \
        } while (__time < __test_time); \
        \
        gather_stats(__key, __time, __iterations); \
        if (__verbose) { \
            printf("  time [s]:                 %.2f/%.2f\n", __time, __test_time); \
            printf("  iterations:               %ld/%ld\n", long(__iterations), long((__iterations * __test_time) / __time)); \
            printf("  performance [i/s]:        %.2f\n", __iterations / __time); \
            printf("  iteration time [us/i]:    %.4f\n\n", (1000000.0 * __time) / __iterations); \
        } \
    }

#define PTEST_KLOOP(__key, __mul, ...) { \
        double __start = clock(); \
        double __time = 0.0f; \
        wsize_t __iterations = 0; \
        wsize_t __k_iterations = __test_iterations; \
        \
        do { \
            for (size_t i=0; i<__k_iterations; ++i) { \
                __VA_ARGS__; \
            } \
            /* Calculate statistics */ \
            __iterations   += __k_iterations; \
            __time          = (clock() - __start) / CLOCKS_PER_SEC; \
        } while (__time < __test_time); \
        \
        gather_stats(__key, __time, __iterations); \
        if (__verbose) { \
            printf("  time [s]:                 %.2f/%.2f\n", __time, __test_time); \
            printf("  iterations:               %ld/%ld\n", long(__iterations), long((__iterations * __test_time) / __time)); \
            printf("  performance [i/s]:        %.2f\n", __iterations / __time); \
            printf("  iteration time [us/i]:    %.4f\n\n", (1000000.0 * __time) / __iterations); \
        } \
    }

#define PTEST_SEPARATOR \
        gather_stats(NULL, 0.0f, 0); \
        printf("\n");

#define PTEST_SEPARATOR2 \
        gather_stats(NULL, 0.0f, 1); \
        printf("\n");

#define PTEST_FAIL_MSG(message, ...) {  \
            fprintf(stderr, "Performance test '%s.%s' has failed at file %s, line %d with message:\n  " message  "\n", \
                    __test_group, __test_name, __FILE__, __LINE__, ## __VA_ARGS__); \
            exit(1); \
        }

#define PTEST_FAIL() {\
            fprintf(stderr, "Performance test '%s.%s' has failed at file %s, line %d\n", \
                    __test_group, __test_name, __FILE__, __LINE__); \
            exit(1); \
        }

#define PTEST_END \
        } performance_test;  /* ptest class */ \
        } /* anonymous namespace */ \
        } /* namespace ptest */

namespace test
{
    class PerformanceTest: public Test
    {
        private:
            friend PerformanceTest *ptest_init();

        private:
            static PerformanceTest    *__root;
            PerformanceTest           *__next;

        protected:
            typedef struct stats_t
            {
                char       *key;            /* The loop indicator */
                char       *time;           /* Actual time [seconds] */
                char       *n_time;         /* Normalized time [seconds] */
                char       *iterations;     /* Number of iterations */
                char       *n_iterations;   /* Normalized number of iterations */
                char       *performance;    /* The performance of test [iterations per second] */
                char       *time_cost;      /* The amount of time spent per iteration [milliseconds per iteration] */
                char       *rel;            /* The relative speed */
                double      cost;           /* The overall cost */
            } stats_t;

        protected:
            size_t                              __test_iterations;
            double                              __test_time;
            mutable lsp::cstorage<stats_t>      __test_stats;

        protected:
            void gather_stats(const char *key, double time, wsize_t iterations);
            static void destroy_stats(stats_t *stats);
            static void estimate(size_t *len, const char *text);
            static void out_text(FILE *out, size_t length, const char *text, int align, const char *padding, const char *tail);

        public:
            explicit PerformanceTest(const char *group, const char *name, float time, size_t iterations);
            virtual ~PerformanceTest();

        public:
            inline PerformanceTest *next()          { return __next; }
            virtual Test *next_test() const         { return const_cast<PerformanceTest *>(__next); };

            void dump_stats(FILE *out) const;
            void free_stats();
    };


    /**
     * Initialize set of performance tests (validate duplicates, etc)
     * @return valid set of performance tests
     */
    PerformanceTest *ptest_init();
}

#endif /* TEST_PTEST_H_ */
