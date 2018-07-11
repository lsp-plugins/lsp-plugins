/*
 * XMLParser.cpp
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <string.h>

namespace lsp
{
    XMLParser::XMLParser()
    {
        nCapacity   = 0;
        nSize       = 0;
        vStack      = NULL;
    }

    XMLParser::~XMLParser()
    {
        if (vStack != NULL)
        {
            node_t *node    = vStack;
            for (size_t i=0; i<nSize; ++i, ++node)
                if (node->tag != NULL)
                {
                    free(node->tag);
                    node->tag   = NULL;
                }

            delete [] vStack;
            vStack = NULL;
        }
    }

    void XMLParser::startElementHandler(void *userData, const XML_Char *name, const XML_Char **atts)
    {
        XMLParser *_this    = reinterpret_cast<XMLParser *>(userData);
        node_t *top         = _this->top();

        XMLHandler *handler = (top->handler != NULL) ? top->handler->startElement(name, atts) : NULL;
        if (handler != NULL)
            handler->enter();

        _this->push(name, handler);
    }

    void XMLParser::endElementHandler(void *userData, const XML_Char *name)
    {
        XMLParser *_this    = reinterpret_cast<XMLParser *>(userData);
        node_t *node        = _this->pop();
        node_t *top         = _this->top();

        // Call callbacks
        if (node->handler != NULL)
            node->handler->quit();
        if (top->handler != NULL)
        {
            top->handler->completed(node->handler);
            top->handler->endElement(node->tag);
        }

        // Free memory
        free(node->tag);
        node->tag       = NULL;
    }

    bool XMLParser::push(const XML_Char *tag, XMLHandler *handler)
    {
        if (nSize >= nCapacity)
        {
            node_t *new_stack  = new node_t[nCapacity + 32];
            if (new_stack == NULL)
                return false;

            for (size_t i=0; i<nSize; ++i)
                new_stack[i]        = vStack[i];

            if (vStack != NULL)
                delete [] vStack;
            vStack              = new_stack;
            nCapacity          += 32;
        }

        node_t *node        = &vStack[nSize];
        if (tag != NULL)
        {
            node->tag           = strdup(tag);
            if (node->tag == NULL)
                return false;
        }
        else
            node->tag           = NULL;

        node->handler       = handler;
        nSize               ++;
        return true;
    }

    XMLParser::node_t *XMLParser::pop()
    {
        return (nSize > 0) ? &vStack[--nSize] : NULL;
    }

    XMLParser::node_t *XMLParser::top()
    {
        return (nSize > 0) ? &vStack[nSize-1] : NULL;
    }

    bool XMLParser::parse(const char *path, XMLHandler *root)
    {
        if (!push(NULL, root))
            return false;

        XML_Parser parser = XML_ParserCreate(NULL);
        XML_SetUserData(parser, this);
        XML_SetElementHandler(parser, startElementHandler, endElementHandler);

        FILE *fd = fopen(path, "r");
        if (fd == NULL)
        {
            XML_ParserFree(parser);
            return false;
        }

        root->enter();

        char buf[4096];
        while (true)
        {
            size_t to_read  = sizeof(buf)/sizeof(char);
            size_t count    = fread(buf, sizeof(char), to_read, fd);
            bool final      = count < to_read;

            if (!XML_Parse(parser, buf, count, final))
            {
                XML_Error code = XML_GetErrorCode(parser);
                lsp_error("XML parsing error: %s", XML_ErrorString(code));
                XML_ParserFree(parser);
                return false;
            }

            if (final)
                break;
        }

        root->quit();

        XML_ParserFree(parser);
        return true;
    }

} /* namespace lsp */
