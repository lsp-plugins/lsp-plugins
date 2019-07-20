/*
 * types.h
 *
 *  Created on: 02 февр. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_TYPES_H_
#define CONTAINER_LV2_TYPES_H_

namespace lsp
{
    enum
    {
        LSP_LV2_PRIVATE     = 1 << 0
    };

    typedef struct lv2_path_t: public path_t
    {
        enum flags_t
        {
            S_EMPTY,
            S_PENDING,
            S_ACCEPTED
        };

        atomic_t    nRequest;
        atomic_t    nChanges;
        size_t      nState;
        size_t      nFlags;
        bool        bRequest;
        size_t      sFlags;
        char        sPath[PATH_MAX];
        char        sRequest[PATH_MAX];

        virtual void init()
        {
            atomic_init(nRequest);
            nState      = S_EMPTY;
            nFlags      = 0;
            bRequest    = false;
            sFlags      = 0;
            sPath[0]    = '\0';
            sRequest[0] = '\0';
        }

        virtual const char *get_path()
        {
            return sPath;
        }

        virtual size_t get_flags()
        {
            return nFlags;
        }

        virtual void accept()
        {
            if (nState != S_PENDING)
                return;
            atomic_add(&nChanges, 1);
            nState  = S_ACCEPTED;
        }

        virtual void commit()
        {
            if (nState != S_ACCEPTED)
                return;
            nState  = S_EMPTY;
        }

        virtual bool pending()
        {
            // Check accepted state
            if (nState == S_PENDING)
                return true;
            else if ((nState != S_EMPTY) || (!bRequest))
                return false;

            // Move pending request to path if present,
            // do it in synchronized mode
            if (atomic_trylock(nRequest))
            {
                // Copy the data
                ::strncpy(sPath, sRequest, PATH_MAX);
                sPath[PATH_MAX-1]   = '\0';
                sRequest[0]         = '\0';
                nFlags              = sFlags;
                sFlags              = 0;
                bRequest            = false;
                nState              = S_PENDING;

                atomic_unlock(nRequest);
            }

            return nState == S_PENDING;
        }

        virtual bool accepted()
        {
            return (nState == S_ACCEPTED);
        }

        /**
         * This is non-RT-safe method to submit new path value to the RT thread
         * @param path path string to submit
         * @param len length of the path string
         * @param flags additional flags
         */
        void submit(const char *path, size_t len, size_t flags = 0)
        {
            // Determine size of path
            size_t count = (len >= PATH_MAX) ? PATH_MAX - 1 : len;

            // Wait until the queue is empty
            while (true)
            {
                // Try to acquire critical section, this will always be true when using LV2 atom transport
                if (atomic_trylock(nRequest))
                {
                    // Copy data to request
                    ::memcpy(sRequest, path, count);
                    sRequest[count]     = '\0';
                    sFlags              = flags;
                    bRequest            = true; // Mark request pending

                    // Release critical section and leave the cycle
                    atomic_unlock(nRequest);
                    break;
                }

                // Wait for a while, this won't happen when lv2
                ipc::Thread::sleep(10);
            }
        }

    } lv2_path_t;
}

#endif /* CONTAINER_LV2_TYPES_H_ */
