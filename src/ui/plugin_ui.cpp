/*
 * pluginui.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <ui/ui.h>

#include <string.h>

namespace lsp
{
    plugin_ui::plugin_ui(const char *name, const plugin_metadata_t *mdata, IWidgetFactory *factory)
    {
        sName           = name;
        pMetadata       = mdata;
        pFactory        = factory;
    }

    plugin_ui::~plugin_ui()
    {
        destroy();
    }

    void plugin_ui::init()
    {
    }


    void plugin_ui::build()
    {
        // Generate path to theme
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/ui/theme.xml", pFactory->path());
        lsp_trace("Loading theme from file %s", path);

        // Load theme
        if (sTheme.load(path))
        {
            // Generate path to UI schema
            snprintf(path, PATH_MAX, "%s/ui/%s.xml", pFactory->path(), sName);
            lsp_trace("Generating UI from file %s", path);

            // Build UI
            ui_builder builder(this);
            if (!builder.build(path))
                lsp_error("Could not build UI from file %s", path);
        }
    }

    void plugin_ui::destroy()
    {
        for (size_t i=0; i<vWidgets.size(); ++i)
            delete vWidgets[i];
        vWidgets.clear();

        if (pFactory != NULL)
        {
            delete pFactory;
            pFactory = NULL;
        }

        for (size_t i=0; i<vIntPorts.size(); ++i)
        {
            if (vIntPorts[i] != NULL)
                delete vIntPorts[i];
        }
        vIntPorts.clear();
        vExtPorts.clear();
    }

    bool plugin_ui::add_port(IUIPort *port, bool external)
    {
        if (!vIntPorts.add(port))
            return false;

        if (external)
        {
            if (!vExtPorts.add(port))
                return false;
        }
        lsp_trace("added port external=%d", int(external));
        return true;
    }

    IUIPort *plugin_ui::port(size_t id, bool external)
    {
        return (external) ? vExtPorts[id] : vIntPorts[id];
    }

    IUIPort *plugin_ui::port(const char *name, bool external)
    {
        cvector<IUIPort> &v = (external) ? vExtPorts : vIntPorts;

        for (size_t i=0; i<v.size(); ++i)
        {
            IUIPort *p  = v[i];
            const port_t *ctl = p->metadata();
            if (ctl == NULL)
                continue;

            if (!strcmp(ctl->id, name))
                return p;
        }

        return NULL;
    }

    IWidget *plugin_ui::createWidget(widget_t w_class)
    {
        IWidget *widget = pFactory->createWidget(this, w_class);
        return (widget != NULL) ? addWidget(widget) : widget;
    }

    IWidget *plugin_ui::createWidget(const char *w_class)
    {
        IWidget *widget = pFactory->createWidget(this, w_class);
        return (widget != NULL) ? addWidget(widget) : widget;
    }

    IWidget *plugin_ui::addWidget(IWidget *widget)
    {
        return (vWidgets.add(widget)) ? widget : NULL;
    }
} /* namespace lsp */
