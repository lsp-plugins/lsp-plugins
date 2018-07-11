/*
 * defs.h
 *
 *  Created on: 30 дек. 2015 г.
 *      Author: sadko
 */

#ifndef _CONTAINER_VST_HELPERS_H_
#define _CONTAINER_VST_HELPERS_H_

// Some helper functions
namespace lsp
{
    inline mesh_t *vst_create_mesh(const port_t *meta)
    {
        size_t buffers      = meta->step;
        size_t buf_size     = meta->start * sizeof(float);
        size_t mesh_size    = sizeof(mesh_t) + sizeof(float *) * buffers;

        // Align values to 64-byte boundaries
        buf_size            = ALIGN_SIZE(buf_size, 0x40);
        mesh_size           = ALIGN_SIZE(mesh_size, 0x40);

        // Allocate pointer
        uint8_t *ptr        = new uint8_t[mesh_size + buf_size * buffers];
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
            mesh->pvData[i]     = reinterpret_cast<float *>(ptr);
            ptr                += buf_size;
        }

        return mesh;
    }

    inline void vst_destroy_mesh(mesh_t *mesh)
    {
        if (mesh != NULL)
            delete [] reinterpret_cast<uint8_t *>(mesh);
    }

    inline ssize_t vst_serialize_string(const char *str, uint8_t *buf, size_t len)
    {
        lsp_trace("str=%s, buf=%p, len=%d", str, buf, int(len));
        size_t slen     = strlen(str);
        if (slen > 0xff)
            slen            = 0xff;
        if ((slen + 1) > len)
            return -1;
        *(buf++)        = slen;
        memcpy(buf, str, slen);
        return slen + 1;
    }

    inline ssize_t vst_deserialize_string(char *str, size_t maxlen, const uint8_t *buf, size_t len)
    {
        lsp_trace("str=%p, maxlen=%d, buf=%p, len=%d", str, int(maxlen), buf, int(len));
        if ((len--) <= 0)
            return -1;
        size_t slen     = *(buf++);
        if (slen > len)
            return -1;
        if ((slen + 1) > maxlen)
            return -2;
        memcpy(str, buf, slen);
        str[slen]       = '\0';

        lsp_trace("str=%s", str);
        return slen + 1;
    }
}

#endif /* _CONTAINER_VST_HELPERS_H_ */
