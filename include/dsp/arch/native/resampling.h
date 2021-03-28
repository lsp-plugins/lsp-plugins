/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 19 нояб. 2016 г.
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

#ifndef DSP_ARCH_NATIVE_RESAMPLING_H_
#define DSP_ARCH_NATIVE_RESAMPLING_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

/*

    #include <stdio.h>
    #include <math.h>

    // Number of lobes
    #define KERNEL_SIZE     4
    // Oversampling times
    #define KERNEL_TIMES    2
    // Number of samples to generate
    #define KERNEL_MAX      64

    double kernel[KERNEL_MAX];

    int main(void)
    {
        ssize_t leaf = KERNEL_SIZE * KERNEL_TIMES;
        ssize_t dots = leaf * 2 + 1;

        for (ssize_t i=0; i<KERNEL_MAX; ++i)
        {
            double xx = double(i - leaf) / double(KERNEL_TIMES);

            if (i >= dots)
                kernel[i] = 0.0;
            else if (i == leaf)
                kernel[i] = 1.0;
            else
            {
                double px = M_PI * xx;
                kernel[i] = (double(KERNEL_SIZE) * sin(px) * sin(px / double(KERNEL_SIZE))) / (px * px);
            }
        }

        printf("leaf=%d, dots=%d, kernel_size=%d, kernel_times=%d\n", int(leaf), int(dots), KERNEL_SIZE, KERNEL_TIMES);
        for (ssize_t i=0; i<KERNEL_MAX; ++i)
            printf("%s%.16ff,\n", (kernel[i] >= 0.0) ? "+" : "", kernel[i]);

        printf("code:\n");
        for (ssize_t i=0; i<KERNEL_MAX; ++i)
        {
            double v = kernel[i];
            if (fabs(v) < 1e-6)
                continue;

            const char *s = (i >= 10) ? "    " : "     ";

            if (v > 0.0)
                printf("dst[%d]%s+= %.16ff * s;\n", int(i), s, v);
            else
                printf("dst[%d]%s-= %.16ff * s;\n", int(i), s, -v);
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
            dst[1]     -= 0.0636843520278618f * s;
            dst[3]     += 0.5731591682507563f * s;
            dst[4]     += s;
            dst[5]     += 0.5731591682507563f * s;
            dst[7]     -= 0.0636843520278618f * s;

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
            dst[1]     += 0.0243170840741611f * s;
            dst[3]     -= 0.1350949115231170f * s;
            dst[5]     += 0.6079271018540265f * s;
            dst[6]     += s;
            dst[7]     += 0.6079271018540265f * s;
            dst[9]     -= 0.1350949115231170f * s;
            dst[11]    += 0.0243170840741611f * s;

            // Move destination buffer
            dst        += 2;
        }
    }

    void lanczos_resample_2x4(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float s     = *(src++);

            // Apply Lanczos 2x3 kernel
            dst[1]     -= 0.0126608778212387f * s;
            dst[3]     += 0.0599094833772629f * s;
            dst[5]     -= 0.1664152316035080f * s;
            dst[7]     += 0.6203830132406946f * s;
            dst[8]     += 1.0000000000000000f * s;
            dst[9]     += 0.6203830132406946f * s;
            dst[11]    -= 0.1664152316035080f * s;
            dst[13]    += 0.0599094833772629f * s;
            dst[15]    -= 0.0126608778212387f * s;

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
            dst[1]     -= 0.0315888188312782f * s;
            dst[2]     -= 0.0854897486982225f * s;
            dst[4]     += 0.3419589947928900f * s;
            dst[5]     += 0.7897204707819555f * s;
            dst[6]     += s;
            dst[7]     += 0.7897204707819555f * s;
            dst[8]     += 0.3419589947928900f * s;
            dst[10]    -= 0.0854897486982225f * s;
            dst[11]    -= 0.0315888188312782f * s;

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
            dst[1]     += 0.0126609519658153f * s;
            dst[2]     += 0.0310789306368038f * s;
            dst[4]     -= 0.0933267410806225f * s;
            dst[5]     -= 0.1458230329384726f * s;
            dst[7]     += 0.3807169003008463f * s;
            dst[8]     += 0.8103009258121772f * s;
            dst[9]     += s;
            dst[10]    += 0.8103009258121772f * s;
            dst[11]    += 0.3807169003008463f * s;
            dst[13]    -= 0.1458230329384726f * s;
            dst[14]    -= 0.0933267410806225f * s;
            dst[16]    += 0.0310789306368038f * s;
            dst[17]    += 0.0126609519658153f * s;

            // Move destination buffer
            dst        += 3;
        }
    }

    void lanczos_resample_3x4(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float s     = *(src++);

            // Apply Lanczos 3x3 kernel
            dst[1]     -= 0.0067568495254777f * s;
            dst[2]     -= 0.0157944094156391f * s;
            dst[4]     += 0.0427448743491113f * s;
            dst[5]     += 0.0622703182267308f * s;
            dst[7]     -= 0.1220498237243924f * s;
            dst[8]     -= 0.1709794973964449f * s;
            dst[10]    += 0.3948602353909778f * s;
            dst[11]    += 0.8175787925827955f * s;
            dst[12]    += 1.0000000000000000f * s;
            dst[13]    += 0.8175787925827955f * s;
            dst[14]    += 0.3948602353909778f * s;
            dst[16]    -= 0.1709794973964449f * s;
            dst[17]    -= 0.1220498237243924f * s;
            dst[19]    += 0.0622703182267308f * s;
            dst[20]    += 0.0427448743491113f * s;
            dst[22]    -= 0.0157944094156391f * s;
            dst[23]    -= 0.0067568495254777f * s;

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
            dst[1]     -= 0.0179051851263444f * s;
            dst[2]     -= 0.0636843520278618f * s;
            dst[3]     -= 0.0847248039068907f * s;
            dst[5]     += 0.2353466775191407f * s;
            dst[6]     += 0.5731591682507563f * s;
            dst[7]     += 0.8773540711908775f * s;
            dst[8]     += s;
            dst[9]     += 0.8773540711908775f * s;
            dst[10]    += 0.5731591682507563f * s;
            dst[11]    += 0.2353466775191407f * s;
            dst[13]    -= 0.0847248039068907f * s;
            dst[14]    -= 0.0636843520278618f * s;
            dst[15]    -= 0.0179051851263444f * s;

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
            dst[1]     += 0.0073559260471942f * s;
            dst[2]     += 0.0243170840741611f * s;
            dst[3]     += 0.0300210914495816f * s;
            dst[5]     -= 0.0677913359005429f * s;
            dst[6]     -= 0.1350949115231170f * s;
            dst[7]     -= 0.1328710183650640f * s;
            dst[9]     += 0.2701898230462341f * s;
            dst[10]    += 0.6079271018540265f * s;
            dst[11]    += 0.8900670517104946f * s;
            dst[12]    += s;
            dst[13]    += 0.8900670517104946f * s;
            dst[14]    += 0.6079271018540265f * s;
            dst[15]    += 0.2701898230462341f * s;
            dst[17]    -= 0.1328710183650640f * s;
            dst[18]    -= 0.1350949115231170f * s;
            dst[19]    -= 0.0677913359005429f * s;
            dst[21]    += 0.0300210914495816f * s;
            dst[22]    += 0.0243170840741611f * s;
            dst[23]    += 0.0073559260471942f * s;

            // Move destination buffer
            dst        += 4;
        }
    }

    void lanczos_resample_4x4(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float s     = *(src++);

            // Apply Lanczos 4x3 kernel
            dst[1]     -= 0.0039757442382413f * s;
            dst[2]     -= 0.0126608778212387f * s;
            dst[3]     -= 0.0150736176408234f * s;

            dst[5]     += 0.0315083921595442f * s;
            dst[6]     += 0.0599094833772629f * s;
            dst[7]     += 0.0555206000541729f * s;

            dst[9]     -= 0.0917789511099593f * s;
            dst[10]    -= 0.1664152316035080f * s;
            dst[11]    -= 0.1525006180521938f * s;

            dst[13]    += 0.2830490423665725f * s;
            dst[14]    += 0.6203830132406946f * s;
            dst[15]    += 0.8945424536042901f * s;

            dst[16]    += 1.0000000000000000f * s;

            dst[17]    += 0.8945424536042901f * s;
            dst[18]    += 0.6203830132406946f * s;
            dst[19]    += 0.2830490423665725f * s;

            dst[21]    -= 0.1525006180521938f * s;
            dst[22]    -= 0.1664152316035080f * s;
            dst[23]    -= 0.0917789511099593f * s;

            dst[25]    += 0.0555206000541729f * s;
            dst[26]    += 0.0599094833772629f * s;
            dst[27]    += 0.0315083921595442f * s;

            dst[29]    -= 0.0150736176408234f * s;
            dst[30]    -= 0.0126608778212387f * s;
            dst[31]    -= 0.0039757442382413f * s;

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
            dst[1]     -= 0.0078021377848166f * s;
            dst[2]     -= 0.0315888188312782f * s;
            dst[3]     -= 0.0636843520278618f * s;
            dst[4]     -= 0.0854897486982225f * s;
            dst[5]     -= 0.0719035699814534f * s;

            dst[7]     += 0.1409309971636486f * s;
            dst[8]     += 0.3419589947928900f * s;
            dst[9]     += 0.5731591682507563f * s;
            dst[10]    += 0.7897204707819555f * s;
            dst[11]    += 0.9440586719628122f * s;

            dst[12]    += s;

            dst[13]    += 0.9440586719628122f * s;
            dst[14]    += 0.7897204707819555f * s;
            dst[15]    += 0.5731591682507563f * s;
            dst[16]    += 0.3419589947928900f * s;
            dst[17]    += 0.1409309971636486f * s;

            dst[19]    -= 0.0719035699814534f * s;
            dst[20]    -= 0.0854897486982225f * s;
            dst[21]    -= 0.0636843520278618f * s;
            dst[22]    -= 0.0315888188312782f * s;
            dst[23]    -= 0.0078021377848166f * s;

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
            dst[1]     += 0.0032875048460955f * s;
            dst[2]     += 0.0126609519658153f * s;
            dst[3]     += 0.0243170840741611f * s;
            dst[4]     += 0.0310789306368038f * s;
            dst[5]     += 0.0248005479513036f * s;

            dst[7]     -= 0.0424907562338176f * s;
            dst[8]     -= 0.0933267410806225f * s;
            dst[9]     -= 0.1350949115231170f * s;
            dst[10]    -= 0.1458230329384726f * s;
            dst[11]    -= 0.1049261531488149f * s;

            dst[13]    += 0.1676517041508127f * s;
            dst[14]    += 0.3807169003008463f * s;
            dst[15]    += 0.6079271018540265f * s;
            dst[16]    += 0.8103009258121772f * s;
            dst[17]    += 0.9500889005216107f * s;

            dst[18]    += s;

            dst[19]    += 0.9500889005216107f * s;
            dst[20]    += 0.8103009258121772f * s;
            dst[21]    += 0.6079271018540265f * s;
            dst[22]    += 0.3807169003008463f * s;
            dst[23]    += 0.1676517041508127f * s;

            dst[25]    -= 0.1049261531488149f * s;
            dst[26]    -= 0.1458230329384726f * s;
            dst[27]    -= 0.1350949115231170f * s;
            dst[28]    -= 0.0933267410806225f * s;
            dst[29]    -= 0.0424907562338176f * s;

            dst[31]    += 0.0248005479513036f * s;
            dst[32]    += 0.0310789306368038f * s;
            dst[33]    += 0.0243170840741611f * s;
            dst[34]    += 0.0126609519658153f * s;
            dst[35]    += 0.0032875048460955f * s;

            // Move destination buffer
            dst        += 6;
        }
    }

    void lanczos_resample_6x4(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float s     = *(src++);

            // Apply Lanczos 6x3 kernel
            dst[1]     -= 0.0018000092949500f * s;
            dst[2]     -= 0.0067568495254777f * s;
            dst[3]     -= 0.0126608778212387f * s;
            dst[4]     -= 0.0157944094156391f * s;
            dst[5]     -= 0.0123019137260206f * s;

            dst[7]     += 0.0200263389720192f * s;
            dst[8]     += 0.0427448743491113f * s;
            dst[9]     += 0.0599094833772629f * s;
            dst[10]    += 0.0622703182267308f * s;
            dst[11]    += 0.0427971267140625f * s;

            dst[13]    -= 0.0597744992948478f * s;
            dst[14]    -= 0.1220498237243924f * s;
            dst[15]    -= 0.1664152316035080f * s;
            dst[16]    -= 0.1709794973964449f * s;
            dst[17]    -= 0.1181145298553785f * s;

            dst[19]    += 0.1776396342037379f * s;
            dst[20]    += 0.3948602353909778f * s;
            dst[21]    += 0.6203830132406946f * s;
            dst[22]    += 0.8175787925827955f * s;
            dst[23]    += 0.9522049170285306f * s;

            dst[24]    += 1.0000000000000000f * s;

            dst[25]    += 0.9522049170285306f * s;
            dst[26]    += 0.8175787925827955f * s;
            dst[27]    += 0.6203830132406946f * s;
            dst[28]    += 0.3948602353909778f * s;
            dst[29]    += 0.1776396342037379f * s;

            dst[31]    -= 0.1181145298553785f * s;
            dst[32]    -= 0.1709794973964449f * s;
            dst[33]    -= 0.1664152316035080f * s;
            dst[34]    -= 0.1220498237243924f * s;
            dst[35]    -= 0.0597744992948478f * s;

            dst[37]    += 0.0427971267140625f * s;
            dst[38]    += 0.0622703182267308f * s;
            dst[39]    += 0.0599094833772629f * s;
            dst[40]    += 0.0427448743491113f * s;
            dst[41]    += 0.0200263389720192f * s;

            dst[43]    -= 0.0123019137260206f * s;
            dst[44]    -= 0.0157944094156391f * s;
            dst[45]    -= 0.0126608778212387f * s;
            dst[46]    -= 0.0067568495254777f * s;
            dst[47]    -= 0.0018000092949500f * s;

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
            dst[1]     -= 0.0043033145538298f * s;
            dst[2]     -= 0.0179051851263444f * s;
            dst[3]     -= 0.0393892611124141f * s;
            dst[4]     -= 0.0636843520278618f * s;
            dst[5]     -= 0.0823353965569232f * s;
            dst[6]     -= 0.0847248039068907f * s;
            dst[7]     -= 0.0600950644541902f * s;

            dst[9]     += 0.0993408208324369f * s;
            dst[10]    += 0.2353466775191407f * s;
            dst[11]    += 0.3985033193355084f * s;
            dst[12]    += 0.5731591682507563f * s;
            dst[13]    += 0.7396427919997760f * s;
            dst[14]    += 0.8773540711908775f * s;
            dst[15]    += 0.9682457746117045f * s;

            dst[16]    += s;

            dst[17]    += 0.9682457746117045f * s;
            dst[18]    += 0.8773540711908775f * s;
            dst[19]    += 0.7396427919997760f * s;
            dst[20]    += 0.5731591682507563f * s;
            dst[21]    += 0.3985033193355084f * s;
            dst[22]    += 0.2353466775191407f * s;
            dst[23]    += 0.0993408208324369f * s;

            dst[25]    -= 0.0600950644541902f * s;
            dst[26]    -= 0.0847248039068907f * s;
            dst[27]    -= 0.0823353965569232f * s;
            dst[28]    -= 0.0636843520278618f * s;
            dst[29]    -= 0.0393892611124141f * s;
            dst[30]    -= 0.0179051851263444f * s;
            dst[31]    -= 0.0043033145538298f * s;

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
            dst[1]     += 0.0018368899607481f * s;
            dst[2]     += 0.0073559260471942f * s;
            dst[3]     += 0.0155961678435580f * s;
            dst[4]     += 0.0243170840741611f * s;
            dst[5]     += 0.0303079634725070f * s;
            dst[6]     += 0.0300210914495816f * s;
            dst[7]     += 0.0204366616947175f * s;

            dst[9]     -= 0.0305684889733737f * s;
            dst[10]    -= 0.0677913359005429f * s;
            dst[11]    -= 0.1054383717904384f * s;
            dst[12]    -= 0.1350949115231170f * s;
            dst[13]    -= 0.1472651639056537f * s;
            dst[14]    -= 0.1328710183650640f * s;
            dst[15]    -= 0.0849124693704824f * s;

            dst[17]    += 0.1205345965259870f * s;
            dst[18]    += 0.2701898230462341f * s;
            dst[19]    += 0.4376469925430009f * s;
            dst[20]    += 0.6079271018540265f * s;
            dst[21]    += 0.7642122243343417f * s;
            dst[22]    += 0.8900670517104946f * s;
            dst[23]    += 0.9717147892357163f * s;

            dst[24]    += s;

            dst[25]    += 0.9717147892357163f * s;
            dst[26]    += 0.8900670517104946f * s;
            dst[27]    += 0.7642122243343417f * s;
            dst[28]    += 0.6079271018540265f * s;
            dst[29]    += 0.4376469925430009f * s;
            dst[30]    += 0.2701898230462341f * s;
            dst[31]    += 0.1205345965259870f * s;

            dst[33]    -= 0.0849124693704824f * s;
            dst[34]    -= 0.1328710183650640f * s;
            dst[35]    -= 0.1472651639056537f * s;
            dst[36]    -= 0.1350949115231170f * s;
            dst[37]    -= 0.1054383717904384f * s;
            dst[38]    -= 0.0677913359005429f * s;
            dst[39]    -= 0.0305684889733737f * s;

            dst[41]    += 0.0204366616947175f * s;
            dst[42]    += 0.0300210914495816f * s;
            dst[43]    += 0.0303079634725070f * s;
            dst[44]    += 0.0243170840741611f * s;
            dst[45]    += 0.0155961678435580f * s;
            dst[46]    += 0.0073559260471942f * s;
            dst[47]    += 0.0018368899607481f * s;

            // Move destination buffer
            dst        += 8;
        }
    }

    void lanczos_resample_8x4(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float s     = *(src++);

            // Apply Lanczos 8x3 kernel
            dst[1]     -= 0.0010124148822791f * s;
            dst[2]     -= 0.0039757442382413f * s;
            dst[3]     -= 0.0082714887261119f * s;
            dst[4]     -= 0.0126608778212387f * s;
            dst[5]     -= 0.0154958216565010f * s;
            dst[6]     -= 0.0150736176408234f * s;
            dst[7]     -= 0.0100753105205530f * s;

            dst[9]     += 0.0145047275409824f * s;
            dst[10]    += 0.0315083921595442f * s;
            dst[11]    += 0.0479233082326825f * s;
            dst[12]    += 0.0599094833772629f * s;
            dst[13]    += 0.0635233253590927f * s;
            dst[14]    += 0.0555206000541729f * s;
            dst[15]    += 0.0341810767869351f * s;

            dst[17]    -= 0.0439036941841078f * s;
            dst[18]    -= 0.0917789511099593f * s;
            dst[19]    -= 0.1356918370096595f * s;
            dst[20]    -= 0.1664152316035080f * s;
            dst[21]    -= 0.1746626357901899f * s;
            dst[22]    -= 0.1525006180521938f * s;
            dst[23]    -= 0.0947284057923417f * s;

            dst[25]    += 0.1285116137825641f * s;
            dst[26]    += 0.2830490423665725f * s;
            dst[27]    += 0.4518581595035692f * s;
            dst[28]    += 0.6203830132406946f * s;
            dst[29]    += 0.7729246687400148f * s;
            dst[30]    += 0.8945424536042901f * s;
            dst[31]    += 0.9729307018702211f * s;

            dst[32]    += 1.0000000000000000f * s;

            dst[33]    += 0.9729307018702211f * s;
            dst[34]    += 0.8945424536042901f * s;
            dst[35]    += 0.7729246687400148f * s;
            dst[36]    += 0.6203830132406946f * s;
            dst[37]    += 0.4518581595035692f * s;
            dst[38]    += 0.2830490423665725f * s;
            dst[39]    += 0.1285116137825641f * s;

            dst[41]    -= 0.0947284057923417f * s;
            dst[42]    -= 0.1525006180521938f * s;
            dst[43]    -= 0.1746626357901899f * s;
            dst[44]    -= 0.1664152316035080f * s;
            dst[45]    -= 0.1356918370096595f * s;
            dst[46]    -= 0.0917789511099593f * s;
            dst[47]    -= 0.0439036941841078f * s;

            dst[49]    += 0.0341810767869351f * s;
            dst[50]    += 0.0555206000541729f * s;
            dst[51]    += 0.0635233253590927f * s;
            dst[52]    += 0.0599094833772629f * s;
            dst[53]    += 0.0479233082326825f * s;
            dst[54]    += 0.0315083921595442f * s;
            dst[55]    += 0.0145047275409824f * s;

            dst[57]    -= 0.0100753105205530f * s;
            dst[58]    -= 0.0150736176408234f * s;
            dst[59]    -= 0.0154958216565010f * s;
            dst[60]    -= 0.0126608778212387f * s;
            dst[61]    -= 0.0082714887261119f * s;
            dst[62]    -= 0.0039757442382413f * s;
            dst[63]    -= 0.0010124148822791f * s;

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
