/*
 * IGraphObject.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_IGRAPHOBJECT_H_
#define _UI_IGRAPHOBJECT_H_

namespace lsp
{
    class IGraphObject: public IWidget
    {
        protected:
            IGraph     *pGraph;

        public:
            IGraphObject(plugin_ui *ui);

            virtual ~IGraphObject();

        public:
            virtual void draw(IGraphCanvas *cv);

            virtual void bind(IGraph *graph);

            virtual IGraph *getGraph();

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* _UI_IGRAPHOBJECT_H_ */
