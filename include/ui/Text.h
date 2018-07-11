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
        protected:
            typedef struct coord_t
            {
                float       fValue;
                IUIPort    *pPort;
            } coord_t;

        private:
            size_t          nCoords;
            coord_t        *vCoords;
//            float          *vCoords;
            char           *sText;
            ColorHolder     sColor;
            float           fHAlign;
            float           fVAlign;
            float           fSize;
            size_t          nCenter;

        protected:
            void        drop_coordinates();
            bool        read_coordinates(const char *value);

        public:
            Text(plugin_ui *ui);

            virtual ~Text();

        public:
            virtual void draw(IGraphCanvas *cv);

            virtual void set(widget_attribute_t att, const char *value);

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* UI_TEXT_H_ */
