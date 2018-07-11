/*
 * Center.h
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_CENTER_H_
#define UI_CENTER_H_

namespace lsp
{
    
    class Center: public IGraphObject
    {
        private:
            float           fLeft;
            float           fTop;
            Color           sColor;

        protected:
            void update();

        public:
            Center(plugin_ui *ui);

            virtual ~Center();

        public:
            virtual void draw(IGraphCanvas *cv);

            virtual void set(widget_attribute_t att, const char *value);

        public:
            inline float left() const   { return fLeft; };
            inline float top() const    { return fTop;  };
    };

} /* namespace lsp */

#endif /* UI_CENTER_H_ */
