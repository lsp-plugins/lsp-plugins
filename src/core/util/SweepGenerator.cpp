/*
 * Sweeper.cpp
 *
 *  Created on: 10 May 2020
 *      Author: crocoduck
 */

#include <core/util/SweepGenerator.h>

namespace lsp
{

    SweepGenerator::SweepGenerator()
    {
        bSync                   = true;

        nSampleRate             = -1;

        sPhaseAcc.nAccumulator  = 0;
        sPhaseAcc.nMaxBits      = 0;
        sPhaseAcc.nBits         = 0;
        sPhaseAcc.nMask         = 0;
        sPhaseAcc.nCtrl         = 0;
        sPhaseAcc.fMultiplier   = 1.0f;

        fSweepDuration          = 0;
        fSweepPeak              = 0;
    }

    SweepGenerator::~SweepGenerator()
    {
    }

    void SweepGenerator::init()
    {
        sPhaseAcc.nAccumulator  = 0;
        sPhaseAcc.nMaxBits      = sizeof(sPhaseAcc.nMaxBits) * 8;
        sPhaseAcc.nBits         = sPhaseAcc.nMaxBits;
        sPhaseAcc.nMask         = -1;
        sPhaseAcc.nCtrl         = 0;
        sPhaseAcc.fMultiplier   = 1.0f;
    }

    void SweepGenerator::destroy()
    {
    }

    void SweepGenerator::reset()
    {
        sPhaseAcc.nAccumulator = 0;
    }

    void SweepGenerator::update_settings()
    {
        if (!bSync)
            return;

        sPhaseAcc.nAccumulator = 0;

        sPhaseAcc.nMask = (1 << sPhaseAcc.nBits) - 1;

        float ctrlFactor = 1.0f / (fSweepDuration * nSampleRate);
        sPhaseAcc.nCtrl = sPhaseAcc.nMask * ctrlFactor + ctrlFactor;

        sPhaseAcc.fMultiplier = fSweepPeak / sPhaseAcc.nMask;
    }

    void SweepGenerator::sweep(float *dst, size_t count)
    {
        while (count > 0)
        {
            *dst = sPhaseAcc.fMultiplier * sPhaseAcc.nAccumulator;

            sPhaseAcc.nAccumulator += sPhaseAcc.nCtrl;

            ++dst;
            --count;
        }
    }
}
