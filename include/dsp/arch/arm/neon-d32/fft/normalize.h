/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 18 дек. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FFT_NORMALIZE_H_
#define DSP_ARCH_ARM_NEON_D32_FFT_NORMALIZE_H_

namespace neon_d32
{
    void normalize_fft3(float *dre, float *dim, const float *re, const float *im, size_t rank)
    {
        IF_ARCH_ARM(
            float k = 1.0f/(1 << rank);
            size_t count = 1 << rank;
        );
        ARCH_ARM_ASM(
            // x8 blocks
            __ASM_EMIT("vdup.32     q0, %y[k]")                 // q0   = k
            __ASM_EMIT("subs        %[count], %[count], $8")
            __ASM_EMIT("vmov        q1, q0")
            __ASM_EMIT("blt         2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[s_re]!, {q4-q5}")
            __ASM_EMIT("vldm        %[s_im]!, {q6-q7}")
            __ASM_EMIT("vmul.f32    q4, q4, q0")
            __ASM_EMIT("vmul.f32    q5, q5, q1")
            __ASM_EMIT("vmul.f32    q6, q6, q0")
            __ASM_EMIT("vmul.f32    q7, q7, q1")
            __ASM_EMIT("subs        %[count], %[count], $8")
            __ASM_EMIT("vstm        %[d_re]!, {q4-q5}")
            __ASM_EMIT("vstm        %[d_im]!, {q6-q7}")
            __ASM_EMIT("bhs         1b")
            __ASM_EMIT("2:")
            : [s_re] "+r" (re), [s_im] "+r" (im),
              [d_re] "+r" (dre), [d_im] "+r" (dim),
              [k] "+t" (k), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1",
              "q4", "q5", "q6", "q7"
        );
    }

    void normalize_fft2(float *re, float *im, size_t rank)
    {
        IF_ARCH_ARM(
            float k = 1.0f/(1 << rank);
            size_t count = 1 << rank;
        );
        ARCH_ARM_ASM(
            // x8 blocks
            __ASM_EMIT("vdup.32     q0, %y[k]")                 // q0   = k
            __ASM_EMIT("subs        %[count], %[count], $8")
            __ASM_EMIT("vmov        q1, q0")
            __ASM_EMIT("blt         2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[d_re], {q4-q5}")
            __ASM_EMIT("vldm        %[d_im], {q6-q7}")
            __ASM_EMIT("vmul.f32    q4, q4, q0")
            __ASM_EMIT("vmul.f32    q5, q5, q1")
            __ASM_EMIT("vmul.f32    q6, q6, q0")
            __ASM_EMIT("vmul.f32    q7, q7, q1")
            __ASM_EMIT("subs        %[count], %[count], $8")
            __ASM_EMIT("vstm        %[d_re]!, {q4-q5}")
            __ASM_EMIT("vstm        %[d_im]!, {q6-q7}")
            __ASM_EMIT("bhs         1b")
            __ASM_EMIT("2:")
            : [d_re] "+r" (re), [d_im] "+r" (im),
              [k] "+t" (k), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1",
              "q4", "q5", "q6", "q7"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FFT_NORMALIZE_H_ */
