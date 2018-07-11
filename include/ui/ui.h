/*
 * ui.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_UI_H_
#define _UI_UI_H_

#define LSP_UI_SIDE

#include <core/types.h>

#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

namespace lsp
{
    class IWidget;
    class IWidgetFactory;

    class IUIPort;

    class plugin_ui;
    class ui_builder;

}

// Data parsing
#define PARSE_INT(var, code) \
    { \
        errno = 0; \
        long __ = strtoll(var, NULL, 10); \
        if (errno == 0) \
            { code; } \
    }

#define PARSE_BOOL(var, code) \
    { \
        bool __ = !strcasecmp(var, "true"); \
        if (! __ ) \
            __ = !strcasecmp(var, "1"); \
        { code; } \
    }

#define PARSE_FLAG(var, dst, flag) PARSE_BOOL(var, if (__) dst |= flag; else dst &= ~flag)

#define PARSE_FLOAT(var, code) \
    { \
        char *saved_locale = setlocale(LC_NUMERIC, "C"); \
        errno = 0; \
        float __ = strtof(var, NULL); \
        if (errno == 0) \
            { code; } \
        setlocale(LC_NUMERIC, saved_locale); \
    }

#define PARSE_DOUBLE(var, code) \
    { \
        char *saved_locale = setlocale(LC_NUMERIC, "C"); \
        errno = 0; \
        double __ = strtod(var, NULL); \
        if (errno == 0) \
            { code; } \
        setlocale(LC_NUMERIC, saved_locale); \
    }

#define BIND_PORT(ui, field, id) \
    field   = ui->port(id); \
    if (field != NULL) \
        field->bind(this);

// Misc data structures
#include <data/cvector.h>

// Core
#include <core/types.h>
#include <core/debug.h>

// Metadata
#include <metadata/metadata.h>

// Configuration parsing
#include <ui/XMLHandler.h>
#include <ui/XMLParser.h>

// Common definitions
#include <ui/io.h>
#include <ui/common.h>

// Colors and themes
#include <ui/Color.h>
#include <ui/Theme.h>
#include <ui/Padding.h>

// Control interface
#include <ui/IUIPort.h>

// Widgets
#include <ui/IWidget.h>
#include <ui/IWidgetFactory.h>
#include <ui/IGraph.h>
#include <ui/IGraphCanvas.h>
#include <ui/IGraphObject.h>
#include <ui/Center.h>
#include <ui/Dot.h>
#include <ui/Axis.h>
#include <ui/Marker.h>
#include <ui/Mesh.h>
#include <ui/Basis.h>
#include <ui/Text.h>
#include <ui/PortAlias.h>

// Plugin UI
#include <ui/plugin_ui.h>
#include <ui/ui_builder.h>

#endif /* _UI_UI_H_ */
