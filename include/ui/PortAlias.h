/*
 * PortAlias.hpp
 *
 *  Created on: 20 апр. 2016 г.
 *      Author: sadko
 */

#ifndef PORTALIAS_HPP_
#define PORTALIAS_HPP_

namespace lsp
{
    class PortAlias: public IWidget
    {
        private:
            char       *sID;
            char       *sAlias;

        public:
            PortAlias(plugin_ui *ui);
            virtual ~PortAlias();

        public:
            virtual void set(widget_attribute_t att, const char *value);

        public:
            inline const char *id() const       { return sID; }
            inline const char *alias() const    { return sAlias; }
    };

} /* namespace lsp */

#endif /* PORTALIAS_HPP_ */
