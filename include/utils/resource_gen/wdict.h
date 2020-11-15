/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 13 июл. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
