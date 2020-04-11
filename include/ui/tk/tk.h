/*
 * tk.h
 *
 *  Created on: 15 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_TK_H_
#define UI_TK_TK_H_

#include <core/types.h>
#include <core/status.h>
#include <data/cvector.h>
#include <data/cstorage.h>
#include <core/LSPString.h>
#include <core/i18n/IDictionary.h>

#include <ui/ws/ws.h>

namespace lsp
{
    namespace tk
    {
        /** Widget orientation
         *
         */
        enum orientation_t
        {
            O_HORIZONTAL,       //!< O_HORIZONTAL horizontal orientation
            O_VERTICAL          //!< O_VERTICAL vertical orientation
        };

        enum scrolling_t
        {
            SCROLL_NONE,        //!< SCROLL_NONE no scrolling permitted
            SCROLL_OPTIONAL,    //!< SCROLL_OPTIONAL scrolling is permitted but in optional state
            SCROLL_ALWAYS       //!< SCROLL_ALWAYS scrolling is permitted but in optional state
        };

        /** Widget classes
         *
         */
        enum wclass_t
        {
            W_UNKNOWN = -1,//!< W_UNKNOWN
            W_ALIGN,
            W_AXIS,
            W_BASIS,
            W_BODY,
            W_BOX,
            W_BUTTON,
            W_CELL,
            W_CENTER,
            W_COMBO,
            W_DOT,
            W_EDIT,
            W_FILE,
            W_GRAPH,
            W_GRID,
            W_GROUP,
            W_INDICATOR,
            W_KNOB,
            W_LABEL,
            W_LED,
            W_LISTBOX,
            W_MARKER,
            W_MENU,
            W_MENUITEM,
            W_MESH,
            W_METER,
            W_MSGBOX,
            W_MSTUD,
            W_PARAM,
            W_PLUGIN,
            W_PORT,
            W_SBAR,
            W_SEP,
            W_SWITCH,
            W_TEXT,
            W_VALUE,
            W_WINDOW
        };

        // List of colors
        enum color_t
        {
            C_UNKNOWN = -1,

            C_BACKGROUND,
            C_BACKGROUND2,
            C_HOLE,
            C_GLASS,

            C_RED,
            C_GREEN,
            C_BLUE,
            C_YELLOW,
            C_CYAN,
            C_MAGENTA,
            C_BLACK,
            C_WHITE,

            C_LABEL_TEXT,
            C_BUTTON_FACE,
            C_BUTTON_TEXT,

            C_KNOB_CAP,
            C_KNOB_SCALE,

            C_LOGO_FACE,
            C_LOGO_TEXT,

            C_GRAPH_AXIS,
            C_GRAPH_MESH,
            C_GRAPH_MARKER,
            C_GRAPH_TEXT,
            C_GRAPH_LINE,
            C_LEFT_CHANNEL,
            C_RIGHT_CHANNEL,
            C_MIDDLE_CHANNEL,
            C_SIDE_CHANNEL,

            C_HLINK_TEXT,
            C_HLINK_HOVER,

            C_STATUS_OK,
            C_STATUS_WARN,
            C_STATUS_ERROR,

            C_INVALID_INPUT
        };

        /** List of available slots for event processing
         *
         */
        enum ui_slot_t
        {
            LSPSLOT_FOCUS_IN,           //!< LSPSLOT_FOCUS_IN Triggered when element takes focus
            LSPSLOT_FOCUS_OUT,          //!< LSPSLOT_FOCUS_IN Triggered when element loses focus
            LSPSLOT_KEY_DOWN,           //!< LSPSLOT_KEY_DOWN Triggered on keyboard key press
            LSPSLOT_KEY_UP,             //!< LSPSLOT_KEY_UP Triggered on keyboard key release
            LSPSLOT_MOUSE_DOWN,         //!< LSPSLOT_MOUSE_DOWN Triggered on mouse button press
            LSPSLOT_MOUSE_UP,           //!< LSPSLOT_MOUSE_UP Triggered on mouse button release
            LSPSLOT_MOUSE_MOVE,         //!< LSPSLOT_MOUSE_MOVE Triggered on mouse pointer motion
            LSPSLOT_MOUSE_SCROLL,       //!< LSPSLOT_MOUSE_SCROLL Triggered on mouse scroll event
            LSPSLOT_MOUSE_CLICK,        //!< LSPSLOT_MOUSE_DBL_CLICK Triggered on mouse click
            LSPSLOT_MOUSE_DBL_CLICK,    //!< LSPSLOT_MOUSE_DBL_CLICK Triggered on mouse double click
            LSPSLOT_MOUSE_TRI_CLICK,    //!< LSPSLOT_MOUSE_TRI_CLICK Triggered on mouse triple click
            LSPSLOT_MOUSE_IN,           //!< LSPSLOT_MOUSE_IN Triggered when mouse first entered widget's area
            LSPSLOT_MOUSE_OUT,          //!< LSPSLOT_MOUSE_OUT Triggered when mouse left widget's area
            LSPSLOT_SHOW,               //!< LSPSLOT_SHOW Triggered when widget becomes visible
            LSPSLOT_HIDE,               //!< LSPSLOT_HIDE Triggered when widget becomes invisible
            LSPSLOT_SUBMIT,             //!< LSPSLOT_SUBMIT Triggered when value(s) stored by the widget is submitted (but can be not changed)
            LSPSLOT_CHANGE,             //!< LSPSLOT_CHANGE Triggered only when value(s) stored by the widget is(are) changed
            LSPSLOT_DESTROY,            //!< LSPSLOT_DESTROY Triggered when widget associated with slot is destroyed
            LSPSLOT_RESIZE,             //!< LSPSLOT_RESIZE Triggered when the geometry of widget has been changed
            LSPSLOT_RESIZE_PARENT,      //!< LSPSLOT_RESIZE_PARENT Triggered when the parent widget has been resized
            LSPSLOT_CLOSE,              //!< LSPSLOT_CLOSE Triggered when the window is requested to close
            LSPSLOT_HSCROLL,            //!< LSPSLOT_HSCROLL Triggered when the horizontal scrolling is applied
            LSPSLOT_VSCROLL,            //!< LSPSLOT_VSCROLL Triggered when the vertical scrolling is applied
            LSPSLOT_ACTIVATE,           //!< LSPSLOT_ACTIVATE Triggered some user-interaction
            LSPSLOT_DRAW3D,             //!< LSPSLOT_DRAW3D Triggered when a 3D-rendering is required
            LSPSLOT_DRAG_REQUEST,       //!< LSPSLOT_DRAG_REQUEST Triggered when a drag request is pending on the widget
            LSPSLOT_BEFORE_POPUP,       //!< LSPSLOT_BEFORE_POPUP Triggered before pop-up element is going to be shown
            LSPSLOT_POPUP,              //!< LSPSLOT_POPUP Triggered after pop-up element has been shown
        };

        typedef struct w_class_t
        {
            const char         *name;
            const w_class_t    *parent;
        } w_class_t;

        enum ui_property_type_t
        {
            PT_INT,         // Integer property
            PT_FLOAT,       // Floating-point property
            PT_BOOL,        // Boolean property
            PT_STRING,      // String (text) property

            PT_UNKNOWN  = -1
        };

        /**
         * Atom identifier
         */
        typedef ssize_t             ui_atom_t;

        /** Get color name by it's identifier
         *
         * @param color color identifier
         * @return color name
         */
        const char *color_name(color_t color);

        /** Get color identifier by it's name
         *
         * @param name color name
         * @return color identifier
         */
        color_t color_id(const char *name);

        // Collaborate with ::lsp::ws namespace
        using namespace ::lsp::ws;
    }
}

// System objects
#include <ui/tk/sys/LSPSlot.h>
#include <ui/tk/sys/LSPTimer.h>
#include <ui/tk/sys/LSPSlotSet.h>
#include <ui/tk/sys/LSPStyle.h>
#include <ui/tk/sys/LSPColor.h>
#include <ui/tk/sys/LSPLocalString.h>
#include <ui/tk/sys/LSPFloat.h>
#include <ui/tk/sys/LSPTheme.h>
#include <ui/tk/sys/LSPDisplay.h>

// Utilitary objects
#include <ui/tk/util/LSPSurface.h>
#include <ui/tk/util/LSPFont.h>
#include <ui/tk/util/LSPPadding.h>
#include <ui/tk/util/LSPTextLines.h>
#include <ui/tk/util/LSPItem.h>
#include <ui/tk/util/LSPItemList.h>
#include <ui/tk/util/LSPItemSelection.h>
#include <ui/tk/util/LSPTextSelection.h>
#include <ui/tk/util/LSPTextCursor.h>
#include <ui/tk/util/LSPFileMask.h>
#include <ui/tk/util/LSPKeyboardHandler.h>
#include <ui/tk/util/LSPSizeConstraints.h>
#include <ui/tk/util/LSPWindowActions.h>
#include <ui/tk/util/LSPTextDataSource.h>
#include <ui/tk/util/LSPTextDataSink.h>
#include <ui/tk/util/LSPUrlSink.h>
#include <ui/tk/util/LSPFileFilterItem.h>
#include <ui/tk/util/LSPFileFilter.h>
#include <ui/tk/util/LSPStyleTrigger.h>

// Widget basics
#include <ui/tk/basic/LSPWidget.h>
#include <ui/tk/basic/LSPComplexWidget.h>
#include <ui/tk/basic/LSPWidgetContainer.h>

// Basic widgets
#include <ui/tk/widgets/LSPVoid.h>
#include <ui/tk/widgets/LSPWindow.h>
#include <ui/tk/widgets/LSPBox.h>
#include <ui/tk/widgets/LSPGrid.h>
#include <ui/tk/widgets/LSPMenuItem.h>
#include <ui/tk/widgets/LSPMenu.h>
#include <ui/tk/widgets/LSPButton.h>
#include <ui/tk/widgets/LSPSwitch.h>
#include <ui/tk/widgets/LSPLabel.h>
#include <ui/tk/widgets/LSPHyperlink.h>
#include <ui/tk/widgets/LSPIndicator.h>
#include <ui/tk/widgets/LSPSeparator.h>
#include <ui/tk/widgets/LSPLed.h>
#include <ui/tk/widgets/LSPKnob.h>
#include <ui/tk/widgets/LSPMeter.h>
#include <ui/tk/widgets/LSPAlign.h>
#include <ui/tk/widgets/LSPGroup.h>
#include <ui/tk/widgets/LSPScrollBar.h>
#include <ui/tk/widgets/LSPFader.h>
#include <ui/tk/widgets/LSPListBox.h>
#include <ui/tk/widgets/LSPComboBox.h>
#include <ui/tk/widgets/LSPEdit.h>
#include <ui/tk/widgets/LSPGraph.h>
#include <ui/tk/widgets/LSPComboGroup.h>
#include <ui/tk/widgets/LSPProgressBar.h>
#include <ui/tk/widgets/LSPAudioSample.h>
#include <ui/tk/widgets/LSPScrollBox.h>

// Dialogs
#include <ui/tk/widgets/dialogs/LSPMessageBox.h>
#include <ui/tk/widgets/dialogs/LSPFileDialog.h>

// Advanced widgets
#include <ui/tk/widgets/LSPAudioFile.h>
#include <ui/tk/widgets/LSPMountStud.h>
#include <ui/tk/widgets/LSPSaveFile.h>
#include <ui/tk/widgets/LSPLoadFile.h>
#include <ui/tk/widgets/LSPFraction.h>

// Grapic widgets
#include <ui/tk/widgets/graph/LSPGraphItem.h>
#include <ui/tk/widgets/graph/LSPAxis.h>
#include <ui/tk/widgets/graph/LSPCenter.h>
#include <ui/tk/widgets/graph/LSPBasis.h>
#include <ui/tk/widgets/graph/LSPMarker.h>
#include <ui/tk/widgets/graph/LSPMesh.h>
#include <ui/tk/widgets/graph/LSPDot.h>
#include <ui/tk/widgets/graph/LSPText.h>
#include <ui/tk/widgets/graph/LSPFrameBuffer.h>

// 3D rendering
#include <ui/tk/widgets/LSPArea3D.h>
#include <ui/tk/widgets/3d/LSPObject3D.h>
#include <ui/tk/widgets/3d/LSPCapture3D.h>
#include <ui/tk/widgets/3d/LSPMesh3D.h>


#endif /* UI_TK_TK_H_ */
