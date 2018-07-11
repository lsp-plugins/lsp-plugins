/*
 * IWidgetFactory.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <string.h>
#include <stdlib.h>

namespace lsp
{
    IWidgetFactory::IWidgetFactory(const char *path)
    {
        pPath           = strdup(path);
    }

    IWidgetFactory::~IWidgetFactory()
    {
        free(pPath);
    }

    IWidget *IWidgetFactory::createWidget(plugin_ui *ui, const char *w_class)
    {
        widget_t type = widget_type(w_class);
        if (type != W_UNKNOWN)
            return createWidget(ui, type);

        return NULL;
    }

    IWidget *IWidgetFactory::createWidget(plugin_ui *ui, widget_t w_class)
    {
        switch (w_class)
        {
            case W_AXIS:        return new Axis(ui);
            case W_MARKER:      return new Marker(ui);
            case W_MESH:        return new Mesh(ui);
            case W_BASIS:       return new Basis(ui);
            default:            return NULL;
        }
    }

    void *IWidgetFactory::root_widget()
    {
        return NULL;
    }
} /* namespace lsp */

