/*
 * resource.h
 *
 *  Created on: 14 июл. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_RESOURCE_H_
#define CORE_RESOURCE_H_

#include <core/types.h>

namespace lsp
{
    enum resource_type_t
    {
        RESOURCE_UNKNOWN,
        RESOURCE_XML,
        RESOURCE_3D_SCENE,
        RESOURCE_PRESET
    };

    typedef struct resource_t
    {
        const char         *id;
        const void         *data;
        int                 type;
    } resource_t;

    extern const char *string_dictionary;
    extern const float float_dictionary[];
    extern const resource_t builtin_resources[];

    const resource_t *resource_get(const char *id, resource_type_t type);
    const resource_t *resource_all();

    const char *resource_fetch_dstring(const void **ptr);
    float       resource_fetch_dfloat(const void **ptr);
    uint64_t    resource_fetch_number(const void **ptr);
    uint8_t     resource_fetch_byte(const void **ptr);
    uint8_t     resource_get_byte(const void **ptr);
    ssize_t     resource_fetch_bytes(void *dst, const void **ptr, size_t count);
    ssize_t     resource_skip_bytes(const void **ptr, size_t count);
}

// This macro should be used for binaries that do not need builtin resources
#define BUILTIN_RESOURCES_STUB \
    namespace lsp \
    { \
        const resource_t builtin_resources[] = \
        { \
            { NULL, NULL, RESOURCE_UNKNOWN } \
        }; \
        \
        const char *string_dictionary = ""; \
        \
        const float float_dictionary[] = { 0.0f }; \
    }

#endif /* CORE_RESOURCE_H_ */
