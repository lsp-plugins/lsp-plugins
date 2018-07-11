/*
 * IGraph.h
 *
 *  Created on: 28 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_IGRAPH_H_
#define UI_IGRAPH_H_

#include <data/cvector.h>

namespace lsp
{
    class IGraphObject;
    class IGraphCanvas;
    class Axis;
    class Center;

    class IGraph
    {
        protected:
            cvector<IGraphObject>   vObjects;
            cvector<Axis>           vAxises;
            cvector<Axis>           vBasises;
            cvector<Center>         vCenters;
//            size_t          nObjects;
//            size_t          nCapacity;
//            IGraphObject  **vObjects;

        public:
            IGraph();
            virtual ~IGraph();

            void            addItem(IGraphObject *obj);
            inline size_t   items() const { return vObjects.size(); };
            size_t          axises() const { return vAxises.size(); };
            size_t          basisAxises() const { return vBasises.size(); };
            size_t          centers() const { return vCenters.size(); };

            inline IGraphObject   *item(size_t index) { return vObjects[index]; };
            Axis           *axis(size_t index) { return vAxises[index]; };
            Axis           *basisAxis(size_t index) { return vBasises[index]; };

            Center         *center(size_t index) { return vCenters[index]; };

            size_t          getAxises(Axis **dst, size_t start, size_t count);
            size_t          getBasisAxises(Axis **dst, size_t start, size_t count);
            size_t          getItems(IGraphObject **dst, size_t start, size_t count);

            /** Queue graph for redraw
             *
             */
            virtual void            markRedraw();
            virtual IGraphCanvas   *canvas();
    };

} /* namespace lsp */

#endif /* UI_IGRAPH_H_ */
