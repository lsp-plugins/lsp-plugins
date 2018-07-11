/*
 * Dot.h
 *
 *  Created on: 18 июл. 2016 г.
 *      Author: sadko
 */

#ifndef UI_DOT_H_
#define UI_DOT_H_

namespace lsp
{
    
    class Dot: public IGraphObject
    {
        protected:
            enum flags_t
            {
                F_EDITABLE      = 1 << 0,
                F_HIGHLIGHT     = 1 << 1,
                F_EDITING       = 1 << 2,
                F_FINE_TUNE     = 1 << 3
            };

        protected:
            IUIPort        *pLeft;
            IUIPort        *pTop;
            IUIPort        *pScroll;

            float           fLeft;
            float           fTop;
            float           fScroll;
            ColorHolder     sColor;
            size_t          nCenter;
            size_t          nFlags;
            size_t          nBasisID;
            size_t          nParallelID;
            ssize_t         nRealX;
            ssize_t         nRealY;
            float           fLastLeft;
            float           fLastTop;
            ssize_t         nMouseX;
            ssize_t         nMouseY;
            size_t          nMouseBtn;
            size_t          nBorder;
            size_t          nPadding;
            size_t          nSize;

        protected:
            void update();

            bool mouse_over(ssize_t x, ssize_t y, size_t &flags);
            void apply_motion(ssize_t dx, ssize_t dy);

        public:
            Dot(plugin_ui *ui);

            virtual ~Dot();

        public:
            virtual void draw(IGraphCanvas *cv);

            virtual void set(widget_attribute_t att, const char *value);

            virtual void notify(IUIPort *port);

            virtual void end();

            virtual size_t button_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual size_t button_release(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual size_t motion(ssize_t x, ssize_t y, size_t state);

            virtual size_t scroll(ssize_t x, ssize_t y, size_t state, size_t direction);
    };

} /* namespace lsp */

#endif /* UI_DOT_H_ */
