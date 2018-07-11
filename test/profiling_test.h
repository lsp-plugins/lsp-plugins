#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>

#include <core/types.h>
#include <core/dsp.h>

#define MIN_PF_RANK 9
#define MAX_PF_RANK 16

namespace profiling_test
{
    using namespace lsp;

    typedef struct core_t
    {
        float *profile_re;
        float *profile_im;
        float *zero;

        float *src_re;
        float *src_im;
        float *out_re;
        float *out_im;
    } core_t;

    void perform_processing(core_t *p, const float *in, float *out, size_t rank)
    {
        size_t elements     = 1 << rank;
        size_t freqs        = elements >> 1;

        // Clear buffers
        dsp::fill_zero(p->out_re, elements);
        dsp::fill_zero(p->out_im, elements);

        // Apply direct FFT
        dsp::direct_fft(p->src_re, p->src_im, in, p->zero, rank);

        // Process reactions
        for (size_t f=1; f<freqs; ++f)
        {
            float re    = p->src_re[f] + p->src_re[elements - f];
            float im    = p->src_im[f] - p->src_im[elements - f];

            const float *p_re = &p->profile_re[(f-1) * elements];
            const float *p_im = &p->profile_im[(f-1) * elements];

            for (size_t i=0; i<elements; ++i)
            {
                p->out_re[i]   +=   re * p_re[i] - im * p_im[i];
                p->out_im[i]   +=   re * p_im[i] + im * p_re[i];
            }
        }

        // Apply reversive FFT
        dsp::reverse_fft(out, p->src_im, p->out_re, p->out_im, rank);
    }

    void test_profiling(float *in_re, float *out_re, size_t rank)
    {
        core_t p;
        size_t elements         = 1 << rank;
        size_t freqs            = elements >> 1;

        p.profile_re            = new float[freqs * elements];
        p.profile_im            = new float[freqs * elements];
        p.zero                  = new float[elements];
        p.src_re                = new float[elements];
        p.src_im                = new float[elements];
        p.out_re                = new float[elements];
        p.out_im                = new float[elements];

        dsp::fill_zero(p.profile_re, freqs * elements);
        dsp::fill_zero(p.profile_im, freqs * elements);
        dsp::fill_zero(p.zero, elements);

        printf("Testing profiling of size %d (rank = %d)...\n", 1 << rank, int(rank));
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<1000; ++i)
                perform_processing(&p, in_re, out_re, rank);

            // Calculate statistics
            iterations     += 1000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        delete [] p.profile_re;
        delete [] p.profile_im;
        delete [] p.zero;
        delete [] p.src_re;
        delete [] p.src_im;
        delete [] p.out_re;
        delete [] p.out_im;
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        size_t buf_size = 1 << MAX_PF_RANK;

        float *in_re    = new float[buf_size];
        float *out_re   = new float[buf_size];

        for (size_t i=0; i < 1 << MAX_PF_RANK; ++i)
            in_re[i]       = float(rand()) / RAND_MAX;

        for (size_t rank=MIN_PF_RANK; rank <= MAX_PF_RANK; ++rank)
            test_profiling(in_re, out_re, rank);

        delete [] in_re;
        delete [] out_re;

        return 0;
    }
    
}
