/*
 * Marker.h
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_MARKER_H_
#define UI_MARKER_H_

namespace lsp
{
    
    class Marker: public IGraphObject
    {
        private:
            size_t          nBasisID;
            size_t          nParallelID;
            float           fValue;
            float           fOffset;
            float           fAngle;
            bool            bSmooth;
            IUIPort        *pPort;
            size_t          nWidth;
            size_t          nCenter;
            ColorHolder     sColor;

        public:
            Marker(plugin_ui *ui);

            virtual ~Marker();

        public:
            virtual void draw(IGraphCanvas *cv);

            virtual void set(widget_attribute_t att, const char *value);

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* UI_MARKER_H_ */
