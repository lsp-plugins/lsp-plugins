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
    inline vst_object_t *vst_object(AEffect *e)
    {
        vst_object_t *o = reinterpret_cast<vst_object_t *>(e->object);
        if ((o != NULL) && (o->magic == LSP_VST_USER_MAGIC))
            return o;
        return NULL;
    }

    inline const plugin_metadata_t *vst_metadata(AEffect *e)
    {
        vst_object_t *u = vst_object(e);
        return (u != NULL) ? u->metadata : NULL;
    }

    inline plugin_t *vst_plugin(AEffect *e)
    {
        vst_object_t *u = vst_object(e);
        return (u != NULL) ? u->plugin : NULL;
    }

    inline int vst_write_string(AEffect *e, VstInt32 code, size_t maxbytes, const char *value)
    {
        // Get user data
        vst_object_t *u   = vst_object(e);
        if (u == NULL)
            return -1;

        // Check string length
        size_t len      = ::strlen(value);
        if (len < maxbytes)
            return (u->master(e, code, 0, 0, const_cast<char *>(value), 0) != 0) ? (len + 1) : 0;

        // Allocate string and copy data
        char *tmp       = reinterpret_cast<char *>(alloca(maxbytes));
        memcpy(tmp, value, maxbytes - 1);
        tmp[maxbytes-1]   = '\0';

        return (u->master(e, code, 0, 0, tmp, 0) != 0) ? maxbytes : 0;
    }
}

#endif /* _CONTAINER_VST_HELPERS_H_ */
