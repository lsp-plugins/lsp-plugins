/*
 * XMLParser.h
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_XMLPARSER_H_
#define UI_XMLPARSER_H_

#include <core/types.h>

#ifdef LSP_USE_EXPAT
    #include <expat.h>
#endif /* LSP_USE_EXPAT */

namespace lsp
{
    #ifdef LSP_USE_EXPAT
        typedef XML_Char            xml_char_t;
    #else
        typedef char                xml_char_t;
    #endif /* LSP_USE_EXPAT */

    class XMLParser
    {
        private:
            typedef struct node_t
            {
                XMLHandler *handler;
            #ifdef LSP_USE_EXPAT
                xml_char_t *tag;
            #else
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

#ifndef LSP_USE_EXPAT
            static const char *fetch_string(const char * &text);
#endif /* LSP_USE_EXPAT */
        private:
            bool push(const xml_char_t *tag, XMLHandler *handler);
            node_t  *pop();
            node_t  *top();

        public:
            XMLParser();
            ~XMLParser();

        public:
            bool parse(const char *path, XMLHandler *root);
    };
} /* namespace lsp */

#endif /* UI_XMLPARSER_H_ */
