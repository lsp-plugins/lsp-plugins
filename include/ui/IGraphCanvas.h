/*
 * IGraphCanvas.h
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef IGRAPHCANVAS_H_
#define IGRAPHCANVAS_H_

namespace lsp
{
    
    class IGraphCanvas
    {
        protected:
            size_t          nWidth;
            size_t          nHeight;
            ssize_t         nLeft;
            ssize_t         nTop;
            Color           sColor;

        public:
            IGraphCanvas(ssize_t width, ssize_t height);
            virtual ~IGraphCanvas();

        public:
            inline size_t  width() const    { return nWidth;            }
            inline size_t  height() const   { return nHeight;           }
            inline ssize_t left() const     { return nLeft;             }
            inline ssize_t right() const    { return nWidth + nLeft;    }
            inline ssize_t top() const      { return nTop;              }
            inline ssize_t bottom() const   { return nTop - nHeight;    }

            virtual void line(ssize_t x1, ssize_t y1, ssize_t x2, ssize_t y2);

            virtual void set_line_width(size_t width);

            virtual void move_to(ssize_t x, ssize_t y);

            virtual void line_to(ssize_t x, ssize_t y);

            virtual void stroke();

            virtual void set_color(const Color &c);

            virtual void clear();
    };

} /* namespace lsp */

#endif /* IGRAPHCANVAS_H_ */
