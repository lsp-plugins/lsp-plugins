/*
 * XMLHandler.h
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_XMLHANDLER_H_
#define UI_XMLHANDLER_H_

#include <expat.h>

namespace lsp
{
    class XMLHandler
    {
        public:
            virtual ~XMLHandler();

        protected:
            static const XML_Char *findAttribute(const XML_Char **atts, const XML_Char *name);

        public:
            /** Called when XML handler is set
             *
             */
            virtual void enter();

            /** Call on tag open
             *
             * @param name tag name
             * @param atts tag attributes
             * @return handler of tag sub-structure or NULL
             */
            virtual XMLHandler     *startElement(const XML_Char *name, const XML_Char **atts);

            /** Call on tag close
             *
             * @param name tag name
             */
            virtual void            endElement(const XML_Char *name);

            /** Called when there will be no more data
             *
             */
            virtual void quit();

            /** Called when child has been fully parsed
             *
             * @param child child that has been fully parsed
             */
            virtual void completed(XMLHandler *child);
    };

} /* namespace lsp */

#endif /* UI_XMLHANDLER_H_ */
