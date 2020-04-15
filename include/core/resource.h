/*
 * resource.h
 *
 *  Created on: 14 июл. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_RESOURCE_H_
#define CORE_RESOURCE_H_

#include <core/types.h>

#ifndef LSP_RESOURCE_PATH
    #define LSP_RESOURCE_PATH           "res"
#endif

#define LSP_BUILTIN_PREFIX          "builtin://"
#define LSP_BUILTIN_PREFIX_LEN      10

namespace lsp
{
    namespace resource
    {
        enum resource_type_t
        {
            RESOURCE_UNKNOWN,
            RESOURCE_XML,
            RESOURCE_3D_SCENE,
            RESOURCE_PRESET,
            RESOURCE_JSON
        };

        typedef struct resource_t
        {
            const char         *id;
            const void         *data;
            int                 type;
        } resource_t;

        extern const char          *string_dictionary;
        extern const float          float_dictionary[];
        extern const resource_t     builtin_resources[];

        const resource_t   *get(const char *id, resource_type_t type);
        const resource_t   *all();

        const char         *fetch_dstring(const void **ptr);
        float               fetch_dfloat(const void **ptr);
        uint64_t            fetch_number(const void **ptr);
        uint8_t             fetch_byte(const void **ptr);
        uint8_t             get_byte(const void **ptr);
        ssize_t             fetch_bytes(void *dst, const void **ptr, size_t count);
        ssize_t             skip_bytes(const void **ptr, size_t count);
    }
}

#define BUILTIN_RESOURCES_STUB_DEF \
    namespace lsp \
    { \
        namespace resource \
        { \
            const resource_t builtin_resources[] = \
            { \
                { NULL, NULL, RESOURCE_UNKNOWN } \
            }; \
            \
            const char *string_dictionary = ""; \
            \
            const float float_dictionary[] = { 0.0f }; \
        } \
    }

// This macro should be used for binaries that do not need builtin resources
#ifdef LSP_BUILTIN_RESOURCES
    #define BUILTIN_RESOURCES_STUB BUILTIN_RESOURCES_STUB_DEF
#else
    #define BUILTIN_RESOURCES_STUB
#endif

#endif /* CORE_RESOURCE_H_ */
