/*
 * IGraph.h
 *
 *  Created on: 28 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_IGRAPH_H_
#define UI_IGRAPH_H_

namespace lsp
{
    class IGraphObject;
    class Axis;

    class IGraph
    {
        protected:
            size_t          nObjects;
            size_t          nCapacity;
            IGraphObject  **vObjects;

        public:
            IGraph();
            virtual ~IGraph();

            void            addItem(IGraphObject *object);
            inline size_t   items() const { return nObjects; };
            size_t          axises() const;
            size_t          basisAxises() const;

            IGraphObject   *item(size_t index) const;
            Axis           *axis(size_t index) const;
            Axis           *basisAxis(size_t index) const;

            size_t          getAxises(Axis **dst, size_t start, size_t count) const;
            size_t          getBasisAxises(Axis **dst, size_t start, size_t count) const;
            size_t          getItems(IGraphObject **dst, size_t start, size_t count) const;

            /** Queue graph for redraw
             *
             */
            virtual void    markRedraw();
    };

} /* namespace lsp */

#endif /* UI_IGRAPH_H_ */
