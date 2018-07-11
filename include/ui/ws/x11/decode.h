/*
 * decode.h
 *
 *  Created on: 8 сент. 2017 г.
 *      Author: sadko
 */

#ifndef UI_WS_X11_DECODE_H_
#define UI_WS_X11_DECODE_H_

namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            mcb_t decode_mcb(size_t code);

            mcd_t decode_mcd(size_t code);

            size_t decode_state(size_t code);

            ws_code_t decode_keycode(KeySym code);
        }
    }
}


#endif /* UI_WS_X11_DECODE_H_ */
