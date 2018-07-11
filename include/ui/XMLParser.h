/*
 * XMLParser.h
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_XMLPARSER_H_
#define UI_XMLPARSER_H_

namespace lsp
{
    class XMLParser
    {
        private:
            typedef struct node_t
            {
                XMLHandler *handler;
                XML_Char   *tag;
            } node_t;

            size_t          nCapacity;
            size_t          nSize;
            node_t         *vStack;
            XMLHandler      hStub; // Stub handler

        private:
            static void startElementHandler(void *userData, const XML_Char *name, const XML_Char **atts);
            static void endElementHandler(void *userData, const XML_Char *name);

        private:
            bool push(const XML_Char *tag, XMLHandler *handler);
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
