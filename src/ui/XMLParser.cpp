/*
 * XMLParser.cpp
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <core/alloc.h>
#include <core/resource.h>

#include <string.h>

#if defined(LSP_BUILTIN_RESOURCES)
    #define XML_CLOSE_TAG       0xff
#endif /* LSP_XML_BUILTIN */

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
#if defined(LSP_USE_EXPAT)
        if (node->tag != NULL)
            lsp_free(node->tag);
#endif /* LSP_USE_EXPAT */

        node->tag       = NULL;
    }

    bool XMLParser::init_node(node_t *node, const char *tag, OldXMLHandler *handler)
    {
#if defined(LSP_USE_EXPAT)
        if (tag != NULL)
        {
            node->tag           = lsp_strdup(tag);
            if (node->tag == NULL)
                return false;
        }
        else
            node->tag           = NULL;
#elif defined(LSP_BUILTIN_RESOURCES)
        node->tag           = tag;
#endif /* LSP_BUILTIN_RESOURCES */

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

        OldXMLHandler *handler = (top->handler != NULL) ? top->handler->startElement(name, atts) : NULL;
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

    bool XMLParser::push(const xml_char_t *tag, OldXMLHandler *handler)
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

    bool XMLParser::parse(const LSPString *path, OldXMLHandler *root)
    {
        if (!push(NULL, root))
            return false;

#ifndef LSP_BUILTIN_RESOURCES
        XML_Parser parser = XML_ParserCreate(NULL);
        XML_SetUserData(parser, this);
        XML_SetElementHandler(parser, startElementHandler, endElementHandler);

        FILE *fd = fopen(path->get_native(), "r");
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
#elif defined(LSP_BUILTIN_RESOURCES)
        // Obtain resource
        const resource_t *res = resource_get(path->get_utf8(), RESOURCE_XML);
        if (res == NULL)
            return false;
        lsp_trace("Resource id=%s, type=%d, data=%p", res->id, int(res->type), res->data);

        // Process data
        const void *data = res->data;

        root->enter();

        ssize_t level = 0;
        do
        {
            uint8_t token = resource_fetch_byte(&data);
            lsp_trace("token = 0x%02x", int(token));

            if (token != XML_CLOSE_TAG)
            {
                // Increment level
                level   ++;
                size_t elements = token;

                // Get tag name
                const char *tag = resource_fetch_dstring(&data);
                lsp_trace("tag = %s", tag);

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
                    *(dst++)    = resource_fetch_dstring(&data);
//                    lsp_trace("  att = %s", dst[-1]);
                    *(dst++)    = resource_fetch_dstring(&data);
//                    lsp_trace("  value = %s", dst[-2]);
                }
                *(dst++)     = NULL;
                *(dst++)     = NULL;

                // Now we are ready to parse tag
                startElementHandler(this, tag, attributes);

                // Finally, delete all attributes
                delete [] attributes;
            }
            else
            {
                endElementHandler(this, NULL); // Tag name is not used
                level--;
            }
        }
        while (level > 0);

        root->quit();

        return true;
#endif /* LSP_BUILTIN_RESOURCES */
    }

} /* namespace lsp */
