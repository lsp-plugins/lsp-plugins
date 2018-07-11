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
        bVisible    = true;
    }

    IGraphObject::~IGraphObject()
    {
    }

    void IGraphObject::draw(IGraphCanvas *cv)
    {
    }

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
        IWidget::notify(port);
        if (pGraph != NULL)
            pGraph->markRedraw();
    }

    size_t IGraphObject::button_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
        return EVF_NONE;
    }

    size_t IGraphObject::button_release(ssize_t x, ssize_t y, size_t state, size_t button)
    {
        return EVF_NONE;
    }

    size_t IGraphObject::motion(ssize_t x, ssize_t y, size_t state)
    {
        return EVF_NONE;
    }

    size_t IGraphObject::scroll(ssize_t x, ssize_t y, size_t state, size_t direction)
    {
        return EVF_NONE;
    }

} /* namespace lsp */
