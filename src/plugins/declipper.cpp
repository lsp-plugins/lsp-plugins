//#include "declipper.h"
//#include <stdio.h>
//
//namespace forzee
//{
//    declipper::declipper()
//    {
//        vBuffer             = NULL;
//
//        // Initialize matrix
//        for (uint32_t i=0; i< DECLIPPER_DOTS*2; ++i)
//            vMatrix[i]  = &vpMatrix[i*(DECLIPPER_DOTS*2 + 1)];
//    }
//
//    declipper::~declipper()
//    {
//        fini();
//    }
//
//    void declipper::fini()
//    {
//        if (vBuffer != NULL)
//        {
//            delete [] vBuffer;
//            vBuffer = NULL;
//        }
//    }
//
//    void declipper::init(int sample_rate)
//    {
//        nSampleRate         = sample_rate;
//        uint32_t frame      = DECLIPPER_DOTS*2 + DECLIPPER_MAX_SAMPLES;
//        uint32_t buf_size   = align(frame + 1);//dsp::round_size(frame+1);
//
//        // Initialize
//        vBuffer             = new float[buf_size];
//        nBufSize            = buf_size;
//        nDetect             = (DECLIPPER_MAX_SAMPLES + DECLIPPER_MIN_SAMPLES) >> 1;
//        nHead               = 0;
//        nTail               = (nHead + nBufSize - frame) & (nBufSize - 1);
//        nClipStart          = 0;
//        nMix                = 0.5f;
//        enMode              = DETECT;
//        nThreshold          = 1.0;
//
//        // Clear buffer
//        for (uint32_t i=0; i<buf_size; ++i)
//            vBuffer[i]  = 0;
//    }
//
//    void declipper::print_matrix()
//    {
//        for (uint32_t i=0; i < DECLIPPER_DOTS*2; ++i)
//        {
//            float *row  = vMatrix[i];
//            printf("[ ");
//            for (uint32_t j=0; j <= DECLIPPER_DOTS*2; ++j)
//            {
//                if (j != 0)
//                    printf(", ");
//                printf("%f", row[j]);
//            }
//            printf(" ]\n");
//        }
//    }
//
//    void declipper::setup(float thresh, float mix, uint32_t samples, bool bypass)
//    {
//        // Calculate values
//        if (samples > DECLIPPER_MAX_SAMPLES)
//            samples = DECLIPPER_MAX_SAMPLES;
//        else if (samples < DECLIPPER_MIN_SAMPLES)
//            samples = DECLIPPER_MIN_SAMPLES;
//
//        if (mix > 1.0f)
//            mix = 1.0f;
//        else if (mix < 0.0f)
//            mix = 0.0f;
//
//        uint32_t frame      = samples + DECLIPPER_DOTS*2;
//
//        // Update parameters
//        nTail               = (nHead + nBufSize - frame) & (nBufSize - 1);
//        nThreshold          = thresh;
//        nDetect             = samples;
//        nMix                = mix;
//
//        if ((!bypass) && (enMode == BYPASS))
//            enMode = DETECT;
//        else if (bypass && (enMode != BYPASS))
//            enMode = BYPASS;
//    }
//
//    void declipper::interpolate(uint32_t start, uint32_t end)
//    {
//        dot_t dots[DECLIPPER_DOTS*2];
//        double roots[DECLIPPER_DOTS*2];
//
//        uint32_t buf_mask   = nBufSize - 1;
//        float time          = 0.0f;
//        uint32_t ptr        = (start + nBufSize - DECLIPPER_DOTS) & buf_mask;
//
//        // Start points
//        dots[0].s           = vBuffer[ptr];
//        dots[0].t           = time++;
//        ptr                 = (ptr + 1) & buf_mask;
//
//        dots[1].s           = vBuffer[ptr];
//        dots[1].t           = time++;
//        ptr                 = (ptr + 1) & buf_mask;
//
//        // End points
//        time               += (nBufSize + end - start) & buf_mask;
//        ptr                 = end;
//
//        dots[2].s           = vBuffer[ptr];
//        dots[2].t           = time++;
//        ptr                 = (ptr + 1) & buf_mask;
//
//        dots[3].s           = vBuffer[ptr];
//        dots[3].t           = time;
//
//        // Fill matrix
//        for (uint32_t i=0; i < DECLIPPER_DOTS*2; ++i)
//        {
//            float *row  = vMatrix[i];
//            float arg   = 1.0;
//            int j       = DECLIPPER_DOTS*2;
//            row[j--]    = dots[i].s;
//
//            while (j >= 0)
//            {
//                row[j--]    = arg;
//                arg        *= dots[i].t;
//            }
//        }
//
//        printf("Initial matrix\n");
//        print_matrix();
//
//        // Process matrix to make it triangle
//        for (uint32_t i=0; i<DECLIPPER_DOTS*2; ++i)
//        {
//            float *row = vMatrix[i];
//
//            // Check if current element is non-zero
//            if (row[i] == 0.0f)
//            {
//                // Find non-zero element to perform exchange
//                for (uint32_t j=i+1; j<DECLIPPER_DOTS*2; ++j)
//                {
//                    if (vMatrix[j][i] != 0.0f)
//                    {
//                        // Swap rows
//                        vMatrix[i]  = vMatrix[j];
//                        vMatrix[j]  = row;
//                        row         = vMatrix[i];
//                        break;
//                    }
//                }
//
//                // This should never happen but keep it...
//                if (row[i] == 0.0) // This means that matrix rank is zero and set of equations has no solution
//                    continue;
//            }
//
//            // Current element is non-zero, can update other elements
//            for (uint32_t j=i+1; j<DECLIPPER_DOTS*2; j++)
//            {
//                float *dst = vMatrix[j];
//                if (dst[i] != 0.0f)
//                {
////                    printf("k = m[%d,%d] (%f) / m[%d,%d] (%f) = %f\n",
////                        j, i, dst[i], i, i, row[i], dst[i] / row[i]);
//                    float k = dst[i] / row[i];
//
////                    printf("m[%d,%d] (%f) = 0\n", j, i, dst[i]);
//
//                    dst[i] = 0.0f;
//                    for (uint32_t l=i+1; l<=DECLIPPER_DOTS*2; ++l)
//                    {
////                        printf("m[%d,%d] (%f) = m[%d,%d] (%f) - k (%f) * m[%d,%d] (%f) = %f\n",
////                            j, l, dst[l], j, l, dst[l], k, i, l, row[l], dst[l] - row[l] * k);
//                        dst[l] -= row[l] * k;
//                    }
//                }
//            }
//
//            printf("Step %d matrix:\n", i);
//            print_matrix();
//        }
//
//        printf("Processed matrix\n");
//        print_matrix();
//
//        // Find roots
//        for (int i=DECLIPPER_DOTS*2-1, k=0; i>=0; --i, ++k)
//        {
//            float *row = vMatrix[i];
//
//            printf("root = m[%d,%d] (%f)\n", i, DECLIPPER_DOTS*2, row[DECLIPPER_DOTS*2]);
//            float root = row[DECLIPPER_DOTS*2];
//
//            for (int j=0; j<k; ++j)
//            {
//                printf("root = root (%f) - m[%d,%d] (%f) * root[%d] (%f) = %f\n",
//                    root, i, DECLIPPER_DOTS*2 - j - 1, row[DECLIPPER_DOTS*2 - j - 1], j, roots[j], root - row[DECLIPPER_DOTS*2 - j - 1]*roots[j]);
//                root = root - row[DECLIPPER_DOTS*2 - j - 1]*roots[j];
//            }
//
//            printf("root[%d] (%f) = root (%f) / m[%d,%d] (%f) = %f\n",
//                    k, roots[k], root, i, i, row[i], root / row[i]);
//
//            roots[k] = root / row[i];
//        }
//
//        printf("Roots:\n");
//        for (uint32_t i=0; i<DECLIPPER_DOTS*2; i++)
//            printf("  [%d] = %f\n", i, roots[i]);
//
//        // And now interpolate
//        time        = 0.0;
//        start       = (start + nBufSize - DECLIPPER_DOTS) & buf_mask;
//        end         = (end + nBufSize + DECLIPPER_DOTS) & buf_mask;
//        ptr         = start;
//
//        while (ptr != end)
//        {
//            float sample= roots[0];
//            float arg   = time;
//
//            for (uint32_t i=1; i<DECLIPPER_DOTS*2; ++i)
//            {
//                sample += arg * roots[i];
//                arg    *= time;
//            }
//
//            // Debug
//            printf("[%f] %f -> %f\n", time, vBuffer[ptr], vBuffer[ptr] + nMix * (sample - vBuffer[ptr]));
//
//            // Store new sample into buffer
//            vBuffer[ptr] += nMix * (sample - vBuffer[ptr]);
//
//            // Move time pointer
//            time       += 1.0;
//            ptr         = (ptr + 1) & buf_mask;
//        }
//    }
//
//    void declipper::process(float *in, float *out, int samples)
//    {
//        int buf_mask = nBufSize - 1;
//
//        for (int i=0; i<samples; ++i)
//        {
//            // Get sample and store it
//            float   s       = in[i];
//            float level     = abs(s);
//            vBuffer[nHead]  = s;
//
//            // Process sample
//            switch (enMode)
//            {
//                case DETECT: // Clipping detection. Find first sample with level > threshold
//                {
//                    if (level >= nThreshold)
//                    {
//                        nClipStart  = nHead;
//                        nDistance   = 1;
//                        enMode      = LOOKUP;
//                    }
//                }
//                break;
//
//                case LOOKUP: // Now signal is clipped, find when level began to be less than threshold
//                {
//                    if (level < nThreshold)
//                    {
//                        enMode      = INTERPOLATE;
//                        nDistance   = 1;
//                    }
//                    else if ((++nDistance) >= nDetect) // Force interpolation
//                    {
//                        enMode      = INTERPOLATE;
//                        nDistance   = 0;
//                    }
//                }
//                break;
//
//                case INTERPOLATE:
//                {
//                    if ((++nDistance) >= DECLIPPER_DOTS)
//                    {
//                        interpolate(nClipStart, (nHead + nBufSize - nDistance) & buf_mask);
//                        enMode = DETECT;
//                    }
//                }
//                break;
//
//                case BYPASS:
//                    break;
//            }
//
//            // Play sample
//            out[i]          = vBuffer[nTail];
//
//            // Update counters
//            nHead           = (nHead + 1) & buf_mask;
//            nTail           = (nTail + 1) & buf_mask;
//        }
//    }
//}
