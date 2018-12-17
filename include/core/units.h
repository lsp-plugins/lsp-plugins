/*
 * units.h
 *
 *  Created on: 12 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_UNITS_H_
#define CORE_UNITS_H_

#include <core/math.h>

// Some physical constants
#define MAX_SAMPLE_RATE                     192000              /* Maximum supported sample rate [samples / s]      */
#define DEFAULT_SAMPLE_RATE                 48000               /* Default sample rate [samples / s]                */
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
#define BPM_MIN                             1.0f                /* Minimum BPM                                      */
#define BPM_MAX                             1000.0f             /* Maximum BPM                                      */
#define BPM_DEFAULT                         120.0f              /* Default BPM                                      */
#define DEFAULT_TICKS_PER_BEAT              1920.0f             /* Default tick per beat resolution                 */
#define MIDI_EVENTS_MAX                     1024                /* Maximum number of MIDI events per buffer         */
#define GOLDEN_RATIO                        1.618               /* Golden ratio                                     */
#define R_GOLDEN_RATIO                      0.618               /* Reverse golden ratio                             */


// Gain levels in decibels
#define GAIN_AMP_P_72_DB                    3981.073            /* +72 dB       */
#define GAIN_AMP_P_60_DB                    1000.0              /* +60 dB       */
#define GAIN_AMP_P_48_DB                    251.18861           /* +48 dB       */
#define GAIN_AMP_P_36_DB                    63.09575            /* +36 dB       */
#define GAIN_AMP_P_24_DB                    15.84893            /* +24 dB       */
#define GAIN_AMP_P_12_DB                    3.98107             /* +12 dB       */
#define GAIN_AMP_P_6_DB                     1.99526             /* +6 dB        */
#define GAIN_AMP_0_DB                       1.0                 /* 0 dB         */
#define GAIN_AMP_M_6_DB                     0.50118             /* -6 dB        */
#define GAIN_AMP_M_12_DB                    0.25119             /* -12 dB       */
#define GAIN_AMP_M_18_DB                    0.12589             /* -18 dB       */
#define GAIN_AMP_M_24_DB                    0.06310             /* -24 dB       */
#define GAIN_AMP_M_36_DB                    0.01585             /* -36 dB       */
#define GAIN_AMP_M_48_DB                    0.00398             /* -48 dB       */
#define GAIN_AMP_M_60_DB                    0.001               /* -60 dB       */
#define GAIN_AMP_M_72_DB                    0.00025             /* -72 dB       */
#define GAIN_AMP_M_84_DB                    0.0000628           /* -84 dB       */
#define GAIN_AMP_M_INF_DB                   0.0                 /* -inf dB      */

#define GAIN_AMP_MIN                        1e-6
#define GAIN_AMP_MAX                        1e+6

#define GAIN_AMP_M_20_DB                    0.1                 /* -20 dB       */
#define GAIN_AMP_M_40_DB                    0.01                /* -40 dB       */
#define GAIN_AMP_M_60_DB                    0.001               /* -60 dB       */
#define GAIN_AMP_M_80_DB                    0.0001              /* -80 dB       */
#define GAIN_AMP_M_100_DB                   0.00001             /* -100 dB      */
#define GAIN_AMP_M_120_DB                   0.000001            /* -120 dB      */
#define GAIN_AMP_P_20_DB                    10.0                /* +20 dB       */
#define GAIN_AMP_P_40_DB                    100.0               /* +40 dB       */
#define GAIN_AMP_P_60_DB                    1000.0              /* +60 dB       */
#define GAIN_AMP_P_80_DB                    10000.0             /* +80 dB       */

#define GAIN_AMP_N_12_DB                    GAIN_AMP_M_12_DB
#define GAIN_AMP_N_24_DB                    GAIN_AMP_M_24_DB
#define GAIN_AMP_N_36_DB                    GAIN_AMP_M_36_DB
#define GAIN_AMP_N_48_DB                    GAIN_AMP_M_48_DB
#define GAIN_AMP_N_60_DB                    GAIN_AMP_M_60_DB
#define GAIN_AMP_N_72_DB                    GAIN_AMP_M_72_DB

//#define GAIN_AMP_M_0_1_DB                   0.988553094657
#define GAIN_AMP_S_0_1_DB                   0.01157945426
#define GAIN_AMP_S_0_5_DB                   0.05925372518
#define GAIN_AMP_S_1_DB                     0.1220184543

// Float saturation limits
#define FLOAT_SAT_P_NAN                     0.0f
#define FLOAT_SAT_N_NAN                     0.0f
#define FLOAT_SAT_P_INF                     1e+10f
#define FLOAT_SAT_N_INF                     -1e+10f
#define FLOAT_SAT_P_NAN_I                   0
#define FLOAT_SAT_N_NAN_I                   0
#define FLOAT_SAT_P_INF_I                   0x501502f9
#define FLOAT_SAT_N_INF_I                   0xd01502f9

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

    /** Convert decibels to gain value
     *
     * @param db decibels
     * @return gain
     */
    inline float db_to_gain(float db)
    {
        return expf(db * M_LN10 * 0.05f);
    }

    /** Convert decibels to power value
     *
     * @param db decibels
     * @return power
     */
    inline float db_to_power(float db)
    {
        return expf(db * M_LN10 * 0.1f);
    }

    /** Convert gain value to decibels
     *
     * @param gain gain value
     * @return decibels
     */
    inline float gain_to_db(float gain)
    {
        return (20.0f / M_LN10) * logf(gain);
    }

    /** Convert powerr value to decibels
     *
     * @param pwr power value
     * @return decibels
     */
    inline float power_to_db(float pwr)
    {
        return (10.0f / M_LN10) * logf(pwr);
    }
}

#endif /* CORE_UNITS_H_ */
