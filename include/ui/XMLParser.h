/*
 * XMLParser.h
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_XMLPARSER_H_
#define UI_XMLPARSER_H_

#include <core/types.h>

#if defined(LSP_USE_EXPAT)
    #include <expat.h>
#elif defined(LSP_USE_MSXML)
    #include <msxml.h>
#endif /* LSP_USE_EXPAT */

namespace lsp
{
    #if defined(LSP_USE_EXPAT)
        typedef XML_Char            xml_char_t;
    #else
        typedef char                xml_char_t;
    #endif /* LSP_USE_EXPAT */

    class XMLParser
    {
        private:
            XMLParser & operator = (const XMLParser &);

        private:
            typedef struct node_t
            {
                XMLHandler *handler;
            #if defined(LSP_USE_EXPAT)
                xml_char_t *tag;
            #elif defined(LSP_USE_MSXML)
                // TODO
            #elif defined(LSP_XML_BUILTIN)
                const char *tag;
            #endif /* LSP_USE_EXPAT */
            } node_t;

            size_t          nCapacity;
            size_t          nSize;
            node_t         *vStack;
            XMLHandler      hStub; // Stub handler

        private:
            static void free_node(node_t *node);
            bool init_node(node_t *node, const char *tag, XMLHandler *handler);

            static void startElementHandler(void *userData, const xml_char_t *name, const xml_char_t **atts);
            static void endElementHandler(void *userData, const xml_char_t *);

#if defined(LSP_XML_BUILTIN)
            static const char *fetch_string(const char * &text);
#endif /* LSP_XML_BUILTIN */
        private:
            bool push(const xml_char_t *tag, XMLHandler *handler);
            node_t  *pop();
            node_t  *top();

        public:
            explicit XMLParser();
            ~XMLParser();

        public:
            /** Parse XML document
             *
             * @param path path to XML document
             * @param root root handler
             * @return status of operation
             */
            bool    parse(const char *path, XMLHandler *root);

            /** Send start element event
             *
             * @param name start element name
             * @param atts start element attributes
             */
            void startElement(const xml_char_t *name, const xml_char_t **atts);

            /** Send end element event
             *
             * @param name end element name
             */
            void endElement(const xml_char_t *name);
    };
} /* namespace lsp */

#endif /* UI_XMLPARSER_H_ */
