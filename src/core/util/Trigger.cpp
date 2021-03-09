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
        construct();
    }

    Trigger::~Trigger()
    {
        destroy();
    }

    void Trigger::construct()
    {
        fPrevious                       = 0.0f;

        enTriggerMode                   = TRG_MODE_REPEAT;
        enTriggerType                   = TRG_TYPE_NONE;
        enTriggerState                  = TRG_STATE_WAITING;

        nTriggerHold                    = 0;
        nTriggerHoldCounter             = 0;

        sLocks.bSingleLock              = false;
        sLocks.bManualAllow             = false;
        sLocks.bManualLock              = false;

        sSimpleTrg.fThreshold           = 0.0f;

        sAdvancedTrg.fThreshold         = 0.0f;
        sAdvancedTrg.fHysteresis        = 0.0f;
        sAdvancedTrg.fLowerThreshold    = 0.0f;
        sAdvancedTrg.fUpperThreshold    = 0.0f;
        sAdvancedTrg.bDisarm            = false;

        bSync                           = true;
    }

    void Trigger::destroy()
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
        if (enTriggerMode == TRG_MODE_SINGLE)
        {
            if (sLocks.bSingleLock)
            {
                enTriggerState = TRG_STATE_WAITING;
                return;
            }
        }

        if (enTriggerMode == TRG_MODE_MANUAL)
        {
            if (!sLocks.bManualAllow || sLocks.bManualLock)
            {
                enTriggerState = TRG_STATE_WAITING;
                return;
            }
        }

        float diff = value - fPrevious;

        switch (enTriggerType)
        {
            case TRG_TYPE_SIMPLE_RISING_EDGE:
            {
                enTriggerState = (diff > 0.0f) ? TRG_STATE_ARMED : TRG_STATE_WAITING;

                if ((enTriggerState == TRG_STATE_ARMED) &&
                    (value >= sSimpleTrg.fThreshold) &&
                    (nTriggerHoldCounter >= nTriggerHold)
                )
                {
                    enTriggerState = TRG_STATE_FIRED;
                    nTriggerHoldCounter = 0;
                }
                else
                    enTriggerState = TRG_STATE_WAITING;
            }
            break;

            case TRG_TYPE_SIMPLE_FALLING_EDGE:
            {
                enTriggerState = (diff < 0.0f) ? TRG_STATE_ARMED : TRG_STATE_WAITING;

                if ((enTriggerState == TRG_STATE_ARMED) &&
                    (value <= sSimpleTrg.fThreshold) &&
                    (nTriggerHoldCounter >= nTriggerHold)
                )
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
                    enTriggerState = TRG_STATE_WAITING;
                    sAdvancedTrg.bDisarm = false;
                }

                if ((diff > 0.0f) &&
                    (value >= sAdvancedTrg.fLowerThreshold) &&
                    (fPrevious < sAdvancedTrg.fLowerThreshold) &&
                    (value < sAdvancedTrg.fThreshold) &&
                    (nTriggerHoldCounter >= nTriggerHold)
                )
                    enTriggerState = TRG_STATE_ARMED;

                if ((enTriggerState == TRG_STATE_ARMED) &&
                    (diff > 0.0f) &&
                    (value >= sAdvancedTrg.fUpperThreshold) &&
                    (fPrevious < sAdvancedTrg.fUpperThreshold)
                )
                {
                    enTriggerState = TRG_STATE_FIRED;
                    nTriggerHoldCounter = 0;
                    sAdvancedTrg.bDisarm = true;
                }

                if (value < sAdvancedTrg.fLowerThreshold)
                    sAdvancedTrg.bDisarm = true;
            }
            break;

            case TRG_TYPE_ADVANCED_FALLING_EDGE:
            {
                if (sAdvancedTrg.bDisarm)
                {
                    enTriggerState = TRG_STATE_WAITING;
                    sAdvancedTrg.bDisarm = false;
                }

                if ((diff < 0.0f) &&
                    (value <= sAdvancedTrg.fUpperThreshold) &&
                    (fPrevious > sAdvancedTrg.fUpperThreshold) &&
                    (value > sAdvancedTrg.fThreshold) &&
                    (nTriggerHoldCounter >= nTriggerHold)
                )
                    enTriggerState = TRG_STATE_ARMED;

                if ((enTriggerState == TRG_STATE_ARMED) &&
                    (diff < 0.0f) &&
                    (value <= sAdvancedTrg.fLowerThreshold) &&
                    (fPrevious > sAdvancedTrg.fLowerThreshold)
                )
                {
                    enTriggerState = TRG_STATE_FIRED;
                    nTriggerHoldCounter = 0;
                    sAdvancedTrg.bDisarm = true;
                }

                if (value > sAdvancedTrg.fUpperThreshold)
                    sAdvancedTrg.bDisarm = true;
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
            break;
        }

        if (enTriggerState == TRG_STATE_FIRED)
        {
            if (enTriggerMode == TRG_MODE_SINGLE)
                sLocks.bSingleLock = true;

            if (enTriggerMode == TRG_MODE_MANUAL)
            {
                sLocks.bManualAllow = false;
                sLocks.bManualLock = true;
            }
        }

        ++nTriggerHoldCounter;

        fPrevious = value;
    }

    void Trigger::dump(IStateDumper *v) const
    {
        v->write("fpRevious", fPrevious);

        v->write("enTriggerMode", enTriggerMode);
        v->write("enTriggerType", enTriggerType);
        v->write("enTriggerState", enTriggerState);

        v->write("nTriggerHold", nTriggerHold);
        v->write("nTriggerHoldCounter", nTriggerHoldCounter);

        v->begin_object("sLocks", &sLocks, sizeof(sLocks));
        {
            v->write("bSingleLock", sLocks.bSingleLock);
            v->write("bManualAllow", sLocks.bManualAllow);
            v->write("bManualLock", sLocks.bManualLock);
        }
        v->end_object();

        v->begin_object("sSimpleTrg", &sSimpleTrg, sizeof(sSimpleTrg));
        {
            v->write("fThreshold", sSimpleTrg.fThreshold);
        }
        v->end_object();

        v->begin_object("sAdvancedTrg", &sAdvancedTrg, sizeof(sAdvancedTrg));
        {
            v->write("fThreshold", sAdvancedTrg.fThreshold);
            v->write("fHysteresis", sAdvancedTrg.fHysteresis);
            v->write("fLowerThreshold", sAdvancedTrg.fLowerThreshold);
            v->write("fUpperThreshold", sAdvancedTrg.fUpperThreshold);
            v->write("bDisarm", sAdvancedTrg.bDisarm);
        }
        v->end_object();

        v->write("bSync", bSync);
    }
}
