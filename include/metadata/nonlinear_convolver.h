/*
 * nonlinear_convolver.h
 *
 *  Created on: 12 Apr 2018
 *      Author: crocoduck
 */

#ifndef METADATA_NONLINEAR_CONVOLVER_H_
#define METADATA_NONLINEAR_CONVOLVER_H_

namespace lsp
{
    struct nonlinear_convolver_mono_metadata
    {
            static const plugin_metadata_t metadata;

            static const float GAIN_DFL = 1.0f;

            enum order_selector_t
            {
                MODEL_ORDER_2,
                MODEL_ORDER_3,
                MODEL_ORDER_4,
                MODEL_ORDER_6,
                MODEL_ORDER_8,

                MODEL_ORDER_DFL = MODEL_ORDER_8
            };

            enum windowSize_selector_t
            {
                WSIZE_ORDER_512,
                WSIZE_ORDER_1024,
                WSIZE_ORDER_2048,
                WSIZE_ORDER_4096,
                WSIZE_ORDER_8192,
                WSIZE_ORDER_16384,
                WSIZE_ORDER_32768,
                WSIZE_ORDER_65536,

                WSIZE_ORDER_DFL = WSIZE_ORDER_4096
            };

            static const size_t KERNELS_MESH_SIZE    = 1024;
    };
}

#endif /* METADATA_NONLINEAR_CONVOLVER_H_ */
