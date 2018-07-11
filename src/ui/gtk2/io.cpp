/*
 * io.cpp
 *
 *  Created on: 20 июля 2016 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    size_t gtk2_decode_mcf(size_t flags)
    {
        size_t result = 0;
        #define F_CK(a, b) if ((flags & (a))) result |= (b);

        F_CK(GDK_SHIFT_MASK, MCF_SHIFT)
        F_CK(GDK_LOCK_MASK, MCF_LOCK)
        F_CK(GDK_CONTROL_MASK, MCF_CONTROL)
        F_CK(GDK_MOD1_MASK, MCF_MOD1)
        F_CK(GDK_MOD2_MASK, MCF_MOD2)
        F_CK(GDK_MOD3_MASK, MCF_MOD3)
        F_CK(GDK_MOD4_MASK, MCF_MOD4)
        F_CK(GDK_MOD5_MASK, MCF_MOD5)
        F_CK(GDK_BUTTON1_MASK, MCF_LEFT)
        F_CK(GDK_BUTTON2_MASK, MCF_MIDDLE)
        F_CK(GDK_BUTTON3_MASK, MCF_RIGHT)
        F_CK(GDK_BUTTON4_MASK, MCF_BUTTON4)
        F_CK(GDK_BUTTON5_MASK, MCF_BUTTON5)
        F_CK(GDK_SUPER_MASK, MCF_SUPER)
        F_CK(GDK_HYPER_MASK, MCF_HYPER)
        F_CK(GDK_META_MASK, MCF_META)
        F_CK(GDK_RELEASE_MASK, MCF_RELEASE)

        #undef F_CK

        return result;
    }

    size_t gtk2_encode_mcf(size_t flags)
    {
        size_t result = 0;
        #define F_CK(b, a) if ((flags & (a))) result |= (b);

        F_CK(GDK_SHIFT_MASK, MCF_SHIFT)
        F_CK(GDK_LOCK_MASK, MCF_LOCK)
        F_CK(GDK_CONTROL_MASK, MCF_CONTROL)
        F_CK(GDK_MOD1_MASK, MCF_MOD1)
        F_CK(GDK_MOD2_MASK, MCF_MOD2)
        F_CK(GDK_MOD3_MASK, MCF_MOD3)
        F_CK(GDK_MOD4_MASK, MCF_MOD4)
        F_CK(GDK_MOD5_MASK, MCF_MOD5)
        F_CK(GDK_BUTTON1_MASK, MCF_LEFT)
        F_CK(GDK_BUTTON2_MASK, MCF_MIDDLE)
        F_CK(GDK_BUTTON3_MASK, MCF_RIGHT)
        F_CK(GDK_BUTTON4_MASK, MCF_BUTTON4)
        F_CK(GDK_BUTTON5_MASK, MCF_BUTTON5)
        F_CK(GDK_SUPER_MASK, MCF_SUPER)
        F_CK(GDK_HYPER_MASK, MCF_HYPER)
        F_CK(GDK_META_MASK, MCF_META)
        F_CK(GDK_RELEASE_MASK, MCF_RELEASE)

        #undef F_CK

        return result;
    }

    size_t gtk2_decode_mcb(size_t button)
    {
        switch (button)
        {
            case 1: return MCB_LEFT;
            case 2: return MCB_MIDDLE;
            case 3: return MCB_RIGHT;
            case 4: return MCB_BUTTON4;
            case 5: return MCB_BUTTON5;
        }

        return MCB_NONE;
    }

    size_t gtk2_encode_mcb(size_t button)
    {
        switch (button)
        {
            case MCB_LEFT: return 1;
            case MCB_MIDDLE: return 2;
            case MCB_RIGHT: return 3;
            case MCB_BUTTON4: return 4;
            case MCB_BUTTON5: return 5;
        }

        return 0;
    }

    size_t gtk2_decode_mcd(size_t direction)
    {
        switch (direction)
        {
            case GDK_SCROLL_UP: return MCD_UP;
            case GDK_SCROLL_DOWN: return MCD_DOWN;
            case GDK_SCROLL_LEFT: return MCD_LEFT;
            case GDK_SCROLL_RIGHT: return MCD_RIGHT;
        }
        return MCD_NONE;
    }

    size_t gtk2_encode_mcd(size_t direction)
    {
        switch (direction)
        {
            case MCD_UP: return GDK_SCROLL_UP;
            case MCD_DOWN: return GDK_SCROLL_DOWN;
            case MCD_LEFT: return GDK_SCROLL_LEFT;
            case MCD_RIGHT: return GDK_SCROLL_RIGHT;
        }
        return 0;
    }
}
