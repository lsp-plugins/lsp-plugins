/*
 * IDisplay.cpp
 *
 *  Created on: 12 дек. 2016 г.
 *      Author: sadko
 */

#include <core/status.h>
#include <ui/ws/ws.h>

namespace lsp
{
    namespace ws
    {
        IDisplay::IDisplay()
        {
            nTaskID     = 0;
        }

        IDisplay::~IDisplay()
        {
        }

        int IDisplay::init(int argc, const char **argv)
        {
            return STATUS_SUCCESS;
        }

        void IDisplay::destroy()
        {
        }

        int IDisplay::main()
        {
            return STATUS_SUCCESS;
        }

        int IDisplay::main_iteration()
        {
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
