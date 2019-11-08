/*
 * common.h
 *
 *  Created on: 1 февр. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_FILTERS_COMMON_H_
#define CORE_FILTERS_COMMON_H_

#include <core/types.h>

namespace lsp
{
    enum filter_type_t
    {
        // Disabled filter
        FLT_NONE,

        FLT_BT_AMPLIFIER,
        FLT_MT_AMPLIFIER,

        // RLC filters
        FLT_BT_RLC_LOPASS,
        FLT_MT_RLC_LOPASS,
        FLT_BT_RLC_HIPASS,
        FLT_MT_RLC_HIPASS,
        FLT_BT_RLC_LOSHELF,
        FLT_MT_RLC_LOSHELF,
        FLT_BT_RLC_HISHELF,
        FLT_MT_RLC_HISHELF,
        FLT_BT_RLC_BELL,
        FLT_MT_RLC_BELL,
        FLT_BT_RLC_RESONANCE,
        FLT_MT_RLC_RESONANCE,
        FLT_BT_RLC_NOTCH,
        FLT_MT_RLC_NOTCH,
        FLT_BT_RLC_ALLPASS,
        FLT_MT_RLC_ALLPASS,
        FLT_BT_RLC_ALLPASS2,
        FLT_MT_RLC_ALLPASS2,
        FLT_BT_RLC_LADDERPASS,
        FLT_MT_RLC_LADDERPASS,
        FLT_BT_RLC_LADDERREJ,
        FLT_MT_RLC_LADDERREJ,
        FLT_BT_RLC_BANDPASS,
        FLT_MT_RLC_BANDPASS,
        FLT_BT_RLC_ENVELOPE,
        FLT_MT_RLC_ENVELOPE,

        // Butterworth-Chebyshev filters
        FLT_BT_BWC_LOPASS,
        FLT_MT_BWC_LOPASS,
        FLT_BT_BWC_HIPASS,
        FLT_MT_BWC_HIPASS,
        FLT_BT_BWC_LOSHELF,
        FLT_MT_BWC_LOSHELF,
        FLT_BT_BWC_HISHELF,
        FLT_MT_BWC_HISHELF,
        FLT_BT_BWC_BELL,
        FLT_MT_BWC_BELL,
        FLT_BT_BWC_LADDERPASS,
        FLT_MT_BWC_LADDERPASS,
        FLT_BT_BWC_LADDERREJ,
        FLT_MT_BWC_LADDERREJ,
        FLT_BT_BWC_BANDPASS,
        FLT_MT_BWC_BANDPASS,
        FLT_BT_BWC_ALLPASS,
        FLT_MT_BWC_ALLPASS,

        // Linkwitz–Riley filters
        FLT_BT_LRX_LOPASS,
        FLT_MT_LRX_LOPASS,
        FLT_BT_LRX_HIPASS,
        FLT_MT_LRX_HIPASS,
        FLT_BT_LRX_LOSHELF,
        FLT_MT_LRX_LOSHELF,
        FLT_BT_LRX_HISHELF,
        FLT_MT_LRX_HISHELF,
        FLT_BT_LRX_BELL,
        FLT_MT_LRX_BELL,
        FLT_BT_LRX_LADDERPASS,
        FLT_MT_LRX_LADDERPASS,
        FLT_BT_LRX_LADDERREJ,
        FLT_MT_LRX_LADDERREJ,
        FLT_BT_LRX_BANDPASS,
        FLT_MT_LRX_BANDPASS,
        FLT_BT_LRX_ALLPASS,
        FLT_MT_LRX_ALLPASS,

        // APO (textbook) Style digital biquad filters (DR stands for direct design: coefficient served directly in digital domain)
        FLT_DR_APO_LOPASS,
        FLT_DR_APO_HIPASS,
        FLT_DR_APO_BANDPASS,
        FLT_DR_APO_NOTCH,
        FLT_DR_APO_ALLPASS,
        FLT_DR_APO_ALLPASS2,
        FLT_DR_APO_PEAKING,
        FLT_DR_APO_LOSHELF,
        FLT_DR_APO_HISHELF,
        FLT_DR_APO_LADDERPASS,
        FLT_DR_APO_LADDERREJ,
    };

    typedef struct filter_params_t
    {
        size_t      nType;      // Filter class
        float       fFreq;      // Frequency
        float       fFreq2;     // Second frequency (for bandpass/allpass2 filter)
        float       fGain;      // Gain
        size_t      nSlope;     // Filter slope
        float       fQuality;   // Quality factor
    } filter_params_t;

    const size_t FILTER_BUFFER_MAX          = 0x1000;
    const size_t FILTER_RANK_MIN            = 8;
    const size_t FILTER_RANK_MAX            = 12;
    const size_t FILTER_CONVOLUTION_MAX     = (1 << FILTER_RANK_MAX);
    const size_t FILTER_CHAINS_MAX          = 0x20;
}

#endif /* INCLUDE_CORE_FILTERS_COMMON_H_ */
