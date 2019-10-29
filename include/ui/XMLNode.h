/*
 * XMLNode.h
 *
 *  Created on: 29 окт. 2019 г.
 *      Author: sadko
 */

#ifndef UI_XMLNODE_H_
#define UI_XMLNODE_H_

#include <core/LSPString.h>

namespace lsp
{
    
    class XMLNode
    {
        private:
            XMLNode & operator = (const XMLNode &);

        public:
            explicit XMLNode();
            virtual ~XMLNode();

        protected:
            static const LSPString *find_attribute(const LSPString * const *atts, const LSPString *name);
            static const LSPString *find_attribute(const LSPString * const *atts, const char *name);

        public:
            /** Called when XML handler is set
             *
             */
            virtual status_t        enter();

            /** Call on tag open
             *
             * @param child pointer to pass child node handler if necessary
             * @param name tag name
             * @param atts NULL-terminated list of attributes
             * @return handler of tag sub-structure or NULL
             */
            virtual status_t        start_element(XMLNode **child, const LSPString *name, const LSPString * const *atts);

            /** Call on tag close
             *
             * @param name tag name
             */
            virtual status_t        end_element(const LSPString *name);

            /** Called when there will be no more data
             *
             */
            virtual status_t        quit();

            /** Called when child has been fully parsed
             *
             * @param child child that has been fully parsed
             */
            virtual status_t        completed(XMLNode *child);
    };

} /* namespace lsp */

#endif /* UI_XMLNODE_H_ */
