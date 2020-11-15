/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 27 янв. 2016 г.
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

#ifndef CORE_IWRAPPER_H_
#define CORE_IWRAPPER_H_

#include <core/ipc/ITask.h>
#include <core/ipc/IExecutor.h>
#include <core/ICanvas.h>
#include <core/port_data.h>
#include <core/KVTStorage.h>

namespace lsp
{
    class plugin_t;

    class IWrapper
    {
        private:
            IWrapper & operator = (const IWrapper &);

        protected:
            plugin_t       *pPlugin;

        public:
            explicit IWrapper(plugin_t *plugin);
            virtual ~IWrapper();

        public:
            /** Get executor service
             *
             * @return executor service
             */
            virtual ipc::IExecutor *get_executor();

            /** Query for inline display drawing
             *
             */
            virtual void query_display_draw();

            /** Get current time position
             *
             * @return current time position
             */
            virtual const position_t *position();

            /**
             * Create/resize canvas
             * @param cv valid pointer to ICanvas object or NULL if there's none
             * @param width the requested width of canvas
             * @param height the requested height of canvas
             * @return the pointer to valid canvas object or NULL on error
             */
            virtual ICanvas *create_canvas(ICanvas *&cv, size_t width, size_t height);

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
             * Notify the host about internal state change
             */
            virtual void state_changed();

            /**
             * Dump the state of plugin
             */
            virtual void dump_plugin_state();
    };

} /* namespace lsp */

#endif /* CORE_IWRAPPER_H_ */
