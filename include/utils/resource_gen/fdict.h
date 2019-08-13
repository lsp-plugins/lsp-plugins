/*
 * fdict.h
 *
 *  Created on: 13 июл. 2019 г.
 *      Author: sadko
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
