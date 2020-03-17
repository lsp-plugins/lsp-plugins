/*
 * RootDictionary.h
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: sadko
 */

#ifndef CORE_I18N_ROOTDICTIONARY_H_
#define CORE_I18N_ROOTDICTIONARY_H_

#include <data/cvector.h>
#include <core/i18n/IDictionary.h>
#include <core/io/Path.h>

namespace lsp
{
    /**
     * Class implements root dictionary logic which operates on all
     * sub-dictionaries
     */
    class Dictionary: public IDictionary
    {
        private:
            Dictionary & operator = (const Dictionary &);

        protected:
            typedef struct node_t
            {
                LSPString       sKey;
                IDictionary    *pDict;
                bool            bRoot;
            } node_t;

        protected:
            cvector<node_t>     vNodes;
            LSPString           sPath;

        protected:
            status_t        load_json(IDictionary **dict, const LSPString *path);
            status_t        load_builtin(IDictionary **dict, const LSPString *path);
            status_t        create_child(IDictionary **dict, const LSPString *path);
            status_t        init_dictionary(IDictionary *d, const LSPString *path);
            status_t        load_dictionary(const LSPString *id, IDictionary **dict);

        public:
            explicit Dictionary();
            virtual ~Dictionary();

        public:
            using IDictionary::lookup;
            using IDictionary::init;

            virtual status_t lookup(const LSPString *key, LSPString *value);

            virtual status_t lookup(const LSPString *key, IDictionary **value);

            virtual status_t get_value(size_t index, LSPString *key, LSPString *value);

            virtual status_t get_child(size_t index, LSPString *key, IDictionary **dict);

            virtual size_t size();

            virtual status_t init(const LSPString *path);

        public:
            /**
             * Clear dictionary contents
             */
            void        clear();

    };

} /* namespace lsp */

#endif /* CORE_I18N_ROOTDICTIONARY_H_ */
