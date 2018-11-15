/*
 * ctl.h
 *
 *  Created on: 15 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTL_H_
#define UI_CTL_CTL_H_

#include <core/types.h>

#include <locale.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include <metadata/metadata.h>
#include <data/cvector.h>

#include <ui/tk/tk.h>

namespace lsp
{
    namespace ctl
    {
        using namespace lsp::tk;

        enum widget_ctl_t
        {
            WC_UNKNOWN = -1,//!< W_UNKNOWN
            WC_ALIGN,
            WC_AXIS,
            WC_BASIS,
            WC_BODY,
            WC_BOX,
            WC_BUTTON,
            WC_CELL,
            WC_CENTER,
            WC_CGROUP,
            WC_COMBO,
            WC_DOT,
            WC_EDIT,
            WC_FADER,
            WC_FBUFFER,
            WC_FILE,
            WC_FRAC,
            WC_GRAPH,
            WC_GRID,
            WC_GROUP,
            WC_HBOX,
            WC_HGRID,
            WC_HLINK,
            WC_HSBAR,
            WC_HSEP,
            WC_INDICATOR,
            WC_KNOB,
            WC_LABEL,
            WC_LED,
            WC_LISTBOX,
            WC_MARKER,
            WC_MESH,
            WC_METER,
            WC_MSTUD,
            WC_PARAM,
            WC_PLUGIN,
            WC_PORT,
            WC_SAVE,
            WC_SBAR,
            WC_SEP,
            WC_SWITCH,
            WC_TEXT,
            WC_TTAP,
            WC_VALUE,
            WC_VBOX,
            WC_VGRID,
            WC_VSBAR,
            WC_VSEP,
            WC_WINDOW
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
            A_SCALE_HUE_ID,
            A_SCALE_SAT_ID,
            A_SCALE_LIGHT_ID,
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
            A_VISIBILITY_ID,
            A_VISIBILITY_KEY,
            A_PROPORTIONAL,
            A_EXPAND,
            A_FILL,
            A_KEY,
            A_CENTER,
            A_LOGARITHMIC,
            A_HUE_ID,
            A_SAT_ID,
            A_LIGHT_ID,
            A_COORD,
            A_HEAD_ID,
            A_TAIL_ID,
            A_FADEIN_ID,
            A_FADEOUT_ID,
            A_LENGTH_ID,
            A_STATUS_ID,
            A_MESH_ID,
            A_DETAILED,
            A_TRANSPOSE,
            A_LED,
            A_REVERSIVE,
            A_TYPE,
            A_FONT_SIZE,
            A_ACTIVITY_ID,
            A_SCROLL_ID,
            A_VPOS_ID,
            A_HPOS_ID,
            A_SMOOTH,
            A_PRECISION,
            A_EDITABLE,
            A_SAME_LINE,
            A_OFFSET,
            A_VISIBILITY,
            A_STEREO,
            A_ID2,
            A_COLOR2,
            A_HUE2_ID,
            A_SAT2_ID,
            A_LIGHT2_ID,
            A_ACTIVITY2_ID,
            A_HORIZONTAL,
            A_VERTICAL,
            A_ACTIVITY,
            A_ACTIVITY2,
            A_STEP,
            A_TINY_STEP,
            A_DEFAULT,
            A_RADIUS,
            A_URL,
            A_COMMAND_ID,
            A_PROGRESS_ID,
            A_PATH_ID,
            A_HFILL,
            A_VFILL,
            A_FORMAT_ID,
            A_BIND,
            A_OPACITY,
            A_TRANSPARENCY,
            A_MODE
        };

        // Helper functions
        /** Decode widget type to enum
         *
         * @param name widget name
         * @return widget type
         */
        widget_ctl_t widget_ctl(const char *name);

        /** Get widget tag
         *
         * @param type widget type
         * @return widget tag
         */
        const char *widget_ctl(widget_ctl_t type);

        /** Decode widget attribute
         *
         * @param name widget name
         * @return widget attribute
         */
        widget_attribute_t widget_attribute(const char *name);
    }
}

// Core headers
#include <ui/ctl/CtlRegistry.h>
#include <ui/ctl/CtlPortListener.h>
#include <ui/ctl/CtlPort.h>
#include <ui/ctl/CtlExpression.h>
#include <ui/ctl/CtlColor.h>
#include <ui/ctl/CtlPadding.h>

// Parsing header
#include <ui/ctl/parse.h>

// Widget controllers
#include <ui/ctl/CtlWidget.h>
#include <ui/ctl/CtlPluginWindow.h>
#include <ui/ctl/CtlBox.h>
#include <ui/ctl/CtlGrid.h>
#include <ui/ctl/CtlButton.h>
#include <ui/ctl/CtlSwitch.h>
#include <ui/ctl/CtlLabel.h>
#include <ui/ctl/CtlHyperlink.h>
#include <ui/ctl/CtlIndicator.h>
#include <ui/ctl/CtlSeparator.h>
#include <ui/ctl/CtlLed.h>
#include <ui/ctl/CtlKnob.h>
#include <ui/ctl/CtlMeter.h>
#include <ui/ctl/CtlAlign.h>
#include <ui/ctl/CtlGroup.h>
#include <ui/ctl/CtlCell.h>
#include <ui/ctl/CtlScrollBar.h>
#include <ui/ctl/CtlFader.h>
#include <ui/ctl/CtlListBox.h>
#include <ui/ctl/CtlComboBox.h>
#include <ui/ctl/CtlEdit.h>
#include <ui/ctl/CtlAudioFile.h>
#include <ui/ctl/CtlSaveFile.h>
#include <ui/ctl/CtlComboGroup.h>

#include <ui/ctl/CtlGraph.h>
#include <ui/ctl/CtlAxis.h>
#include <ui/ctl/CtlCenter.h>
#include <ui/ctl/CtlBasis.h>
#include <ui/ctl/CtlMarker.h>
#include <ui/ctl/CtlMesh.h>
#include <ui/ctl/CtlDot.h>
#include <ui/ctl/CtlText.h>
#include <ui/ctl/CtlFrameBuffer.h>

#include <ui/ctl/CtlFraction.h>
#include <ui/ctl/CtlTempoTap.h>

// Specials
#include <ui/ctl/CtlPortAlias.h>
#include <ui/ctl/CtlSwitchedPort.h>
#include <ui/ctl/CtlControlPort.h>
#include <ui/ctl/CtlPathPort.h>
#include <ui/ctl/CtlValuePort.h>
#include <ui/ctl/CtlPortHandler.h>
#include <ui/ctl/CtlConfigSource.h>
#include <ui/ctl/CtlConfigHandler.h>

#endif /* UI_CTL_CTL_H_ */
