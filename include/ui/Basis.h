/*
 * Basis.h
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_BASIS_H_
#define UI_BASIS_H_

namespace lsp
{
    class Basis: public IWidget
    {
        private:
            ssize_t         nID;

        public:
            Basis(plugin_ui *ui);

            virtual ~Basis();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            inline ssize_t getID() { return nID; }
    };

} /* namespace lsp */

#endif /* UI_BASIS_H_ */
