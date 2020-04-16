/*
 * BuiltinDictionary.h
 *
 *  Created on: 27 февр. 2020 г.
 *      Author: sadko
 */

#ifndef CORE_I18N_BUILTINDICTIONARY_H_
#define CORE_I18N_BUILTINDICTIONARY_H_

#include <data/cstorage.h>
#include <core/i18n/IDictionary.h>
#include <core/resource.h>

namespace lsp
{
    class BuiltinDictionary: public IDictionary
    {
        private:
            BuiltinDictionary & operator = (const BuiltinDictionary &);

        protected:
            typedef struct node_t
            {
                const char         *sKey;
                const char         *sValue;
                BuiltinDictionary  *pChild;
                bool                bBad;
            } node_t;

        protected:
            LSPString           sPath;
            cstorage<node_t>    vNodes;

        protected:
            status_t            parse_dictionary(const resource::resource_t *res);
            node_t             *find_node(const char *key);
            status_t            add_node(const node_t *node);
#ifdef LSP_TRACE
            void                dump(size_t offset);
#endif

        public:
            explicit BuiltinDictionary();
            virtual ~BuiltinDictionary();

        public:
            using IDictionary::init;

            virtual status_t init(const LSPString *path);

            virtual status_t lookup(const char *key, LSPString *value);

            virtual status_t lookup(const char *key, IDictionary **value);

            virtual status_t lookup(const LSPString *key, LSPString *value);

            virtual status_t lookup(const LSPString *key, IDictionary **value);

            virtual status_t get_value(size_t index, LSPString *key, LSPString *value);

            virtual status_t get_child(size_t index, LSPString *key, IDictionary **dict);

            virtual size_t size();
    };

} /* namespace lsp */

#endif /* CORE_I18N_BUILTINDICTIONARY_H_ */
