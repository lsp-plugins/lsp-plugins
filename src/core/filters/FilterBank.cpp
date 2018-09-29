/*
 * FilterBank.cpp
 *
 *  Created on: 2 сент. 2016 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <core/types.h>
#include <dsp/dsp.h>
#include <core/filters/FilterBank.h>

// Currently disable x8 banks on non-experimental stuff
namespace lsp
{
    FilterBank::FilterBank()
    {
        vFilters    = NULL;
        vChains     = NULL;
        nItems      = 0;
        nMaxItems   = 0;
        nLastItems  = -1;
        vData       = NULL;
        vBackup     = NULL;
    }

    FilterBank::~FilterBank()
    {
        destroy();
    }

    void FilterBank::destroy()
    {
        if (vData != NULL)
        {
            lsp_free(vData);
            vData       = NULL;
        }

        vFilters    = NULL;
        vChains     = NULL;
        vBackup     = NULL;
        nItems      = 0;
        nMaxItems   = 0;
        nLastItems  = -1;
    }

    bool FilterBank::init(size_t filters)
    {
        destroy();

        // Calculate data size
        size_t n_banks      = (filters/8) + 3;
        size_t bank_alloc   = ALIGN_SIZE(sizeof(biquad_t), BIQUAD_ALIGN) * n_banks;
        size_t chain_alloc  = sizeof(biquad_x1_t) * filters;
        size_t backup_alloc = sizeof(float) * BIQUAD_D_ITEMS * n_banks;

        // Allocate data
        size_t allocate     = bank_alloc + chain_alloc + backup_alloc + BIQUAD_ALIGN;
        vData               = lsp_tmalloc(uint8_t, allocate);
        if (vData == NULL)
            return false;

        // Align and initialize pointers
        uint8_t *ptr        = ALIGN_PTR(vData, BIQUAD_ALIGN);
        vFilters            = reinterpret_cast<biquad_t *>(ptr);
        ptr                += bank_alloc;
        vChains             = reinterpret_cast<biquad_x1_t *>(ptr);
        ptr                += chain_alloc;
        vBackup             = reinterpret_cast<float *>(ptr);
        ptr                += backup_alloc;

        // Update parameters
        nItems              = 0;
        nMaxItems           = filters;
        nLastItems          = -1;

        return true;
    }

    biquad_x1_t *FilterBank::add_chain()
    {
        if (nItems >= nMaxItems)
            return (nItems <= 0) ? NULL : &vChains[nItems-1];
        return &vChains[nItems++];
    }

    void FilterBank::end(bool clear)
    {
        size_t items    = nItems;
        biquad_x1_t *c  = vChains;
        biquad_t *b     = vFilters;

        // Contents of c:
        // a0 a0 a1 a2
        // b1 b2 0  0

        // Add 8x filter bank
        while (items >= 8)
        {
            biquad_x8_t *f = &b->x8;

            f->a0[0]    = c[0].a[0];
            f->a0[1]    = c[1].a[0];
            f->a0[2]    = c[2].a[0];
            f->a0[3]    = c[3].a[0];
            f->a0[4]    = c[4].a[0];
            f->a0[5]    = c[5].a[0];
            f->a0[6]    = c[6].a[0];
            f->a0[7]    = c[7].a[0];

            f->a1[0]    = c[0].a[2];
            f->a1[1]    = c[1].a[2];
            f->a1[2]    = c[2].a[2];
            f->a1[3]    = c[3].a[2];
            f->a1[4]    = c[4].a[2];
            f->a1[5]    = c[5].a[2];
            f->a1[6]    = c[6].a[2];
            f->a1[7]    = c[7].a[2];

            f->a2[0]    = c[0].a[3];
            f->a2[1]    = c[1].a[3];
            f->a2[2]    = c[2].a[3];
            f->a2[3]    = c[3].a[3];
            f->a2[4]    = c[4].a[3];
            f->a2[5]    = c[5].a[3];
            f->a2[6]    = c[6].a[3];
            f->a2[7]    = c[7].a[3];

            f->b1[0]    = c[0].b[0];
            f->b1[1]    = c[1].b[0];
            f->b1[2]    = c[2].b[0];
            f->b1[3]    = c[3].b[0];
            f->b1[4]    = c[4].b[0];
            f->b1[5]    = c[5].b[0];
            f->b1[6]    = c[6].b[0];
            f->b1[7]    = c[7].b[0];

            f->b2[0]    = c[0].b[1];
            f->b2[1]    = c[1].b[1];
            f->b2[2]    = c[2].b[1];
            f->b2[3]    = c[3].b[1];
            f->b2[4]    = c[4].b[1];
            f->b2[5]    = c[5].b[1];
            f->b2[6]    = c[6].b[1];
            f->b2[7]    = c[7].b[1];

            c          += 8;
            b          ++;
            items      -= 8;
        }

        // Add 4x filter bank
        if (items & 4)
        {
            biquad_x4_t *f = &b->x4;

            f->a0[0]    = c[0].a[0];
            f->a0[1]    = c[1].a[0];
            f->a0[2]    = c[2].a[0];
            f->a0[3]    = c[3].a[0];

            f->a1[0]    = c[0].a[2];
            f->a1[1]    = c[1].a[2];
            f->a1[2]    = c[2].a[2];
            f->a1[3]    = c[3].a[2];

            f->a2[0]    = c[0].a[3];
            f->a2[1]    = c[1].a[3];
            f->a2[2]    = c[2].a[3];
            f->a2[3]    = c[3].a[3];

            f->b1[0]    = c[0].b[0];
            f->b1[1]    = c[1].b[0];
            f->b1[2]    = c[2].b[0];
            f->b1[3]    = c[3].b[0];

            f->b2[0]    = c[0].b[1];
            f->b2[1]    = c[1].b[1];
            f->b2[2]    = c[2].b[1];
            f->b2[3]    = c[3].b[1];

            c          += 4;
            b          ++;
        }

        // Add 2x filter bank
        if (items & 2)
        {
            biquad_x2_t *f = &b->x2;

            f->a[0]     = c[0].a[0];
            f->a[1]     = c[0].a[1];
            f->a[2]     = c[0].a[2];
            f->a[3]     = c[0].a[3];
            f->a[4]     = c[1].a[0];
            f->a[5]     = c[1].a[1];
            f->a[6]     = c[1].a[2];
            f->a[7]     = c[1].a[3];

            f->b[0]     = c[0].b[0];
            f->b[1]     = c[0].b[1];
            f->b[2]     = c[0].b[2];
            f->b[3]     = c[0].b[3];
            f->b[4]     = c[1].b[0];
            f->b[5]     = c[1].b[1];
            f->b[6]     = c[1].b[2];
            f->b[7]     = c[1].b[3];

            c          += 2;
            b          ++;
        }

        // Add 1x filter
        if (items & 1)
        {
            biquad_x1_t *f = &b->x1;

            f->a[0]     = c->a[0];
            f->a[1]     = c->a[1];
            f->a[2]     = c->a[2];
            f->a[3]     = c->a[3];
            f->b[0]     = c->b[0];
            f->b[1]     = c->b[1];
            f->b[2]     = c->b[2];
            f->b[3]     = c->b[3];

            c          ++;
            b          ++;
        }

        // Clear delays if structure has changed
        if ((clear) || (nItems != nLastItems))
            reset();
        nLastItems      = nItems;
    }

    void FilterBank::reset()
    {
        size_t items    = nItems >> 3;
        if (nItems & 4)
            items ++;
        if (nItems & 2)
            items ++;
        if (nItems & 1)
            items ++;

        biquad_t *b     = vFilters;
        while (items--)
        {
            dsp::fill_zero(b->d, BIQUAD_D_ITEMS);
            b++;
        }
    }

    void FilterBank::process(float *out, const float *in, size_t samples)
    {
        size_t items        = nItems;
        biquad_t *f         = vFilters;

        if (items == 0)
        {
            dsp::copy(out, in, samples);
            return;
        }

        while (items >= 8)
        {
            dsp::biquad_process_x8(out, in, samples, f);
            in         = out;  // actual data for the next chain is in output buffer now
            f         ++;
            items     -= 8;
        }

        if (items & 4)
        {
            dsp::biquad_process_x4(out, in, samples, f);
            in         = out;  // actual data for the next chain is in output buffer now
            f         ++;
        }

        if (items & 2)
        {
            dsp::biquad_process_x2(out, in, samples, f);
            in         = out;  // actual data for the next chain is in output buffer now
            f         ++;
        }

        if (items & 1)
            dsp::biquad_process_x1(out, in, samples, f);
    }

    void FilterBank::impulse_response(float *out, size_t samples)
    {
        // Backup and clean all delays
        biquad_t *f         = vFilters;
        float *dst          = vBackup;

        size_t items        = nItems >> 3;
        if (nItems & 4)
            items ++;
        if (nItems & 2)
            items ++;
        if (nItems & 1)
            items ++;

        for (size_t i=0; i < items; ++i)
        {
            dsp::copy(dst, f->d, BIQUAD_D_ITEMS);
            dsp::fill_zero(f->d, BIQUAD_D_ITEMS);
            dst                += BIQUAD_D_ITEMS;
            f                  ++;
        }

        // Generate impulse response
        dsp::fill_zero(out, samples);
        out[0]              = 1.0f;
        process(out, out, samples);

        // Restore all delays
        dst                 = vBackup;
        f                   = vFilters;

        for (size_t i=0; i < items; ++i)
        {
            dsp::copy(f->d, dst, BIQUAD_D_ITEMS);
            dst                += BIQUAD_D_ITEMS;
            f                  ++;
        }
    }

} /* namespace lsp */
