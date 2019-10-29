/*
 * OldXMLHandler.h
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_OLDXMLHANDLER_H_
#define UI_OLDXMLHANDLER_H_

namespace lsp
{
    class OldXMLHandler
    {
        public:
            virtual ~OldXMLHandler();

        protected:
            static const char *findAttribute(const char **atts, const char *name);

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
            virtual OldXMLHandler     *startElement(const char *name, const char **atts);

            /** Call on tag close
             *
             * @param name tag name
             */
            virtual void            endElement(const char *name);

            /** Called when there will be no more data
             *
             */
            virtual void quit();

            /** Called when child has been fully parsed
             *
             * @param child child that has been fully parsed
             */
            virtual void completed(OldXMLHandler *child);
    };

} /* namespace lsp */

#endif /* UI_OLDXMLHANDLER_H_ */
