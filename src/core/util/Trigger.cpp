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

            case TRG_TYPE_ADVANCED_RISING_EDGE:
            {
                if (sAdvancedTrg.bDisarm)
                {
                    enTriggerState == TRG_STATE_WAITING;
                    sAdvancedTrg.bDisarm = false;
                }

                if ((value >= sAdvancedTrg.fThreshold - sAdvancedTrg.fHysteresis)  && (nTriggerHoldCounter >= nTriggerHold))
                    enTriggerState = TRG_STATE_ARMED;

                if ((enTriggerState == TRG_STATE_ARMED) && (value >= sAdvancedTrg.fThreshold + sAdvancedTrg.fHysteresis))
                {
                    enTriggerState = TRG_STATE_FIRED;
                    nTriggerHoldCounter = 0;
                    sAdvancedTrg.bDisarm = true;
                }
            }
            break;

            case TRG_TYPE_ADVANCED_FALLING_EDGE:
            {
                if (sAdvancedTrg.bDisarm)
                {
                    enTriggerState == TRG_STATE_WAITING;
                    sAdvancedTrg.bDisarm = false;
                }

                if ((value <= sAdvancedTrg.fThreshold + sAdvancedTrg.fHysteresis)  && (nTriggerHoldCounter >= nTriggerHold))
                    enTriggerState = TRG_STATE_ARMED;

                if ((enTriggerState == TRG_STATE_ARMED) && (value <= sAdvancedTrg.fThreshold - sAdvancedTrg.fHysteresis))
                {
                    enTriggerState = TRG_STATE_FIRED;
                    nTriggerHoldCounter = 0;
                    sAdvancedTrg.bDisarm = true;
                }
            }
            break;

            case TRG_TYPE_NONE:
            default:
            {
                enTriggerState = TRG_STATE_WAITING;

                // Just trigger after the hold time elapsed, no conditions.
                if (nTriggerHoldCounter >= nTriggerHold)
                {
                        enTriggerState = TRG_STATE_FIRED;
                        nTriggerHoldCounter = 0;
                }
            }
        }

        ++nTriggerHoldCounter;
    }

    void Trigger::dump(IStateDumper *v) const
    {
        v->write("enTriggerType", enTriggerType);
        v->write("enTriggerState", enTriggerState);

        v->write("nTriggerHold", nTriggerHold);
        v->write("nTriggerHoldCounter", nTriggerHoldCounter);

        v->begin_object("sSimpleTrg", &sSimpleTrg, sizeof(sSimpleTrg));
        {
            v->write("fThreshold", sSimpleTrg.fThreshold);
            v->write("fPrevious", sSimpleTrg.fPrevious);
        }
        v->end_object();

        v->begin_object("sAdvancedTrg", &sAdvancedTrg, sizeof(sAdvancedTrg));
        {
            v->write("fThreshold", sAdvancedTrg.fThreshold);
            v->write("fHysteresis", sAdvancedTrg.fHysteresis);
            v->write("bDisarm", sAdvancedTrg.bDisarm);
        }
        v->end_object();

        v->write("bSync", bSync);
    }
}
