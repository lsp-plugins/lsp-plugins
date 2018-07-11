/*
 * XMLParser.cpp
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <core/alloc.h>

#include <string.h>

#ifndef LSP_USE_EXPAT
    #define XML_OPEN_TAG        '\x55'
    #define XML_CLOSE_TAG       '\xaa'
    #define XML_END_DOCUMENT    '\0'
#endif /* LSP_USE_EXPAT */

namespace lsp
{
#ifndef LSP_USE_EXPAT
    extern const resource_t xml_resources[];

//    const resource_t xml_resources[] = { NULL, NULL };
#endif /* LSP_USE_EXPAT */


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
            for (size_t i=0; i<nSize; ++i)
                free_node(&vStack[i]);

            delete [] vStack;
            vStack = NULL;
        }
    }

    void XMLParser::free_node(node_t *node)
    {
        if (node == NULL)
            return;
#ifdef LSP_USE_EXPAT
        if (node->tag != NULL)
            lsp_free(node->tag);
#endif /* LSP_USE_EXPAT */

        node->tag       = NULL;
    }

    bool XMLParser::init_node(node_t *node, const char *tag, XMLHandler *handler)
    {
#ifdef LSP_USE_EXPAT
        if (tag != NULL)
        {
            node->tag           = lsp_strdup(tag);
            if (node->tag == NULL)
                return false;
        }
        else
            node->tag           = NULL;
#else
        node->tag           = tag;
#endif /* LSP_USE_EXPAT */

        node->handler       = handler;
        return true;
    }

    void XMLParser::startElementHandler(void *userData, const xml_char_t *name, const xml_char_t **atts)
    {
        XMLParser *_this    = reinterpret_cast<XMLParser *>(userData);
        _this->startElement(name, atts);
    }

    void XMLParser::endElementHandler(void *userData, const xml_char_t *tag)
    {
        XMLParser *_this    = reinterpret_cast<XMLParser *>(userData);
        _this->endElement(tag);
    }

    void XMLParser::startElement(const xml_char_t *name, const xml_char_t **atts)
    {
        node_t *top         = this->top();

        XMLHandler *handler = (top->handler != NULL) ? top->handler->startElement(name, atts) : NULL;
        if (handler != NULL)
            handler->enter();

        push(name, handler);
    }

    void XMLParser::endElement(const xml_char_t *)
    {
        node_t *node        = this->pop();
        node_t *top         = this->top();

        // Call callbacks
        if (node->handler != NULL)
            node->handler->quit();
        if (top->handler != NULL)
        {
            top->handler->completed(node->handler);
            top->handler->endElement(node->tag);
        }

        // Free memory
        free_node(node);
    }

#ifndef LSP_USE_EXPAT
    const char *XMLParser::fetch_string(const char * &text)
    {
        const char *result = text;
        text += strlen(text) + 1;
        return result;
    }
#endif /* LSP_USE_EXPAT */

    bool XMLParser::push(const xml_char_t *tag, XMLHandler *handler)
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

        if (!init_node(&vStack[nSize], tag, handler))
            return false;
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

#ifdef LSP_USE_EXPAT
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

        // Free parser and close file
        XML_ParserFree(parser);
        fclose(fd);
        return true;
#else
        for (const resource_t *res = xml_resources; (res->id != NULL) && (res->text != NULL); ++res)
        {
            // Check that resource matched
            if (strcmp(res->id, path) != 0)
                continue;

            // Process data
            const char *text = res->text;

            root->enter();

            bool last = false;
            while (!last)
            {
                char code = *(text++);
                switch (code)
                {
                    case XML_OPEN_TAG:
                    {
                        // Determine number of elements
                        size_t elements = *(text++);

                        // Get tag name
                        const char *tag = fetch_string(text);

                        // Allocate list of attributes
                        const char **attributes = new const char *[(elements + 1) * 2];
                        if (attributes == NULL)
                        {
                            lsp_error("Not enough memory");
                            return false;
                        }

                        // Fill list with attributes
                        const char **dst = attributes;
                        for (size_t i=0; i<elements; ++i)
                        {
                            *(dst++)    = fetch_string(text);
                            *(dst++)    = fetch_string(text);
                        }
                        *(dst++)     = NULL;
                        *(dst++)     = NULL;

                        // Now we are ready to parse tag
                        startElementHandler(this, tag, attributes);

                        // Finally, delete all attributes
                        delete [] attributes;

                        break;
                    }
                    case XML_CLOSE_TAG:
                    {
                        endElementHandler(this, NULL); // Tag name is not used
                        break;
                    }
                    case XML_END_DOCUMENT:
                        last = true;
                        break;
                    default:
                        lsp_error("Document structure corrupted: unknown code: 0x%x", int(code));
                        return false;
                }
            }

            root->quit();

            return true;
        }

        return false;
#endif /* LSP_USE_EXPAT */
    }

} /* namespace lsp */
