/*
 * search.h
 *
 *  Created on: 25 мая 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_SEARCH_H_
#define DSP_ARCH_NATIVE_SEARCH_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    float min(const float *src, size_t count)
    {
        if (count == 0)
            return 0.0f;

        float min = src[0];
        for (size_t i=1; i<count; ++i)
        {
            float tmp = src[i];
            if (tmp < min)
                min = tmp;
        }
        return min;
    }

    float max(const float *src, size_t count)
    {
        if (count == 0)
            return 0.0f;

        float max = src[0];
        for (size_t i=1; i<count; ++i)
        {
            float tmp = src[i];
            if (tmp > max)
                max = tmp;
        }
        return max;
    }

    float abs_min(const float *src, size_t count)
    {
        if (count == 0)
            return 0.0f;

        float min = fabs(src[0]);
        for (size_t i=1; i<count; ++i)
        {
            float tmp = fabs(src[i]);
            if (tmp < min)
                min     = tmp;
        }
        return min;
    }

    float abs_max(const float *src, size_t count)
    {
        if (count == 0)
            return 0.0f;

        float max = fabs(src[0]);
        for (size_t i=1; i<count; ++i)
        {
            float tmp = fabs(src[i]);
            if (tmp > max)
                max     = tmp;
        }
        return max;
    }

    void minmax(const float *src, size_t count, float *min, float *max)
    {
        if (count == 0)
        {
            *min    = 0.0f;
            *max    = 0.0f;
            return;
        }

        float a_min = src[0], a_max = src[0];
        for (size_t i=1; i<count; ++i)
        {
            float tmp   = src[i];
            if (tmp < a_min)
                a_min       = tmp;
            if (tmp > a_max)
                a_max       = tmp;
        }
        *min    = a_min;
        *max    = a_max;
    }

    void abs_minmax(const float *src, size_t count, float *min, float *max)
    {
        if (count == 0)
        {
            *min    = 0.0f;
            *max    = 0.0f;
            return;
        }

        float a_min = fabs(src[0]), a_max = fabs(src[0]);

        for (size_t i=1; i<count; ++i)
        {
            float tmp   = fabs(src[i]);
            if (tmp < a_min)
                a_min       = tmp;
            if (tmp > a_max)
                a_max       = tmp;
        }
        *min    = a_min;
        *max    = a_max;
    }

    size_t min_index(const float *src, size_t count)
    {
        if (count <= 0)
            return 0;

        size_t index = 0;
        float vmin = src[0];
        for (size_t i=1; i<count; ++i)
        {
            float v = src[i];
            if (v < vmin)
            {
                index   = i;
                vmin    = v;
            }
        }
        return index;
    }

    size_t max_index(const float *src, size_t count)
    {
        if (count <= 0)
            return 0;

        size_t index = 0;
        float vmax = src[0];
        for (size_t i=1; i<count; ++i)
        {
            float v = src[i];
            if (v > vmax)
            {
                index   = i;
                vmax    = v;
            }
        }
        return index;
    }

    size_t abs_min_index(const float *src, size_t count)
    {
        if (count <= 0)
            return 0;

        size_t index = 0;
        float s = fabs(src[0]);
        for (size_t i=1; i<count; ++i)
        {
            float d = fabs(src[i]);
            if (d < s)
            {
                index   = i;
                s       = d;
            }
        }
        return index;
    }

    size_t abs_max_index(const float *src, size_t count)
    {
        if (count <= 0)
            return 0;

        size_t index = 0;
        float s = fabs(src[0]);
        for (size_t i=1; i<count; ++i)
        {
            float d = fabs(src[i]);
            if (d > s)
            {
                index   = i;
                s       = d;
            }
        }
        return index;
    }

    void minmax_index(const float *src, size_t count, size_t *min, size_t *max)
    {
        size_t imin = 0, imax = 0;
        if (count > 0)
        {
            float vmin = src[0];
            float vmax = vmin;

            for (size_t i=1; i<count; ++i)
            {
                float v = src[i];
                if (v < vmin)
                {
                    imin    = i;
                    vmin    = v;
                }
                if (v > vmax)
                {
                    imax    = i;
                    vmax    = v;
                }
            }
        }

        *min = imin;
        *max = imax;
    }

    void abs_minmax_index(const float *src, size_t count, size_t *min, size_t *max)
    {
        size_t imin = 0, imax = 0;
        if (count > 0)
        {
            float vmin = fabs(src[0]);
            float vmax = vmin;

            for (size_t i=1; i<count; ++i)
            {
                float v = fabs(src[i]);
                if (v < vmin)
                {
                    imin    = i;
                    vmin    = v;
                }
                if (v > vmax)
                {
                    imax    = i;
                    vmax    = v;
                }
            }
        }

        *min = imin;
        *max = imax;
    }
}

#endif /* DSP_ARCH_NATIVE_SEARCH_H_ */
