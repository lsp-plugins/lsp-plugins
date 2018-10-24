/*
 * resampling.h
 *
 *  Created on: 19 нояб. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_RESAMPLING_H_
#define DSP_ARCH_NATIVE_RESAMPLING_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

/*

    The Lanczos kernel can be evaluated by using this code:

    #include <stdio.h>
    #include <math.h>

    // Number of lobes
    #define KERNEL_SIZE     3
    // Oversampling times
    #define KERNEL_TIMES    8
    // Number of samples to generate
    #define KERNEL_MAX      64

    double kernel[KERNEL_MAX];

    int main(void)
    {
        ssize_t leaf = KERNEL_SIZE * KERNEL_TIMES;
        ssize_t dots = leaf * 2 + 1;

        printf("leaf=%d, dots=%d, kernel_size=%d, kernel_times=%d\n", int(leaf), int(dots), KERNEL_SIZE, KERNEL_TIMES);

        for (ssize_t i=0; i<KERNEL_MAX; ++i)
        {
            if (i >= dots)
                kernel[i] = 0.0;
            else if (i == leaf)
                kernel[i] = 1.0;
            else
            {
                double px = M_PI * (i - leaf) / KERNEL_TIMES;
                kernel[i] = leaf * sin(px) * sin(px / double(leaf)) / (px * px);
            }

            printf("%.16f\n", kernel[i]);
        }

        return 0;
    }

 */

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

    void lanczos_resample_6x2(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float s     = *(src++);

            // Apply Lanczos 6x2 kernel
            dst[1]     -= 0.0835168749111343f * s;
            dst[2]     -= 0.1602001404590001f * s;
            dst[3]     -= 0.2067943377468982f * s;
            dst[4]     -= 0.2025752314530442f * s;
            dst[5]     -= 0.1343073344351001f * s;

            dst[7]     += 0.1894744904080251f * s;
            dst[8]     += 0.4114005618526707f * s;
            dst[9]     += 0.6348032780190205f * s;
            dst[10]    += 0.8259440903283795f * s;
            dst[11]    += 0.9546266788436470f * s;

            dst[12]    += s;

            dst[13]    += 0.9546266788436470f * s;
            dst[14]    += 0.8259440903283795f * s;
            dst[15]    += 0.6348032780190205f * s;
            dst[16]    += 0.4114005618526707f * s;
            dst[17]    += 0.1894744904080251f * s;

            dst[19]    -= 0.1343073344351001f * s;
            dst[20]    -= 0.2025752314530442f * s;
            dst[21]    -= 0.2067943377468982f * s;
            dst[22]    -= 0.1602001404590001f * s;
            dst[23]    -= 0.0835168749111343f * s;

            // Move destination buffer
            dst        += 6;
        }
    }

    void lanczos_resample_6x3(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float s     = *(src++);

            // Apply Lanczos 6x3 kernel
            dst[1]     += 0.0539107649662794f * s;
            dst[2]     += 0.0996822843396933f * s;
            dst[3]     += 0.1233221256242928f * s;
            dst[4]     += 0.1149032936217488f * s;
            dst[5]     += 0.0717178866781924f * s;

            dst[7]     -= 0.0853379770956540f * s;
            dst[8]     -= 0.1630759564500400f * s;
            dst[9]     -= 0.2097908159917360f * s;
            dst[10]    -= 0.2048873332640742f * s;
            dst[11]    -= 0.1354777817419537f * s;

            dst[13]    += 0.1903132912211352f * s;
            dst[14]    += 0.4125642785884168f * s;
            dst[15]    += 0.6358120571822271f * s;
            dst[16]    += 0.8265269098929101f * s;
            dst[17]    += 0.9547949937819170f * s;

            dst[18]    += s;

            dst[19]    += 0.9547949937819170f * s;
            dst[20]    += 0.8265269098929101f * s;
            dst[21]    += 0.6358120571822271f * s;
            dst[22]    += 0.4125642785884168f * s;
            dst[23]    += 0.1903132912211352f * s;

            dst[25]    -= 0.1354777817419537f * s;
            dst[26]    -= 0.2048873332640742f * s;
            dst[27]    -= 0.2097908159917360f * s;
            dst[28]    -= 0.1630759564500400f * s;
            dst[29]    -= 0.0853379770956540f * s;

            dst[31]    += 0.0717178866781924f * s;
            dst[32]    += 0.1149032936217488f * s;
            dst[33]    += 0.1233221256242928f * s;
            dst[34]    += 0.0996822843396933f * s;
            dst[35]    += 0.0539107649662794f * s;

            // Move destination buffer
            dst        += 6;
        }
    }

    void lanczos_resample_8x2(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float s     = *(src++);

            // Apply Lanczos 8x2 kernel
            dst[1]     -= 0.0635086982303136f * s;
            dst[2]     -= 0.1261005767591211f * s;
            dst[3]     -= 0.1779172640206227f * s;
            dst[4]     -= 0.2091519082144436f * s;
            dst[5]     -= 0.2112875836092082f * s;
            dst[6]     -= 0.1782608868029105f * s;
            dst[7]     -= 0.1073988642554405f * s;

            dst[9]     += 0.1385297654836717f * s;
            dst[10]    += 0.2990219260297742f * s;
            dst[11]    += 0.4693478599070060f * s;
            dst[12]    += 0.6355976112274050f * s;
            dst[13]    += 0.7835048889119777f * s;
            dst[14]    += 0.8999547969760605f * s;
            dst[15]    += 0.9743975231790596f * s;

            dst[16]    += s;

            dst[17]    += 0.9743975231790596f * s;
            dst[18]    += 0.8999547969760605f * s;
            dst[19]    += 0.7835048889119777f * s;
            dst[20]    += 0.6355976112274050f * s;
            dst[21]    += 0.4693478599070060f * s;
            dst[22]    += 0.2990219260297742f * s;
            dst[23]    += 0.1385297654836717f * s;

            dst[25]    -= 0.1073988642554405f * s;
            dst[26]    -= 0.1782608868029105f * s;
            dst[27]    -= 0.2112875836092082f * s;
            dst[28]    -= 0.2091519082144436f * s;
            dst[29]    -= 0.1779172640206227f * s;
            dst[30]    -= 0.1261005767591211f * s;
            dst[31]    -= 0.0635086982303136f * s;

            // Move destination buffer
            dst        += 8;
        }
    }

    void lanczos_resample_8x3(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float s     = *(src++);

            // Apply Lanczos 8x3 kernel
            dst[1]     += 0.0413762983728270f * s;
            dst[2]     += 0.0800907151667399f * s;
            dst[3]     += 0.1098388956286788f * s;
            dst[4]     += 0.1250635280404858f * s;
            dst[5]     += 0.1218381732799711f * s;
            dst[6]     += 0.0985951550643596f * s;
            dst[7]     += 0.0565868891962354f * s;

            dst[9]     -= 0.0643160644975587f * s;
            dst[10]    -= 0.1274947024165923f * s;
            dst[11]    -= 0.1796106608775988f * s;
            dst[12]    -= 0.2108456783342821f * s;
            dst[13]    -= 0.2127234580145891f * s;
            dst[14]    -= 0.1792608653618989f * s;
            dst[15]    -= 0.1078863334836789f * s;

            dst[17]    += 0.1389094368005209f * s;
            dst[18]    += 0.2996235873365793f * s;
            dst[19]    += 0.4700032639087741f * s;
            dst[20]    += 0.6361653568810439f * s;
            dst[21]    += 0.7838984051945062f * s;
            dst[22]    += 0.9001556302107272f * s;
            dst[23]    += 0.9744518753544421f * s;

            dst[24]    += s;

            dst[25]    += 0.9744518753544421f * s;
            dst[26]    += 0.9001556302107272f * s;
            dst[27]    += 0.7838984051945062f * s;
            dst[28]    += 0.6361653568810439f * s;
            dst[29]    += 0.4700032639087741f * s;
            dst[30]    += 0.2996235873365793f * s;
            dst[31]    += 0.1389094368005209f * s;

            dst[33]    -= 0.1078863334836789f * s;
            dst[34]    -= 0.1792608653618989f * s;
            dst[35]    -= 0.2127234580145891f * s;
            dst[36]    -= 0.2108456783342821f * s;
            dst[37]    -= 0.1796106608775988f * s;
            dst[38]    -= 0.1274947024165923f * s;
            dst[39]    -= 0.0643160644975587f * s;

            dst[41]    += 0.0565868891962354f * s;
            dst[42]    += 0.0985951550643596f * s;
            dst[43]    += 0.1218381732799711f * s;
            dst[44]    += 0.1250635280404858f * s;
            dst[45]    += 0.1098388956286788f * s;
            dst[46]    += 0.0800907151667399f * s;
            dst[47]    += 0.0413762983728270f * s;

            // Move destination buffer
            dst        += 8;
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

    void downsample_6x(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            *dst    = *src;
            dst     ++;
            src     += 6;
        }
    }

    void downsample_8x(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            *dst    = *src;
            dst     ++;
            src     += 8;
        }
    }
}

#endif /* DSP_ARCH_NATIVE_RESAMPLING_H_ */
