/*
 * dsp.hpp
 *
 *  Created on: 02 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_DSP_H_
#define CORE_DSP_H_

#include <core/types.h>
#include <stddef.h>
#include <math.h>
#include <string.h>

namespace lsp
{
    class dsp
    {
        protected:
            dsp(); // Deny direct instantiation

        public:
            virtual ~dsp(); // Virtual destructor for pure virtual class

        public:
            /** Copy data: dst[i] = src[i]
             *
             * @param dst destination pointer
             * @param src source pointer
             * @param count number of elements
             */
            virtual void copy(float *dst, const float *src, size_t count) = 0;

            /** Copy data multiplied by value: dst[i] = src[i] * k
             *
             * @param dst destination pointer
             * @param src source pointer
             * @param k multiplier
             * @param count number of elements
             */
            virtual void copy_multiplied(float *dst, const float *src, float k, size_t count) = 0;

            virtual void fill(float *dst, float value, size_t count) = 0;
            virtual void fill_zero(float *dst, size_t count) = 0;
            virtual void fill_one(float *dst, size_t count) = 0;
            virtual void fill_minus_one(float *dst, size_t count) = 0;

            /** Calculate absolute values: dst[i] = abs(src[i])
             *
             * @param dst destination vector
             * @param src source vector
             * @param count number of elements
             */
            virtual void abs(float *dst, const float *src, size_t count) = 0;

            /** Calculate absolute normalized values: dst[i] = abs(src[i]) / max { abs(src) }
             *
             * @param dst destination vector
             * @param src source vector
             * @param count number of elements
             */
            virtual void abs_normalized(float *dst, const float *src, size_t count) = 0;

            /** Calculate normalized values: dst[i] = src[i] / (max { abs(src) })
             *
             * @param dst destination vector
             * @param src source vector
             * @param count number of elements
             */
            virtual void normalize(float *dst, const float *src, size_t count) = 0;

            /** Calculate min { src }
             *
             * @param src source vector
             * @param count number of elements
             * @return minimum value
             */
            virtual float min(const float *src, size_t count) = 0;

            /** Calculate max { src }
             *
             * @param src source vector
             * @param count number of elements
             * @return maximum value
             */
            virtual float max(const float *src, size_t count) = 0;

            /** Calculate @ min { src }
             *
             * @param src source vector
             * @param count number of elements
             * @return minimum value index
             */
            virtual size_t min_index(const float *src, size_t count) = 0;

            /** Calculate @ max { src }
             *
             * @param src source vector
             * @param count number of elements
             * @return maximum value
             */
            virtual size_t max_index(const float *src, size_t count) = 0;

            /** Simply multiply: dst[i] = src[i] * k
             *
             * @param dst destination
             * @param src source
             * @param k multiplier
             * @param count number of elements
             */
            virtual void multiply(float *dst, const float *src, float k, size_t count) = 0;

            /** Calculate sum {from 0 to count-1} (a[i] * b[i])
             *
             * @param a first vector
             * @param b second vector
             * @param count number of elements
             * @return scalar multiplication
             */
            virtual float scalar_mul(const float *a, const float *b, size_t count) = 0;

            /** Calculate dst[i] = dst[i] * k + src[i] * p
             *
             * @param dst accumulator list
             * @param src change list
             * @param k keep value
             * @param p push value
             * @param count number of elements
             */
            virtual void accumulate(float *dst, const float *src, float k, float p, size_t count) = 0;

            /** Calculate dst[i] = dst[i] + src[i] * k
             *
             * @param dst destination array
             * @param src source array
             * @param k multiplier
             * @param count number of elements
             */
            virtual void add_multiplied(float *dst, const float *src, float k, size_t count) = 0;

            /** Calculate dst[i] = dst[i] - src[i] * k
             *
             * @param dst destination array
             * @param src source array
             * @param k multiplier
             * @param count number of elements
             */
            virtual void sub_multiplied(float *dst, const float *src, float k, size_t count) = 0;

            /** Calculate dst[i] = dst[i] + (src[i] - dst[i]) * k
             *
             * @param dst destination
             * @param src function value to integrate
             * @param k time factor
             * @param count
             */
            virtual void integrate(float *dst, const float *src, float k, size_t count) = 0;

            /** Calculate dst[i] = src1[i] * k1 + src2[i] * k2
             *
             * @param dst destination buffer
             * @param src1 source buffer 1
             * @param src2 source buffer 2
             * @param k1 multiplier 1
             * @param k2 multiplier 2
             */
            virtual void mix(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count) = 0;

            /** Get mostly optimized DSP instance
             *
             * @return DSP instance
             */
            static dsp * createInstance();
    };

    typedef dsp * (*dsp_factory_t)();

} /* namespace forzee */

#endif /* CORE_DSP_HPP_ */
