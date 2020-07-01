/*
 * SpectralProcessor.h
 *
 *  Created on: 1 июл. 2020 г.
 *      Author: sadko
 */

#ifndef CORE_UTIL_SPECTRALPROCESSOR_H_
#define CORE_UTIL_SPECTRALPROCESSOR_H_

#include <core/types.h>

namespace lsp
{
    typedef void (* spectral_processor_func_t)(void *object, void *subject, float *spectrum, size_t rank);
    
    /**
     * Spectral processor class, performs spectral transform of the input signal
     * and launches callback function to process the signal spectrum
     */
    class SpectralProcessor
    {
        private:
            SpectralProcessor & operator = (const SpectralProcessor &);

        protected:
            size_t                      nRank;      // Current FFT rank
            size_t                      nMaxRank;   // Maximum FFT rank
            float                       fPreGain;   // Preamplifier gain
            float                       fPhase;     // Phase
            bool                        bUpdate;    // Update flag

            // Bindings
            spectral_processor_func_t   pFunc;      // Function
            void                       *pObject;    // Object to operate
            void                       *pSubject;   // Subject to operate

        public:
            explicit SpectralProcessor();
            virtual ~SpectralProcessor();

            /**
             * Initialize spectral processor
             * @param max_rank maximum FFT rank
             * @return status of operation
             */
            bool            init(size_t max_rank);

            /**
             * Destroy spectral processor
             */
            void            destroy();

        public:
            /**
             * Bind spectral processor to the handler
             * @param func function to call
             * @param object the target object to pass to the function
             * @param subject the target subject to pass to the function
             */
            void            bind(spectral_processor_func_t func, void *object, void *subject);

            /**
             * Unbind spectral processor
             */
            void            unbind();

            /**
             * Check that spectral processor needs update
             * @return true if spectral processor needs update
             */
            inline bool     needs_update() const        { return bUpdate;           }

            /**
             * Update settings of the spectral processor
             */
            void            update_settings();

            /**
             * Get the FFT rank
             * @return FFT rank
             */
            inline size_t   get_rank() const            { return nRank;             }

            /**
             * Get processing phase
             * @return processing phase
             */
            inline float    phase() const               { return fPhase;            }

            /**
             * Set processing phase
             * @param phase the phase value between 0 and 1
             */
            void            set_phase(float phase);

            /**
             * Set the FFT rank
             */
            void            set_rank(size_t rank);

            /**
             * Set preamplification gain
             * @return preamplification gain
             */
            inline float    pre_gain() const            { return fPreGain;          }

            /**
             * Set preamplification gain
             * @param v preamplification gain
             */
            inline void     set_pre_gain(float v)       { fPreGain = v;             }

            /**
             * Get latency of the spectral processor
             * @return latency of the spectral processor
             */
            inline size_t   latency() const             { return 1 << (nRank - 1);  }

            /**
             * Perform audio processing
             * @param dst destination buffer
             * @param src source buffer
             * @param count number of samples to process
             */
            void            process(float *dst, const float *src, size_t count);
    };

} /* namespace lsp */

#endif /* CORE_UTIL_SPECTRALPROCESSOR_H_ */
