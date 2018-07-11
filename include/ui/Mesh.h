/*
 * Mesh.h
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_MESH_H_
#define UI_MESH_H_

namespace lsp
{
    
    class Mesh: public IGraphObject
    {
        private:
            size_t          nBasis;
            size_t          nCapacity;
            ssize_t        *vBasis;
            IUIPort        *pPort;
            Color           sColor;
            size_t          nWidth;

        protected:
            typedef struct point_t
            {
                float x;
                float y;
            } point_t;

        public:
            Mesh(plugin_ui *ui);

            virtual ~Mesh();

        public:
            virtual void draw(IGraphCanvas *cv);

            virtual void set(widget_attribute_t att, const char *value);

            virtual void add(IWidget *widget);
    };

} /* namespace lsp */

#endif /* UI_MESH_H_ */
