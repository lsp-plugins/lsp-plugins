/*
 * common.cpp
 *
 *  Created on: 21 янв. 2019 г.
 *      Author: sadko
 */

#include <core/3d/common.h>

namespace lsp
{
#ifdef LSP_DEBUG
    const color3d_t C3D_RED         = { 1.0f, 0.0f, 0.0f, 1.0f };
    const color3d_t C3D_GREEN       = { 0.0f, 1.0f, 0.0f, 1.0f };
    const color3d_t C3D_DARKGREEN   = { 0.0f, 0.75f, 0.0f, 1.0f };
    const color3d_t C3D_BLUE        = { 0.0f, 0.0f, 1.0f, 1.0f };
    const color3d_t C3D_CYAN        = { 0.0f, 1.0f, 1.0f, 1.0f };
    const color3d_t C3D_MAGENTA     = { 1.0f, 0.0f, 1.0f, 1.0f };
    const color3d_t C3D_YELLOW      = { 1.0f, 1.0f, 0.0f, 1.0f };
    const color3d_t C3D_ORANGE      = { 1.0f, 0.5f, 0.0f, 1.0f };
    const color3d_t C3D_GRAY        = { 0.75f, 0.75f, 0.75f, 1.0f };
#endif /* LSP_DEBUG */
}


