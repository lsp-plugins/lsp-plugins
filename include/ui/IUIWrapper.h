/*
 * IUIWrapper.h
 *
 *  Created on: 25 апр. 2016 г.
 *      Author: sadko
 */

#ifndef IUIWRAPPER_H_
#define IUIWRAPPER_H_

#include <core/KVTStorage.h>

namespace lsp
{
    class IUIWrapper
    {
        private:
            IUIWrapper & operator = (const IUIWrapper &);

        public:
            explicit IUIWrapper();
            virtual ~IUIWrapper();

        public:
            /** Callback method, executes when the UI has been shown
             *
             */
            virtual void ui_activated();

            /** Callback method, executes when the UI has been hidden
             *
             */
            virtual void ui_deactivated();

            /**
             * Lock KVT storage and return pointer to the storage,
             * this is non-RT-safe operation
             * @return pointer to KVT storage or NULL if KVT is not supported
             */
            virtual KVTStorage *kvt_lock();

            /**
             * Try to lock KVT storage and return pointer to the storage on success
             * @return pointer to KVT storage or NULL
             */
            virtual KVTStorage *kvt_trylock();

            /**
             * Release the KVT storage
             * @return true on success
             */
            virtual bool kvt_release();
    };

} /* namespace lsp */

#endif /* IUIWRAPPER_H_ */
