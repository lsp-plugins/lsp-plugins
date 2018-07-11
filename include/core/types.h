/*
 * types.h
 *
 *  Created on: 26 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_TYPES_H_
#define CORE_TYPES_H_

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __linux__
    #include <linux/limits.h>
#endif /* __linux__ */

// For IDEs: define this symbol in IDE to properly compile and debug
#ifdef LSP_IDE_DEBUG
    #define LSP_USE_EXPAT
    //#define LSP_HOST_SIMULATION
#endif /* LSP_IDE_DEBUG */

#define __ASM_EMIT(code)                    code "\n\t"
#ifdef __i386__
    #define __ASM_EMIT32(code)                  code "\n\t"
    #define __IF_32(...)                        __VA_ARGS__
#endif /* __i386__ */

#ifdef __x86_64__
    #define __ASM_EMIT64(code)                  code "\n\t"
    #define __IF_64(...)                        __VA_ARGS__
#endif

#ifndef __ASM_EMIT32
    #define __ASM_EMIT32(code)
#endif /* __ASM_EMIT64 */

#ifndef __ASM_EMIT64
    #define __ASM_EMIT64(code)
#endif /* __ASM_EMIT64 */

#ifndef __IF_32
    #define __IF_32(...)
#endif /* __IF_32 */

#ifndef __IF_64
    #define __IF_64(...)
#endif /* __IF_64 */


#define __lsp_forced_inline                 __attribute__((always_inline))
#define __lsp_aligned16                     __attribute__ ((aligned (16)))
#define __lsp_aligned64                     __attribute__ ((aligned (64)))

// Include units
#include <core/sugar.h>
#include <core/units.h>
#include <core/characters.h>

namespace lsp
{
    typedef __uint32_t      uint32_t;

    enum mesh_state_t
    {
        M_WAIT,         // Mesh is waiting for data request
        M_EMPTY,        // Mesh is empty
        M_DATA          // Mesh contains data
    };

    // Mesh port structure
    typedef struct mesh_t
    {
        mesh_state_t    nState;
        size_t          nBuffers;
        size_t          nItems;
        float          *pvData[];

        inline bool isEmpty() const         { return nState == M_EMPTY; };
        inline bool containsData() const    { return nState == M_DATA; };
        inline bool isWaiting() const       { return nState == M_WAIT;  };

        inline void data(size_t bufs, size_t items)
        {
            nBuffers    = bufs;
            nItems      = items;
            nState      = M_DATA; // This should be the last operation
        }

        inline void cleanup()
        {
            nBuffers    = 0;
            nItems      = 0;
            nState      = M_EMPTY; // This should be the last operation
        }

        inline void setWaiting()
        {
            nState      = M_WAIT; // This should be the last operation
        }
    } mesh_t;

    // Path port structure
    typedef struct path_t
    {
        /** Virtual destructor
         *
         */
        virtual ~path_t();

        /** Initialize path
         *
         */
        virtual void init();

        /** Get actual path
         *
         * @return actual path
         */
        virtual const char *get_path();

        /** Accept the pending request for path change,
         * the port of the path will not trigger as changed
         * until commit() is called
         */
        virtual void accept();

        /** The state change request was processed,
         * the port is ready to receive new events
         *
         */
        virtual void commit();

        /** Check if there is pending request
         *
         * @return true if there is a pending state-change request
         */
        virtual bool pending();

        /** Check if there is accepted request
         *
         * @return true if there is accepted request
         */
        virtual bool accepted();

    } path_t;

    typedef struct resource_t
    {
        const char *id;
        const char *text;
    } resource_t;
}

#endif /* CORE_TYPES_H_ */
