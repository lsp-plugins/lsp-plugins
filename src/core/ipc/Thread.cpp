/*
 * Thread.cpp
 *
 *  Created on: 25 февр. 2019 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <dsp/atomic.h>
#include <core/ipc/Thread.h>

#include <time.h>
#include <errno.h>
#include <unistd.h>

namespace lsp
{
    namespace ipc
    {
        __thread Thread *Thread::pThis = NULL;
        
#if defined(PLATFORM_WINDOWS)
    #define CLR_HANDLE(hThread) hThread     = INVALID_HANDLE_VALUE;
#else
    #define CLR_HANDLE(hThread) hThread     = 0;
#endif

        Thread::Thread()
        {
            enState             = TS_CREATED;
            nResult             = STATUS_OK;
            bCancelled          = false;
            CLR_HANDLE(hThread);
            sBinding.proc       = NULL;
            sBinding.arg        = NULL;
            sBinding.runnable   = NULL;
        }
        
        Thread::Thread(thread_proc_t proc)
        {
            enState             = TS_CREATED;
            nResult             = STATUS_OK;
            bCancelled          = false;
            CLR_HANDLE(hThread);
            sBinding.proc       = proc;
            sBinding.arg        = NULL;
            sBinding.runnable   = NULL;
        }

        Thread::Thread(thread_proc_t proc, void *arg)
        {
            enState             = TS_CREATED;
            nResult             = STATUS_OK;
            bCancelled          = false;
            CLR_HANDLE(hThread);
            sBinding.proc       = proc;
            sBinding.runnable   = NULL;
            sBinding.arg        = arg;
        }

        Thread::Thread(IRunnable *runnable)
        {
            enState             = TS_CREATED;
            nResult             = STATUS_OK;
            bCancelled          = false;
            CLR_HANDLE(hThread);
            sBinding.proc       = NULL;
            sBinding.arg        = NULL;
            sBinding.runnable   = runnable;
        }

        Thread::~Thread()
        {
#if defined(PLATFORM_WINDOWS)
            if (hThread != INVALID_HANDLE_VALUE)
                CloseHandle(hThread);
#endif /* PLATFORM_WINDOWS */

            CLR_HANDLE(hThread);
        }

        status_t Thread::run()
        {
            if (sBinding.proc != NULL)
                return sBinding.proc(sBinding.arg);
            else if (sBinding.runnable != NULL)
                return sBinding.runnable->run();
            return STATUS_OK;
        }

        status_t Thread::cancel()
        {
            switch (enState)
            {
                case TS_PENDING:
                case TS_RUNNING:
                    break;
                default:
                    return STATUS_BAD_STATE;
            }

            bCancelled  = true;
            return STATUS_OK;
        }
    
#if defined(PLATFORM_WINDOWS)
        DWORD WINAPI Thread::thread_launcher(_In_ LPVOID lpParameter)
        {
            Thread *_this = reinterpret_cast<Thread *>(lpParameter);
            pThis           = _this;

            // Wait until we are ready to launch
            while (!atomic_cas(&_this->enState, TS_PENDING, TS_RUNNING)) {}

            // Execute the thread
            status_t res    = _this->run();

            // Commit the 'FINISHED' status
            int state;
            do
            {
                state       = _this->enState;
            } while (!atomic_cas(&_this->enState, state, TS_FINISHED));

            _this->nResult  = res;
            return 0;
        }

        status_t Thread::start()
        {
            DWORD tid;
            HANDLE thandle = CreateThread(NULL, 0, thread_launcher, this, 0, &tid);
            if (thandle == INVALID_HANDLE_VALUE)
                return STATUS_UNKNOWN_ERR;

            hThread     = thandle;
            enState     = TS_PENDING;
            return STATUS_OK;
        }

        status_t Thread::join()
        {
            switch (enState)
            {
                case TS_CREATED:
                    return STATUS_BAD_STATE;
                case TS_PENDING:
                case TS_RUNNING:
                {
                    DWORD res = WaitForSingleObject(hThread, INFINITE);
                    if ((res != WAIT_OBJECT_0) && (res != WAIT_ABANDONED))
                        return STATUS_UNKNOWN_ERR;
                    break;
                }
                case TS_FINISHED:
                    return STATUS_OK;
                default:
                    return STATUS_BAD_STATE;
            }
            return STATUS_OK;
        }

        status_t Thread::sleep(wsize_t millis)
        {
            if (pThis == NULL)
            {
                while (millis > 0)
                {
                    DWORD interval  = (millis > 500) ? 500 : millis;
                    Sleep(interval);
                    millis         -= interval;
                }
            }
            else
            {
                if (pThis->bCancelled)
                    return STATUS_CANCELLED;

                while (millis > 0)
                {
                    if (pThis->bCancelled)
                        return STATUS_CANCELLED;

                    DWORD interval  = (millis > 100) ? 100 : millis;
                    Sleep(interval);
                    millis         -= interval;
                }
            }

            return STATUS_OK;
        }

        size_t Thread::system_cores()
        {
            SYSTEM_INFO     os_sysinfo;
            GetSystemInfo(&os_sysinfo);

            return os_sysinfo.dwNumberOfProcessors;
        }
#else
        void *Thread::thread_launcher(void *arg)
        {
            Thread *_this   = reinterpret_cast<Thread *>(arg);
            pThis           = _this;

            // Cleanup cancellation state
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

            // Wait until we are ready to launch
            while (!atomic_cas(&_this->enState, TS_PENDING, TS_RUNNING)) {}

            // Execute the thread
            status_t res    = _this->run();

            // Commit the 'FINISHED' status
            int state;
            do
            {
                state       = _this->enState;
            } while (!atomic_cas(&_this->enState, state, TS_FINISHED));

            _this->nResult  = res;
            return NULL;
        }

        status_t Thread::start()
        {
            pthread_t tid;
            if (pthread_create(&tid, NULL, &thread_launcher, this) != 0)
                return STATUS_UNKNOWN_ERR;

            hThread     = tid;
            enState     = TS_PENDING;
            return STATUS_OK;
        }

        status_t Thread::join()
        {
            switch (enState)
            {
                case TS_CREATED:
                    return STATUS_BAD_STATE;
                case TS_PENDING:
                case TS_RUNNING:
                    if (pthread_join(hThread, NULL) != 0)
                        return STATUS_UNKNOWN_ERR;
                    break;
                case TS_FINISHED:
                    return STATUS_OK;
                default:
                    return STATUS_BAD_STATE;
            }
            return STATUS_OK;
        }

        status_t Thread::sleep(wsize_t millis)
        {
            struct timespec req, rem;

            if (pThis == NULL)
            {
                req.tv_sec  = millis / 1000;
                req.tv_nsec = (millis % 1000) * 1000000;

                while (::nanosleep(&req, &rem) != 0)
                {
                    int code = errno;
                    if (code != EINTR)
                        return STATUS_UNKNOWN_ERR;
                    req = rem;
                }
            }
            else
            {
                if (pThis->bCancelled)
                    return STATUS_CANCELLED;

                while (millis > 0)
                {
                    if (pThis->bCancelled)
                        return STATUS_CANCELLED;

                    wsize_t interval  = (millis > 100) ? 100 : millis;
                    req.tv_sec  = 0;
                    req.tv_nsec = interval * 1000000;

                    while (::nanosleep(&req, &rem) != 0)
                    {
                        int code = errno;
                        if (code != EINTR)
                            return STATUS_UNKNOWN_ERR;

                        if (pThis->bCancelled)
                            return STATUS_CANCELLED;
                        req = rem;
                    }

                    millis     -= interval;
                }
            }

            return STATUS_OK;
        }

        size_t Thread::system_cores()
        {
            return sysconf(_SC_NPROCESSORS_ONLN);
        }
#endif /* PLATFORM_WINDOWS */

    } /* namespace ipc */
} /* namespace lsp */
