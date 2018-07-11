/*
 * Text.h
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_TEXT_H_
#define UI_TEXT_H_

namespace lsp
{
    
    class Text: public IGraphObject
    {
        private:
            size_t          nCoords;
            float          *vCoords;
            char           *sText;
            Color           sColor;
            float           fHAlign;
            float           fVAlign;
            float           fSize;
            size_t          nCenter;

        protected:
            bool read_coordinates(const char *value);

        public:
            Text(plugin_ui *ui);

            virtual ~Text();

        public:
            virtual void draw(IGraphCanvas *cv);

            virtual void set(widget_attribute_t att, const char *value);
    };

} /* namespace lsp */

#endif /* UI_TEXT_H_ */
