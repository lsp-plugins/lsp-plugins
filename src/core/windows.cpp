/*
 * Windowing.cpp
 *
 *  Created on: 20 февр. 2016 г.
 *      Author: sadko
 */

#include <core/windows.h>
#include <math.h>

namespace lsp
{
    namespace windows
    {
        const char *windows[] =
        {
            "Hann",
            "Hamming",
            "Blackman",
            "Lanczos",
            "Gaussian",
            "Poisson",
            "Parzen",
            "Tukey",
            "Welch",
            "Nuttall",
            "Blackman-Nuttall",
            "Blackman-Harris",
            "Hann-Poisson",
            "Bartlett-Hann",
            "Bartlett-Fejer",
            "Triangular",
            "Rectangular",
            "Flat top",
            "Cosine",
            NULL
        };

        void window(float *dst, size_t n, window_t type)
        {
            switch (type)
            {
                case HANN: hann(dst, n); break;
                case HAMMING: hamming(dst, n); break;
                case BLACKMAN: blackman(dst, n); break;
                case LANCZOS: lanczos(dst, n); break;
                case GAUSSIAN: gaussian(dst, n); break;
                case POISSON: poisson(dst, n); break;
                case PARZEN: parzen(dst, n); break;
                case TUKEY: tukey(dst, n); break;
                case WELCH: welch(dst, n); break;
                case NUTTALL: nuttall(dst, n); break;
                case BLACKMAN_NUTTALL: blackman_nuttall(dst, n); break;
                case BLACKMAN_HARRIS: blackman_harris(dst, n); break;
                case HANN_POISSON: hann_poisson(dst, n); break;
                case BARTLETT_HANN: bartlett_hann(dst, n); break;
                case BARTLETT_FEJER: bartlett_fejer(dst, n); break;
                case TRIANGULAR: triangular(dst, n); break;
                case RECTANGULAR: rectangular(dst, n); break;
                case FLAT_TOP: flat_top(dst, n); break;
                case COSINE: cosine(dst, n); break;
                default:
                    break;
            }
        }

        void rectangular(float *dst, size_t n)
        {
            while (n--)
                (*dst++)    = 1.0f;
        }

        void triangular_general(float *dst, size_t n, int dn)
        {
            if (n == 0)
                return;

            float l =  (dn > 0) ? n + 1 :
                        (dn < 0) ? n - 1 :
                        n;
            if (l == 0.0f)
            {
                *dst = 0.0f;
                return;
            }
            l           = 2.0f / l;
            float c     = (n - 1) * 0.5;

            for (size_t i=0; i<n; ++i)
                dst[i]      = 1.0f - fabs((i - c) * l);
        }

        void bartlett_fejer(float *dst, size_t n)
        {
            triangular_general(dst, n, -1);
        }

        void triangular(float *dst, size_t n)
        {
            triangular_general(dst, n, 0);
        }

        void parzen(float *dst, size_t n)
        {
            if (n == 0)
                return;
            float n_2       = 0.5 * n;
            float n_4       = 0.25 * n;
            float n__2      = 1.0 / n_2;

            for (size_t i=0; i<n; ++i)
            {
                float x     = fabs(i - n_2);
                float k     = x * n__2;
                float p     = 1.0f - k;

                if (x <= n_4)
                    dst[i]      = 1.0f - 6.0f * k * k * p;
                else
                    dst[i]      = 2.0f * p * p * p;
            }
        }

        void welch(float *dst, size_t n)
        {
            if (n == 0)
                return;

            float c     = (n - 1) * 0.5f;
            float mc    = 1.0f / c;

            for (size_t i=0; i<n; ++i)
            {
                float t     = (i - c) * mc;
                dst[i]      = 1.0f - t * t;
            }
        }

        void hamming_general(float *dst, size_t n, float a, float b)
        {
            if (n == 0)
                return;

            float f = 2.0f * M_PI / (n - 1);
            for (size_t i=0; i<n; ++i)
                dst[i]  = a - b * cosf(i * f);
        }

        void hann(float *dst, size_t n)
        {
            hamming_general(dst, n, 0.5f, 0.5f);
        }

        void hamming(float *dst, size_t n)
        {
            hamming_general(dst, n, 0.54f, 0.46f);
        }

        void blackman_general(float *dst, size_t n, float a)
        {
            if (n == 0)
                return;

            float a2        = a * 0.5f;
            float a0        = 0.5f - a2;
            float f1        = 2.0f * M_PI / (n - 1);
            float f2        = f1 * 2.0f;

            for (size_t i=0; i<n; ++i)
                dst[i]  = a0 - 0.5 * cosf(i * f1) + a2 * cosf(i * f2);
        }

        void blackman(float *dst, size_t n)
        {
            return blackman_general(dst, n, 0.16f);
        }

        void nuttall_general(float *dst, size_t n, float a0, float a1, float a2, float a3)
        {
            if (n == 0)
                return;

            float f1        = 2.0f * M_PI / (n - 1);
            float f2        = f1 * 2.0f;
            float f3        = f1 * 3.0f;

            for (size_t i=0; i<n; ++i)
                dst[i]  = a0 - a1 * cosf(i * f1) + a2 * cosf(i * f2) - a3 * cosf(i * f3);
        }

        void nuttall(float *dst, size_t n)
        {
            return nuttall_general(dst, n, 0.355768f, 0.487396f, 0.144232f, 0.012604f);
        }

        void blackman_nuttall(float *dst, size_t n)
        {
            return nuttall_general(dst, n, 0.3635819f, 0.4891775f, 0.1365995f, 0.0106411f);
        }

        void blackman_harris(float *dst, size_t n)
        {
            return nuttall_general(dst, n, 0.35875f, 0.48829f, 0.14128f, 0.01168f);
        }

        void flat_top_general(float *dst, size_t n, float a0, float a1, float a2, float a3, float a4)
        {
            if (n == 0)
                return;
            float f1        = 2.0f * M_PI / (n - 1);
            float f2        = f1 * 2.0f;
            float f3        = f1 * 3.0f;
            float f4        = f1 * 4.0f;
            float norm      = 1.0f / (a0 - a1 * cosf(n * 0.5 * f1) + a2 * cosf(n * 0.5 * f2) - a3 * cosf(n * 0.5 * f3) + a4 * cosf(n * 0.5 * f4));

            for (size_t i=0; i<n; ++i)
                dst[i]  = norm * (a0 - a1 * cosf(i * f1) + a2 * cosf(i * f2) - a3 * cosf(i * f3) + a4 * cosf(i * f4));
        }

        void flat_top(float *dst, size_t n)
        {
            return flat_top_general(dst, n, 1.0f, 1.93f, 1.29f, 0.388f, 0.028f);
        }

        void cosine(float *dst, size_t n)
        {
            if (n == 0)
                return;

            float f         = M_PI / (n - 1);
            for (size_t i=0; i<n; ++i)
                dst[i]      = sinf(f * i);
        }

        void gaussian_general(float *dst, size_t n, float s)
        {
            if ((n == 0) || (s > 0.5))
                return;
            float c     = (n - 1) * 0.5f;
            float sc    = 1.0f / (c * s);
            for (size_t i=0; i<n; ++i)
            {
                float v     = (i - c) * sc;
                dst[i]      = expf(-0.5f * v * v);
            }
        }

        void gaussian(float *dst, size_t n)
        {
            return gaussian_general(dst, n, 0.4);
        }

        void poisson_general(float *dst, size_t n, float t)
        {
            float c = (n - 1) * 0.5f;
            t       = - 1.0f / t;
            for (size_t i=0; i<n; ++i)
                dst[i] = expf(t * fabs(i - c));
        }

        void poisson(float *dst, size_t n)
        {
            poisson_general(dst, n, n * 0.5f);
        }

        void bartlett_hann_general(float *dst, size_t n, float a0, float a1, float a2)
        {
            if (n == 0)
                return;
            float k1    = 1.0f / (n - 1);
            float k2    = 2.0f * M_PI * k1;
            for (size_t i=0; i<n; ++i)
                dst[i] = a0 - a1 * fabs(i * k1 - 0.5f) - a2 * cosf(i * k2);
        }

        void bartlett_hann(float *dst, size_t n)
        {
            return bartlett_hann_general(dst, n, 0.62f, 0.48f, 0.38f);
        }

        void hann_poisson_general(float *dst, size_t n, float a)
        {
            if (n == 0)
                return;

            float f     = 2.0f * M_PI / (n - 1);
            float k1    = (n - 1) * 0.5;
            float k2    = - a / k1;
            for (size_t i=0; i<n; ++i)
                dst[i]  = (0.5 - 0.5 * cosf(i * f)) * expf(k2 * fabs(k1 - i));
        }

        void hann_poisson(float *dst, size_t n)
        {
            return hann_poisson_general(dst, n, 2.0f);
        }

        void lanczos(float *dst, size_t n)
        {
            if (n == 0)
                return;

            float k = 2.0f * M_PI / (n - 1);
            for (size_t i=0; i<n; ++i)
            {
                float x = k * i - M_PI;
                dst[i]  = (x == 0.0f) ? 1.0f : sinf(x) / x;
            }
        }

        void tukey_general(float *dst, size_t n, float a)
        {
            if (n == 0)
                return;
            if (a == 0.0f)
            {
                rectangular(dst, n);
                return;
            }

            size_t last     = n - 1;
            size_t b1       = 0.5 * a * last;
            size_t b2       = last - b1;
            float k         = M_PI * 2.0f / (a * last);
            float x         = M_PI - 2.0f * M_PI / a;
            for (size_t i=0; i<n; ++i)
            {
                if (i <= b1)
                    dst[i]      = 0.5f + 0.5f * cosf(k * i - M_PI);
                else if (i > b2)
                    dst[i]      = 0.5f + 0.5f * cosf(k * i + x);
                else
                    dst[i]      = 1.0f;
            }
        }

        void tukey(float *dst, size_t n)
        {
            return tukey_general(dst, n, 0.5f);
        }

    }
}
