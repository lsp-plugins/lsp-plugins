/*
 * BuiltinDictionary.h
 *
 *  Created on: 27 февр. 2020 г.
 *      Author: sadko
 */

#ifndef CORE_I18N_BUILTINDICTIONARY_H_
#define CORE_I18N_BUILTINDICTIONARY_H_

#include <core/i18n/IDictionary.h>

namespace lsp
{
    class BuiltinDictionary: public IDictionary
    {
        private:
            BuiltinDictionary & operator = (const BuiltinDictionary &);

        public:
            explicit BuiltinDictionary();
            virtual ~BuiltinDictionary();
    };

} /* namespace lsp */

#endif /* CORE_I18N_BUILTINDICTIONARY_H_ */
