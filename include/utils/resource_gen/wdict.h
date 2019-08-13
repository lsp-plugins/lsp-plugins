/*
 * wdict.h
 *
 *  Created on: 13 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UTILS_RESOURCE_GEN_WDICT_H_
#define UTILS_RESOURCE_GEN_WDICT_H_

#include <core/types.h>
#include <data/cvector.h>

namespace lsp
{
    namespace resgen
    {
        typedef struct xml_word_t
        {
            size_t      length;
            size_t      offset;
            size_t      refs;
            char        text[];

            inline int compare(const xml_word_t *w) const
            {
                int h = length * refs - w->length * w->refs;
                if (h != 0)
                    return h;

                return strcmp(text, w->text);
            }
        } xml_word_t;

        static xml_word_t *res_dict_get(cvector<xml_word_t> *dict, const char *key)
        {
            size_t items    = dict->size();
            size_t len      = strlen(key);
            for (size_t i=0; i<items; ++i)
            {
                xml_word_t *w   = dict->at(i);
                if (w->length != len)
                    continue;
                if (!strcmp(w->text, key))
                    return w;
            }

            return NULL;
        }

        static bool res_dict_add(cvector<xml_word_t> *dict, const char *item)
        {
            xml_word_t *w = res_dict_get(dict, item);
            if (w != NULL)
            {
                w->refs        ++;
                return true;
            }

            // Create new word for the dictionary and initialize it
            size_t length       = strlen(item);
            w                   = reinterpret_cast<xml_word_t *>(malloc(sizeof(xml_word_t) + length + 1));
            if (item == 0)
                return false;
            w->length           = length;
            w->offset           = 0;
            w->refs             = 1;
            memcpy(w->text, item, length + 1);

            // Add new word
            if (!dict->add(w))
            {
                free(w);
                return false;
            }

            return true;
        }
    }
}

#endif /* UTILS_RESOURCE_GEN_WDICT_H_ */
