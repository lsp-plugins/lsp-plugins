/*
 * alloc.cpp
 *
 *  Created on: 25 апр. 2016 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <core/sugar.h>
#include <string.h>

#if defined(LSP_DEBUG) && defined(LSP_MEMORY_PROFILING)

#define SAFE_HDR_SIZE           ALIGN_SIZE(sizeof(header_t), DEFAULT_ALIGN)
#define SAFE_TAIL_SIZE          ALIGN_SIZE(sizeof(tail_t), DEFAULT_ALIGN)
#define SAFE_MAGIC              uint32_t(0x5a5a5a5a)

// Override operator new, new[], delete, delete[]

void *operator new(size_t size)
{
    return lsp::safe_malloc(__lsp_location, size, LSP_ALLOC_MODE_NEW);
}

void *operator new[](size_t size)
{
    return lsp::safe_malloc(__lsp_location, size, LSP_ALLOC_MODE_ARRNEW);
}

void operator delete(void *ptr)
{
    lsp::safe_free(__lsp_location, ptr, LSP_ALLOC_MODE_NEW);
}

void operator delete[](void *ptr)
{
    lsp::safe_free(__lsp_location, ptr, LSP_ALLOC_MODE_ARRNEW);
}

namespace lsp
{
#pragma pack(push, 1)
    typedef struct header_t
    {
        size_t      size;
        uint32_t    mode;
        const char *location;
        uint32_t    magic;
    } header_t;

    typedef struct tail_t
    {
        uint32_t    magic;
    } tail_t;
#pragma pack(pop)

    void *safe_malloc(const char *location, size_t size, uint32_t mode)
    {
        size_t allocate     = SAFE_HDR_SIZE + SAFE_TAIL_SIZE + ALIGN_SIZE(size, DEFAULT_ALIGN);
        uint8_t *ptr        = reinterpret_cast<uint8_t *>(::malloc(allocate));
        if (ptr == NULL)
            return NULL;

        // Encapsulate the block
        ptr                += SAFE_HDR_SIZE;
        header_t *head      = reinterpret_cast<header_t *>(ptr - sizeof(header_t));
        tail_t *tail        = reinterpret_cast<tail_t *>(ptr + size);
        head->size          = size;
        head->mode          = mode;
        head->location      = location;
        head->magic         = SAFE_MAGIC;
        tail->magic         = SAFE_MAGIC;

        return ptr;
    }

    void *safe_calloc(const char *location, size_t count, size_t size)
    {
        size               *= count;
        size_t allocate     = SAFE_HDR_SIZE + SAFE_TAIL_SIZE + ALIGN_SIZE(size, DEFAULT_ALIGN);
        uint8_t *ptr        = reinterpret_cast<uint8_t *>(::malloc(allocate));
        if (ptr == NULL)
            return NULL;

        // Encapsulate the block
        ptr                += SAFE_HDR_SIZE;
        header_t *head      = reinterpret_cast<header_t *>(ptr - sizeof(header_t));
        tail_t *tail        = reinterpret_cast<tail_t *>(ptr + size);
        head->size          = size;
        head->mode          = LSP_ALLOC_MODE_CALLOC;
        head->location      = location;
        head->magic         = SAFE_MAGIC;
        tail->magic         = SAFE_MAGIC;

        // Clear the block as required by calloc
        ::bzero(ptr, size);

        return ptr;
    }

    void *safe_realloc(const char *location, void *ptr, size_t size, uint32_t mode)
    {
        // Work like real realloc
        if (size == 0)
        {
            safe_free(location, ptr, mode);
            return NULL;
        }
        else if (ptr == NULL)
            return safe_malloc(location, size, LSP_ALLOC_MODE_MALLOC);

        // Validate data buffer
        if (!safe_validate(location, ptr, mode))
            return NULL;

        // Re-allocate memory
        size_t allocate     = SAFE_HDR_SIZE + SAFE_TAIL_SIZE + ALIGN_SIZE(size, DEFAULT_ALIGN);
        uint8_t *block      = reinterpret_cast<uint8_t *>(ptr);
        uint8_t *new_block  = reinterpret_cast<uint8_t *>(::realloc(block - SAFE_HDR_SIZE, allocate));
        if (new_block == NULL)
            return NULL;

        // Encapsulate the block
        new_block          += SAFE_HDR_SIZE;

        header_t *head      = reinterpret_cast<header_t *>(new_block - sizeof(header_t));
        tail_t *tail        = reinterpret_cast<tail_t *>(new_block + size);
        head->size          = size;
        head->mode          = LSP_ALLOC_MODE_MALLOC;
        head->location      = location;
        head->magic         = SAFE_MAGIC;
        tail->magic         = SAFE_MAGIC;

        return new_block;
    }

    bool safe_free(const char *location, void *ptr, uint32_t mode)
    {
        // Validate the block
        if (!safe_validate(location, ptr, mode))
            return false;

        // Free the block
        uint8_t *block      = reinterpret_cast<uint8_t *>(ptr);
        ::free(block - SAFE_HDR_SIZE);
        return true;
    }

    bool safe_validate(const char *location, const void *ptr, uint32_t mode)
    {
        if (ptr == NULL)
        {
            lsp_error("MEMORY VIOLATION");
            lsp_error("Tried to validate invalid pointer:");
            lsp_error("  - The passed argument is NULL");
            lsp_error("  - Check performed at %s", location);
            return false;
        }

        const uint8_t *block    = reinterpret_cast<const uint8_t *>(ptr);
        const header_t *head    = reinterpret_cast<const header_t *>(block - sizeof(header_t));
        if (head->magic != SAFE_MAGIC)
        {
            lsp_error("MEMORY VIOLATION");
            lsp_error("Validation of pointer %p failed: ", ptr);
            lsp_error("  - Lower bound of the data block is corrupted (invalid magic number 0x%08x)", int(head->magic));
            lsp_error("  - Check performed at %s", location);
            lsp_error("  - Block originally allocated at %s", head->location);
            return false;
        }
        if ((head->mode & mode) == 0)
        {
            lsp_error("MEMORY VIOLATION");
            lsp_error("Validation of pointer %p failed: ", ptr);
            lsp_error("  - Pointer allocated with flags: 0x%02x", int(head->mode));
            lsp_error("  - Tried to validate with flags: 0x%02x", int(mode));
            lsp_error("  - Check performed at %s", location);
            lsp_error("  - Block originally allocated at %s", head->location);
            return false;
        }

        const tail_t *tail      = reinterpret_cast<const tail_t *>(block + head->size);
        if (tail->magic != SAFE_MAGIC)
        {
            lsp_error("MEMORY VIOLATION");
            lsp_error("Validation of pointer %p failed: ", ptr);
            lsp_error("  - Upper bound of the data block is corrupted (invalid magic number 0x%08x)", int(tail->magic));
            lsp_error("  - Check performed at %s", location);
            lsp_error("  - Block originally allocated at %s", head->location);
            return false;
        }

        return true;
    }

    char *safe_strdup(const char *location, const char *str)
    {
        size_t allocate     = strlen(str) + 1;
        char *dst           = reinterpret_cast<char *>(safe_malloc(location, allocate, LSP_ALLOC_MODE_MALLOC));
        if (dst == NULL)
            return NULL;
        memcpy(dst, str, allocate);
        return dst;
    }
}

#endif /* LSP_DEBUG */

namespace lsp
{
    char *lsp_strbuild(const char *str, size_t len)
    {
        char *ptr   = reinterpret_cast<char *>(lsp_malloc(sizeof(char)*(len + 1)));
        if (ptr == NULL)
            return NULL;
        ::memcpy(ptr, str, sizeof(char)*len);
        ptr[len]    = '\0';
        return ptr;
    }
}
