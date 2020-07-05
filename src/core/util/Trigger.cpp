/*
 * Trigger.cpp
 *
 *  Created on: 29 Jan 2020
 *      Author: crocoduck
 */

#include <core/util/Trigger.h>
#include <core/debug.h>
#include <dsp/dsp.h>

#define MEM_LIM_SIZE 16

namespace lsp
{
    Trigger::Trigger()
    {
        enTriggerType           = TRG_TYPE_NONE;
        enTriggerState          = TRG_STATE_WAITING;

        nTriggerHold            = 0;
        nTriggerHoldCounter     = 0;

        sSimpleTrg.fThreshold   = 0.0f;
        sSimpleTrg.fPrevious    = 0.0f;

        nExternalTriggerCounter = 0;

        bSync                   = true;
    }

    Trigger::~Trigger()
    {
    }

    void Trigger::update_settings()
    {
        if (!bSync)
            return;

        nTriggerHoldCounter = 0;
        bSync = false;
    }

    void Trigger::single_sample_processor(float value)
    {
        switch (enTriggerType)
        {
            case TRG_TYPE_SIMPLE_RISING_EDGE:
            {
                float diff = value - sSimpleTrg.fPrevious;

                if (diff > 0.0f)
                    enTriggerState = TRG_STATE_ARMED;
                else
                {
                    enTriggerState = TRG_STATE_WAITING;
                }

                if ((enTriggerState == TRG_STATE_ARMED) && (value >= sSimpleTrg.fThreshold) && (nTriggerHoldCounter >= nTriggerHold))
                {
                    enTriggerState = TRG_STATE_FIRED;
                    nTriggerHoldCounter = 0;
                }
                else
                    enTriggerState = TRG_STATE_WAITING;

                sSimpleTrg.fPrevious = value;
            }
            break;

            case TRG_TYPE_SIMPLE_FALLING_EDGE:
            {
                float diff = value - sSimpleTrg.fPrevious;

                if (diff < 0.0f)
                    enTriggerState = TRG_STATE_ARMED;
                else
                {
                    enTriggerState = TRG_STATE_WAITING;
                }

                if ((enTriggerState == TRG_STATE_ARMED) && (value <= sSimpleTrg.fThreshold) && (nTriggerHoldCounter >= nTriggerHold))
                {
                    enTriggerState = TRG_STATE_FIRED;
                    nTriggerHoldCounter = 0;
                }
                else
                    enTriggerState = TRG_STATE_WAITING;
            }
            break;

            case TRG_TYPE_EXTERNAL:
            {
                if ((enTriggerState == TRG_STATE_ARMED) && (nExternalTriggerCounter == 0))
                {
                    enTriggerState = TRG_STATE_FIRED;
                    nTriggerHoldCounter = 0;
                }
                else if (enTriggerState == TRG_STATE_FIRED)
                {
                    enTriggerState = TRG_STATE_WAITING;
                }
                else
                {
                    --nExternalTriggerCounter;
                }
            }
            break;

            case TRG_TYPE_NONE:
            default:
                return;
        }

        ++nTriggerHoldCounter;
    }
}
