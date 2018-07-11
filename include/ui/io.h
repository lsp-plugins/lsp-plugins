/*
 * io.h
 *
 *  Created on: 20 июля 2016 г.
 *      Author: sadko
 */

#ifndef UI_IO_H_
#define UI_IO_H_

namespace lsp
{
    /** Mouse controller buttons
     *
     */
    enum mcb_t
    {
        MCB_NONE            = 0,
        MCB_LEFT            = 1,
        MCB_MIDDLE          = 2,
        MCB_RIGHT           = 3,
        MCB_BUTTON4         = 4,
        MCB_BUTTON5         = 5
    };

    /** Mouse controller flags
     *
     */
    enum mcf_t
    {
        MCF_LEFT            = 1 << 0,
        MCF_MIDDLE          = 1 << 1,
        MCF_RIGHT           = 1 << 2,
        MCF_BUTTON4         = 1 << 3,
        MCF_BUTTON5         = 1 << 4,

        MCF_SHIFT           = 1 << 5,
        MCF_LOCK            = 1 << 6,
        MCF_CONTROL         = 1 << 7,

        MCF_MOD1            = 1 << 8,
        MCF_MOD2            = 1 << 9,
        MCF_MOD3            = 1 << 10,
        MCF_MOD4            = 1 << 11,
        MCF_MOD5            = 1 << 12,

        MCF_SUPER           = 1 << 13,
        MCF_HYPER           = 1 << 14,
        MCF_META            = 1 << 15,
        MCF_RELEASE         = 1 << 16
    };

    /** Mouse scroll direction
     *
     */
    enum mcd_t
    {
        MCD_NONE            = 0,
        MCD_UP              = 1,
        MCD_DOWN            = 2,
        MCD_LEFT            = 3,
        MCD_RIGHT           = 4
    };

    /** Event processing flags
     *
     */
    enum event_flags_t
    {
        EVF_NONE            = 0,        // Nothing to do
        EVF_HANDLED         = 1 << 0,   // Event has been processed
        EVF_STOP            = 1 << 1,   // Stop further propagation of event to other elements
        EVF_GRAB            = 1 << 2    // Grab all further events first
    };

    enum ui_event_type_t
    {
        UIE_UNKNOWN,
        UIE_KEY_DOWN,
        UIE_KEY_UP,
        UIE_MOUSE_DOWN,
        UIE_MOUSE_UP,
        UIE_MOUSE_MOVE,
        UIE_REDRAW,
        UIE_SIZE_REQUEST,
        UIE_RESIZE,
        UIE_SHOW,
        UIE_HIDE,
        UIE_CLOSE
    };

    typedef struct ui_event_t
    {
        size_t              nType;      // Type of event, see ui_event_type_t
        ssize_t             nLeft;      // Left position of something
        ssize_t             nTop;       // Top position of something
        ssize_t             nWidth;     // Width of something
        ssize_t             nHeight;    // Height of something
        size_t              nCode;      // Key code or button
        size_t              nState;     // State
    } ui_event_t;

    typedef struct ui_size_request_t
    {
        ssize_t             nMinWidth;
        ssize_t             nMinHeight;
        ssize_t             nMaxWidth;
        ssize_t             nMaxHeight;
    } ui_size_request_t;

    typedef struct ui_realize_t
    {
        ssize_t             nLeft;
        ssize_t             nTop;
        size_t              nWidth;
        size_t              nHeight;
    } ui_realize_t;
}

#endif /* UI_IO_H_ */
