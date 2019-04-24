/*
 * IDisplay.cpp
 *
 *  Created on: 12 дек. 2016 г.
 *      Author: sadko
 */

#include <core/status.h>
#include <ui/ws/ws.h>
#include <core/io/Dir.h>
#include <metadata/metadata.h>

#ifdef LSP_IDE_DEBUG
    #ifdef PLATFORM_UNIX_COMPATIBLE
        #include <rendering/glx/factory.h>

        namespace lsp
        {
            extern glx_factory_t   glx_factory;
        }
    #endif
#endif /* LSP_IDE_DEBUG */

namespace lsp
{
    namespace ws
    {
        IDisplay::IDisplay()
        {
            nTaskID     = 0;
            s3DFactory  = NULL;
            nCurrent3D  = 0;
            nPending3D  = 0;
        }

        IDisplay::~IDisplay()
        {
        }

        R3DBackendInfo *IDisplay::enumBackend(size_t id) const
        {
            return s3DLibs.get(id);
        };

        void IDisplay::lookup3DBackends(const io::Path *path)
        {
            io::Dir dir;

            status_t res = dir.open(path);
            if (res != STATUS_OK)
                return;

            io::Path child;
            LSPString item, prefix;
            if (!prefix.set_ascii(LSP_R3D_BACKEND_PREFIX))
                return;

            io::fattr_t fattr;
            while ((res = dir.read(&item, false)) == STATUS_OK)
            {
                if (!item.starts_with(&prefix))
                    continue;

                if ((res = child.set(path, &item)) != STATUS_OK)
                    continue;
                if ((res = child.stat(&fattr)) != STATUS_OK)
                    continue;

                switch (fattr.type)
                {
                    case io::fattr_t::FT_DIRECTORY:
                    case io::fattr_t::FT_BLOCK:
                    case io::fattr_t::FT_CHARACTER:
                        continue;
                    default:
                        register3DBackend(&child);
                        break;
                }
            }
        }

        void IDisplay::lookup3DBackends(const char *path)
        {
            io::Path tmp;
            if (tmp.set(path) != STATUS_OK)
                return;
            lookup3DBackends(&tmp);
        }

        void IDisplay::lookup3DBackends(const LSPString *path)
        {
            io::Path tmp;
            if (tmp.set(path) != STATUS_OK)
                return;
            lookup3DBackends(&tmp);
        }

        status_t IDisplay::register3DBackend(const io::Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return register3DBackend(path->as_string());
        }

        status_t IDisplay::register3DBackend(const char *path)
        {
            LSPString tmp;
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!tmp.set_utf8(path))
                return STATUS_NO_MEM;
            return register3DBackend(&tmp);
        }

        status_t IDisplay::commit_r3d_factory(const LSPString *path, r3d_factory_t *factory)
        {
            for (size_t id=0; ; ++id)
            {
                // Get metadata record
                const r3d_backend_metadata_t *meta = factory->metadata(factory, id);
                if (meta == NULL)
                    break;
                else if (meta->id == NULL)
                    continue;

                // Create library descriptor
                r3d_library_t *r3dlib   = new r3d_library_t();
                if (r3dlib == NULL)
                    return STATUS_NO_MEM;

                r3dlib->builtin     = (path != NULL) ? NULL : factory;
                r3dlib->local_id    = id;
                if (path == NULL)
                {
                    if (!r3dlib->library.set(path))
                    {
                        delete r3dlib;
                        return STATUS_NO_MEM;
                    }
                }

                if ((!r3dlib->uid.set_utf8(meta->id)) ||
                    (!r3dlib->display.set_utf8((meta->display != NULL) ? meta->display : meta->id)))
                {
                    delete r3dlib;
                    return STATUS_NO_MEM;
                }

                // Add library descriptor to the list
                if (!s3DLibs.add(r3dlib))
                {
                    delete r3dlib;
                    return STATUS_NO_MEM;
                }
            }

            return STATUS_OK;
        }

        status_t IDisplay::register3DBackend(const LSPString *path)
        {
            ipc::Library lib;

            // Open library
            status_t res = lib.open(path);
            if (res != STATUS_OK)
                return res;

            // Lookup function
            lsp_r3d_factory_function_t func = reinterpret_cast<lsp_r3d_factory_function_t>(lib.import(R3D_FACTORY_FUNCTION_NAME));
            if (func == NULL)
            {
                lib.close();
                return STATUS_NOT_FOUND;
            }

            // Try to instantiate factory
            r3d_factory_t *factory  = func(LSP_MAIN_VERSION);
            if (factory == NULL)
            {
                lib.close();
                return STATUS_NOT_FOUND;
            }

            // Fetch metadata
            res = commit_r3d_factory(path, factory);

            // Close the library
            lib.close();
            return res;
        }

        int IDisplay::init(int argc, const char **argv)
        {
            status_t res;

            #ifdef LSP_IDE_DEBUG
                #ifdef PLATFORM_UNIX_COMPATIBLE
                    res = commit_r3d_factory(NULL, &glx_factory); // Remember built-in factory
                    if (res != STATUS_OK)
                        return res;
                #endif
            #endif /* LSP_IDE_DEBUG */

            // Scan for another locations
            io::Path path;
            res = ipc::Library::get_self_file(&path);
            if (res == STATUS_OK)
                res     = path.parent();
            if (res == STATUS_OK)
                lookup3DBackends(&path);

            return STATUS_OK;
        }

        void IDisplay::destroy()
        {
            // Destroy all backends
            for (size_t j=0,m=s3DBackends.size(); j<m;++j)
            {
                // Get backend
                r3d_backend_t *backend = s3DBackends.get(j);
                if (backend == NULL)
                    continue;

                // Destroy backend
                backend->destroy(backend);
            }

            // Flush list of backends and close library
            s3DBackends.flush();
            s3DFactory = NULL;
            s3DLibrary.close();
        }

        int IDisplay::main()
        {
            return STATUS_SUCCESS;
        }

        void IDisplay::destroy_backend(r3d_backend_t *backend)
        {
            // Try to remove backend
            if (!s3DBackends.remove(backend, true))
                return;

            // Destroy backend
            backend->destroy(backend);

            // Need to unload library?
            if (s3DBackends.size() <= 0)
            {
                s3DFactory  = NULL;
                s3DLibrary.close();
            }
        }

        IR3DBackend *IDisplay::create3DBackend()
        {
            // TODO
            return NULL;
        }

        status_t IDisplay::main_iteration()
        {
            // TODO: Sync backends


            return STATUS_SUCCESS;
        }

        void IDisplay::quit_main()
        {
        }

        size_t IDisplay::screens()
        {
            return 0;
        }

        size_t IDisplay::default_screen()
        {
            return 0;
        }

        status_t IDisplay::screen_size(size_t screen, ssize_t *w, ssize_t *h)
        {
            return STATUS_BAD_ARGUMENTS;
        }

        INativeWindow *IDisplay::createWindow()
        {
            return NULL;
        }

        INativeWindow *IDisplay::createWindow(size_t screen)
        {
            return NULL;
        }

        INativeWindow *IDisplay::createWindow(void *handle)
        {
            return NULL;
        }

        ISurface *IDisplay::createSurface(size_t width, size_t height)
        {
            return NULL;
        }
    
        bool IDisplay::taskid_exists(taskid_t id)
        {
            for (size_t i=0, n=sTasks.size(); i<n; ++i)
            {
                dtask_t *task = sTasks.at(i);
                if (task == NULL)
                    continue;
                if (task->nID == id)
                    return true;
            }
            return false;
        }

        taskid_t IDisplay::submitTask(timestamp_t time, task_handler_t handler, void *arg)
        {
            if (handler == NULL)
                return -STATUS_BAD_ARGUMENTS;

            ssize_t first = 0, last = sTasks.size() - 1;

            // Find the place to add the task
            while (first <= last)
            {
                ssize_t center  = (first + last) >> 1;
                dtask_t *t      = sTasks.at(center);
                if (t->nTime <= time)
                    first           = center + 1;
                else
                    last            = center - 1;
            }

            // Generate task ID
            do
            {
                nTaskID     = (nTaskID + 1) & 0x7fffff;
            } while (taskid_exists(nTaskID));

            // Add task to the found place keeping it's time order
            dtask_t *t = sTasks.insert(first);
            if (t == NULL)
                return -STATUS_NO_MEM;

            t->nID          = nTaskID;
            t->nTime        = time;
            t->pHandler     = handler;
            t->pArg         = arg;

            return t->nID;
        }

        status_t IDisplay::cancelTask(taskid_t id)
        {
            if (id < 0)
                return STATUS_INVALID_UID;

            // Remove task from the queue
            for (size_t i=0; i<sTasks.size(); ++i)
                if (sTasks.at(i)->nID == id)
                {
                    sTasks.remove(i);
                    return STATUS_OK;
                }

            return STATUS_NOT_FOUND;
        }

        status_t IDisplay::fetchClipboard(size_t id, const char *ctype, clipboard_handler_t handler, void *arg)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t IDisplay::writeClipboard(size_t id, IClipboard *c)
        {
            return STATUS_NOT_IMPLEMENTED;
        }
    }

} /* namespace lsp */
