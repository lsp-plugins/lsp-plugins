/*
 * Dictionary.h
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: sadko
 */

#ifndef CORE_I18N_FILEDICTIONARY_H_
#define CORE_I18N_FILEDICTIONARY_H_

#include <core/i18n/IDictionary.h>

namespace lsp
{
    class FileDictionary: public IDictionary
    {
        private:
            FileDictionary & operator = (const FileDictionary &);

        public:
            explicit FileDictionary();
            virtual ~FileDictionary();
    };

} /* namespace lsp */

#endif /* CORE_I18N_FILEDICTIONARY_H_ */
