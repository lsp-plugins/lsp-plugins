/*
 * Convolver.h
 *
 *  Created on: 29 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_CONVOLVER_H_
#define CORE_CONVOLVER_H_

#include <core/types.h>
#include <core/ShiftBuffer.h>

namespace lsp
{

    class Convolver
    {
        private:
            ShiftBuffer     vBuffer;        // History buffer

            const float    *pConv;          // Convolution source
            size_t          nConvSize;      // Convolution size

            float          *pRend;           // Rendered convolution
            size_t          nRendLen;       // Rendered length of convolution

            bool            bRender;        // Render flag: need to re-render convolution
            size_t          nLength;        // Current length of convolution
            float           fLength;        // Current relative length of convolution

        protected:
            void    render_convolution();

        public:
            Convolver();
            ~Convolver();

        public:
            /** Initialize convolver
             *
             * @param convoluition convolution
             * @param size maximum convolution size
             * @return status of operations
             */
            bool init(const float *convolution, size_t size);

            /** Destroy convolver
             *
             */
            void destroy();

            /** Process samples
             *
             * @param dst destination buffer
             * @param src source buffer
             * @param count number of samples to process
             */
            void process(float *dst, const float *src, size_t count);

            /** Get Relative length of convolution
             *
             * @return relative length of convolution
             */
            inline float get_length() const     { return fLength;   };

            /** Set length of convolution
             *
             * @param length length of convolution
             */
            void set_length(float length);
    };

} /* namespace lsp */

#endif /* CORE_CONVOLVER_H_ */
