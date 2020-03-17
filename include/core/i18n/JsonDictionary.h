/*
 * Dictionary.h
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: sadko
 */

#ifndef CORE_I18N_FILEDICTIONARY_H_
#define CORE_I18N_FILEDICTIONARY_H_

#include <data/cvector.h>
#include <core/i18n/IDictionary.h>
#include <core/files/json/Parser.h>

namespace lsp
{
    class JsonDictionary: public IDictionary
    {
        private:
            JsonDictionary & operator = (const JsonDictionary &);

        protected:
            typedef struct node_t
            {
                LSPString           sKey;
                LSPString           sValue;
                JsonDictionary     *pChild;
            } node_t;

        protected:
            cvector<node_t>     vNodes;

        protected:
            status_t    add_node(const node_t *node);
            status_t    parse_json(json::Parser *p);
            node_t     *find_node(const LSPString *key);

        public:
            explicit JsonDictionary();
            virtual ~JsonDictionary();

        public:
            using IDictionary::init;
            using IDictionary::get_value;
            using IDictionary::get_child;
            using IDictionary::lookup;

            virtual status_t init(const LSPString *path);

            virtual status_t lookup(const LSPString *key, LSPString *value);

            virtual status_t lookup(const LSPString *key, IDictionary **value);

            virtual status_t get_value(size_t index, LSPString *key, LSPString *value);

            virtual status_t get_child(size_t index, LSPString *key, IDictionary **dict);

            virtual size_t size();
    };

} /* namespace lsp */

#endif /* CORE_I18N_FILEDICTIONARY_H_ */
