/*
 * IGraph.cpp
 *
 *  Created on: 28 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>

namespace lsp
{
    IGraph::IGraph()
    {
    }
    
    IGraph::~IGraph()
    {
        vObjects.clear();
        vAxises.clear();
        vBasises.clear();
        vCenters.clear();
    }

    void IGraph::addItem(IGraphObject *obj)
    {
        // Check object class
        if (obj->isAxis())
        {
            Axis *axis = static_cast<Axis *>(obj);
            vAxises.add(axis);
            if (axis->isBasis())
                vBasises.add(axis);
        }
        if (obj->isCenter())
        {
            Center *center = static_cast<Center *>(obj);
            vCenters.add(center);
        }

        // Add to list of all objects
        vObjects.add(obj);

        // Bind object
        obj->bind(this);
    }

    size_t IGraph::getAxises(Axis **dst, size_t start, size_t count)
    {
        size_t n_count = vAxises.size();

        for (size_t i=0; i<count; ++i)
        {
            if (start >= n_count)
                return i;
            dst[i]      = vAxises.at(start++);
        }

        return count;
    }

    size_t IGraph::getBasisAxises(Axis **dst, size_t start, size_t count)
    {
        size_t n_count = vBasises.size();

        for (size_t i=0; i<count; ++i)
        {
            if (start >= n_count)
                return i;
            dst[i]      = vBasises.at(start++);
        }

        return count;
    }

    size_t IGraph::getItems(IGraphObject **dst, size_t start, size_t count)
    {
        size_t n_count = vObjects.size();

        for (size_t i=0; i<count; ++i)
        {
            if (start >= n_count)
                return i;
            dst[i]      = vObjects.at(start++);
        }

        return count;
    }

    IGraphCanvas *IGraph::canvas()
    {
        return NULL;
    }

    void IGraph::markRedraw()
    {
    }

} /* namespace lsp */
