/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 25 апр. 2016 г.
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

            /**
             * Request plugin for dump of the internal state
             */
            virtual void dump_state_request();
    };

} /* namespace lsp */

#endif /* IUIWRAPPER_H_ */
