/*
 * types.h
 *
 *  Created on: 18 мая 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_JACK_TYPES_H_
#define CONTAINER_JACK_TYPES_H_

namespace lsp
{
    typedef struct jack_path_t: public path_t
    {
        enum flags_t
        {
            F_PENDING       = 1 << 0,
            F_ACCEPTED      = 1 << 1
        };

        atomic_t    nRequest;
        atomic_t    nSerial;
        atomic_t    nCommit;

        size_t      nFlags;
        size_t      nXFlags;
        size_t      nXFlagsReq;

        char        sPath[PATH_MAX];
        char        sRequest[PATH_MAX];

        virtual void init()
        {
            atomic_init(nRequest);
            nSerial         = 0;
            nCommit         = 0;

            nFlags          = 0;
            nXFlags         = 0;
            nXFlagsReq      = 0;

            sPath[0]        = '\0';
            sRequest[0]     = '\0';
        }

        virtual const char *get_path()
        {
            return sPath;
        }

        virtual size_t get_flags()
        {
            return nXFlags;
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

            // Check for pending change
            if (atomic_trylock(nRequest))
            {
                // Update state of the DSP
                if (nSerial != nCommit)
                {
                    // Copy the data
                    ::strncpy(sPath, sRequest, PATH_MAX);
                    sPath[PATH_MAX-1]   = '\0';
                    nFlags              = F_PENDING;
                    nXFlags             = nXFlagsReq;
                    nXFlagsReq          = 0;

                    // Update commit
                    nCommit             ++;
                }

                atomic_unlock(nRequest);
            }

            return (nFlags & F_PENDING);
        }

        virtual bool accepted()
        {
            return nFlags & F_ACCEPTED;
        }

        void submit(const char *path, size_t flags)
        {
            // Wait until the queue is empty
            while (true)
            {
                // Try to acquire critical section
                if (atomic_trylock(nRequest))
                {
                    // Write request to DSP
                    ::strcpy(sRequest, path);
                    nXFlagsReq      = flags;
                    nSerial         ++;

                    // Release critical section and leave the cycle
                    atomic_unlock(nRequest);
                    break;
                }

                // Wait for a while (10 milliseconds)
                ipc::Thread::sleep(10);
            }
        }

    } jack_path_t;

    inline mesh_t *jack_create_mesh(const port_t *meta)
    {
        size_t buffers      = meta->step;
        size_t buf_size     = meta->start * sizeof(float);
        size_t mesh_size    = sizeof(mesh_t) + sizeof(float *) * buffers;

        // Align values to 64-byte boundaries
        buf_size            = ALIGN_SIZE(buf_size, 0x40);
        mesh_size           = ALIGN_SIZE(mesh_size, 0x40);

        // Allocate pointer
        uint8_t *ptr        = reinterpret_cast<uint8_t *>(lsp_malloc(mesh_size + buf_size * buffers));
        if (ptr == NULL)
            return NULL;

        // Initialize references
        mesh_t *mesh        = reinterpret_cast<mesh_t *>(ptr);
        mesh->nState        = M_EMPTY;
        mesh->nBuffers      = 0;
        mesh->nItems        = 0;
        ptr                += mesh_size;
        for (size_t i=0; i<buffers; ++i)
        {
            mesh->pvData[i]    = reinterpret_cast<float *>(ptr);
            ptr                += buf_size;
        }

        return mesh;
    }

    inline void jack_destroy_mesh(mesh_t *mesh)
    {
        if (mesh != NULL)
        {
            lsp_free(mesh);
            mesh = NULL;
        }
    }
}

#endif /* CONTAINER_JACK_TYPES_H_ */
