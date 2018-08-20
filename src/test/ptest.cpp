/*
 * ptest.cpp
 *
 *  Created on: 16 авг. 2018 г.
 *      Author: sadko
 */

#include <string.h>
#include <malloc.h>
#include <test/ptest.h>

namespace test
{
    PerformanceTest *PerformanceTest::__root = NULL;

    PerformanceTest::PerformanceTest(const char *group, const char *name, float time, size_t iterations)
    {
        __test_group        = group;
        __test_name         = name;
        __test_time         = time;
        __test_iterations   = iterations;
        __next              = __root;
        __verbose           = true;

        // Self-register
        __root              = this;
    }

    PerformanceTest::~PerformanceTest()
    {
        free_stats();
    }

    bool PerformanceTest::ignore() const
    {
        return false;
    }

    void PerformanceTest::destroy_stats(stats_t *stats)
    {
        if (stats->key != NULL)
            free(stats->key);
        if (stats->time != NULL)
            free(stats->time);
        if (stats->n_time != NULL)
            free(stats->n_time);
        if (stats->iterations != NULL)
            free(stats->iterations);
        if (stats->n_iterations != NULL)
            free(stats->n_iterations);
        if (stats->performance != NULL)
            free(stats->performance);
        if (stats->time_cost != NULL)
            free(stats->time_cost);
    }

    void PerformanceTest::estimate(size_t *len, const char *text)
    {
        size_t slen = (text != NULL) ? strlen(text) : 0;
        if (slen > (*len))
            *len = slen;
    }

    void PerformanceTest::gather_stats(const char *key, double time, wsize_t iterations)
    {
        stats_t *stats = __test_stats.add();
        if (stats == NULL)
            return;

        stats->key          = NULL;
        stats->time         = NULL;
        stats->n_time       = NULL;
        stats->iterations   = NULL;
        stats->n_iterations = NULL;
        stats->performance  = NULL;
        stats->time_cost    = NULL;

        if (key == NULL)
            return;

        stats->key      = strdup(key);
        asprintf(&stats->time, "%.2f", time);
        asprintf(&stats->n_time, "%.2f", __test_time);
        asprintf(&stats->iterations, "%lld", (long long)(iterations));
        asprintf(&stats->n_iterations, "%lld", (long long)((iterations * __test_time) / time));
        asprintf(&stats->performance, "%.2f", (iterations / time));
        asprintf(&stats->time_cost, "%.4f", (1000000.0 * time) / iterations);

        if ((stats->key == NULL) ||
                (stats->time == NULL) ||
                (stats->n_time == NULL) ||
                (stats->iterations == NULL) ||
                (stats->n_iterations == NULL) ||
                (stats->performance == NULL) ||
                (stats->time_cost == NULL))
        {
            destroy_stats(stats);
            __test_stats.remove(stats);
        }
    }

    /*
     Table drawing symbols:
        ┌ ─ ┬ ─ ┐

        │   │   │

        ├ ─ ┼ ─ ┤

        │   │   │

        └ ─ ┴ ─ ┘
     */

    void PerformanceTest::out_text(size_t length, const char *text, int align, const char *padding, const char *tail)
    {
        size_t tlen     = (text != NULL) ? strlen(text) : 0;
        length         -= tlen;
        if (align >= 0)
        {
            size_t pad = (align == 0) ? ((length + 1) >> 1) : length;
            length -= pad;
            while (pad--)
                fputs(padding, stdout);
        }
        if (text != NULL)
            fputs(text, stdout);

        while (length--)
            fputs(padding, stdout);
        if (tail != NULL)
            fputs(tail, stdout);
    }

    void PerformanceTest::dump_stats() const
    {
        size_t key          = strlen("Case");
        size_t time         = strlen("Time[s]");
        size_t n_time       = strlen("Samp[s]");
        size_t iterations   = strlen("Iter");
        size_t n_iterations = strlen("Est");
        size_t performance  = strlen("Perf[i/s]");
        size_t time_cost    = strlen("Cost[us/i]");

        // Estimate size of all columns
        for (size_t i=0, n=__test_stats.size(); i < n; ++i)
        {
            stats_t *stats = __test_stats.at(i);

            estimate(&key, stats->key);
            estimate(&time, stats->time);
            estimate(&n_time, stats->n_time);
            estimate(&iterations, stats->iterations);
            estimate(&n_iterations, stats->n_iterations);
            estimate(&performance, stats->performance);
            estimate(&time_cost, stats->time_cost);
        }

        // Output table header
        fputs("┌", stdout);
        out_text(key, "Case", -1, "─", "┬");
        out_text(time, "Time[s]", 1, "─", "┬");
        out_text(iterations, "Iter", 1, "─", "┬");
        out_text(n_time, "Samp[s]", 1, "─", "┬");
        out_text(n_iterations, "Est", 1, "─", "┬");
        out_text(performance, "Perf[i/s]", 1, "─", "┬");
        out_text(time_cost, "Cost[us/i]", 1, "─", "┐\n");

        bool separator = false;

        // Output table data
        for (size_t i=0, n=__test_stats.size(); i < n; ++i)
        {
            stats_t *stats = __test_stats.at(i);
            if (stats->key != NULL)
            {
                if (separator)
                {
                    fputs("├", stdout);
                    out_text(key, NULL, -1, "─", "┼");
                    out_text(time, NULL, 1, "─", "┼");
                    out_text(iterations, NULL, 1, "─", "┼");
                    out_text(n_time, NULL, 1, "─", "┼");
                    out_text(n_iterations, NULL, 1, "─", "┼");
                    out_text(performance, NULL, 1, "─", "┼");
                    out_text(time_cost, NULL, 1, "─", "┤\n");
                }
                separator = false;

                fputs("│", stdout);
                out_text(key, stats->key, -1, " ", "│");
                out_text(time, stats->time, 1, " ", "│");
                out_text(iterations, stats->iterations, 1, " ", "│");
                out_text(n_time, stats->n_time, 1, " ", "│");
                out_text(n_iterations, stats->n_iterations, 1, " ", "│");
                out_text(performance, stats->performance, 1, " ", "│");
                out_text(time_cost, stats->time_cost, 1, " ", "│\n");
            }
            else
                separator = true;
        }

        // Output table footer
        fputs("└", stdout);
        out_text(key, NULL, -1, "─", "┴");
        out_text(time, NULL, 1, "─", "┴");
        out_text(iterations, NULL, 1, "─", "┴");
        out_text(n_time, NULL, 1, "─", "┴");
        out_text(n_iterations, NULL, 1, "─", "┴");
        out_text(performance, NULL, 1, "─", "┴");
        out_text(time_cost, NULL, 1, "─", "┘\n");
    }

    void PerformanceTest::free_stats()
    {
        for (size_t i=0, n=__test_stats.size(); i < n; ++i)
            destroy_stats(__test_stats.at(i));
        __test_stats.flush();
    }

    PerformanceTest *ptest_init()
    {
        // Ensure that there are no duplicates in performance tests
        for (PerformanceTest *first = PerformanceTest::__root; first != NULL; first = first->__next)
        {
            const char *group = first->group();
            const char *name  = first->name();

            for (PerformanceTest *next = first->__next; next != NULL; next = next->__next)
            {
                if (strcasecmp(group, next->group()))
                    continue;
                if (strcasecmp(name, next->name()))
                    continue;

                fprintf(stderr, "Performance test '%s.%s' has duplicate instance\n", group, name);
                return NULL;
            }
        }

        return PerformanceTest::__root;
    }
}


