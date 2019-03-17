/*
 * Thread.h
 *
 *  Created on: 25 февр. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_IPC_THREAD_H_
#define INCLUDE_CORE_IPC_THREAD_H_

#include <core/types.h>
#include <core/status.h>

#if defined(PLATFORM_WINDOWS)
    #include <processthreadsapi.h>
#else
    #include <pthread.h>
#endif /* PLATFORM_WINDOWS */

#include <core/ipc/IRunnable.h>

namespace lsp
{
    namespace ipc
    {
        enum thread_state_t
        {
            TS_CREATED,
            TS_PENDING,
            TS_RUNNING,
            TS_FINISHED
        };

        /**
         * Thread procedure that can be launched
         * @param arg
         * @return
         */
        typedef status_t (* thread_proc_t)(void *arg);

        /**
         * Thread class
         */
        class Thread: public IRunnable
        {
            protected:
                typedef struct binding_t
                {
                    thread_proc_t   proc;
                    union
                    {
                        IRunnable      *runnable;
                        void           *arg;
                    };
                } binding_t;

            private:
                static __thread Thread     *pThis;
                volatile int                enState;
                volatile bool               bCancelled;
                volatile status_t           nResult;

#if defined(PLATFORM_WINDOWS)
                HANDLE                      hThread;        // Windows threads
#else
                pthread_t                   hThread;        // POSIX threads
#endif  /* PLATFORM_WINDOWS */

            protected:
                binding_t                   sBinding;

            private:
#if defined(PLATFORM_WINDOWS)
                static DWORD WINAPI thread_launcher(_In_ LPVOID lpParameter);
#else
                static void *thread_launcher(void *arg);
#endif /* PLATFORM_WINDOWS */

                Thread & operator = (const Thread &src);    // Deny copying

            public:
                explicit Thread();
                explicit Thread(thread_proc_t proc);
                explicit Thread(thread_proc_t proc, void *arg);
                explicit Thread(IRunnable *runnable);

                virtual ~Thread();

            public:
                /**
                 * The thread's main execution method
                 * @return status of thread execution
                 */
                virtual status_t run();

                /**
                 * Launch the created thread
                 * @return status of operation
                 */
                status_t start();

                /**
                 * Send cancel request to the thread
                 * @return status of operation
                 */
                status_t cancel();

                /**
                 * Wait thread for completion
                 * @return status of operation
                 */
                status_t join();

                /**
                 * Force current thread to leep for amount of milliseconds
                 * @param millis
                 */
                static status_t sleep(wsize_t millis);

                /**
                 * Return the current thread
                 * @return current thread or NULL if current thread is not an instance of ipc::Thread class
                 */
                static inline Thread *current() { return pThis; }

                /** Check that cancellation signal has been delivered to the current thread
                 * @return true if cancellation signal is pending
                 */
                static inline bool is_cancelled() { return (pThis != NULL) ? pThis->bCancelled : false; };

                /** Check that cancellation signal has been delivered to the thread
                 * @return true if thread has been cancelled
                 */
                inline bool cancelled() const { return bCancelled; };

                /**
                 * Check whether thread has finished
                 * @return true if thread has finished
                 */
                inline bool finished() const { return enState == TS_FINISHED; }

                /**
                 * Get thread state
                 * @return thread state
                 */
                inline thread_state_t state() const { return thread_state_t(enState); };

                /**
                 * Return the execution result of the thread
                 * @return execution result of the thread
                 */
                status_t get_result() const { return (enState == TS_FINISHED) ? nResult : STATUS_BAD_STATE; };

                /**
                 * Return number of execution cores supported by the system
                 * @return number of logical CPUs in the system available for processing
                 */
                static size_t system_cores();
        };
    
    } /* namespace ipc */
} /* namespace lsp */

#endif /* INCLUDE_CORE_IPC_THREAD_H_ */
