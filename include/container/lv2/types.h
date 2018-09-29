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
    typedef struct lv2_path_t: public path_t
    {
        enum flags_t
        {
            S_EMPTY,
            S_PENDING,
            S_ACCEPTED
        };

        atomic_t    nRequest;
        size_t      nState;
        bool        bRequest;
        char        sPath[PATH_MAX];
        char        sRequest[PATH_MAX];

        virtual void init()
        {
            atomic_init(nRequest);
            nState      = S_EMPTY;
            bRequest    = false;
            sPath[0]    = '\0';
            sRequest[0] = '\0';
        }

        virtual const char *get_path()
        {
            return sPath;
        }

        virtual void accept()
        {
            if (nState != S_PENDING)
                return;
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
                strcpy(sPath, sRequest);
                sPath[PATH_MAX-1]   = '\0';
                sRequest[0]         = '\0';
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

        void submit(const char *path, size_t len)
        {
            // Determine size of path
            size_t count = (len >= PATH_MAX) ? PATH_MAX - 1 : len;

            // Wait until the queue is empty
            struct timespec spec = { 0, 1 * 1000 * 1000 }; // 1 msec
            while (true)
            {
                // Try to acquire critical section, this will always be true when using LV2 atom transport
                if (atomic_trylock(nRequest))
                {
                    // Copy data to request
                    memcpy(sRequest, path, count);
                    sRequest[count]     = '\0';
                    bRequest            = true; // Mark request pending

                    // Release critical section
                    atomic_unlock(nRequest);

                    // Leave the cycle
                    break;
                }

                // Wait for a while, this won't happen when lv2
                nanosleep(&spec, NULL);
            }
        }

    } lv2_path_t;
}

#endif /* CONTAINER_LV2_TYPES_H_ */
