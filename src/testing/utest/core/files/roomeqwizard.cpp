/*
 * roomeqwizard.cpp
 *
 *  Created on: 7 сент. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/helpers.h>
#include <core/files/RoomEQWizard.h>

using namespace lsp;

UTEST_BEGIN("core.files", roomeqwizard)

    void check_filter(const room_ew::filter_t *f,
            bool enabled, room_ew::filter_type_t type,
            double fc, double gain, double Q
        )
    {
        printf(
            "Filter %s [%d] Fc %.0f Hz Gain %.1f dB Q %.7f\n",
            (f->enabled) ? "ON" : "OFF", f->filterType,
            double(f->fc), double(f->gain), double(f->Q)
        );
        UTEST_ASSERT(f->enabled == enabled);
        UTEST_ASSERT(f->filterType == type);
        UTEST_ASSERT(float_equals_absolute(f->fc, fc));
        UTEST_ASSERT(float_equals_absolute(f->gain, gain));
        if (Q >= 0.0)
            UTEST_ASSERT(float_equals_absolute(f->Q, Q, 0.5e-3f));
    }

    void read_file(const char *fname)
    {
        room_ew::config_t *cfg = NULL;

        // Load the equalizer settings
        UTEST_ASSERT(room_ew::load(fname, &cfg) == STATUS_OK);

        // Check configuration
        UTEST_ASSERT(cfg != NULL);
        UTEST_ASSERT(::strcmp(cfg->sNotes, "test notes") == 0);
        UTEST_ASSERT(::strcmp(cfg->sEqType, "Generic") == 0);
        UTEST_ASSERT(cfg->nVerMaj == 5);
        UTEST_ASSERT(cfg->nVerMin == 19);
        UTEST_ASSERT(cfg->nFilters == 20);
        UTEST_ASSERT(cfg->vFilters != NULL);

        room_ew::filter_t *vf = cfg->vFilters;
        size_t idx = 0;

        check_filter(&vf[idx++], true, room_ew::PK, 100.00, 10.00, 0.7100000);
        check_filter(&vf[idx++], true, room_ew::LS, 1000.00, 10.00, -1);
        check_filter(&vf[idx++], true, room_ew::HS, 1000.00, -10.00, -1);
        check_filter(&vf[idx++], true, room_ew::PK, 10000.00, -10.00, 0.7100000);
        check_filter(&vf[idx++], true, room_ew::HS, 321.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::NONE, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::PK, 100.00, 0.00, 10.0000000);
        check_filter(&vf[idx++], true, room_ew::MODAL, 100.00, 0.00, 13.643000);
        check_filter(&vf[idx++], true, room_ew::LP, 100.00, 0.00, 0.7071068);
        check_filter(&vf[idx++], true, room_ew::HP, 100.00, 0.00, 0.7071068);
        check_filter(&vf[idx++], true, room_ew::LPQ, 100.00, 0.00, 0.7070000);
        check_filter(&vf[idx++], true, room_ew::HPQ, 100.00, 0.00, 0.7070000);
        check_filter(&vf[idx++], true, room_ew::LS, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::HS, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::LS6, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::HS6, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::LS12, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::HS12, 100.00, 0.00, -1);
        check_filter(&vf[idx++], false, room_ew::NO, 100.00, 0.00, -1);
        check_filter(&vf[idx++], true, room_ew::AP, 100.00, 0.00, 0.7070000);

        ::free(cfg);
    }

    UTEST_MAIN
    {
        printf("Testing binary file...\n");
        read_file("res/test/rew/Test11.req");

        printf("Testing binary file...\n");
        read_file("res/test/rew/Test11.txt");
    }

UTEST_END


