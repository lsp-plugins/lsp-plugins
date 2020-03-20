/*
 * ws.h
 *
 *  Created on: 16 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_WS_WS_H_
#define UI_WS_WS_H_

#include <core/types.h>
#include <core/debug.h>
#include <core/status.h>

// TODO: for release this code shoulde be deleted and defined in makefile
// OS-specific windowing system selection
#if defined(PLATFORM_LINUX) || defined(PLATFORM_BSD)
    #define USE_X11_DISPLAY
#elif defined(PLATFORM_WINDOWS)
    #define USE_WINAPI_DISPLAY
#else
    #error "Unsupported platform"
#endif

#include <core/io/IInStream.h>
#include <core/io/IOutStream.h>

namespace lsp
{
    namespace ws
    {
        typedef uint32_t            ws_code_t;

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
            MCB_BUTTON5         = 5,
            MCB_BUTTON6         = 6,
            MCB_BUTTON7         = 7
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

            MCF_ALT             = 1 << 8,
            MCF_MOD2            = 1 << 9,
            MCF_MOD3            = 1 << 10,
            MCF_MOD4            = 1 << 11,
            MCF_MOD5            = 1 << 12,

            MCF_SUPER           = 1 << 13,
            MCF_HYPER           = 1 << 14,
            MCF_META            = 1 << 15,
            MCF_RELEASE         = 1 << 16,

            MCF_BTN_MASK        = MCF_LEFT | MCF_MIDDLE | MCF_RIGHT | MCF_BUTTON4 | MCF_BUTTON5
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

        /**
         * Different grab group types,
         * sorted according to the priority of grab
         * in ascending order
         */
        enum grab_t
        {
            GRAB_LOWEST,
            GRAB_LOW,
            GRAB_NORMAL,
            GRAB_HIGH,
            GRAB_HIGHEST,

            GRAB_DROPDOWN,                  // Dropdown list

            GRAB_MENU,                      // Simple menu
            GRAB_EXTRA_MENU,                // Menu over menu

            __GRAB_TOTAL
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

        /** Different drag actions
         *
         */
        enum drag_t
        {
            DRAG_COPY           = 0,//!< DRAG_COPY
            DRAG_MOVE           = 1,//!< DRAG_MOVE
            DRAG_LINK           = 2,//!< DRAG_LINK
            DRAG_ASK            = 3,//!< DRAG_ASK
            DRAG_PRIVATE        = 4,//!< DRAG_PRIVATE
            DRAG_DIRECT_SAVE    = 5 //!< DRAG_DIRECT_SAVE
        };

        enum mouse_pointer_t
        {
            MP_NONE,
            MP_ARROW,       // Standard arrow
            MP_HAND,        // Hand pointer
            MP_CROSS,       // Crosshair
            MP_IBEAM,       // Text-editing I-beam
            MP_DRAW,        // Drawing tool (pencil)
            MP_PLUS,        // Plus
            MP_SIZE_NESW,   // Sizing cursor oriented diagonally from northeast to southwest
            MP_SIZE_NS,     // Sizing cursor oriented vertically
            MP_SIZE_WE,     // Sizing cursor oriented horizontally
            MP_SIZE_NWSE,   // Sizing cursor oriented diagonally from northwest to southeast
            MP_UP_ARROW,    // Arrow pointing up
            MP_HOURGLASS,   // Hourglass
            MP_DRAG,        // Arrow with a blank page in the lower-right corner
            MP_NO_DROP,     // Diagonal slash through a white circle
            MP_DANGER,      // Danger cursor
            MP_HSPLIT,      // Black double-vertical bar with arrows pointing right and left
            MP_VPSLIT,      // Black double-horizontal bar with arrows pointing up and down
            MP_MULTIDRAG,   // Arrow with three blank pages in the lower-right corner
            MP_APP_START,   // Arrow combined with an hourglass
            MP_HELP,        // Arrow next to a black question mark

            // Aliases
            MP_SIZE         = MP_ARROW,
            MP_DEFAULT      = MP_ARROW,
            MP_TEXT         = MP_IBEAM,
            MP_VSIZE        = MP_SIZE_NS,
            MP_HSIZE        = MP_SIZE_WE,
            MP_WAIT         = MP_HOURGLASS,
            MP_ARROW_WAIT   = MP_APP_START,
            MP_HYPERLINK    = MP_HAND,
            MP_PENCIL       = MP_DRAW,
            MP_TABLE_CELL   = MP_PLUS,

            // Boundaries
            __MP_LAST       = MP_HELP,
            __MP_COUNT      = __MP_LAST + 1
        };

        enum ui_event_type_t
        {
            UIE_UNKNOWN,
            UIE_KEY_DOWN,
            UIE_KEY_UP,
            UIE_MOUSE_DOWN,
            UIE_MOUSE_UP,
            UIE_MOUSE_MOVE,
            UIE_MOUSE_SCROLL,
            UIE_MOUSE_DBL_CLICK,
            UIE_MOUSE_TRI_CLICK,
            UIE_MOUSE_IN,
            UIE_MOUSE_OUT,
            UIE_REDRAW,
            UIE_RENDER,
            UIE_SIZE_REQUEST,
            UIE_RESIZE,
            UIE_SHOW,
            UIE_HIDE,
            UIE_CLOSE,
            UIE_FOCUS_IN,
            UIE_FOCUS_OUT,

            UIE_DRAG_ENTER,
            UIE_DRAG_LEAVE,
            UIE_DRAG_REQUEST,

            UIE_TOTAL,
            UIE_FIRST = UIE_KEY_DOWN,
            UIE_LAST = UIE_CLOSE,
            UIE_END = UIE_UNKNOWN
        };

        enum border_style_t
        {
            BS_DIALOG,              // Not sizable; no minimize/maximize menu
            BS_SINGLE,              // Not sizable; minimize/maximize menu
            BS_NONE,                // Not sizable; no visible border line
            BS_POPUP,               // Popup window
            BS_COMBO,               // Combo box window
            BS_SIZABLE
        };

        enum window_action_t
        {
            WA_MOVE         = 1 << 0,
            WA_RESIZE       = 1 << 1,
            WA_MINIMIZE     = 1 << 2,
            WA_MAXIMIZE     = 1 << 3,
            WA_CLOSE        = 1 << 4,
            WA_STICK        = 1 << 5,
            WA_SHADE        = 1 << 6,
            WA_FULLSCREEN   = 1 << 7,
            WA_CHANGE_DESK  = 1 << 8,

            WA_ALL          = WA_MOVE | WA_RESIZE | WA_MINIMIZE | WA_MAXIMIZE | WA_CLOSE | WA_STICK | WA_SHADE | WA_FULLSCREEN | WA_CHANGE_DESK,
            WA_NONE         = 0,
            WA_SINGLE       = WA_MOVE | WA_STICK | WA_MINIMIZE | WA_SHADE | WA_CHANGE_DESK | WA_CLOSE,
            WA_DIALOG       = WA_MOVE | WA_STICK | WA_SHADE,
            WA_POPUP        = WA_NONE,
            WA_COMBO        = WA_NONE,
            WA_SIZABLE      = WA_ALL
        };

        typedef uint64_t    ui_timestamp_t;

        typedef struct ws_event_t
        {
            size_t              nType;      // Type of event, see ui_event_type_t
            ssize_t             nLeft;      // Left position of something
            ssize_t             nTop;       // Top position of something
            ssize_t             nWidth;     // Width of something
            ssize_t             nHeight;    // Height of something
            ws_code_t           nCode;      // Key code, button, scroll direction
            size_t              nState;     // State
            ui_timestamp_t      nTime;      // Event timestamp in milliseconds
        } ws_event_t;

        typedef struct size_request_t
        {
            ssize_t             nMinWidth;
            ssize_t             nMinHeight;
            ssize_t             nMaxWidth;
            ssize_t             nMaxHeight;
        } size_request_t;

        typedef struct realize_t
        {
            ssize_t             nLeft;
            ssize_t             nTop;
            ssize_t             nWidth;
            ssize_t             nHeight;
        } realize_t;

        typedef struct padding_t
        {
            size_t              nLeft;
            size_t              nRight;
            size_t              nTop;
            size_t              nBottom;
        } padding_t;

        /** Event handler identifier
         *
         */
        typedef ssize_t         ui_handler_id_t;

        enum surface_type_t
        {
            ST_UNKNOWN,
            ST_IMAGE,
            ST_XLIB,
            ST_PROXY
        };

        typedef struct font_parameters_t
        {
            float Ascent;       // The distance that the font extends above the baseline
            float Descent;      // The distance that the font extends below the baseline
            float Height;       // The recommended vertical distance between baselines when setting consecutive lines of text with the font
            float MaxXAdvance;  // The maximum distance in the X direction that the origin is advanced for any glyph in the font
            float MaxYAdvance;  // The maximum distance in the Y direction that the origin is advanced for any glyph in the font
        } font_parameters_t;

        typedef struct text_parameters_t
        {
            float XBearing;     // The horizontal distance from the origin to the leftmost part of the glyphs as drawn
            float YBearing;     // The vertical distance from the origin to the topmost part of the glyphs as drawn
            float Width;        // Width of the glyphs as drawn
            float Height;       // Height of the glyphs as drawn
            float XAdvance;     // Distance to advance in the X direction after drawing these glyphs
            float YAdvance;     // distance to advance in the Y direction after drawing these glyphs
        } text_parameters_t;

        /** Corners to perform surface drawing
         *
         */
        enum corner_t
        {
            CORNER_LEFT_TOP         = 1 << 0,                                                                       //!< CORNER_LEFT_TOP
            CORNER_RIGHT_TOP        = 1 << 1,                                                                       //!< CORNER_RIGHT_TOP
            CORNER_LEFT_BOTTOM      = 1 << 2,                                                                       //!< CORNER_LEFT_BOTTOM
            CORNER_RIGHT_BOTTOM     = 1 << 3,                                                                       //!< CORNER_RIGHT_BOTTOM

            CORNERS_TOP             = CORNER_LEFT_TOP | CORNER_RIGHT_TOP,                                           //!< CORNERS_TOP
            CORNERS_BOTTOM          = CORNER_LEFT_BOTTOM | CORNER_RIGHT_BOTTOM,                                     //!< CORNERS_BOTTOM
            CORNERS_LEFT            = CORNER_LEFT_TOP | CORNER_LEFT_BOTTOM,                                         //!< CORNERS_LEFT
            CORNERS_RIGHT           = CORNER_RIGHT_TOP | CORNER_RIGHT_BOTTOM,                                       //!< CORNERS_RIGHT
            CORNERS_ALL             = CORNER_LEFT_TOP | CORNER_RIGHT_TOP | CORNER_LEFT_BOTTOM | CORNER_RIGHT_BOTTOM,//!< CORNERS_ALL
            CORNERS_NONE            = 0                                                                             //!< CORNERS_NONE
        };

        // Different kinds of clipboards
        enum clipboard_id_t
        {
            CBUF_PRIMARY,
            CBUF_SECONDARY,
            CBUF_CLIPBOARD,

            _CBUF_TOTAL
        };

        typedef struct clip_format_t
        {
            const char     *content_type;
            const char     *charset;
        } clip_format_t;

        /** timestamp type
         *
         */
        typedef uint64_t    timestamp_t;

        /** Task handler
         *
         * @param time current time at which the timer was executed
         * @param arg argument passed to task handler
         * @return status of operation
         */
        typedef status_t    (* task_handler_t)(timestamp_t time, void *arg);

        /** Clipboard handler
         *
         * @param arg passed to the handler argument
         * @param s status of operation
         * @param is clipboard input stream object
         * @return status of operation
         */
        typedef status_t    (* clipboard_handler_t)(void *arg, status_t s, io::IInStream *is);

        /** Display task identifier
         *
         */
        typedef ssize_t     taskid_t;
    }
}

// Keycode definition
#include <ui/ws/keycodes.h>

// Common definitions
#include <ui/ws/IDataSink.h>
#include <ui/ws/IDataSource.h>
#include <ui/ws/IEventHandler.h>
#include <ui/ws/ISurface.h>
#include <ui/ws/IDisplay.h>
#include <ui/ws/INativeWindow.h>
#include <ui/ws/IR3DBackend.h>

#endif /* INCLUDE_UI_WS_WS_H_ */
