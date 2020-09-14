/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Stefano Tronci <stefano.tronci@protonmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 Apr 2018
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
