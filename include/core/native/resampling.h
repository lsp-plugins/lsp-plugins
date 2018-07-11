/*
 * resampling.h
 *
 *  Created on: 19 нояб. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_NATIVE_RESAMPLING_H_
#define CORE_NATIVE_RESAMPLING_H_

namespace lsp
{
    namespace native
    {
        void lanczos_resample_2x2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float s     = *(src++);

                // Apply Lanczos 2x2 kernel
                dst[1]     -= 0.1664152316035080f * s;
                dst[3]     += 0.6203830132406946f * s;
                dst[4]     += s;
                dst[5]     += 0.6203830132406946f * s;
                dst[7]     -= 0.1664152316035080f * s;

                // Move destination buffer
                dst        += 2;
            }
        }

        void lanczos_resample_2x3(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float s     = *(src++);

                // Apply Lanczos 2x3 kernel
                dst[1]     += 0.0939539981090991f * s;
                dst[3]     -= 0.1910530560835854f * s;
                dst[5]     += 0.6293724479752082f * s;
                dst[6]     += s;
                dst[7]     += 0.6293724479752082f * s;
                dst[9]     -= 0.1910530560835854f * s;
                dst[11]    += 0.0939539981090991f * s;

                // Move destination buffer
                dst        += 2;
            }
        }

        void lanczos_resample_3x2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float s     = *(src++);

                // Apply Lanczos 3x2 kernel
                dst[1]     -= 0.1451906347823569f * s;
                dst[2]     -= 0.1903584501504231f * s;
                dst[4]     += 0.4051504629060886f * s;
                dst[5]     += 0.8228011237053413f * s;
                dst[6]     += s;
                dst[7]     += 0.8228011237053413f * s;
                dst[8]     += 0.4051504629060886f * s;
                dst[10]    -= 0.1903584501504231f * s;
                dst[11]    -= 0.1451906347823569f * s;

                // Move destination buffer
                dst        += 3;
            }
        }

        void lanczos_resample_3x3(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float s     = *(src++);

                // Apply Lanczos 3x3 kernel
                dst[1]     += 0.0890793429479492f * s;
                dst[2]     += 0.1055060549370832f * s;
                dst[4]     -= 0.1562250559899557f * s;
                dst[5]     -= 0.1993645686793863f * s;
                dst[7]     += 0.4097746665281485f * s;
                dst[8]     += 0.8251285571768335f * s;
                dst[9]     += s;
                dst[10]    += 0.8251285571768335f * s;
                dst[11]    += 0.4097746665281485f * s;
                dst[13]    -= 0.1993645686793863f * s;
                dst[14]    -= 0.1562250559899557f * s;
                dst[16]    += 0.1055060549370832f * s;
                dst[17]    += 0.0890793429479492f * s;

                // Move destination buffer
                dst        += 3;
            }
        }

        void lanczos_resample_4x2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float s     = *(src++);

                // Apply Lanczos 4x2 kernel
                dst[1]     -= 0.1187292496637064f * s;
                dst[2]     -= 0.2001458972657618f * s;
                dst[3]     -= 0.1729186314209981f * s;
                dst[5]     += 0.2957854651930789f * s;
                dst[6]     += 0.6325370350028462f * s;
                dst[7]     += 0.8988707620097378f * s;
                dst[8]     += s;
                dst[9]     += 0.8988707620097378f * s;
                dst[10]    += 0.6325370350028462f * s;
                dst[11]    += 0.2957854651930789f * s;
                dst[13]    -= 0.1729186314209981f * s;
                dst[14]    -= 0.2001458972657618f * s;
                dst[15]    -= 0.1187292496637064f * s;

                // Move destination buffer
                dst        += 4;
            }
        }

        void lanczos_resample_4x3(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float s     = *(src++);

                // Apply Lanczos 4x3 kernel
                dst[1]     += 0.0749573750027847f * s;
                dst[2]     += 0.1184264228024920f * s;
                dst[3]     += 0.0943496807888575f * s;
                dst[5]     -= 0.1241641556633335f * s;
                dst[6]     -= 0.2067943377468982f * s;
                dst[7]     -= 0.1768665375130828f * s;
                dst[9]     += 0.2981808178680967f * s;
                dst[10]    += 0.6348032780190205f * s;
                dst[11]    += 0.8996736756130924f * s;
                dst[12]    += s;
                dst[13]    += 0.8996736756130924f * s;
                dst[14]    += 0.6348032780190205f * s;
                dst[15]    += 0.2981808178680967f * s;
                dst[17]    -= 0.1768665375130828f * s;
                dst[18]    -= 0.2067943377468982f * s;
                dst[19]    -= 0.1241641556633335f * s;
                dst[21]    += 0.0943496807888575f * s;
                dst[22]    += 0.1184264228024920f * s;
                dst[23]    += 0.0749573750027847f * s;

                // Move destination buffer
                dst        += 4;
            }
        }

        void downsample_2x(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                *dst    = *src;
                dst     ++;
                src     += 2;
            }
        }

        void downsample_3x(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                *dst    = *src;
                dst     ++;
                src     += 3;
            }
        }

        void downsample_4x(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                *dst    = *src;
                dst     ++;
                src     += 4;
            }
        }
    }
}

#endif /* CORE_NATIVE_RESAMPLING_H_ */
