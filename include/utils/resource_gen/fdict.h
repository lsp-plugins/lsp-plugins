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

#ifndef UTILS_RESOURCE_GEN_FDICT_H_
#define UTILS_RESOURCE_GEN_FDICT_H_

#include <core/types.h>
#include <data/cstorage.h>

namespace lsp
{
    namespace resgen
    {
        typedef struct dict_float_t
        {
            float       value;
            size_t      refs;
            size_t      index;

            inline int compare(const dict_float_t *w) const
            {
                int diff = refs - w->refs;
                if (diff != 0)
                    return diff;
                float a = fabs(w->value);
                float b = fabs(value);
                return (a<b) ? -1 : (a>b) ? 1 : 0;
            }

            void swap(dict_float_t *b)
            {
                ::swap(value, b->value);
                ::swap(refs, b->refs);
                ::swap(index, b->index);
            }
        } dict_float_t;

        static dict_float_t *float_dict_get(cstorage<dict_float_t> *dict, float v)
        {
            for (size_t i=0, n=dict->size(); i<n; ++i)
            {
                dict_float_t *w   = dict->at(i);
                if (w->value == v)
                    return w;
            }
            return NULL;
        }

        static bool float_dict_add(cstorage<dict_float_t> *dict, float value)
        {
            dict_float_t *item = float_dict_get(dict, value);
            if (item != NULL)
            {
                ++item->refs;
                return true;
            }

            if ((item = dict->add()) == NULL)
                return false;

            item->value = value;
            item->index = 0;
            item->refs  = 1;

            return true;
        }
    }
}


#endif /* UTILS_RESOURCE_GEN_FDICT_H_ */
