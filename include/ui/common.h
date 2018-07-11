/*
 * common.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_COMMON_H_
#define _UI_COMMON_H_

#include <core/types.h>

namespace lsp
{
    // List of all available widget types
    enum widget_t
    {
        W_UNKNOWN = -1,
        W_PLUGIN,
        W_MSTUD,
        W_BUTTON,
        W_VBOX,
        W_HBOX,
        W_GRID,
        W_CELL,
        W_LABEL,
        W_PARAM,
        W_VALUE,
        W_INDICATOR,
        W_LED,
        W_KNOB,
        W_GROUP,
        W_ALIGN,
        W_SWITCH,
        W_GRAPH,
        W_AXIS,
        W_MARKER,
        W_BODY,
        W_MESH,
        W_BASIS
    };

    // List of all available widget attributes
    enum widget_attribute_t
    {
        A_UNKNOWN = -1,
        A_WIDTH,
        A_HEIGHT,
        A_SIZE,
        A_TEXT,
        A_ROWS,
        A_COLS,
        A_COLOR,
        A_BG_COLOR,
        A_SCALE_COLOR,
        A_BORDER_COLOR,
        A_TEXT_COLOR,
        A_FORMAT,
        A_ID,
        A_BALANCE,
        A_INVERT,
        A_TOGGLE,
        A_BORDER,
        A_PADDING,
        A_PAD_LEFT,
        A_PAD_TOP,
        A_PAD_RIGHT,
        A_PAD_BOTTOM,
        A_VPOS,
        A_HPOS,
        A_VSCALE,
        A_HSCALE,
        A_VSPACING,
        A_HSPACING,
        A_SPACING,
        A_ANGLE,
        A_ASPECT,
        A_UNITS,
        A_VALIGN,
        A_HALIGN,
        A_RESIZABLE,
        A_MIN,
        A_MAX,
        A_BASIS,
        A_PARALLEL,
        A_VALUE,
        A_VISIBLE,
        A_VISIBILITY,
        A_PROPORTIONAL,
        A_EXPAND,
        A_FILL,
        A_KEY
    };

    // List of colors
    enum color_t
    {
        C_UNKNOWN = -1,

        C_BACKGROUND,
        C_HOLE,
        C_GLASS,

        C_RED,
        C_GREEN,
        C_BLUE,
        C_YELLOW,
        C_CYAN,
        C_MAGENTA,
        C_BLACK,

        C_LABEL_TEXT,
        C_BUTTON_FACE,

        C_KNOB_CAP,
        C_KNOB_SCALE,

        C_LOGO_FACE,
        C_LOGO_TEXT,

        C_GRAPH_AXIS,
        C_GRAPH_MESH
    };

    // Helper functions
    /** Decode widget type to enum
     *
     * @param name widget name
     * @return widget type
     */
    widget_t widget_type(const char *name);

    /** Get widget tag
     *
     * @param type widget type
     * @return widget tag
     */
    const char *widget_type(widget_t type);

    /** Decode widget attribute
     *
     * @param name widget name
     * @return widget attribute
     */
    widget_attribute_t widget_attribute(const char *name);

    const char *color_name(color_t color);

    color_t color_id(const char *name);
}

#endif /* _UI_COMMON_H_ */
