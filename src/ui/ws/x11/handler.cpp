/*
 * handler.cpp
 *
 *  Created on: 23 авг. 2019 г.
 *      Author: sadko
 */

#include <ui/ws/x11/ws.h>

namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            void X11_STUB_HANDLER_X64()
            {
                __asm__ __volatile__ (
                    "mov    %%rsi, %%rdx\n\t"
                    "mov    %%rdi, %%rsi\n\t"
                    "mov    $0x123456789abcdef0, %%rdi\n\t"
                    "mov    $0xfedcba0123456789, %%rax\n\t"
                    "jmp   *%%rax"
                    : : :
                );
            }

#if defined(ARCH_X86_64)
            static const uint8_t call_stub[] =
            {
                0x48, 0x89, 0xf2,                                               //  mov         %rsi,%rdx
                0x48, 0x89, 0xfe,                                               //  mov         %rdi,%rsi
                0x48, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //  movabs      0, %rdi
                0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //  movabs      0, %rax
                0xff, 0xe0                                                      //  jmpq        *%rax
            };

            bool link_x11_stub_function(uint8_t *dst, X11Display *x11dpy, x11_error_handler_t handler)
            {
                dst += 8;
                *(reinterpret_cast<uint64_t *>(&dst[8]))  = uint64_t(x11dpy);
                dst += 10;
                *(reinterpret_cast<uint64_t *>(&dst[18]))  = uint64_t(handler);
            }
#elif defined(ARCH_I386)
            static const uint8_t call_stub[] =
            {
                0x48, 0x89, 0xf2,                                               //  mov         %rsi,%rdx
                0x48, 0x89, 0xfe,                                               //  mov         %rdi,%rsi
                0x48, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //  movabs      0, %rdi
                0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //  movabs      0, %rax
                0xff, 0xe0                                                      //  jmpq        *%rax
            };
#else
            static const uint8_t call_stub[] = {};

            bool link_x11_stub_function(uint8_t *dst, X11Display *x11dpy, x11_error_handler_t handler)
            {
                return false;
            }
#endif

            XErrorHandler alloc_x11_error_handler(X11Display *x11dpy, x11_error_handler_t handler)
            {
                return NULL;
            }

            void free_x11_error_handler(XErrorHandler handler)
            {
            }
        }
    }
}
