/*
 * types.h
 *
 *  Created on: 01 апр. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_VST_TYPES_H_
#define CONTAINER_VST_TYPES_H_

namespace lsp
{
    typedef struct vst_path_t: public path_t
    {
        enum flags_t
        {
            F_PENDING       = 1 << 0,
            F_ACCEPTED      = 1 << 1
        };

        atomic_t    nDspRequest;
        atomic_t    nDspSerial;
        atomic_t    nDspCommit;
        atomic_t    nUiSerial;
        atomic_t    nUiCommit;

        size_t      nFlags;

        char        sPath[PATH_MAX];
        char        sDspRequest[PATH_MAX];
        char        sUiPath[PATH_MAX];

        virtual void init()
        {
            atomic_init(nDspRequest);
            nDspSerial      = 0;
            nDspCommit      = 0;
            nUiSerial       = 0;
            nUiCommit       = 0;

            nFlags          = 0;

            sPath[0]        = '\0';
            sDspRequest[0]  = '\0';
            sUiPath[0]      = '\0';
        }

        virtual const char *get_path()
        {
            return sPath;
        }

        virtual void accept()
        {
            if (nFlags & F_PENDING)
                nFlags     |= F_ACCEPTED;
        }

        virtual void commit()
        {
            if (nFlags & (F_PENDING | F_ACCEPTED))
                nFlags      = 0;
        }

        virtual bool pending()
        {
            // Check accepted flags
            if (nFlags & F_PENDING)
                return !(nFlags & F_ACCEPTED);

            // Check for pending request
            if (atomic_trylock(nDspRequest))
            {
                // Update state of the DSP
                if (nDspSerial != nDspCommit)
                {
                    // Copy the data
                    strcpy(sPath, sDspRequest);
                    nFlags          = F_PENDING;

                    // Update serial(s)
                    nUiSerial       ++;
                    nDspCommit      ++;
                }

                atomic_unlock(nDspRequest);
            }

            return (nFlags & F_PENDING);
        }

        virtual bool accepted()
        {
            return nFlags & F_ACCEPTED;
        }

        void submit(const char *path, size_t len, bool ui)
        {
            // Determine size of path
            size_t count = (len >= PATH_MAX) ? PATH_MAX - 1 : len;

            // Wait until the queue is empty
            if (ui)
            {
                struct timespec spec = { 0, 1 * 1000 * 1000 }; // 1 msec
                while (true)
                {
                    // Try to acquire critical section
                    if (atomic_trylock(nDspRequest))
                    {
                        // Write DSP request
                        memcpy(sDspRequest, path, count);
                        sDspRequest[count]  = '\0';
                        nDspSerial          ++;

                        // Release critical section
                        atomic_unlock(nDspRequest);

                        // Leave the cycle
                        break;
                    }

                    // Wait for a while
                    nanosleep(&spec, NULL);
                }
            }
            else
            {
                // Write DSP request
                memcpy(sDspRequest, path, count);
                sDspRequest[count]  = '\0';
                nDspSerial          ++;
            }
        }

        bool ui_sync()
        {
            if (!atomic_trylock(nDspRequest))
                return false;
            bool sync = (nUiSerial != nUiCommit);
            if (sync)
            {
                strcpy(sUiPath, sPath);
                nUiCommit++;
            }
            atomic_unlock(nDspRequest);

            return sync;
        }

    } vst_path_t;
}

#endif /* CONTAINER_VST_TYPES_H_ */
