/*
 * XMLNode.h
 *
 *  Created on: 29 окт. 2019 г.
 *      Author: sadko
 */

#ifndef UI_XMLNODE_H_
#define UI_XMLNODE_H_

#include <core/LSPString.h>
#include <data/cvector.h>
#include <core/files/xml/IXMLHandler.h>

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

    class XMLPlaybackNode: public XMLNode
    {
        private:
            XMLPlaybackNode & operator = (const XMLPlaybackNode &src);

        protected:
            enum event_t
            {
                EVT_START_ELEMENT,
                EVT_END_ELEMENT
            };

            typedef struct xml_event_t
            {
                event_t             nEvent;
                cvector<LSPString>  vData;

                inline xml_event_t(event_t type);
                ~xml_event_t();
                status_t add_param(const LSPString *name);
            } xml_event_t;

        private:
            XMLNode                *pHandler;
            cvector<xml_event_t>    vEvents;

        private:
            xml_event_t *add_event(event_t ev);

        public:
            explicit XMLPlaybackNode(XMLNode *handler);

            virtual ~XMLPlaybackNode();

            virtual status_t init(const LSPString * const *atts);

        public:
            /**
             * Playback start element
             * @param handler XML handler
             * @param name element name
             * @param atts element attributes
             * @return status of operation
             */
            virtual status_t playback_start_element(xml::IXMLHandler *handler, const LSPString *name, const LSPString * const *atts);

            /**
             * Playback end element
             * @param handler XML handler
             * @param name element name
             * @param atts element attributes
             * @return status of operation
             */
            virtual status_t playback_end_element(xml::IXMLHandler *handler, const LSPString *name);

            /**
             * Playback recorded data
             * @return
             */
            virtual status_t playback();

            /**
             * Execute the body of playback node
             * @return status of operation
             */
            virtual status_t execute();

            virtual status_t start_element(XMLNode **child, const LSPString *name, const LSPString * const *atts);

            virtual status_t end_element(const LSPString *name);

    };

} /* namespace lsp */

#endif /* UI_XMLNODE_H_ */
