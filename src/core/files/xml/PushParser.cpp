/*
 * PushParser.cpp
 *
 *  Created on: 29 окт. 2019 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/files/xml/PushParser.h>

namespace lsp
{
    namespace xml
    {
        
        PushParser::PushParser()
        {
        }
        
        PushParser::~PushParser()
        {
        }

        void PushParser::drop_list(cvector<LSPString> *list)
        {
            for (size_t i=0, n=list->size(); i<n; ++i)
            {
                LSPString *item = list->at(i);
                if (item != NULL)
                    delete item;
            }
            list->clear();
        }

        status_t PushParser::parse_file(IXMLHandler *handler, const char *path, const char *charset)
        {
            IXMLHandler stub;
            status_t res = sParser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_file(IXMLHandler *handler, const LSPString *path, const char *charset)
        {
            IXMLHandler stub;
            status_t res = sParser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_file(IXMLHandler *handler, const io::Path *path, const char *charset)
        {
            IXMLHandler stub;
            status_t res = sParser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IXMLHandler *handler, io::IInStream *is, size_t flags, const char *charset)
        {
            IXMLHandler stub;
            status_t res = sParser.wrap(is, flags, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IXMLHandler *handler, const char *str, const char *charset)
        {
            IXMLHandler stub;
            status_t res = sParser.wrap(str, charset);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IXMLHandler *handler, const LSPString *str)
        {
            IXMLHandler stub;
            status_t res = sParser.wrap(str);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_data(IXMLHandler *handler, io::IInSequence *seq, size_t flags)
        {
            IXMLHandler stub;
            status_t res = sParser.wrap(seq, flags);
            if (res == STATUS_OK)
                res = parse_document((handler != NULL) ? handler : &stub);
            if (res == STATUS_OK)
                res = sParser.close();
            else
                sParser.close();
            return res;
        }

        status_t PushParser::parse_document(IXMLHandler *handler)
        {
            status_t token, res, last = -1;
            LSPString tmp, *ptmp;
            cvector<LSPString> ctag;

            do
            {
                // Get next token
                if ((token = sParser.read_next()) < 0)
                {
                    res = -token;
                    break;
                }

                // Is there tag element pending?
                if ((ctag.size() > 0) && ((token != XT_ATTRIBUTE) && (token != XT_ENTITY_RESOLVE)))
                {
                    // Add NULL-terminating element
                    if (!ctag.add(NULL))
                        return STATUS_NO_MEM;

                    // Analyze state
                    LSPString **atts        = ctag.get_array();
                    size_t n                = ctag.size();
                    if (n & 1) // Nubmber of elements should be even
                    {
                        res     = STATUS_CORRUPTED;
                        break;
                    }

                    // Call handler
                    res     = handler->start_element(atts[0], &atts[1]);
                    drop_list(&ctag);
                    if (res != STATUS_OK)
                        break;
                }

                // Dispatch event
                last = token;
                switch (token)
                {
                    case XT_START_DOCUMENT:
                        res = handler->start_document(
                                sParser.xml_version(),
                                sParser.version(),
                                sParser.encoding(),
                                sParser.is_standalone()
                            );
                        break;

                    case XT_END_DOCUMENT:
                        res = handler->end_document();
                        break;

                    case XT_CDATA:
                        res = handler->cdata(sParser.value());
                        break;

                    case XT_CHARACTERS:
                        res = handler->characters(sParser.value());
                        break;

                    case XT_COMMENT:
                        res = handler->comment(sParser.value());
                        break;

                    case XT_ENTITY_RESOLVE:
                        res = handler->resolve(&tmp, sParser.name());
                        if (res == STATUS_OK)
                            res = sParser.set_value(&tmp);
                        tmp.clear();
                        break;

                    case XT_START_ELEMENT:
                        // Create copy of tag name
                        if ((ptmp = sParser.name()->clone()) == NULL)
                        {
                            res = STATUS_NO_MEM;
                            break;
                        }
                        else if (!ctag.add(ptmp))
                        {
                            delete ptmp;
                            res = STATUS_NO_MEM;
                            break;
                        }
                        break;

                    case XT_ATTRIBUTE:
                        // Create copy of attribute name
                        if ((ptmp = sParser.name()->clone()) == NULL)
                        {
                            res = STATUS_NO_MEM;
                            break;
                        }
                        else if (!ctag.add(ptmp))
                        {
                            delete ptmp;
                            res = STATUS_NO_MEM;
                            break;
                        }

                        // Create copy of tag value
                        if ((ptmp = sParser.value()->clone()) == NULL)
                        {
                            res = STATUS_NO_MEM;
                            break;
                        }
                        else if (!ctag.add(ptmp))
                        {
                            delete ptmp;
                            res = STATUS_NO_MEM;
                            break;
                        }
                        break;

                    case XT_END_ELEMENT:
                        res = handler->end_element(sParser.name());
                        break;

                    case XT_PROCESSING_INSTRUCTION:
                        res = handler->processing(sParser.name(), sParser.value());
                        break;

                    case XT_DTD:
                        res = handler->doctype(sParser.doctype(), sParser.pub_literal(), sParser.sys_literal());
                        break;

                    default:
                        res = STATUS_CORRUPTED;
                        break;
                }
            } while (res == STATUS_OK);

            // Drop list
            drop_list(&ctag);

            // Return result
            return ((res == STATUS_EOF) && (last == XT_END_DOCUMENT)) ?  STATUS_OK : res;
        }
    
    } /* namespace xml */
} /* namespace lsp */
