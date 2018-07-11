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
            IGraphObject(plugin_ui *ui, widget_t w_class);

            virtual ~IGraphObject();

        public:
            virtual void draw(IGraphCanvas *cv);

            virtual void bind(IGraph *graph);

            virtual IGraph *getGraph();

            virtual void notify(IUIPort *port);

        public:
            // Events
            virtual size_t button_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual size_t button_release(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual size_t motion(ssize_t x, ssize_t y, size_t state);

            virtual size_t scroll(ssize_t x, ssize_t y, size_t state, size_t direction);

        public:
            inline bool isAxis()        { return getClass() == W_AXIS;      };
            inline bool isMarker()      { return getClass() == W_MARKER;    };
            inline bool isMesh()        { return getClass() == W_MESH;      };
            inline bool isCenter()      { return getClass() == W_CENTER;    };
            inline bool isDot()         { return getClass() == W_CENTER;    };
    };

} /* namespace lsp */

#endif /* _UI_IGRAPHOBJECT_H_ */
