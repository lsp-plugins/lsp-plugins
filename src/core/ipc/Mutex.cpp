/*
 * Mutex.cpp
 *
 *  Created on: 25 февр. 2019 г.
 *      Author: sadko
 */

#include <core/ipc/Mutex.h>
#include <errno.h>

namespace lsp
{
    namespace ipc
    {
#if defined(PLATFORM_WINDOWS)
        Mutex::Mutex()
        {
            hMutex      = CreateMutexW(NULL, FALSE, NULL);
            nThreadId   = -1;
            nLocks      = 0;
        }

        Mutex::~Mutex()
        {
            CloseHandle(hMutex);
        }

        bool Mutex::lock() const
        {
            if (nThreadId == GetCurrentThreadId())
            {
                ++nLocks;
                return true;
            }

            DWORD res = WaitForSingleObject(hMutex, INFINITE);
            if (res == WAIT_OBJECT_0)
            {
                ++nLocks;
                nThreadId   = GetCurrentThreadId();
                return true;
            }
            return false;
        }

        bool Mutex::try_lock() const
        {
            if (nThreadId == DWORD(-1))
            {
                DWORD res = WaitForSingleObject(hMutex, 0);
                if (res == WAIT_OBJECT_0)
                {
                    ++nLocks;
                    nThreadId   = GetCurrentThreadId();
                    return true;
                }
            }
            else if (nThreadId == GetCurrentThreadId())
            {
                ++nLocks;
                return true;
            }

            return false;
        }

        bool Mutex::unlock() const
        {
            if (nThreadId != GetCurrentThreadId())
                return false;

            bool result = true;
            if (!(--nLocks))
            {
                nThreadId   = -1;
                result = ReleaseMutex(hMutex);
                if (!result)
                {
                    nThreadId   = GetCurrentThreadId();
                    ++nLocks;
                }
            }
            return result;
        }

#elif defined(PLATFORM_LINUX)
        bool Mutex::lock() const
        {
            int res;

            // Check that we already own the mutex
            pthread_t tid   = pthread_self();
            if (nThreadId == tid)
            {
                ++nLocks;
                return true;
            }

            while (true)
            {
                if (atomic_cas(&nLock, 1, 0))
                {
                    if (!(nLocks++))
                        nThreadId       = tid; // Save thread identifier
                    return true;
                }

                // Issue wait
                res = syscall(SYS_futex, &nLock, FUTEX_WAIT, 0, NULL, 0, 0);
                if ((res == ENOSYS) || (res == EAGAIN))
                    pthread_yield();
            }
        }

        bool Mutex::try_lock() const
        {
            // Check that we already own the mutex
            pthread_t tid   = pthread_self();
            if (nThreadId == pthread_self())
            {
                ++nLocks;
                return true;
            }

            if (atomic_cas(&nLock, 1, 0))
            {
                if (!(nLocks++))
                    nThreadId       = tid; // Save thread identifier
                return true;
            }

            return false;
        }

        bool Mutex::unlock() const
        {
            if (nThreadId != pthread_self())
                return false;
            if (!(--nLocks))
            {
                nThreadId       = -1;
                atomic_cas(&nLock, 0, 1);
                syscall(SYS_futex, &nLock, FUTEX_WAKE, 1, NULL, 0, 0);
            }
            return true;
        }

#else
        Mutex::Mutex()
        {
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(&sMutex, &attr);
            pthread_mutexattr_destroy(&attr);
        }

        Mutex::~Mutex()
        {
            pthread_mutex_destroy(&sMutex);
        }
#endif /* PLATFORM_LINUX */

    } /* namespace ipc */
} /* namespace lsp */
