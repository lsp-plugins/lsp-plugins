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
            WC_UNKNOWN = -1,

            WC_ALIGN,
            WC_AXIS,
            WC_BASIS,
            WC_BODY,
            WC_BOX,
            WC_BUTTON,
            WC_CAPTURE3D,
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
            WC_HSBOX,
            WC_HSEP,
            WC_INDICATOR,
            WC_KNOB,
            WC_LABEL,
            WC_LED,
            WC_LISTBOX,
            WC_LOAD,
            WC_MARKER,
            WC_MESH,
            WC_METER,
            WC_MSTUD,
            WC_PARAM,
            WC_PLUGIN,
            WC_PORT,
            WC_PROGRESS,
            WC_SAMPLE,
            WC_SAVE,
            WC_SBAR,
            WC_SBOX,
            WC_SEP,
            WC_SOURCE3D,
            WC_STATUS,
            WC_SWITCH,
            WC_TEXT,
            WC_THREADCOMBO,
            WC_TTAP,
            WC_VALUE,
            WC_VBOX,
            WC_VGRID,
            WC_VIEWER3D,
            WC_VOID,
            WC_VSBAR,
            WC_VSBOX,
            WC_VSEP,
            WC_WINDOW
        };

        // List of all available widget attributes
        enum widget_attribute_t
        {
            A_UNKNOWN = -1,

            A_ACTIVITY,
            A_ACTIVITY2,
            A_ACTIVITY2_ID,
            A_ACTIVITY_ID,
            A_ANGLE,
            A_ANGLE_ID,
            A_ASPECT,
            A_BALANCE,
            A_BASIS,
            A_BG_COLOR,
            A_BIND,
            A_BORDER,
            A_BORDER_COLOR,
            A_BRIGHT,
            A_CENTER,
            A_COLOR,
            A_COLOR2,
            A_COLS,
            A_COMMAND_ID,
            A_COORD,
            A_CURVATURE_ID,
            A_CYCLE,
            A_DEFAULT,
            A_DETAILED,
            A_DISTANCE_ID,
            A_DURATION_ID,
            A_DX,
            A_DY,
            A_EDITABLE,
            A_EMBED,
            A_EXPAND,
            A_FADEIN_ID,
            A_FADEOUT_ID,
            A_FILL,
            A_FONT_SIZE,
            A_FORMAT,
            A_FORMAT_ID,
            A_FOV,
            A_HALIGN,
            A_HEAD_ID,
            A_HEIGHT,
            A_HEIGHT_ID,
            A_HFILL,
            A_HORIZONTAL,
            A_HPOS,
            A_HPOS_ID,
            A_HSCALE,
            A_HSCROLL,
            A_HSPACING,
            A_HUE2_ID,
            A_HUE_ID,
            A_HUE_SHIFT,
            A_ID,
            A_ID2,
            A_INVERT,
            A_KEY,
            A_KVT_ROOT,
            A_LED,
            A_LENGTH,
            A_LENGTH_ID,
            A_LIGHT2_ID,
            A_LIGHT_ID,
            A_LOGARITHMIC,
            A_MAX,
            A_MAX_DURATION_ID,
            A_MAX_HEIGHT,
            A_MAX_WIDTH,
            A_MESH_ID,
            A_MIN,
            A_MIN_HEIGHT,
            A_MIN_WIDTH,
            A_MODE,
            A_MODE_ID,
            A_OFFSET,
            A_OPACITY,
            A_ORIENTATION_ID,
            A_PAD_BOTTOM,
            A_PAD_LEFT,
            A_PAD_RIGHT,
            A_PAD_TOP,
            A_PADDING,
            A_PARALLEL,
            A_PATH_ID,
            A_PITCH_ID,
            A_PRECISION,
            A_PROGRESS_ID,
            A_PROPORTIONAL,
            A_RADIUS,
            A_RADIUS_ID,
            A_RESIZABLE,
            A_REVERSIVE,
            A_ROLL_ID,
            A_ROWS,
            A_SAME_LINE,
            A_SAT2_ID,
            A_SAT_ID,
            A_SCALE_COLOR,
            A_SCALE_HUE,
            A_SCALE_HUE_ID,
            A_SCALE_LIGHT_ID,
            A_SCALE_SAT_ID,
            A_SCROLL_ID,
            A_SIZE,
            A_SIZE_ID,
            A_SMOOTH,
            A_SPACING,
            A_STATUS_ID,
            A_STEP,
            A_STEREO,
            A_TAIL_ID,
            A_TEXT,
            A_TEXT_COLOR,
            A_TINY_STEP,
            A_TOGGLE,
            A_TRANSPARENCY,
            A_TRANSPOSE,
            A_TYPE,
            A_WUID,
            A_UNITS,
            A_URL,
            A_VALIGN,
            A_VALUE,
            A_VERTICAL,
            A_VFILL,
            A_VISIBILITY,
            A_VISIBILITY_ID,
            A_VISIBILITY_KEY,
            A_VISIBLE,
            A_VPOS,
            A_VPOS_ID,
            A_VSCALE,
            A_VSCROLL,
            A_VSPACING,
            A_WIDTH,
            A_X_INDEX,
            A_XPOS_ID,
            A_XSCALE_ID,
            A_Y_INDEX,
            A_YAW_ID,
            A_YPOS_ID,
            A_YSCALE_ID,
            A_ZPOS_ID,
            A_ZSCALE_ID
        };

        typedef struct ctl_class_t
        {
            const char         *name;
            const ctl_class_t  *parent;
        } ctl_class_t;

        // Helper functions
        /** Decode widget type to enum
         *
         * @param name widget name
         * @return widget type
         */
        widget_ctl_t widget_ctl(const char *name);

        /** Get widget tag name
         *
         * @param type widget type
         * @return widget tag or NULL if not found
         */
        const char *widget_ctl(widget_ctl_t type);

        /** Decode widget attribute
         *
         * @param name widget name
         * @return widget attribute
         */
        widget_attribute_t widget_attribute(const char *name);

        /**
         * Get widget attribute name
         * @param type attribute type
         * @return attribute name or NULL if not found
         */
        const char *widget_attribute(widget_attribute_t type);

        /**
         * Get widget scrolling
         * @param value value representing scrolling
         * @return scrolling type
         */
        scrolling_t widget_scroll(const char *value);
    }
}

// Core headers
#include <ui/ctl/CtlRegistry.h>
#include <ui/ctl/CtlPortListener.h>
#include <ui/ctl/CtlPortResolver.h>
#include <ui/ctl/CtlPort.h>
#include <ui/ctl/CtlExpression.h>
#include <ui/ctl/CtlColor.h>
#include <ui/ctl/CtlPadding.h>
#include <ui/ctl/CtlKvtListener.h>

// Parsing header
#include <ui/ctl/parse.h>

// Widget controllers
#include <ui/ctl/CtlWidget.h>
#include <ui/ctl/CtlVoid.h>
#include <ui/ctl/CtlPluginWindow.h>
#include <ui/ctl/CtlBox.h>
#include <ui/ctl/CtlScrollBox.h>
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
#include <ui/ctl/CtlLoadFile.h>
#include <ui/ctl/CtlComboGroup.h>
#include <ui/ctl/CtlProgressBar.h>
#include <ui/ctl/CtlAudioSample.h>

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

#include <ui/ctl/CtlViewer3D.h>
#include <ui/ctl/CtlCapture3D.h>
#include <ui/ctl/CtlSource3D.h>

// Specials
#include <ui/ctl/CtlPortAlias.h>
#include <ui/ctl/CtlSwitchedPort.h>
#include <ui/ctl/CtlControlPort.h>
#include <ui/ctl/CtlPathPort.h>
#include <ui/ctl/CtlValuePort.h>
#include <ui/ctl/CtlPortHandler.h>
#include <ui/ctl/CtlConfigSource.h>
#include <ui/ctl/CtlConfigHandler.h>
#include <ui/ctl/CtlThreadComboBox.h>

#endif /* UI_CTL_CTL_H_ */
