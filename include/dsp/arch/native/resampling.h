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
    #define KERNEL_TIMES    2
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

            printf("%.16ff,\n", kernel[i]);
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
