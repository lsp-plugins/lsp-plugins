/*
 * IGraphObject.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>

namespace lsp
{
    IGraphObject::IGraphObject(plugin_ui *ui, widget_t w_class):
        IWidget(ui, w_class)
    {
        pGraph      = NULL;
    }

    IGraphObject::~IGraphObject()
    {
    }

    void IGraphObject::draw(IGraphCanvas *cv)
    {    }

    void IGraphObject::bind(IGraph *graph)
    {
        pGraph      = graph;
    }

    IGraph *IGraphObject::getGraph()
    {
        return pGraph;
    }

    void IGraphObject::notify(IUIPort *port)
    {
        if (pGraph != NULL)
            pGraph->markRedraw();
    }

} /* namespace lsp */
