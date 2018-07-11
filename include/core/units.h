/*
 * units.h
 *
 *  Created on: 12 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_UNITS_H_
#define CORE_UNITS_H_

#include <math.h>

// Some physical constants
#define MAX_SAMPLE_RATE                     192000              /* Maximum supported sample rate [samples / s]      */
#define DECIBEL_INF_THRESH                  1.19209289551e-40   /* Float value that is equal -INF dB                */
#define AIR_ADIABATIC_INDEX                 1.4                 /* Adiabatic index for the Air                      */
#define AIR_MOLAR_MASS                      28.98               /* Molar mass of the air [g/mol]                    */
#define GAS_CONSTANT                        8.3144598           /* Gas constant [ j/(mol * K) }                     */
#define TEMP_ABS_ZERO                       -273.15             /* Temperature of the absolute zero [ C ]           */
#define MAX_SOUND_SPEED                     500                 /* Maximum speed of the sound [ m/s ]               */
#define CMP_TOLERANCE                       1e-6                /* Float compare tolerance                          */
#define AMPLIFICATION_THRESH                1e-8                /* Amplification threshold                          */
#define SOUND_SPEED_M_S                     340.29f             /* Sound speed [ m / s ]                            */
#define SPEC_FREQ_MIN                       10.0f               /* Minimum frequency [ Hz ]                         */
#define SPEC_FREQ_MAX                       24000.0f            /* Maximum frequency [ Hz ]                         */

namespace lsp
{
    /** Convert temperature from Celsium degrees to sound speed [m/s]
     *
     * @param temp temperature [ Celsium degrees ]
     * @return sound speed [m/s]
     */
    inline float sound_speed(float temp)
    {
        return sqrtf(AIR_ADIABATIC_INDEX * GAS_CONSTANT * (temp - TEMP_ABS_ZERO) * 1000 /* g/kg */ / AIR_MOLAR_MASS);
    }

    /** Convert samples [samp] to time [s]
     *
     * @param sr sample rate [samp/s]
     * @param samples number of samples [samp]
     * @return time [s]
     */
    inline float samples_to_seconds(float sr, float samples)
    {
        return samples / sr;
    }

    /** Convert time [s] to samples [samp]
     *
     * @param sr sample rate [samp/s]
     * @param time [s]
     * @return samples [samp]
     */
    inline float seconds_to_samples(float sr, float time)
    {
        return time * sr;
    }

    /** Convert samples [samp] to milliseconds [ms]
     *
     * @param sr sample rate
     * @param samples number of samples
     * @return milliseconds
     */
    inline float samples_to_millis(float sr, float samples)
    {
        return samples_to_seconds(sr, samples) * 1000.0f;
    }

    /** Convert samples [samp] to distance [m]
     *
     * @param sr sample rate [samp/s]
     * @param speed sound speed [m/s]
     * @param samples number of samples [samp]
     * @return distance [m]
     */
    inline float samples_to_meters(float sr, float speed, float samples)
    {
        return (samples * speed) / sr;
    }

    /** Convert samples [samp] to distance [cm]
     *
     * @param sr sample rate [samp/s]
     * @param speed sound speed [m/s]
     * @param samples number of samples [samp]
     * @return distance [cm]
     */
    inline float samples_to_centimeters(float sr, float speed, float samples)
    {
        return samples_to_meters(sr, speed, samples) * 100.0f;
    }

    /** Convert time [ms] to samples [samp]
     *
     * @param sr sample rate [samp/s]
     * @param time time [ms]
     * @return samples [samp]
     */
    inline float millis_to_samples(float sr, float time)
    {
        return seconds_to_samples(sr, time * 0.001f);
    }
}

#endif /* CORE_UNITS_H_ */
