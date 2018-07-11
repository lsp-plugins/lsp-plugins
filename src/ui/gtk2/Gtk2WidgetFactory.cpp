/*
 * Gtk2WidgetFactory.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    Gtk2WidgetFactory::Gtk2WidgetFactory(const char *path):
        IWidgetFactory(path)
    {
        pWidget = NULL;
    }

    Gtk2WidgetFactory::~Gtk2WidgetFactory()
    {
    }

    IWidget *Gtk2WidgetFactory::createWidget(plugin_ui *ui, widget_t w_class)
    {
        switch (w_class)
        {
            case W_PLUGIN:      return pWidget = new Gtk2Window(ui);
            case W_MSTUD:       return new Gtk2MountStud(ui);
            case W_BODY:        return new Gtk2Body(ui);
            case W_BUTTON:      return new Gtk2Button(ui);
            case W_VBOX:        return new Gtk2Box(ui, false);
            case W_HBOX:        return new Gtk2Box(ui, true);
            case W_GRID:        return new Gtk2Grid(ui);
            case W_CELL:        return new Gtk2Cell(ui);
            case W_LABEL:       return new Gtk2Label(ui, LT_TEXT);
            case W_PARAM:       return new Gtk2Label(ui, LT_PARAM);
            case W_VALUE:       return new Gtk2Label(ui, LT_VALUE);
            case W_INDICATOR:   return new Gtk2Indicator(ui);
            case W_LED:         return new Gtk2Led(ui);
            case W_KNOB:        return new Gtk2Knob(ui);
            case W_GROUP:       return new Gtk2Group(ui);
            case W_ALIGN:       return new Gtk2Align(ui);
            case W_SWITCH:      return new Gtk2Switch(ui);
            case W_GRAPH:       return new Gtk2Graph(ui);
            case W_COMBO:       return new Gtk2ComboBox(ui);
            case W_FILE:        return new Gtk2File(ui);
            case W_VSEP:        return new Gtk2VSeparator(ui);
            case W_HSEP:        return new Gtk2HSeparator(ui);
            case W_METER:       return new Gtk2Meter(ui);
            default:            return IWidgetFactory::createWidget(ui, w_class);
        }

        return NULL;
    }

    void *Gtk2WidgetFactory::root_widget()
    {
        return pWidget->widget();
    }

} /* namespace lsp */

