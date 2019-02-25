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

            while (true)
            {
                if (nLock)
                {
                    // Increment number of waiters
                    res = atomic_swap(&nLock, 0);
                    if (res) // Lock succeeded ?
                    {
                        if (!(nLocks++))
                            nThreadId       = pthread_self(); // Save thread identifier
                        return true;
                    }
                }

                // Check that we already own the mutex
                if (nThreadId == pthread_self())
                {
                    ++nLocks;
                    return true;
                }

                // Increment number of waiters
                atomic_add(&nWaiters, 1);

                // Issue wait
                do
                {
                    res = syscall(SYS_futex, &nLock, FUTEX_WAIT, 1, NULL, 0, 0);
                    if (res == ENOSYS)
                        pthread_yield();
                } while (res != 0);

                // Decrement number of waiters
                atomic_add(&nWaiters, -1);
            }
        }

        bool Mutex::try_lock() const
        {
            if (nLock)
            {
                // Increment number of waiters
                if (atomic_swap(&nLock, 0)) // Lock succeeded ?
                {
                    if (!(nLocks++))
                        nThreadId       = pthread_self(); // Save thread identifier
                    return true;
                }
            }

            // Check that we already own the mutex
            if (nThreadId == pthread_self())
            {
                ++nLocks;
                return true;
            }

            return false;
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
