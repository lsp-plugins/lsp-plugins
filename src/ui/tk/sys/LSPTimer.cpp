/*
 * LSPTimer.cpp
 *
 *  Created on: 1 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <time.h>

namespace lsp
{
    namespace tk
    {
        LSPTimer::LSPTimer()
        {
            pDisplay        = NULL;
            pHandler        = NULL;
            pArguments      = NULL;
            nRepeatInterval = 1000;
            nRepeatCount    = 0;
            nFlags          = TF_DEFAULT;
            nErrorCode      = STATUS_OK;
            nTaskID         = -1;
        }

        LSPTimer::~LSPTimer()
        {
            cancel();
        }

        status_t LSPTimer::submit_task(timestamp_t ctime)
        {
            // Check error status
            if ((nFlags & (TF_ERROR | TF_STOP_ON_ERR)) == (TF_ERROR | TF_STOP_ON_ERR))
                return STATUS_OK;
            else if (!(nFlags & TF_LAUNCHED))
                return STATUS_OK;

            // Now check that timer is finite
            if ((!(nFlags & TF_INFINITE)) && (nRepeatCount <= 0))
            {
                nFlags      |= TF_COMPLETED;
                return STATUS_OK;
            }

            // Submit task to display's queue
            nTaskID     = pDisplay->submit_task(ctime + nRepeatInterval, execute, this);
            if (nTaskID < 0)
                return -nTaskID;
            return STATUS_OK;
        }

        status_t LSPTimer::execute(timestamp_t time, void *arg)
        {
            LSPTimer *_this      = static_cast<LSPTimer *>(arg);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;

            return _this->execute_task(time, arg);
        }

        status_t LSPTimer::execute_task(timestamp_t time, void *arg)
        {
            // Decrement number of repeats
            nTaskID             = -1;
            nRepeatCount        --;

            // First execute run() method
            status_t code       = run(time, pArguments);

            // Analyze status of execution
            if ((nFlags & TF_STOP_ON_ERR) && (code != STATUS_OK))
            {
                nErrorCode          = code;
                nFlags             |= TF_ERROR;
            }
            else if (pHandler != NULL)
            {
                // How run handler if present
                code       = pHandler(time, pArguments);

                // Analyze status of execution
                if ((nFlags & TF_STOP_ON_ERR) && (code != STATUS_OK))
                {
                    nErrorCode          = code;
                    nFlags             |= TF_ERROR;
                }
            }

            return submit_task(time);
        }

        void LSPTimer::bind(IDisplay *dpy)
        {
            // Cancel previous execution
            cancel();

            // Store new display pointer
            pDisplay        = dpy;
        }

        void LSPTimer::bind(LSPDisplay *dpy)
        {
            // Cancel previous execution
            cancel();

            // Store new display pointer
            pDisplay        = dpy->display();
        }

        status_t LSPTimer::launch(ssize_t count, size_t interval, timestamp_t delay)
        {
            // Cancel previous execution
            status_t result = cancel();
            if (result != STATUS_OK)
                return result;

            // Update settings
            nFlags          = TF_DEFAULT;
            nErrorCode      = STATUS_OK;
            if (count <= 0)
                nFlags          |= TF_INFINITE;
            nRepeatInterval = interval;

            // Submit first task
            if (delay > 0)
            {
                // Shift the timestamp by delta
                struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                timestamp_t delta = (ts.tv_sec * 1000L) + (ts.tv_nsec / 1000000L); // Get delta in milliseconds
                delay      += delta;
            }

            nTaskID     = pDisplay->submit_task(delay, execute, this);
            if (nTaskID < 0)
                return -nTaskID;

            nFlags         |= TF_LAUNCHED;
            return result;
        }

        void LSPTimer::set_handler(task_handler_t handler)
        {
            pHandler        = handler;
        }

        void LSPTimer::set_handler(task_handler_t handler, void *args)
        {
            pHandler        = handler;
            pArguments      = args;
        }

        void LSPTimer::set_argument(void *args)
        {
            pArguments      = args;
        }

        status_t LSPTimer::cancel()
        {
            if (pDisplay == NULL)
                return STATUS_NOT_BOUND;

            // Cancel task if present
            if (nTaskID >= 0)
            {
                pDisplay->cancel_task(nTaskID);
                nTaskID = -1;
            }

            nFlags         &= ~TF_LAUNCHED;

            return STATUS_OK;
        }

        void LSPTimer::set_stop_on_error(bool stop)
        {
            if (stop)
                nFlags         |= TF_STOP_ON_ERR;
            else
                nFlags         &= ~TF_STOP_ON_ERR;
        }

        status_t LSPTimer::resume()
        {
            // Check that timer is bound
            if (pDisplay == NULL)
                return STATUS_NOT_BOUND;

            // Check execution status
            if ((nFlags & (TF_LAUNCHED | TF_ERROR)) != (TF_LAUNCHED | TF_ERROR))
                return STATUS_BAD_STATE;

            nFlags         &= ~TF_ERROR;
            return submit_task(0);
        }

        status_t LSPTimer::run(timestamp_t time, void *args)
        {
            // Just stub, can be overridden
            return STATUS_OK;
        }

    }
} /* namespace lsp */
