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
        nObjects        = 0;
        nCapacity       = 0;
        vObjects        = NULL;
    }
    
    IGraph::~IGraph()
    {
        if (vObjects != NULL)
            delete  [] vObjects;
        nObjects        = 0;
        nCapacity       = 0;
    }

    void IGraph::addItem(IGraphObject *object)
    {
        if (nObjects >= nCapacity)
        {
            IGraphObject **alloc = new IGraphObject *[nCapacity + 16];
            if (alloc == NULL)
                return;

            // Copy objects
            for (size_t i=0; i<nObjects; ++i)
                alloc[i]    = vObjects[i];

            delete [] vObjects;
            vObjects        = alloc;
            nCapacity      += 16;
        }

        // Add to list
        vObjects[nObjects++]    = object;

        // Bind object
        object->bind(this);
    }

    size_t IGraph::axises() const
    {
        size_t count = 0;
        for (size_t i=0; i<nObjects; ++i)
            if (vObjects[i]->isAxis())
                count ++;
        return count;
    }

    size_t IGraph::basisAxises() const
    {
        size_t count = 0;
        for (size_t i=0; i<nObjects; ++i)
        {
            IGraphObject *obj = vObjects[i];
            if (obj->isAxis())
            {
                Axis *axis = static_cast<Axis *>(obj);
                if (axis->isBasis())
                    count ++;
            }
        }
        return count;
    }

    IGraphObject *IGraph::item(size_t index) const
    {
        return (index < nObjects) ? vObjects[index] : NULL;
    }

    Axis *IGraph::axis(size_t index) const
    {
        size_t count = 0;
        for (size_t i=0; i<nObjects; ++i)
        {
            IGraphObject *obj = vObjects[i];
            if (!obj->isAxis())
                continue;

            if ((count++) == index)
                return static_cast<Axis *>(obj);
        }
        return NULL;
    }

    Axis *IGraph::basisAxis(size_t index) const
    {
        size_t count = 0;
        for (size_t i=0; i<nObjects; ++i)
        {
            IGraphObject *obj = vObjects[i];
            if (!obj->isAxis())
                continue;

            Axis *axis = static_cast<Axis *>(obj);
            if (!axis->isBasis())
                continue;

            if ((count++) == index)
                return axis;
        }
        return NULL;
    }

    size_t IGraph::getAxises(Axis **dst, size_t start, size_t count) const
    {
        size_t i = 0, id = 0, n = 0;

        // Find first axis
        for ( ; (i < nObjects); ++i)
        {
            if (!vObjects[i]->isAxis())
                continue;

            if ((id++) == start)
                break;
        }

        // Now find other axises
        for (; (i < nObjects) && (count > 0); ++i)
        {
            IGraphObject *obj = vObjects[i];
            if (!obj->isAxis())
                continue;

            Axis *axis = static_cast<Axis *>(obj);
            *(dst++)    = axis;
            n           ++;
            count       --;
        }

        return n;
    }

    size_t IGraph::getBasisAxises(Axis **dst, size_t start, size_t count) const
    {
        size_t i = 0, id = 0, n = 0;

        // Find first axis
        for ( ; (i < nObjects); ++i)
        {
            if (!vObjects[i]->isAxis())
                continue;
            if ((id++) == start)
                break;
        }

        // Now find other axises
        for (; (i < nObjects) && (count > 0); ++i)
        {
            IGraphObject *obj = vObjects[i];
            if (!obj->isAxis())
                continue;

            Axis *axis = static_cast<Axis *>(obj);
            if (!axis->isBasis())
                continue;

            *(dst++)    = axis;
            n           ++;
            count       --;
        }

        return n;
    }

    size_t IGraph::getItems(IGraphObject **dst, size_t start, size_t count) const
    {
        size_t n = 0;
        for (size_t i=start; (i < nObjects) && (count > 0); ++i, ++n, --count)
            *(dst++) = vObjects[i];

        return n;
    }

    Center *IGraph::center(size_t index) const
    {
        size_t count = 0;
        for (size_t i=0; i<nObjects; ++i)
        {
            IGraphObject *obj = vObjects[i];
            if (!obj->isCenter())
                continue;

            if ((count++) == index)
                return static_cast<Center *>(obj);
        }
        return NULL;
    }

    IGraphCanvas *IGraph::canvas()
    {
        return NULL;
    }

    void IGraph::markRedraw()
    {
    }

} /* namespace lsp */
