/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 дек. 2016 г.
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

#ifndef UI_WS_IDISPLAY_H_
#define UI_WS_IDISPLAY_H_

#include <data/cstorage.h>
#include <data/cvector.h>
#include <rendering/backend.h>
#include <rendering/factory.h>
#include <core/ipc/Library.h>

namespace lsp
{
    namespace ws
    {
        class INativeWindow;
        class IR3DBackend;

        typedef struct R3DBackendInfo
        {
            LSPString   library;
            LSPString   uid;
            LSPString   display;
        } R3DBackendInfo;

        /** Display
         *
         */
        class IDisplay
        {
            protected:
                typedef struct dtask_t
                {
                    taskid_t        nID;
                    timestamp_t     nTime;
                    task_handler_t  pHandler;
                    void           *pArg;
                } dtask_t;

                typedef struct r3d_library_t: public R3DBackendInfo
                {
                    r3d_factory_t  *builtin;        // Built-in factory
                    size_t          local_id;       // Local identifier
                } r3d_library_t;

            protected:
                taskid_t                nTaskID;
                cstorage<dtask_t>       sTasks;
                dtask_t                 sMainTask;
                cvector<r3d_library_t>  s3DLibs;            // List of libraries that provide 3D backends
                cvector<IR3DBackend>    s3DBackends;        // List of all 3D backend instances
                ipc::Library            s3DLibrary;         // Current backend library used
                r3d_factory_t          *s3DFactory;         // Pointer to the factory object
                ssize_t                 nCurrent3D;         // Current 3D backend
                ssize_t                 nPending3D;         // Pending 3D backend

            protected:
                friend class IR3DBackend;

                bool                taskid_exists(taskid_t id);
                void                deregister_backend(IR3DBackend *lib);
                status_t            switch_r3d_backend(r3d_library_t *backend);
                status_t            commit_r3d_factory(const LSPString *path, r3d_factory_t *factory);
                void                detach_r3d_backends();
                void                call_main_task(timestamp_t time);

            public:
                explicit IDisplay();
                virtual ~IDisplay();

            public:
                virtual int init(int argc, const char **argv);
                virtual void destroy();

                virtual int main();
                virtual status_t main_iteration();
                virtual void quit_main();

                virtual size_t screens();
                virtual size_t default_screen();

                virtual void sync();

                virtual status_t screen_size(size_t screen, ssize_t *w, ssize_t *h);

            public:
                /**
                 * Enumerate backends
                 * @param id backend number starting with 0
                 * @return backend descriptor or NULL if backend with such identifier does not exist
                 */
                const R3DBackendInfo *enum_backend(size_t id) const;

                /**
                 * Get currently used backend for 3D rendering
                 * @return selected backend descriptor
                 */
                const R3DBackendInfo *current_backend() const;

                /**
                 * Get currently used backend identifier for 3D rendering
                 * @return selected backend identifier
                 */
                ssize_t current_backend_id() const;

                /**
                 * Select backend for rendering by specifying it's descriptor
                 * This function does not chnage the backend immediately,
                 * instead of this the backend switch operation is performed in the
                 * main loop
                 * @param backend backend for rendering
                 * @return status of operation
                 */
                status_t select_backend(const R3DBackendInfo *backend);

                /**
                 * Select backend for rendering by specifying it's descriptor identifier
                 * This function does not chnage the backend immediately,
                 * instead of this the backend switch operation is performed in the
                 * main loop
                 * @param id backend's descriptor identifier
                 * @return status of operation
                 */
                status_t select_backend_id(size_t id);

                /**
                 * Lookup the specified directory for existing 3D backends
                 * @param path the directory to perform lookup
                 */
                void lookup_r3d_backends(const io::Path *path);

                /**
                 * Lookup the specified directory for existing 3D backends
                 * @param path the directory to perform lookup
                 */
                void lookup_r3d_backends(const char *path);

                /**
                 * Lookup the specified directory for existing 3D backends
                 * @param path the directory to perform lookup
                 */
                void lookup_r3d_backends(const LSPString *path);

                /**
                 * Try to register the library as a 3D backend
                 * @param path the path to the library
                 * @return status of operation
                 */
                status_t register_r3d_backend(const io::Path *path);

                /**
                 * Try to register the library as a 3D backend
                 * @param path the path to the library
                 * @return status of operation
                 */
                status_t register_r3d_backend(const char *path);

                /**
                 * Try to register the library as a 3D backend
                 * @param path the path to the library
                 * @return status of operation
                 */
                status_t register_r3d_backend(const LSPString *path);


                /** Create native window
                 *
                 * @return native window
                 */
                virtual INativeWindow *create_window();

                /** Create window at the specified screen
                 *
                 * @param screen screen
                 * @return status native window
                 */
                virtual INativeWindow *create_window(size_t screen);

                /** Create native window as child of specified window handle
                 *
                 * @return native window as child of specified window handle
                 */
                virtual INativeWindow *create_window(void *handle);

                /**
                 * Wrap window handle
                 * @param handle handle to wrap
                 * @return native window as wrapper of the handle
                 */
                virtual INativeWindow *wrap_window(void *handle);

                /**
                 * Create 3D backend for graphics
                 * @return pointer to created backend
                 */
                virtual IR3DBackend *create_r3D_backend(INativeWindow *parent);

                /** Create surface for drawing
                 *
                 * @param width surface width
                 * @param height surface height
                 * @return surface or NULL on error
                 */
                virtual ISurface *create_surface(size_t width, size_t height);

                /** Submit task for execution
                 *
                 * @param time time when the task should be triggered (timestamp in milliseconds)
                 * @param handler task handler
                 * @param arg task handler argument
                 * @return submitted task identifier or negative error code
                 */
                virtual taskid_t submit_task(timestamp_t time, task_handler_t handler, void *arg);

                /** Cancel submitted task
                 *
                 * @param id task identifier to cancel
                 * @return status of operation
                 */
                virtual status_t cancel_task(taskid_t id);

                /**
                 * Associate data source with the specified clipboard
                 * @param id clipboard identifier
                 * @param src data source to use
                 * @return status of operation
                 */
                virtual status_t set_clipboard(size_t id, IDataSource *src);

                /**
                 * Sink clipboard data source to the specified handler.
                 * After the data source has been processed, release() should
                 * be called on data source
                 * @param id clipboard identifier
                 * @return pointer to data source or NULL
                 */
                virtual status_t get_clipboard(size_t id, IDataSink *dst);

                /**
                 * Reject drag request because drag is not supported at the current position
                 * @return status of operation
                 */
                virtual status_t reject_drag();

                /**
                 * Accept drag request
                 * @param sink the sink that will handle data transfer
                 * @param action drag action
                 * @param internal true if we want to receive notifications inside of the drag rectangle
                 * @param r parameters of the drag rectangle, can be NULL
                 * @return status of operation
                 */
                virtual status_t accept_drag(IDataSink *sink, drag_t action, bool internal, const realize_t *r);

                /**
                 * Get currently pending content type of a drag
                 * @return NULL-terminated list of pending content types,
                 *   may be NULL if there is no currently pending Drag&Drop request
                 */
                virtual const char * const *get_drag_ctypes();

                /**
                 * Set callback which will be called after each main iteration
                 * @param handler callback handler routine
                 * @param arg additional argument
                 */
                void set_main_callback(task_handler_t handler, void *arg);
        };

    } /* namespace ws */
} /* namespace lsp */

#endif /* UI_IDISPLAY_H_ */
