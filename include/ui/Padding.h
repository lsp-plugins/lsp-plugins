/*
 * Padding.hpp
 *
 *  Created on: 30 мая 2016 г.
 *      Author: sadko
 */

#ifndef UI_PADDING_H_
#define UI_PADDING_H_

namespace lsp
{
    
    class Padding
    {
        private:
            size_t  nLeft;
            size_t  nRight;
            size_t  nTop;
            size_t  nBottom;

        public:
            inline Padding()
            {
                nLeft       = 0;
                nRight      = 0;
                nTop        = 0;
                nBottom     = 0;
            }

            inline size_t left() const      { return nLeft;     }
            inline size_t right() const     { return nRight;    }
            inline size_t top() const       { return nTop;      }
            inline size_t bottom() const    { return nBottom;   }

            inline size_t left(size_t value)    { return nLeft = value;     }
            inline size_t right(size_t value)   { return nRight = value;    }
            inline size_t top(size_t value)     { return nTop = value;      }
            inline size_t bottom(size_t value)  { return nBottom = value;   }

            bool set(widget_attribute_t att, const char *value);
    };

} /* namespace lsp */

#endif /* UI_PADDING_HPP_ */
